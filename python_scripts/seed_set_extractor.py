#!/usr/bin/python3

'''
    Python script which extracts a seed set of size k from a larger seed set
    and saves it as a seed set file
'''

import argparse
import csv
import settings
import tools

def printArgs():
    print("Seed set arguments")
    print(" dataset: {}".format(args.dataset))
    print("IMM: ")
    print(" * k: {}".format(args.immK))
    print(" * epsilon: {}".format(args.epsilon))
    print(" * file: {}".format(generatePath("imm")))
    print("RTIM: ")
    print(" * rtimK: {}".format(args.rtimK))
    print(" * reach: {}".format(args.reach))
    print(" * ap: {}".format(args.activationProb))
    print(" * rtimSeedSize: {}".format(args.rtimSeedSize))
    print(" * file: {}".format(generatePath("rtim")))
    print("STREAM: ")
    print(" * model: {}".format(args.model))
    print(" * version: {}".format(args.version))
    print(" * streamSize: {}".format(args.streamSize))
    print(" * file: {}".format(generatePath("stream")))
    print("RTIM INTERSECT: {}".format(generatePath("rtim_intersect")))
    print("STREAM INTERSECT: {}".format(generatePath("stream_intersect")))


def extractSeedSet(args):
    seedSet = tools.importSeedSet(args, "rtim")
    path = "../data/{}/rtim/live/".format(args.dataset)
    path += "{}_k{}_r{}_ap{}_{}_v{}_s{}_ss{}.txt".format(settings.datasets[args.dataset], args.k, tools.properDoubleFormat(args.reach), tools.properDoubleFormat(args.activationProb), settings.keywords[args.model], args.version, args.streamSize, args.k)
    with open(path, 'w', newline='') as f:
        writer = csv.writer(f)
        for i in range(args.k):
            writer.writerow([seedSet[i]])
    print("Saved seed set of size {} to {}".format(args.k, path))


if __name__ == "__main__":
    '''
        * -d        | --dataset         | dataset
        * -ss       | --seedSize        | original seed set size
        * -origK    | --origK            | original k
        * -k        | --k               | desired seed set size
        * -r        | --reach           |
        * -ap       | --activationProb  |
        * -m        | --model           |
        * -stream   | --streamSize      |
        * -v        | --version         |
    '''
    parser = argparse.ArgumentParser(description="Seed Set Extractor")
    parser.add_argument("-d","--dataset",default="test")
    parser.add_argument("-rtimK", "--rtimK", type=int, default=800)
    parser.add_argument("-ss","--rtimSeedSize", type=int, default=800)
    parser.add_argument("-r", "--reach", type=float, default=1)
    parser.add_argument("-ap", "--activationProb", type=float, default=0.8)
    parser.add_argument("-m", "--model", default="uniform_rand_repeat")
    parser.add_argument("-v", "--version", type=int, default=1)
    parser.add_argument("-stream", "--streamSize")
    parser.add_argument("-k", "--k", type=int, default=50)

    args = parser.parse_args()

    extractSeedSet(args)
