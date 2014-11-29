#!/usr/bin/env python3

import sys
import generate_results

def find_best_M(N):
    best_M = 1
    best_result = float("inf")
    for M in range(1, 20 + 1):
        result = generate_results.get_time_multith(N, M)
        if result < best_result:
            best_result = result
            best_M = M
    return best_M

def find_best_M_average(N, iterations):
    best_M_array = []
    for i in range(iterations):
        best_M_array.append(find_best_M(N))
    best_M_array = sorted(best_M_array)
    #print(best_M_array)
    return max(set(best_M_array), key = best_M_array.count)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("usage: ./find_best_M.py N")
        exit()
    
    N = int(sys.argv[1])
    print(find_best_M_average(N, 20))
