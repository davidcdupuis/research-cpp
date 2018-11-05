#!/usr/bin/python3
'''
    Script to compute intersection of two 'arrays' and save the result in a file
'''

''' !!!! CHANGE ALL FILE PATHS !!!! '''

import argparse
import random
import csv

datasets = {
'test' : 'TS',
'nethept' : 'NE',
'dblp' : 'DB',
'livejournal' : 'LJ',
'orkut' : 'OR',
'twitter' : 'TW',
'youtube' : 'YO'
}

keywords = {
    'uniform_rand_repeat' : 'urr',
    'uniform_rand_no_repeat' : 'urnr'
}

def properDoubleFormat(dbl):
    result = str(dbl)
    result = result.replace(".",",")
    return result


def generatePath(type, size=0):
    path = ""
    if (type == "imm"):
        # data/nethept/imm/basic/NE_k50_e0,1_ss.txt
        path = "data/{}/imm/basic/".format(args.dataset)
        path += "{}_k{}_e{}_ss.txt".format(datasets[args.dataset], args.immK, properDoubleFormat(args.epsilon))
    elif (type == "rtim"):
        # data/nethept/rtim/live/NE_k50_r5_ap0,8_urr_v1_s15229_ss45.txt
        path = "data/{}/rtim/live/".format(args.dataset)
        path += "{}_k{}_r{}_ap{}_{}_v{}_s{}_ss{}.txt".format(datasets[args.dataset], args.rtimK, args.reach, properDoubleFormat(args.activationProb), keywords[args.model], args.version, args.streamSize, args.rtimSeedSize)
    elif (type == "stream"):
        # data/nethept/streams/urr/v1/NE_urr_v1_s15229_st.txt
        path = "data/{}/streams/{}/v{}/".format(args.dataset, args.model, args.version)
        path += "{}_{}_v{}_s{}_st.txt".format(datasets[args.dataset], keywords[args.model], args.version, args.streamSize)
    elif (type == "imm_progress"):
        # data/nethept/imm/live/progress/NE_k50_e0,5_urr_v1_s15229_prg.txt
        path = "data/{}/imm/live/progress/".format(args.dataset)
        path += "{}_k{}_e{}_{}_v{}_s{}_prg.txt".format(datasets[args.dataset], args.immK, properDoubleFormat(args.epsilon), keywords[args.model], args.version, args.streamSize)
    elif (type == "rtim_intersect"):
        # data/nethept/imm/rtim_common/NE_k50_e0,1_k50_r5_ap0,8_urr_v1_s15229_ss20.txt
        path = "data/{}/imm/rtim_common/".format(args.dataset)
        path += "{}_k{}_e{}_k{}_r{}_ap{}_ss{}_{}_v{}_s{}_common{}.txt".format(datasets[args.dataset], args.immK, properDoubleFormat(args.epsilon), args.rtimK, args.reach, properDoubleFormat(args.activationProb), args.rtimSeedSize, keywords[args.model], args.version, args.streamSize, size)
    elif (type == "stream_intersect"):
        # data/{}/live/NE_k50_e0,1_urr_v1_s15229_ss25.txt
        path = "data/{}/live/".format(args.dataset)
        path += "{}_k{}_e{}_{}_v{}_s{}_ss{}.txt".format(datasets[args.dataset], args.immK, properDoubleFormat(args.epsilon), keywords[args.model], args.version, args.streamSize, size)
    return path


def numberNodes():
    nodes = 0
    file_name = 'data/{0}/attributes.txt'.format(args.dataset)
    with open(file_name, 'r') as f:
        nodes = int(next(f).strip("n=").strip("\n"))
    print("Number of nodes: {}".format(nodes))
    return nodes


def importSeedSet(type):
    path = ""
    if (type == "imm"):
        path = generatePath(type)
        print("Importing IMM seed set from: {}".format(path))
    elif (type == "rtim"):
        path = generatePath(type)
        print("Importing RTIM seed set from: {}".format(path))
    with open(path, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    print("{} seed set length: {}".format(type, len(content)))
    return content


def importStream():
    # data/nethept/streams/urr/v1/NE_urr_v1_s15229_st.txt
    path = generatePath("stream")
    print("Importing stream from: {}".format(path))
    with open(path, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    print("Stream length: {}".format(len(content)))
    return content


def saveImmStreamIntersect(intersect):
    # data/nethept/imm/live/NE_k50_e0,01_urr_v1_s15229_ss.txt
    path = generatePath("stream_intersect", len(intersect))
    with open(path, 'w') as f:
        writer = csv.writer(f)
        for val in intersect:
            writer.writerow([val])
    print("IMM-Stream intersect saved successfully to {}".format(path))


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
    print("Intersect size: {}".format(len(intersect)))
    return result


def getRTIMIntersection(immSeed, rtimSeed):
    intersect = list(set(immSeed) & set(rtimSeed))
    print("Intersection size: ".format(len(intersect)))
    return intersect


def getImmStreamNoRTIM(immSeed, stream, rtimSeed):
    return list((set(immSeed) & set(stream)) - set(rtimSeed))


def initiateProgressLog():
    # data/nethept/imm/live/progress/NE_k50_e0,5_urr_v1_s15229_prg.txt
    path = generatePath("imm_progress")
    with open(path, 'w') as f:
        writer = csv.writer(f)
        writer.writerow(['seen','user_index','seed_size'])
    print("Progress log initiated at {}:".format(path))


def saveProgress(seen, user_index, seed_size):
    # data/nethept/imm/live/progress/NE_k50_e0,5_urr_v1_s15229_prg.txt
    path = generatePath("imm_progress")
    with open(path, 'a') as f:
        writer = csv.writer(f)
        writer.writerow([seen, user_index, seed_size])


def saveRTIMIntersect(intersect):
    path = generatePath("rtim_intersect", len(intersect))
    with open(path, 'w') as f:
        writer = csv.writer(f)
        for val in intersect:
            writer.writerow([val])
    print("IMM-RTIM intersect saved successfully to {}".format(path))


def saveCommonLog(type, immSeed, otherArray, intersect):
    immFile = generatePath("imm")
    otherFile = ""
    commonFile = ""
    if (type == "rtim"):
        otherFile = generatePath("rtim")
        commonFile = generatePath("rtim_intersect", len(intersect))
    elif (type == "stream"):
        otherFile = generatePath("stream")
        commonFile = generatePath("stream_intersect", len(intersect))
    path = 'data/{0}/logs/intersect.log'.format(args.dataset)
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


def saveBothLog(streamIntersectSize, rtimIntersectSize, noRTIMSize):
    path = "data/{}/logs/intersect.log".format(args.dataset)
    with open(path, 'a') as f:
        f.write("IMM")
        f.write(" - epsilon = {}".format(args.epsilon))
        f.write(" - size = {}".format(args.immK))
        f.write("RTIM")
        f.write(" - reach = {}".format(args.reach))
        f.write(" - ap = {}".format(args.activationProb))
        f.write(" - size = {}".format(args.rtimSeedSize))
        f.write("STREAM")
        f.write(" - model = {}".format(args.model))
        f.write(" - version = {}".format(args.version))
        f.write(" - size = {}".format(args.streamSize))
        f.write("(IMM ∩ Stream) size = {}".format(streamIntersectSize))
        f.write("(IMM ∩ RTIM) size = {}".format(rtimIntersectSize))
        f.write("((IMM ∩ Stream) \ RTIM) size = {}".format(noRTIMSize))
        f.write("----------------------------------------------------")
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

    parser = argparse.ArgumentParser(description="Live Models generator")

    # GENERAL ARGUMENTS
    parser.add_argument('-d', '--dataset', required=True, default="nethept",
                        help='What data to generate model for.')
    parser.add_argument('-t', '--type', default='stream')

    # IMM ARGUMENTS
    parser.add_argument('-immK', '--immK', type=int, default=50,
                        help='size of imm seed set')
    parser.add_argument('-e', '--epsilon', type=float, default=0.5)

    # STREAM ARGUMENTS
    parser.add_argument('-m', '--model', default="uniform_rand_repeat",
                        help='What model to use')
    parser.add_argument('-stream', '--streamSize', type=int, default=-1,
                        help='size of stream')
    parser.add_argument('-v', '--version', type=int, default=1)

    # RTIM ARGUMENTS
    parser.add_argument('-r', '--reach', type=int, default=5)
    parser.add_argument('-ap','--activationProb', type=float, default=0.8)
    parser.add_argument('-rtimK', '--rtimK', type=int,default=50)
    parser.add_argument('-rtimS', '--rtimSeedSize', type=int, default=50)

    args = parser.parse_args()

    if (args.type == "stream"):
        # import immSeedSet
        immSeed = importSeedSet("imm")
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
        immSeed = importSeedSet("imm")
        # import rtimSeedSet
        rtimSeed = importSeedSet("rtim")
        # find intersection
        intersect = getRTIMIntersection(immSeed, rtimSeed)
        # save intersection
        saveRTIMIntersect(intersect)
        # save log
        saveCommonLog("rtim", immSeed, rtimSeed, intersect)
    elif (args.type == "both"):
        # compute both stream and rtim intersection together
        immSeed = importSeedSet("imm")
        stream = importStream()
        rtimSeed = importSeedSet("rtim")
        # find stream intersection and save
        # immStreamIntersect = getStreamIntersection(stream, immSeed)
        # saveImmStreamIntersect(immStreamIntersect)
        # find rtim intersection and save
        # immRTIMIntersect = getRTIMIntersection(immSeed, rtimSeed)
        # saveRTIMIntersect(immRTIMIntersect)
        # find users in IMM-Stream intersection but not in RTIM
        res = getImmStreamNoRTIM(immSeed, stream, rtimSeed)
        # saveBothLog(len(immStreamIntersect), len(immRTIMIntersect), len(res))
        print((res, len(res)))
    elif (args.type == "test"):
        print("∩")
