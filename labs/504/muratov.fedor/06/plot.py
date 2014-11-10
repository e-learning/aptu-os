# /usr/bin/env python3
__author__ = 'muratov'

import os

if __name__ == '__main__':
    plotfile = open("plotfile", "w")
    inputfile = open("TASK1_DATA", "r")
    lines = inputfile.readlines()
    for lineNum in range(len(lines)):
        if lineNum % 3 == 2:
            plotfile.write(lines[lineNum])
    plotfile.close()
    os.system("gnuplot plot.plt")
    os.system("rm plotfile")