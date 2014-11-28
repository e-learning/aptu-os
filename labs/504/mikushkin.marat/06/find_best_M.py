#!/usr/bin/env python3

import generate_results

def find_best_M():
    M_sum = 0
    
    for N in range(1, 10**5 + 1, 1000):
        best_M = 1
        best_result = float("inf")
        for M in range(1, 20 + 1):
            result = generate_results.get_time_multith(N, M)
            if result < best_result:
                best_result = result
                best_M = M
        M_sum += best_M
    M_average = M_sum / 100
    return M_average

if __name__ == '__main__':
    best_M_average = 0
    for i in range(7):
        best_M_average += find_best_M()
    best_M_average /= 7
    print(best_M_average)
