#!/usr/bin/python3

import argparse
import csv
import settings
import tools

dataset = "twitter"
scores = []
# import influence scores of dataset 
file_path = "../data/{0}/rtim/pre_process/{1}_infS.txt".format(dataset,settings.datasets[dataset])
with open(file_path, "r") as f:
    for line in f:
        vals = line.rstrip("\n").split(" ")
        scores.append(float(vals[1]))

# sort influence scores
scores.sort(reverse=True)

# save the first 500,000 top scores
print(scores[:50:])
file_path = "../data/{0}/{1}_sortedScores.txt".format(dataset, settings.datasets[dataset])
i = 0
with open(file_path, "w") as f:
    writer = csv.writer(f)
    for score in scores:
        writer.writerow([score])
        if(score < 2):
            break
# plot distribution (optional)
