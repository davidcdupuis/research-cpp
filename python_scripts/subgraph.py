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

    degrees = degrees[degrees[:,1].argsort()][::-1]
    # print(degrees[:10:])

    # EXTRACT TOP 25, 50 and 75 %
    percs = [25, 50, 75]
    limits = [0,0,0]
    print("Original attributes: ({}, {})".format(nodes, edges))
    for i in range(len(percs)):
        limits[i] = int(percs[i] * nodes/100)
    print(limits)
    # use set instead of array
    top_25 = set()
    top_50 = set()
    top_75 = set()
    for i in range(nodes):
        if( i < limits[0]):
            top_25.add(degrees[i][0])
        if (i < limits[1]):
            top_50.add(degrees[i][0])
        if (i < limits[2]):
            top_75.add(degrees[i][0])

    # GENERATE NODE KEYS
    keys_25 = {}
    keys_50 = {}
    keys_75 = {}
    top_25_sorted = sorted(top_25)
    top_50_sorted = sorted(top_50)
    top_75_sorted = sorted(top_75)
    for i in range(len(top_25_sorted)):
        keys_25[top_25_sorted[i]] = i
    for i in range(len(top_50_sorted)):
        keys_50[top_50_sorted[i]] = i
    for i in range(len(top_75_sorted)):
        keys_75[top_75_sorted[i]] = i

    print(keys_50[0])
    # SAVE SUBGRAPHS
    # READ EDGES AND SAVE TO FILE ONLY EDGES WHERE BOTH USERS BELONG A TOP %
    new_edges = [0,0,0]
    f1 = open("../data/{0}/osim/{0}_wc_top_25.inf".format(args.dataset), 'w')
    f2 = open("../data/{0}/osim/{0}_wc_top_50.inf".format(args.dataset), 'w')
    f3 = open("../data/{0}/osim/{0}_wc_top_75.inf".format(args.dataset), 'w')
    with open(file_path, 'r') as f:
        for line in f:
            vals = line.rstrip("\n").split(" ")
            left = int(vals[0])
            right = int(vals[1])
            if(left in top_25 and right in top_25):
                # save edge to file
                # f1.write(line)
                f1.write(str(keys_25[left]) + " " + str(keys_25[right]) + " " + vals[2] + "\n")
                new_edges[0] += 1
            if(left in top_50 and right in top_50):
                # f2.write(line)
                f2.write(str(keys_50[left]) + " " + str(keys_50[right]) + " " + vals[2] + "\n")
                new_edges[1] += 1
            if(left in top_75 and right in top_75):
                # f3.write(line)
                f3.write(str(keys_75[left]) + " " + str(keys_75[right]) + " " + vals[2] + "\n")
                new_edges[2] += 1
    f1.close()
    f2.close()
    f3.close()

    # SAVE SUBGRAPH ATTRIBUTES
    # 25%
    f1 = open("../data/{}/osim/attributes_top_25.inf".format(args.dataset),'w')
    f1.write("n={}\n".format(limits[0]))
    f1.write("m={}\n".format(new_edges[0]))
    f1.close()
    print("New attributes (25%): ({}, {})".format(limits[0], new_edges[0]))

    # 50%
    f2 = open("../data/{}/osim/attributes_top_50.inf".format(args.dataset),'w')
    f2.write("n={}\n".format(limits[1]))
    f2.write("m={}\n".format(new_edges[1]))
    f2.close()
    print("New attributes (50%): ({}, {})".format(limits[1], new_edges[1]))

    # 75%
    f3 = open("../data/{}/osim/attributes_top_75.inf".format(args.dataset),'w')
    f3.write("n={}\n".format(limits[2]))
    f3.write("m={}\n".format(new_edges[2]))
    f3.close()
    print("New attributes (75%): ({}, {})".format(limits[2], new_edges[2]))

    # SAVE SUBGRAPH NODE KEYS
    # 25%
    f1 = open("../data/{0}/osim/{0}_top_25_keys.txt".format(args.dataset),"w")
    for i in range(len(top_25_sorted)):
        f1.write(str(top_25_sorted[i]) + " " + str(i) + "\n")
    f1.close()

    # 50%
    f2 = open("../data/{0}/osim/{0}_top_50_keys.txt".format(args.dataset),"w")
    for i in range(len(top_50_sorted)):
        f2.write(str(top_50_sorted[i]) + " " + str(i) + "\n")
    f2.close()

    # 75%
    f3 = open("../data/{0}/osim/{0}_top_75_keys.txt".format(args.dataset),"w")
    for i in range(len(top_75_sorted)):
        f3.write(str(top_75_sorted[i]) + " " + str(i) + "\n")
    f3.close()
