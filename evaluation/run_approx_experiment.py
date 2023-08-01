import argparse
import json
import os
import subprocess
from tqdm import tqdm
import slurminade

slurminade.update_default_configuration(partition="alg", constraint="alggen03", exclusive="")


@slurminade.slurmify()
def execute(output_file, command):
    with open(output_file, "w") as f:
        subprocess.Popen(command, stdout=f, stderr=f).communicate()

def read_cgal_polygon(file_name):
    def convert_number(string):
        if '/' in string:
            n, d = string.split('/')
            return float(n) / float(d)
        else:
            return float(string)

    polygon = []

    with open(file_name, 'r') as f:
        f.seek(0)
        content = f.readline().split()
        n = int(content[0])
        for i in range(n):
            x = content[2 * i + 1]
            y = content[2 * i + 2]
            polygon.append((convert_number(x), convert_number(y)))

    return polygon


parser = argparse.ArgumentParser()

parser.add_argument('-d', '--dir', required=True)
parser.add_argument('-o', '--out-dir', required=True)
parser.add_argument('-e', '--executable', required=True)
parser.add_argument('-t', '--time', default=1000, type=float)
parser.add_argument('-r', '--radius', default=1, type=float)
args = parser.parse_args()

if not os.path.exists(args.out_dir):
    os.makedirs(args.out_dir)

with open(os.path.join(args.out_dir, "config.json"), "w") as f:
    json.dump(vars(args), f)

for file in tqdm([f for f in os.listdir(args.dir) if f.endswith('.poly')]):
    input_file = os.path.join(args.dir, file)
    output_file = os.path.join(args.out_dir, file.replace('.poly', '_raw.out'))
    output_json = os.path.join(args.out_dir, file.replace('.poly', '.json'))

    if os.path.isfile(output_json):
        print("Output for", output_json, "already present")
        continue

    if os.path.isfile(output_file):
        os.remove(output_file)
        print("Removed output file", output_file)

    execute(output_file, [args.executable, input_file,
                          output_json,
                          str(args.time),
                          str(args.radius)])
