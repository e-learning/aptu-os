#!/usr/bin/env python3
__author__ = 'muratov'
import sys

import generation


description = """
optimum.py
params:
    -M: количество чисел для проверки эффективности
    -N: количество итераций для взятия среднего значения.
Exaple:
>python3 optimum.py 10000000
"""

if __name__ == '__main__':
    if len(sys.argv) == 1:
        print("usage arg:N")
    else:
        iterations = 50
        cores = generation.report(False)
        acceptable = [1, cores, cores + 1, cores * 2, cores * 2 + 1]
        averages = []
        for core in acceptable:
            sum_ = 0
            print("check {0} threads".format(core))
            for iteration in range(iterations + 1):
                sum_ += generation.computeSieve(sys.argv[1], core)
            averages.append(sum_ / iterations)
        good = 0
        for i in range(len(acceptable)):
            print("threads {0} - average {1}".format(acceptable[i], averages[i]))
            if averages[good] > averages[i]:
                good = i
        print("for N = {1} good tier is M = {0} ".format(acceptable[good], sys.argv[1]))