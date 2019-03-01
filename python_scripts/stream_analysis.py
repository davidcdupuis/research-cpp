import argparse
import random
import csv
import settings
import numpy as np
import tools


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--datasset')
    args = parser.parse_args()
    args.dataset = "test"
    version = "1"
    model = "urr"

    V = tools.numberNodes(args)
    # size = V * 10 / 100
    size = 20
    users = {}
    for x in xrange(V):
        users[x] = {"score":0,"freq":0}

    # 3. read stream and count freq (by incrementation)
    path = "../data/{0}/streams/{1}/v{2}/{3}_{1}_v{2}_s{4}_st.txt".format(args.dataset, model, version, settings.datasets[args.dataset], size)
    with open(path, 'r') as f:
        for line in f:
            user_id = int(line.strip())
            users[user_id]["freq"] += 1

    # 4. read inf. scores and append to users who have freq > 0
    path = "../data/{0}/rtim/pre_process/{1}_infS.txt".format(args.dataset, settings.datasets[args.dataset])
    with open(path, 'r') as f:
        for line in f:
            user_vals = line.rstrip("\n").split(" ")
            users[int(user_vals[0])]["score"] = float(user_vals[1])

    # 5. record information of users who have freq > 0
    for i in users:
        if users[i]["freq"] > 0:
            print("user: {} ({}, {})".format(i, users[i]["score"], users[i]["freq"]))

    path = "../data/{0}/{1}_stream_{2}_v{3}_info.csv".format(args.dataset, settings.datasets[args.dataset], model, version)
    with open(path, 'w') as f:
        writer = csv.writer(f)
        writer.writerow(["user_id","user_score","user_freq"])
        for i in users:
            if users[i]["freq"] > 0:
                writer.writerow([i, users[i]["score"],users[i]["freq"]])
        #user_id user_score user_frequency
