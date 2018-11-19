#!/usr/bin/python3

import settings
import argparse

'''
    Function to properly convert a double into a string format
    i.e. 0.8000 = 0,8
'''
def properDoubleFormat(dbl):
    result = str(dbl)
    result = result.replace(".",",").rstrip("0").rstrip(",")
    return result


'''
    Function to generate the proper file path based on the file type
'''
def generatePath(args, type, size=0):
    path = ""
    if (type == "imm"):
        # data/nethept/imm/basic/NE_k50_e0,1_ss.txt
        path = "../data/{}/imm/basic/".format(args.dataset)
        path += "{}_k{}_e{}_ss.txt".format(settings.datasets[args.dataset], args.immK, properDoubleFormat(args.epsilon))
    elif (type == "rtim"):
        # data/nethept/rtim/live/NE_k50_r5_ap0,8_urr_v1_s15229_ss45.txt
        path = "../data/{}/rtim/live_{}/".format(args.dataset, settings.keywords[args.model])
        path += "{}_k{}_r{}_ap{}_{}_v{}_s{}_ss{}.txt".format(settings.datasets[args.dataset], args.rtimK, properDoubleFormat(args.reach), properDoubleFormat(args.activationProb), settings.keywords[args.model], args.version, args.streamSize, args.rtimSeedSize)
    elif (type == "stream"):
        # data/nethept/streams/urr/v1/NE_urr_v1_s15229_st.txt
        path = "../data/{}/streams/{}/v{}/".format(args.dataset, args.model, args.version)
        path += "{}_{}_v{}_s{}_st.txt".format(settings.datasets[args.dataset], settings.keywords[args.model], args.version, args.streamSize)
    elif (type == "imm_progress"):
        # data/nethept/imm/live/progress/NE_k50_e0,5_urr_v1_s15229_prg.txt
        path = "../data/{}/imm/live_{}/progress/".format(args.dataset, settings.keywords[args.model])
        path += "{}_k{}_e{}_{}_v{}_s{}_prg.txt".format(settings.datasets[args.dataset], args.immK, properDoubleFormat(args.epsilon), settings.keywords[args.model], args.version, args.streamSize)
    elif (type == "rtim_intersect"):
        # data/nethept/imm/rtim_common/NE_k50_e0,1_k50_r5_ap0,8_urr_v1_s15229_ss20.txt
        path = "../data/{}/imm/rtim_common_{}/".format(args.dataset, settings.keywords[args.model])
        path += "{}_k{}_e{}_k{}_r{}_ap{}_ss{}_{}_v{}_s{}_common{}.txt".format(settings.datasets[args.dataset], args.immK, properDoubleFormat(args.epsilon), args.rtimK, properDoubleFormat(args.reach), properDoubleFormat(args.activationProb), args.rtimSeedSize, settings.keywords[args.model], args.version, args.streamSize, size)
    elif (type == "stream_intersect"):
        # data/{}/live/NE_k50_e0,1_urr_v1_s15229_ss25.txt
        path = "../data/{}/imm/live_{}/".format(args.dataset, settings.keywords[args.model])
        path += "{}_k{}_e{}_{}_v{}_s{}_ss{}.txt".format(settings.datasets[args.dataset], args.immK, properDoubleFormat(args.epsilon), settings.keywords[args.model], args.version, args.streamSize, size)
    elif (type == "no_rtim"):
        path = "../data/{}/imm/live_{}/no_rtim/".format(args.dataset, settings.keywords[args.model])
        path += "{}_immK{}_e{}_rtimK{}_r{}_ap{}_{}_v{}_s{}_ss{}.txt".format(settings.datasets[args.dataset], args.immK, properDoubleFormat(args.epsilon), args.rtimK, properDoubleFormat(args.reach), properDoubleFormat(args.activationProb), settings.keywords[args.model], args.version, args.streamSize, args.rtimSeedSize)
    return path

'''
    Function to extract the number of nodes from attributes.txt
'''
def numberNodes(args):
    nodes = 0
    file_name = '../data/{0}/attributes.txt'.format(args.dataset)
    with open(file_name, 'r') as f:
        nodes = int(next(f).strip("n=").strip("\n"))
    print("Number of nodes: {}".format(nodes))
    return nodes

'''
    Function to import seed set from a file based on type and passed arguments
'''
def importSeedSet(args, type):
    path = ""
    if (type == "imm"):
        path = generatePath(args, type)
        print("Importing IMM seed set from: {}".format(path))
    elif (type == "rtim"):
        path = generatePath(args, type)
        print("Importing RTIM seed set from: {}".format(path))
    with open(path, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    print("{} seed set length: {}".format(type, len(content)))
    return content

'''
    Function to import stream from a file based on passed arguments
'''
def importStream(args):
    # data/nethept/streams/urr/v1/NE_urr_v1_s15229_st.txt
    path = generatePath(args, "stream")
    print("Importing stream from: {}".format(path))
    with open(path, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    print("Stream length: {}".format(len(content)))
    return content
