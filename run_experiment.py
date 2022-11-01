import argparse
import os
import subprocess
from tqdm import tqdm
from shapely.geometry import Polygon


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
parser.add_argument('-i', '--iterations', default=2, type=int)
parser.add_argument('-t', '--time', default=1000, type=float)
parser.add_argument('-r', '--radius', default=1, type=float)
args = parser.parse_args()

if not os.path.exists(args.out_dir):
    os.makedirs(args.out_dir)

for file in tqdm([f for f in os.listdir(args.dir) if f.endswith('.poly')]):
    input_file = os.path.join(args.dir, file)
    """
    polygon = Polygon(read_cgal_polygon(input_file))
    print(polygon.area)
    continue
    """

    output_file = os.path.join(args.out_dir, file.replace('.poly', '_raw.out'))
    output_json = os.path.join(args.out_dir, file.replace('.poly', '.json'))

    if os.path.isfile(output_json):
        print("Output for", output_json, "already present")
        continue
    if os.path.isfile(output_file):
        os.remove(output_file)
        print("Removed output file", output_file)

    with open(output_file, "w") as f:
        subprocess.Popen([args.executable, input_file, output_json, str(args.iterations), str(args.time), str(args.radius)], stdout=f, stderr=f).communicate()