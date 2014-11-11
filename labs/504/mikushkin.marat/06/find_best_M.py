#!/usr/bin/env python3

import os
import multiprocessing
import subprocess

def get_time(n, m):
    if m == 1:
        out = subprocess.Popen(['./sieve_unith', str(n)], stdout=subprocess.PIPE).communicate()
    else:
        out = subprocess.Popen(['./sieve_multith', str(n), str(m)], stdout=subprocess.PIPE).communicate()
    return float(out[0].split()[0])

def get_time_multith(n, m):
    out = subprocess.Popen(['./sieve_multith', str(n), str(m)], stdout=subprocess.PIPE).communicate()
    return float(out[0].split()[0])

def fill_task1_data():
    task1_data_file = open("TASK1_DATA", "w")

    for M in range(1, 20 + 1):
        for N in range(1, 10**5, 1000):
            result = get_time(N, M)
            task1_data_file.write(str(M) + "; " + str(N) + "; " + str(result) + "\n")

def fill_task2_data():
    task1_data_file = open("TASK2_DATA", "w")
    for N in range(1, 10**5, 1000):
        result_unith = get_time(N, 1)
        result_multith = get_time_multith(N, 1)
        task1_data_file.write(str(result_unith) + " | " + str(result_multith) + "; " + str(N) + "\n")

def find_best_M():
    task1_data_file = open("TASK1_DATA", "w")
    
    M_sum = 0
    
    for N in range(1, 10**5 + 1, 1000):
        best_M = 1
        best_result = float("inf")
        for M in range(1, 20 + 1):
            result = get_time_multith(N, M)
            if result < best_result:
                best_result = result
                best_M = M
            task1_data_file.write(str(M) + "; " + str(N) + "; " + str(result) + "\n")
        M_sum += best_M
    M_average = M_sum / 100
    return M_average

if __name__ == '__main__':
    #fill_task1_data()
    #fill_task2_data()
    best_M = find_best_M()
    print(best_M)
    
    M_average = 0
    for i in range(10):
        M_average += find_best_M()
        #print(M_average)
    M_average /= 10
    print(M_average)
