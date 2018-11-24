#!/usr/bin/python3
# -*- coding: utf-8 -*-

import argparse
import csv
import settings
import tools


def extractProgress(path):
    with open("temp.txt", 'w') as tmp:
        with open(path, 'r') as f:
            f.readline()
            for line in f:
                vals = line.rstrip("\n").rstrip("\r").split(",")
                if (int(vals[2])%100 == 0 ):
                    tmp.write(vals[0])
                    # tmp.write(" ")
                    # tmp.write(vals[2])
                    tmp.write("\n")
    # content = [x.strip() for x in content]




if __name__ == "__main__":
    '''
        * -d        | --dataset         | dataset
        * -ss       | --seedSize        | original seed set size
        * -origK    | --origK           | original k
        * -k        | --k               | desired seed set size
        * -r        | --reach           |
        * -ap       | --activationProb  |
        * -m        | --model           |
        * -stream   | --streamSize      |
        * -v        | --version         |
    '''
    parser = argparse.ArgumentParser(description="Progress extractor")
    parser.add_argument("-f","--filePath")

    args = parser.parse_args()

    extractProgress("../data/" + args.filePath)
