#!/usr/bin/python3
'''
    Script to compute intersection of two 'arrays' and save the result in a file
'''

import argparse
import random
import csv
import settings
import tools


def importStream():
    # data/nethept/streams/urr/v1/NE_urr_v1_s15229_st.txt
    path = tools.generatePath(args, "stream")
    print("Importing stream from: {}".format(path))
    with open(path, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    print("Stream length: {}".format(len(content)))
    return content


def saveImmStreamIntersect(intersect):
    # data/nethept/imm/live/NE_k50_e0,01_urr_v1_s15229_ss.txt
    path = tools.generatePath(args, "stream_intersect", len(intersect))
    with open(path, 'w') as f:
        writer = csv.writer(f)
        for val in intersect:
            writer.writerow([val])
    print("IMM ∩ Stream intersect saved successfully to {}".format(path))


def getStreamIntersection(stream, immSeed):
    initiateProgressLog()
    result = []
    seen = 0
    for i in stream:
        seen += 1
        for j in immSeed:
            if i == j:
                result.append(j)
                immSeed.remove(j)
                saveProgress( seen, j, len(result))
    print("(IMM ∩ Stream) size = {}".format(len(result)))
    return result


def getRTIMIntersection(immSeed, rtimSeed):
    intersect = list(set(immSeed) & set(rtimSeed))
    print("(IMM ∩ RTIM) size = {}".format(len(intersect)))
    return intersect


def getImmStreamNoRTIM(immStream, immRTIM):
    intersect = list(set(immStream) - set(immRTIM))
    print("(IMM ∩ Stream)\RTIM len = {}".format(len(intersect)))
    return intersect


def initiateProgressLog():
    # data/nethept/imm/live/progress/NE_k50_e0,5_urr_v1_s15229_prg.txt
    path = tools.generatePath(args, "imm_progress")
    with open(path, 'w') as f:
        writer = csv.writer(f)
        writer.writerow(['seen','user_index','seed_size'])
    print("Progress log initiated at {}:".format(path))


def saveProgress(seen, user_index, seed_size):
    # data/nethept/imm/live/progress/NE_k50_e0,5_urr_v1_s15229_prg.txt
    path = tools.generatePath(args, "imm_progress")
    with open(path, 'a') as f:
        writer = csv.writer(f)
        writer.writerow([seen, user_index, seed_size])


def saveRTIMIntersect(intersect):
    path = tools.generatePath(args, "rtim_intersect", len(intersect))
    with open(path, 'w') as f:
        writer = csv.writer(f)
        for val in intersect:
            writer.writerow([val])
    print("IMM ∩ RTIM saved successfully to {}".format(path))


def saveCommonLog(type, immSeed, otherArray, intersect):
    immFile = tools.generatePath(args, "imm")
    otherFile = ""
    commonFile = ""
    if (type == "rtim"):
        otherFile = tools.generatePath(args, "rtim")
        commonFile = tools.generatePath(args, "rtim_intersect", len(intersect))
    elif (type == "stream"):
        otherFile = tools.generatePath(args, "stream")
        commonFile = tools.generatePath(args, "stream_intersect", len(intersect))
    path = '../data/{0}/logs/intersect.log'.format(args.dataset)
    with open(path, 'a') as f:
        f.write("IMM seed set")
        f.write(" - file_name: {}".format(immFile))
        f.write(" - size: {}".format(len(immSeed)))
        if (type == "rtim"):
            f.write("RTIM seed set")
        elif (type == "stream"):
            f.write("Stream")
        f.write(" - file_name: {}".format(otherFile))
        f.write(" - size: {}".format(len(otherArray)))
        f.write("Intersection")
        f.write(" - file_name: {}".format(commonFile))
        f.write(" - size: {}".format(len(intersect)))
        f.write("----------------------------------------------------")
    print("Common log saved at: {}".format(path))


def saveIMMStreamNoRtimIntersect(intersect):
    path = tools.generatePath(args, "no_rtim")
    with open(path, 'w') as f:
        writer = csv.writer(f)
        for val in intersect:
            writer.writerow([val])
    print("(IMM ∩ Stream)\RTIM saved successfully to {}".format(path))


def saveBothLog(streamIntersectSize, rtimIntersectSize, noRTIMSize):
    path = "../data/{}/logs/intersect.log".format(args.dataset)
    with open(path, 'a') as f:
        f.write("IMM\n")
        f.write(" - epsilon = {}\n".format(args.epsilon))
        f.write(" - size = {}\n".format(args.immK))
        f.write("RTIM\n")
        f.write(" - reach = {}\n".format(args.reach))
        f.write(" - ap = {}\n".format(args.activationProb))
        f.write(" - size = {}\n".format(args.rtimSeedSize))
        f.write("STREAM\n")
        f.write(" - model = {}\n".format(args.model))
        f.write(" - version = {}\n".format(args.version))
        f.write(" - size = {}\n".format(args.streamSize))
        f.write("(IMM ∩ Stream) size = {}\n".format(streamIntersectSize))
        f.write("(IMM ∩ RTIM) size = {}\n".format(rtimIntersectSize))
        f.write("((IMM ∩ Stream) \ RTIM) size = {}\n".format(noRTIMSize))
        f.write("----------------------------------------------------\n")
    print("Common log saved at: {}".format(path))


def printArgs():
    print("Arguments")
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
        * -d        | --dataset         | [nethept, twitter, ...]
        * -t        | --type            | [stream, rtim, test]
        * -immK     | --immK            |
        * -rtimK    | --rtimK           |
        * -rtimS    | --rtimSeedSize    | 
        * -e        | --epsilon         | 
        * -m        | --model           |
        * -stream   | --streamSize      |
        * -v        | --version         |
        * -r        | --reach           |
        * -ap       | --activationProb  |
    '''

    parser = argparse.ArgumentParser(description="Intersect")

    # GENERAL ARGUMENTS
    parser.add_argument('-d', '--dataset', required=True, default="nethept",
                        help='What data to generate model for.')
    parser.add_argument('-t', '--type', default='stream')

    # IMM ARGUMENTS
    parser.add_argument('-immK', '--immK', type=int, default=50,
                        help='size of imm seed set')
    parser.add_argument('-e', '--epsilon', type=float, default=0.01)

    # STREAM ARGUMENTS
    parser.add_argument('-m', '--model', default="uniform_rand_repeat",
                        help='What model to use')
    parser.add_argument('-stream', '--streamSize', type=int, default=-1,
                        help='size of stream')
    parser.add_argument('-v', '--version', type=int, default=1)

    # RTIM ARGUMENTS
    parser.add_argument('-r', '--reach', type=int, default=1)
    parser.add_argument('-ap','--activationProb', type=float, default=0.8)
    parser.add_argument('-rtimK', '--rtimK', type=int,default=50)
    parser.add_argument('-rtimS', '--rtimSeedSize', type=int, default=50)

    args = parser.parse_args()

    if (args.type == "stream"):
        # import immSeedSet
        immSeed = tools.importSeedSet(args, "imm")
        # import stream
        stream = importStream()
        # find intersection
        intersect = getStreamIntersection(stream, immSeed)
        # save intersection
        saveImmStreamIntersect(intersect)
        # save log
        saveCommonLog("stream", immSeed, stream, intersect)
    elif (args.type == "rtim"):
        # import immSeedSet
        immSeed = tools.importSeedSet(args, "imm")
        # import rtimSeedSet
        rtimSeed = tools.importSeedSet(args, "rtim")
        # find intersection
        intersect = getRTIMIntersection(immSeed, rtimSeed)
        # save intersection
        saveRTIMIntersect(intersect)
        # save log
        saveCommonLog("rtim", immSeed, rtimSeed, intersect)
    elif (args.type == "both"):
        # compute both stream and rtim intersection together
        immSeed = tools.importSeedSet(args, "imm")
        stream = importStream()
        rtimSeed = tools.importSeedSet(args, "rtim")

        # Find IMM ∩ RTIM
        immRTIMIntersect = getRTIMIntersection(immSeed, rtimSeed)
        saveRTIMIntersect(immRTIMIntersect)

        # Find IMM ∩ Stream
        immStreamIntersect = getStreamIntersection(stream, immSeed)
        saveImmStreamIntersect(immStreamIntersect)

        # Find (IMM ∩ Stream)\RTIM = (IMM ∩ Stream) - (IMM ∩ RTIM)
        immStreamNoRtimIntersect = getImmStreamNoRTIM(immStreamIntersect, immRTIMIntersect)
        saveIMMStreamNoRtimIntersect(immStreamNoRtimIntersect)

        saveBothLog(len(immStreamIntersect), len(immRTIMIntersect), len(immStreamNoRtimIntersect))

    elif (args.type == "test"):
        print("∩")
