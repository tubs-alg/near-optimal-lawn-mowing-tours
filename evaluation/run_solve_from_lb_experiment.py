import argparse
import os
import subprocess
from tqdm import tqdm
from shapely.geometry import Polygon
import json
import slurminade

slurminade.update_default_configuration(partition="alg", constraint="alggen03",
                                        exclude="algry01,algry04", exclusive="")


@slurminade.slurmify()
def execute(output_file, command):
    with open(output_file, "w") as f:
        subprocess.Popen(command, stdout=f, stderr=f).communicate()


def load_it_count(file):
    with open(file, "r") as f:
        return len(json.load(f)["iterations"])


if __name__ == "__main__":

    parser = argparse.ArgumentParser()

    parser.add_argument('-d', '--dir', required=True)
    parser.add_argument('-o', '--out-dir', required=True)
    parser.add_argument('-ld', '--lower-bound-dir', required=True)
    parser.add_argument('-e', '--executable', required=True)
    parser.add_argument('-t', '--time', default=1000, type=float)
    parser.add_argument('-m', '--max-witness-size', default=20, type=int)
    args = parser.parse_args()

    if not os.path.exists(args.out_dir):
        os.makedirs(args.out_dir)

    for file in tqdm([f for f in os.listdir(args.dir) if f.endswith('.poly')]):
        input_file = os.path.join(args.dir, file)

        lb_file = os.path.join(args.lower_bound_dir, file.replace(".poly", ".json"))

        if not os.path.exists(lb_file):
            print(lb_file, "does not exist. Skipping")
            continue

        output_file = os.path.join(args.out_dir, file.replace('.poly', '_raw.out'))
        output_json = os.path.join(args.out_dir, file.replace('.poly', '.json'))

        if os.path.isfile(output_json):
            if load_it_count(lb_file) > load_it_count(output_json):
                os.remove(output_json)
                print("Removed output json", output_json)
            else:
                print("Output for", output_json, "already present")
                continue

        if os.path.isfile(output_file):
            os.remove(output_file)
            print("Removed output file", output_file)

        execute.distribute(output_file, [args.executable, input_file, output_json,
                                         lb_file,
                                         str(args.time), str(args.max_witness_size)])
