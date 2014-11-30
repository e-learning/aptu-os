#!/usr/bin/env python3

import sys
import generate_results

N_array = generate_results.getNarray()

def find_best_M(N):
    best_M = 1
    best_result = generate_results.get_time(N, 1)
    for M in range(1, 20 + 1):
        result = generate_results.get_time_multith(N, M)
        #results = [generate_results.get_time_multith(N, M) for _ in range(21)]
        #results = sorted(results)
        #print(results)
        #result = results[10]
        #print(str(M) + " " + str(result))
        if result < best_result:
            best_result = result
            best_M = M
    return best_M

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("usage: ./find_best_M.py N")
        exit()
    
    N = int(sys.argv[1])
    print(find_best_M(N))
