#!/usr/bin/env python2.7
# 2.7

import os
import multiprocessing
import subprocess

__author__ = 'Sergey Tselovalnikov'


def get_time(n, m):
    out = subprocess.Popen(['./sieve_multith', str(n), str(m), '-i10'], stdout=subprocess.PIPE).communicate()
    return int(out[0].split()[0])


if __name__ == '__main__':
    n = raw_input()
    mint = 1
    maxt = multiprocessing.cpu_count() * 10
    mintime = get_time(n, 1)
    opt = 1
    for i in xrange(mint + 1, maxt):
        newtime = get_time(n, i)
        if newtime < mintime:
            mintime = newtime
            opt = i
    print opt