# -*- coding: utf-8 -*-
#!/usr/bin/python

import argparse
import random
import csv
import settings
import numpy as np
import tools

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Live Models generator")
    parser.add_argument('-d', '--dataset', required=True,
                        help='What data to generate model for.')
    args = parser.parse_args()

    nodes = tools.numberNodes(args)
    edges = 0

    # COUNT DEGREES AND SORT RESULTS
    degrees = np.zeros(shape=(nodes,2),dtype=int)

    file_path = '../data/{0}/{0}_wc.inf'.format(args.dataset)
    print("Reading graph from: {}".format(file_path))
    with open(file_path, 'r') as f:
        for line in f:
            vals = line.rstrip("\n").split(" ")
            left = int(vals[0])
            right = int(vals[1])
            if (degrees[left][0] == 0):
                degrees[left][0] = left
            if (degrees[right][0] == 0):
                degrees[right][0] = right
            degrees[left][1] += 1
            degrees[right][1] += 1
            edges += 1

    degrees = degrees[degrees[:,1].argsort()][::-1] # ?
    limits = [50, 100, 200, 500, 1000, 1353, 2250, 3204, 4290]
    #degrees[:limits[0]+1]

    for lim in limits:
        seedSet = degrees[:lim+1]
        print(seedSet[:10])
        print("------")
        # save new seed set to file
        file_path = "../data/{0}/osim/topConnected/{0}_wc_top_k{1}_ss.txt".format(args.dataset,lim)
        with open(file_path, 'w') as f:
            for seed in seedSet:
                f.write(str(seed[0]) + "\n")
    # print(degrees[:10])
    # print(degrees[:10:])

    # loop through degrees and add users
