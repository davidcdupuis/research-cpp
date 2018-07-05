#!/usr/bin/python3
'''
    Based on model and dataset generate file(s) to use as online users for live
    process
'''

import argparse
import random
import csv

models = ['rand_repeat', 'rand_no_repeat', 'random_long']

def numberNodes(dataset):
    nodes = 0
    file_name = 'data/{0}/attributes.txt'.format(dataset)
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
        file_name = 'data/{0}/availability_models/rand_repeat/rand_repeat_m{2}/rand_repeat_{1}_m{2}.txt'.format(dataset, size, i)
        stream = [random.choice(range(nodes)) for _ in range(size)]
        with open(file_name, 'w', newline='') as f:
            writer = csv.writer(f)
            for val in stream:
                writer.writerow([val])
        print("> Saved rand_repeat data to {}".format(file_name))

def rand_no_repeat(dataset, nodes, size, num=1):
    for i in range(1, num + 1):
        file_name = 'data/{0}/availability_models/rand_no_repeat/rand_no_repeat_m{2}/rand_no_repeat_{1}_m{2}.txt'.format(dataset, size, i)
        stream = random.sample(range(nodes), size)
        with open(file_name, 'w', newline='') as f:
            writer = csv.writer(f)
            for val in stream:
                writer.writerow([val])
        print("> Saved rand_no_repeat data to {}".format(file_name))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Live Models generator")
    parser.add_argument('-m', '--model', default="random_basic",
                        help='What model to use')
    parser.add_argument('-d', '--dataset', required=True,
                        help='What data to generate model for.')
    parser.add_argument('-n', '--number', type=int,
                        help='How many random datasets you want')
    # parser.add_argument('-V', '--nodes', type=int, help='number of nodes')
    parser.add_argument('-s', '--stream', type=int, default=-1,
                        help='size of stream')
    args = parser.parse_args()

    print("Model [{}]".format(args.model))
    print("Dataset [{}]".format(args.dataset))

    # graph, _ = research_data.import_graph_data(args.dataset)
    # keys = graph.keys()
    nodes = numberNodes(args.dataset)
    if (args.stream == -1):
        args.stream = nodes
    print("Stream size: {}".format(args.stream))

    if args.model == "rand_repeat":
        rand_repeat(args.dataset, nodes, args.stream, args.number)
    elif args.model == "rand_no_repeat":
        rand_no_repeat(args.dataset, nodes, args.stream, args.number)
