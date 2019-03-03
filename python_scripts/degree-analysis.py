# -*- coding: utf-8 -*-
#!/usr/bin/python
import argparse
import random
import csv
import settings
import numpy
import tools
import math

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Degree Analysis")
    parser.add_argument('-d', '--dataset', required=True)
    args = parser.parse_args()

    mean = 0
    var = 0
    std = 0

    nodes = tools.numberNodes(args)
    edges = 0
    degrees = numpy.zeros((nodes,), dtype=int)

    file_path = '../data/{0}/{0}_wc.inf'.format(args.dataset)
    print("Reading graph from: {}".format(file_path))
    with open(file_path, 'r') as f:
        for line in f:
            vals = line.rstrip("\n").split(" ")
            left = int(vals[0])
            right = int(vals[1])

            degrees[left] += 1
            degrees[right] += 1
            edges += 1

    mean = degrees.mean()
    var = degrees.var()
    std = degrees.std()

    print(args.dataset)
    print("Nodes: {} , Edges: {}".format(nodes, edges))
    print("Degree Mean: {}".format(mean))
    print("Degree Variance: {}".format(var))
    print("Degree Standard Deviation: {}".format(std))

    path = "../data/{}/degree_info.txt".format(args.dataset)
    with open(path, 'w') as f:
        f.write("{}\n".format(args.dataset))
        f.write("Nodes: {} | Edges: {}\n".format(nodes, edges))
        f.write("Degree Mean: {}\n".format(mean))
        f.write("Degree Variance: {}\n".format(var))
        f.write("Degree Standard Deviation: {}\n".format(std))
