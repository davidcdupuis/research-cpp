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



if __name__ == "__main__":
    '''
        * -d        | --dataset         | dataset
        * -ss       | --seedSize        | original seed set size
        * -maxK     | --maxK            | original k
        * -k        | --k               | desired seed set size
        * -r        | --reach           |
        * -ap       | --activationProb  |
        * -m        | --model           |
        * -stream   | --streamSize      |
        * -v        | --version         |
    '''
    parser = argparse.ArgumentParser(description="Seed Set Extractor")
    parser.add_argument("-d","--dataset",default="test")
    parser.add_argument("-ss","--seedSize", type=int, default=800)
    parser.add_argument("")
    parser.add_argument("-k", "--k", type=int, default=50)
    parser.add_argument("")
    parser.add_argument("")
    parser.add_argument("")
    parser.add_argument("")
    parser.add_argument("")

    args = parser.parse_args()

    seedSet = tools.importSeedSet(args, "rtim")
    for i in range(args.k + 1):
        print(seedSet[i])
