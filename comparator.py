#!/usr/bin/python3
'''
    Script to compare the influence difference between two files
'''
import argparse
import csv

def compare(dataset, file1, file2):
    '''
        Open both files together, compare each line and save to f1_f2_comparison.txt
        Print number of values that are greater by a score of 5 and those smalle
    '''
    folder = 'data/{}/rtim/'.format(dataset)
    t1 = file1.rstrip('.txt')
    t2 = file2.rstrip('.txt')
    file3 = folder + '{}_{}_comparison.txt'.format(t1, t2)
    file1 = folder + file1
    file2 = folder + file2
    greater = 0
    lower = 0
    with open(file1, 'r') as f1, open(file2, 'r') as f2,  open(file3, 'w') as f3:
        f3.write('{} - {}\n'.format(t1, t2))
        print('{} - {}'.format(t1, t2))
        for line1, line2 in zip(f1, f2):
            line1 = line1.strip('\n').split(' ')
            line2 = line2.strip('\n').split(' ')
            if (line1[0] != line2[0]):
                print("Error, user index not same:{} | {}".format(line1[0], line2[0]))
            diff = round(float(line1[1]) - float(line2[1]), 4)
            if (diff > 0.5):
                f3.write('{} +{}\n'.format(line1[0], diff))
                greater += 1
                # print('{} +{}'.format(line1[0], diff))
            elif (diff < -0.5):
                f3.write('{} -{}\n'.format(line1[0], diff))
                lower += 1
                # print('{} -{}'.format(line1[0], diff))
            else:
                f3.write('{} {}\n'.format(line1[0], diff))
                # print('{} {}'.format(line1[0], 0))
    print("Diff > 0.5: {}".format(greater))
    print("Diff < -0.5: {}".format(lower))


    pass

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Comparator: compares two inf score files")
    parser.add_argument('-d','--dataset',help='name of dataset')
    parser.add_argument('-f1', help='first file')
    parser.add_argument('-f2', help='Second file')
    args = parser.parse_args()

    print("-------------Arguments------------")
    print("Dataset: {}".format(args.dataset))
    print("First file: {}".format(args.f1))
    print("Second file: {}".format(args.f2))
    print("----------------------------------")
    compare(args.dataset, args.f1, args.f2)
