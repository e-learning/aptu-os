#!/usr/bin/env python3
__author__ = 'muratov'

import os
import datetime

list_N = [10 ** 2, 10 ** 5, 2 * 10 ** 7]
file_task1 = "TASK1_DATA"
max_thread = 21


def computeSieve(nums, threads):
    """
    TASK1_DATA
    """
    arg = ""
    if threads == 1:
        arg = "./sieve_unith "
    else:
        arg = "./sieve_multith "
    arg += str(nums) + " "
    if threads != 1:
        arg += str(threads)
    start = datetime.datetime.now()
    os.system(arg)
    end = datetime.datetime.now()
    elapsed = end - start
    elapsedMS = int(elapsed.total_seconds() * 1000)
    return elapsedMS


def task1(fileName, threads, nums):
    with open(fileName, "w") as file:
        for thread_num in threads:
            for num in nums:
                ms = computeSieve(num, thread_num)
                toWrite = "{0}; {1}; {2};\n".format(thread_num, num, ms)
                file.write(toWrite)
                print(toWrite)


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


def report(writeis=True):
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
                    if writeis:
                        file.write("Processor type: " + l[0] + "\n")
                        file.write("Cores: " + str(len(l)))
                    return len(l)


if __name__ == '__main__':
    print("task1")
    task1("TASK1_DATA", list(range(1, 21)), [10 ** 2, 10 ** 5, 10 ** 8])
    print("task2")
    task2()
    print("report")
    report()
