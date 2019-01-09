#!/usr/bin/python3

import argparse
import random
import csv
import settings
import numpy as np
import tools

def saveInDegrees(dataset, nodes, inDeg, outDeg):
    file_path = "../data/{0}/{1}_degrees.txt".format(dataset, settings.datasets[dataset])
    with open(file_path, 'w') as f:
        writer = csv.writer(f, delimiter=' ')
        writer.writerow(['user', 'in', 'out'])
        for i in range(0, nodes):
            writer.writerow([i, outDeg[i], inDeg[i]])
    print("Saved influence degrees to: {}".format(file_path))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Live Models generator")
    parser.add_argument('-d', '--dataset', required=True,
                        help='What data to generate model for.')
    args = parser.parse_args()

    nodes = tools.numberNodes(args)

    inDegrees = np.zeros((nodes,), dtype=int)
    outDegrees = np.zeros((nodes,), dtype=int)
    # sumDegrees = np.zeros((nodes,), dtype=int)

    # print("in    : {}".format(inDegrees))
    # print("out   : {}".format(outDegrees))

    file_path = '../data/{0}/{0}_wc.inf'.format(args.dataset)
    i = 0
    with open(file_path, 'r') as f:
        for line in f:
            vals = line.rstrip("\n").split(" ")
            left = int(vals[0])
            right = int(vals[1])
            outDegrees[left] += 1
            inDegrees[right] += 1
    # print("in    : {}".format(inDegrees))
    # print("out   : {}".format(outDegrees))
    # sumDegrees = inDegrees + outDegrees
    # print("sum   : {}".format(sumDegrees))
    # total = np.sum(sumDegrees)
    # print("total = {}".format(total))
    # distribution = 1.0 * sumDegrees / total
    # print("distib: {}".format(distribution))
    saveInDegrees(args.dataset, nodes, inDegrees, outDegrees)
