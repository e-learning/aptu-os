#!/usr/bin/env python3
__author__ = 'muratov'

import os
import datetime

list_N = [10 ** 2, 10 ** 5, 2 * 10 ** 7]
file_task1 = "TASK1_DATA"
max_thread = 21
def task1():
    """
    TASK1_DATA
    """
    file = open(file_task1, "w")
    for quantityTHreads in range(1, max_thread):
        for n in list_N:
            startTime = datetime.datetime.now()
            if quantityTHreads == 1:
                os.system("./sieve_unith" + " " + str(n))
            else:
                os.system("./sieve_multith" + " " + str(n) + " " + str(quantityTHreads))
            end = datetime.datetime.now()
            elapsed = end - startTime
            elapsedMS = int(elapsed.total_seconds() * 1000)
            file.write(str(quantityTHreads) + "; " + str(n) + "; " + str(elapsedMS) + "\n")
            # print("\tlasted {0} {1}".format(quantityTHreads, n))


def task2():
    """
    TASK2_DATA
    """
    file = open("TASK2_DATA", "w")
    for n in list_N:
        for i in range(2):
            startTime = datetime.datetime.now()
            sep = None
            if i == 0:
                os.system("./sieve_unith" + " " + str(n))
                sep = " | "
            else:
                os.system("./sieve_multith" + " " + str(n) + " 1")
                sep = "; "
            end = datetime.datetime.now()
            elapsed = end - startTime
            elapsedMS = int(elapsed.total_seconds() * 1000)
            file.write(str(elapsedMS) + sep)
        file.write(str(n) + ";\n")
        # print("\tlasted {0}".format(n))


def report():
    """
    REPORT
    """
    file = open("REPORT", "w")
    with open('/proc/cpuinfo') as f:
        for line in f:
            # Ignore the blank line separating the information between
            # details about two processing units
            model = ""
            if line.strip():
                if line.rstrip('\n').startswith('model name'):
                    model_name = line.rstrip('\n').split(':')[1]
                    model += model_name + "\n"
                    l = model.split("\n")
                    file.write("Processor type: " + l[0] + "\n")
                    file.write("Cores: " + str(len(l)))
                    break


if __name__ == '__main__':
    print("task1")
    task1()
    print("task2")
    task2()
    print("report")
    report()
