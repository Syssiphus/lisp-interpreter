#!/usr/bin/env python

import re, sys, os

extensions  = [".h", ".c"]
headers     = [".h"]
pattern     = r"#include\s+[<\"]([\w\.]+)[>\"]"
output_name = "output.dot"

def read_file(filename):
    f = open(filename, "r")
    content = f.readlines()
    f.close()
    return content

def scan_file(filename):
    results = []
    for line in read_file(filename):
        match = re.search(pattern, line)
        if match:
            results.append(match.group(1))
    return results

def parse_files(path):
    results = {}
    for root, dirs, files in os.walk(path):
        for f in files:
            lead, ext = os.path.splitext(f)
            if ext in extensions and f[0] != '.':
                full_name = os.path.join(root, f)
                results[f] = scan_file(full_name)
    return results

def convert_data(data):
    retval = {}
    for k in data.keys():
        retval[(k, k.replace(".", "_"))] = map(lambda x: x.replace(".", "_"), data[k])
    return retval

def write_graph(data):
    d = convert_data(data)

    output = "digraph my_graph {\n"

    for (k, l) in d.keys():
        output += "%s [label=\"%s\"];\n" % (l, k)

    for k in d.keys():
        if len(d[k]) != 0:
            output += "%s;\n" % " -> ".join(d[k])

    output += "}\n"

    f = open(output_name, "w")
    f.write(output)
    f.close()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "Usage: %s <pathname>" % sys.argv[0]
    
    write_graph(parse_files(sys.argv[1]))

