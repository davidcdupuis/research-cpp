#!/usr/bin/python3
'''
    Script to compute intersection of two 'arrays' and save the result in a file
'''

import argparse
import random
import csv

def numberNodes(dataset):
    nodes = 0
    file_name = 'data/{0}/attributes.txt'.format(dataset)
    with open(file_name, 'r') as f:
        nodes = int(next(f).strip("n=").strip("\n"))
    print("Number of nodes: {}".format(nodes))
    return nodes

def importSeedSet(dataset, seedSize, epsilon):
    file_name = 'data/{0}/imm/{0}_k{1}e{2}.txt'.format(dataset, seedSize, epsilon)
    print(file_name)
    with open(file_name, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    return content

def importAvailabilityStream(dataset, model, streamSize, version):
    file_name = 'data/{0}/availability_models/{1}/{1}_m{3}/{1}_{2}_m{3}.txt'.format(dataset, model, streamSize, version)
    with open(file_name, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    return content

def saveIntersect(intersect, dataset, seedSize, epsilon, model, streamSize, version):
    file_name = 'data/{0}/imm/Sk{1}e{2}_A{3}{4}m{5}.txt'.format(dataset, seedSize, epsilon, model, streamSize, version)
    with open(file_name, 'w') as f:
        writer = csv.writer(f)
        for val in intersect:
            writer.writerow([val])
    print("intersect saved successfully to {}".format(file_name))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Live Models generator")

    # GENERAL ARGUMENTS
    parser.add_argument('-d', '--dataset', required=True, default="nethept",
                        help='What data to generate model for.')

    # IMM ARGUMENTS
    parser.add_argument('-seed', '--seedSize', type=int, default=-1,
                        help='size of seed set')
    parser.add_argument('-e', '--epsilon', type=float, default=0.1)

    # STREAM ARGUMENTS
    parser.add_argument('-m', '--model', default="rand_repeat",
                        help='What model to use')
    parser.add_argument('-stream', '--streamSize', type=int, default=-1,
                        help='size of stream')
    parser.add_argument('-v', '--version', type=int, default=1)
    args = parser.parse_args()

    seedSet = importSeedSet(args.dataset, args.seedSize, args.epsilon)
    print("Seed set length: " + str(len(seedSet)))
    stream = importAvailabilityStream(args.dataset, args.model, args.streamSize, args.version)
    print("Original stream length: " + str(len(stream)))
    # remove duplicates using a set
    stream = list(set(stream))
    print("Stream without duplicates: " + str(len(stream)))
    # find intersection of lists
    intersect = list(set(seedSet) - set(stream))
    print("Stream without duplicates: " + str(len(intersect)))

    saveIntersect(intersect, args.dataset, args.seedSize , args.epsilon , args.model, args.streamSize , args.version)
