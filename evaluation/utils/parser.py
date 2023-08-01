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