#!/usr/bin/env python3
__author__ = 'muratov'
import os
import sys
import random
import generation

plotScript = """
set ylabel "Время, мс"
set xlabel "Количество потоков"
set logscale y
set xrange [1:20]
set xtics 1
set title "Зависимость времени работы алгоритма от количества потоков"
"""


def getNextColor():
    currentColor = "#000000"
    while True:
        yield currentColor
        currentColor = "#"
        for i in range(6):
            currentColor += hex(random.randint(0, 15))[2]


if __name__ == '__main__':
    if len(sys.argv) == 1:
        print("usage: params:arg1 arg2...argN\nwhere arg1,..argN nums for test and plot")
    else:
        colors = getNextColor()
        nums = [int(x) for x in sys.argv[1:]]
        plotScript += "plot "
        for i in range(len(nums)):
            generation.task1("plot{0}".format(nums[i]), list(range(1, 21)), [nums[i]])
            plotScript += "'plot{0}' using 1:3 with lines lw 3 lt rgb '{1}' title \"{2}\", ".format(nums[i], next(colors), nums[i])
            print()
        plotScript += "\npause -1\n"
        executeFile = "plotter.plt"
        with open(executeFile, "w") as file:
            file.write(plotScript)
        os.system("gnuplot {0}".format(executeFile))
        for line in nums:
            os.system("rm plot{0}".format(line))
        os.system("rm {0}".format(executeFile))
