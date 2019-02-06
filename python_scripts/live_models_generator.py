#!/usr/bin/python3
'''
    Based on model and dataset generate file(s) to use as online users for live
    process
'''

import argparse
import random
import csv
import settings
import numpy
import tools
import math

models = ['rand_repeat', 'rand_no_repeat', 'random_long']


def numberNodes(dataset):
    nodes = 0
    file_name = '../data/{0}/attributes.txt'.format(dataset)
    with open(file_name, 'r') as f:
        nodes = int(next(f).strip("n=").strip("\n"))
    print("Number of nodes: {}".format(nodes))
    return nodes

def rand_repeat(dataset, nodes, size, num=1):
    '''
        - As many choices as there are users
        - Repetition is possible
    '''
    # initialize array of size 'nodes'
    for i in range(1, num + 1):
        file_name = '../data/{0}/streams/urr/v{1}/{2}_urr_v{1}_s{3}_st.txt'.format(dataset, i, settings.datasets[dataset] ,size)
        stream = [random.choice(range(nodes)) for _ in range(size)]
        with open(file_name, 'w', newline='') as f:
            writer = csv.writer(f)
            for val in stream:
                writer.writerow([val])
        print("> Saved rand_repeat data to {}".format(file_name))

def rand_no_repeat(dataset, nodes, size, num=1):
    for i in range(1, num + 1):
        file_name = '../data/{0}/streams/uniform_rand_no_repeat/v{1}/{2}_urnr_v{1}_s{3}_st.txt'.format(dataset, i, settings.datasets[dataset] ,size)
        stream = random.sample(range(nodes), size)
        with open(file_name, 'w', newline='') as f:
            writer = csv.writer(f)
            for val in stream:
                writer.writerow([val])
        print("> Saved rand_no_repeat data to {}".format(file_name))


def getDistribution(dataset, nodes):
    inDegrees = numpy.zeros((nodes,), dtype=int)
    outDegrees = numpy.zeros((nodes,), dtype=int)

    file_path = '../data/{0}/{0}_wc.inf'.format(args.dataset)
    print("Reading graph from: {}".format(file_path))
    with open(file_path, 'r') as f:
        for line in f:
            vals = line.rstrip("\n").split(" ")
            left = int(vals[0])
            right = int(vals[1])

            outDegrees[left] += 1
            inDegrees[right] += 1
            # print("({0}, out: {1}) | ({2}, in: {3})".format(left, outDegrees[left], right, inDegrees[right]))
    # print("in    : {}".format(inDegrees))
    # print("out   : {}".format(outDegrees))
    # for i in range(nodes):
    #     print(inDegrees[i], outDegrees[i])
    sumDegrees = inDegrees + outDegrees
    # print("sum   : {}".format(sumDegrees))
    total = numpy.sum(sumDegrees)
    # print("total = {}".format(total))
    distribution = 1.0 * sumDegrees / total
    # print("distib: {}".format(distribution))

    return distribution


def save_degrees():
    inDegrees = numpy.zeros((nodes,), dtype=int)
    outDegrees = numpy.zeros((nodes,), dtype=int)

    file_path = '../data/{0}/{0}_wc.inf'.format(args.dataset)
    print("Reading graph from: {}".format(file_path))
    with open(file_path, 'r') as f:
        for line in f:
            vals = line.rstrip("\n").split(" ")
            left = int(vals[0])
            right = int(vals[1])
            outDegrees[left] += 1
            inDegrees[right] += 1

    file_path = '../data/{0}/{0}_degrees.txt'.format(args.dataset)
    print("Saving degrees too: {}".format(file_path))
    with open(file_path, 'w') as f:
        writer = csv.writer(f, delimiter=' ')
        writer.writerow(['user', 'in', 'out'])
        for i in range(nodes):
            writer.writerow([i, inDegrees[i], outDegrees[i]])
    print("> Saved degrees to {}".format(file_path))


def inNOut_repeat(dataset, nodes, streamSize, distribution, version):
    file_name = '../data/{0}/streams/inoutr/v{3}/{1}_inoutr_v{3}_s{2}_st.txt'.format(dataset, settings.datasets[dataset], streamSize, version)
    choices = numpy.random.choice(numpy.arange(nodes,), streamSize, p=distribution)
    with open(file_name, 'w') as f:
        writer = csv.writer(f)
        for choice in choices:
            writer.writerow([choice])
    print("> Saved inNOut_repeat to {}".format(file_name))


def logInNOut_repeat(dataset, nodes, streamSize, distribution, version):
    file_name = '../data/{0}/streams/loginoutr/v{3}/{1}_loginoutr_v{3}_s{2}_st.txt'.format(dataset, settings.datasets[dataset], streamSize, version)
    choices = numpy.random.choice(numpy.arange(nodes,), streamSize, p=distribution)
    with open(file_name, 'w') as f:
        writer = csv.writer(f)
        for choice in choices:
            writer.writerow([choice])
    print("> Saved logInNOut_repeat to {}".format(file_name))


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Live Models generator")
    parser.add_argument('-m', '--model', default="uniform_rand_repeat",
                        help='What model to use')
    parser.add_argument('-d', '--dataset', required=True,
                        help='What data to generate model for.')
    parser.add_argument('-n', '--number', type=int,
                        help='How many random datasets you want')
    # parser.add_argument('-V', '--nodes', type=int, help='number of nodes')
    parser.add_argument('-s', '--stream', type=int, default=-1,
                        help='size of stream')
    parser.add_argument('-v', '--version', type=int, default=-1, help='version')
    args = parser.parse_args()

    print("Model [{}]".format(args.model))
    print("Dataset [{}]".format(args.dataset))

    # graph, _ = research_data.import_graph_data(args.dataset)
    # keys = graph.keys()
    nodes = tools.numberNodes(args)

    if args.model == "uniform_rand_repeat":
        if (args.stream == -1):
            args.stream = nodes
        print("Stream size: {}".format(args.stream))
        rand_repeat(args.dataset, nodes, args.stream, args.number)
    elif args.model == "uniform_rand_no_repeat":
        if (args.stream == -1):
            args.stream = nodes
        print("Stream size: {}".format(args.stream))
        rand_no_repeat(args.dataset, nodes, args.stream, args.number)
    elif args.model == "inNOut_repeat":
        if (args.stream == -1):
            args.stream = int(nodes / 10)
        # print("Stream size: {}".format(args.stream))
        # print(numpy.arange(1,5))
        # choices = numpy.random.choice(numpy.arange(1,5), 10, p=[0.2,0.3,0.3,0.2])
        # print(choices)
        distribution = getDistribution(args.dataset, nodes)
        #print(distribution)
        inNOut_repeat(args.dataset, nodes, args.stream, distribution, args.version)
    elif args.model == "log":
        if (args.stream == -1):
            args.stream = int(nodes / 10)
        distribution = getDistribution(args.dataset, nodes)
        #distribution = numpy.log(distribution);
        print(distribution)
        # logInNOut_repeat(args.dataset, nodes, args.stream, distribution, args.version)
    elif args.model == "degrees":
        save_degrees()
