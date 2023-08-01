import argparse
import os
import slurminade
import _mowing_bindings
from aemeasure import MeasurementSeries, read_as_pandas_table, Database
from utils import strategy_to_readable
import itertools

slurminade.update_default_configuration(partition="alg", constraint="alggen03",
                                        mail_user="perk@ibr.cs.tu-bs.de",
                                        mail_type="ALL")

import socket
from pathlib import Path

if socket.gethostname().startswith("alg"):
    os.environ["GRB_LICENSE_FILE"] = os.path.join(
        Path.home(), ".gurobi", socket.gethostname(), "gurobi.lic"
    )
    if not os.path.exists(os.environ["GRB_LICENSE_FILE"]):
        msg = "Gurobi License File does not exist."
        raise RuntimeError(msg)


@slurminade.slurmify()
def run_lower_bound(output_dir: str,
                    input_file: str,
                    initial_strategy: int,
                    followup_strategy: int,
                    time: float,
                    radius: float,
                    max_initial_witnesses: int,
                    max_witnesses: int,
                    max_iterations: int):
    with MeasurementSeries(output_dir) as ms:
        with ms.measurement() as m:
            output = _mowing_bindings.mowing_lower_bound(input_file,
                                                         radius,
                                                         max_initial_witnesses,
                                                         max_witnesses,
                                                         max_iterations,
                                                         initial_strategy,
                                                         followup_strategy,
                                                         time)

            m["input_file"] = input_file
            m["instance"] = os.path.basename(input_file).rstrip(".poly")
            m["initial_strategy"] = strategy_to_readable(initial_strategy)
            m["followup_strategy"] = strategy_to_readable(followup_strategy)
            m["timelimit"] = time
            m["radius"] = radius
            m["max_initial_witnesses"] = max_initial_witnesses
            m["max_witnesses"] = max_witnesses
            m["max_iterations"] = max_iterations

            for key, val in output.items():
                m[key] = val


@slurminade.slurmify
def pack_after_finish(output_dir):
    Database(output_dir).compress()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument('-d', '--dir', required=True)
    parser.add_argument('-o', '--out-dir', required=True)
    parser.add_argument('-w', '--max-witnesses', default=10, type=int)
    parser.add_argument('-wi', '--max-initial-witnesses', default=15, type=int)
    parser.add_argument('-mi', '--max-iterations', default=5, type=int)
    parser.add_argument('-t', '--time', default=1800, type=float)
    parser.add_argument('-r', '--radius', default=1, type=float)
    args = parser.parse_args()

    if not os.path.exists(args.out_dir):
        os.makedirs(args.out_dir)

    previous_results = read_as_pandas_table(args.out_dir)

    with slurminade.Batch(max_size=5) as batch:
        for initial_strategy, followup_strategy in itertools.product(
                [_mowing_bindings.INITIAL_STRATEGY_CH,
                 _mowing_bindings.INITIAL_STRATEGY_VERTICES],
                [_mowing_bindings.FOLLOWUP_STRATEGY_GRID,
                 _mowing_bindings.FOLLOWUP_STRATEGY_RANDOM,
                 _mowing_bindings.FOLLOWUP_STRATEGY_SKELETON]):
            for file in (f for f in os.listdir(args.dir) if f.endswith('.poly')):
                input_file = os.path.join(args.dir, file)
                instance_name = os.path.basename(input_file).rstrip(".poly")

                if len(previous_results) > 0 and len(previous_results[
                                                         (previous_results["instance"] == instance_name) &
                                                         (previous_results["radius"] == args.radius) &
                                                         (previous_results["initial_strategy"] == strategy_to_readable(
                                                            initial_strategy)) &
                                                         (previous_results["followup_strategy"] == strategy_to_readable(
                                                             followup_strategy))]) > 0:
                    continue

                run_lower_bound.distribute(args.out_dir,
                                           input_file,
                                           initial_strategy,
                                           followup_strategy,
                                           args.time,
                                           args.radius,
                                           args.max_initial_witnesses,
                                           args.max_witnesses,
                                           args.max_iterations)

        jids = batch.flush()  # flush returns a list with all job ids.
        pack_after_finish.wait_for(jids).distribute(args.out_dir)
