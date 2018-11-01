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

def numberNodes(dataset):
    nodes = 0
    file_name = 'data/{0}/attributes.txt'.format(dataset)
    with open(file_name, 'r') as f:
        nodes = int(next(f).strip("n=").strip("\n"))
    print("Number of nodes: {}".format(nodes))
    return nodes

def importIMMSeedSet(dataset, seedSize, epsilon):
    # data/nethept/imm/basic/NE_k50_e0,5_ss.txt
    file_name = 'data/{0}/imm/basic/{1}_k{2}_e{3}_ss.txt'.format(dataset, datasets[dataset], seedSize, properDoubleFormat(epsilon))
    print(file_name)
    with open(file_name, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    return content

def importRtimSeedSet(dataset, seedSize, reach, ap, streamSize, model, version):
    # data/nethept/rtim/live/NE_r5_ap0,8_urr_v1_s15229_ss.txt
    file_name = 'data/{0}/rtim/live/{1}_r{2}_ap{3}_{4}_v{5}_s{6}_ss.txt'.format(dataset, datasets[dataset], reach, properDoubleFormat(ap), keywords[model], version, streamSize)
    print(file_name)
    with open(file_name, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    return content

def importAvailabilityStream(dataset, model, streamSize, version):
    # data/nethept/streams/urr/v1/NE_urr_v1_s15229_st.txt
    file_name = 'data/{0}/streams/{1}/v{2}/{3}_{4}_v{2}_s{5}_st.txt'.format(dataset, model, version, datasets[dataset], keywords[model], streamSize)
    with open(file_name, 'r') as f:
        content = f.readlines()
    content = [x.strip() for x in content]
    return content

def saveImmStreamIntersect(intersect, dataset, seedSize, epsilon, model, streamSize, version):
    # data/nethept/imm/live/NE_k50_e0,01_urr_v1_s15229_ss.txt
    file_name = 'data/{0}/imm/live/{1}_k{2}_e{3}_{4}_v{5}_s{5}_ss.txt'.format(dataset, datasets[dataset], seedSize, properDoubleFormat(epsilon), keywords[model], version, streamSize)
    with open(file_name, 'w') as f:
        writer = csv.writer(f)
        for val in intersect:
            writer.writerow([val])
    print("intersect saved successfully to {}".format(file_name))

def getStreamIntersection(dataset, seedSize, epsilon, model, version, streamSize, stream, immSeed):
    initiateProgressLog(dataset, seedSize, epsilon, model, version, streamSize)
    result = []
    seen = 0
    for i in stream:
        seen += 1
        for j in immSeed:
            if i == j:
                result.append(j)
                immSeed.remove(j)
                saveProgress(dataset, seedSize, epsilon, model, version, streamSize, seen, j, len(result))
    return result

def getRTIMIntersection(immSeed, rtimSeed):
    return (list(set(immSeed) & set(rtimSeed)))

def initiateProgressLog(dataset, seedSize, epsilon, model, version, streamSize):
    # data/nethept/imm/live/progress/NE_k50_e0,5_urr_v1_s15229_prg.txt
    file_name = 'data/{0}/imm/live/progress/{1}_k{2}_e{3}_{4}_v{5}_s{6}_prg.txt'.format(dataset, datasets[dataset], seedSize, properDoubleFormat(epsilon), keywords[model], version, streamSize)
    with open(file_name, 'w') as f:
        writer = csv.writer(f)
        writer.writerow(['seen','user_index','seed_size'])
    print("Progress log initiated at {}:".format(file_name))

def saveProgress(dataset, seedSize, epsilon, model, version, streamSize, seen, user_index, seed_size):
    # data/nethept/imm/live/progress/NE_k50_e0,5_urr_v1_s15229_prg.txt
    file_name = 'data/{0}/imm/live/progress/{1}_k{2}_e{3}_{4}_v{5}_s{6}_prg.txt'.format(dataset, datasets[dataset], seedSize, properDoubleFormat(epsilon), keywords[model], version, streamSize)
    with open(file_name, 'a') as f:
        writer = csv.writer(f)
        writer.writerow([seen, user_index, seed_size])


def saveRTIMIntersect(dataset, seedSize, epsilon, reach, ap, model, version, streamSize, intersect):
    file_name = 'data/{0}/imm/rtim_common/{1}_k{2}_e{3}_r{4}_ap{5}_{6}_v{7}_s{8}_common.txt'.format(dataset, datasets[dataset], seedSize, properDoubleFormat(epsilon), reach, properDoubleFormat(ap), keywords[model], version, streamSize)
    with open(file_name, 'w') as f:
        writer = csv.writer(f)
        for val in intersect:
            writer.writerow([val])
    print("IMM-RTIM intersect saved successfully to {}".format(file_name))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Live Models generator")

    # GENERAL ARGUMENTS
    parser.add_argument('-d', '--dataset', required=True, default="nethept",
                        help='What data to generate model for.')
    parser.add_argument('-t', '--type', default='stream',)

    # IMM ARGUMENTS
    parser.add_argument('-k', '--seedSize', type=int, default=-1,
                        help='size of seed set')
    parser.add_argument('-e', '--epsilon', type=float, default=0.1)

    # STREAM ARGUMENTS
    parser.add_argument('-m', '--model', default="uniform_rand_repeat",
                        help='What model to use')
    parser.add_argument('-stream', '--streamSize', type=int, default=-1,
                        help='size of stream')
    parser.add_argument('-v', '--version', type=int, default=1)

    # RTIM ARGUMENTS
    parser.add_argument('-r', '--reach', type=int)
    parser.add_argument('-ap','--activationProb', type=float)

    args = parser.parse_args()

    if(args.type == 'stream'):
        immSeed = importIMMSeedSet(args.dataset, args.seedSize, args.epsilon)
        print("IMM seed set length: " + str(len(immSeed)))
        stream = importAvailabilityStream(args.dataset, args.model, args.streamSize, args.version)
        print("Original stream length: " + str(len(stream)))
        # find intersection of lists
        intersect = getStreamIntersection(args.dataset, args.seedSize, args.epsilon, args.model, args.version, args.streamSize, stream, immSeed)
        print("Intersect: " + str(len(intersect)))
        saveImmStreamIntersect(intersect, args.dataset, args.seedSize , args.epsilon , args.model, args.streamSize , args.version)
    elif(args.type == 'rtim'):
        immSeed = importIMMSeedSet(args.dataset, args.seedSize, args.epsilon)
        print("IMM seed set length: " + str(len(immSeedSet)))
        rtimSeed = importRtimSeedSet(args.dataset, args.seedSize, args.reach, args.activationProb, args.streamSize, args.model, args.version)
        print("RTIM seed set length: " + str(len(rtimSeedSet)))
        intersect = getRTIMIntersection(immSeed, rtimSeed)
        print("Intersection size: " + str(len(intersect)))
        saveRTIMIntersect(args.dataset, args.seedSize, args.epsilon, args.reach, args.activationProb, args.model, args.version, args.streamSize, intersect)
    elif (args.type == "test"):
        value = 0.5000
        print(properDoubleFormat(value))
