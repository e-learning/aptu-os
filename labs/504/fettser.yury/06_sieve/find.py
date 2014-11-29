#!/usr/bin/env python2.7
# 2.7
import os
import multiprocessing
import subprocess

def get_time(n, m):
    out = subprocess.Popen(['./sieve_multith', str(n), str(m), '-i10'], stdout=subprocess.PIPE).communicate()
    return int(out[0].split()[0])

n = raw_input()
min = 1
max = multiprocessing.cpu_count() * 10
mintime = get_time(n, 1)
opt = 1
for i in range(min + 1, max):
	newtime = get_time(n, i)
	if newtime < mintime:
		mintime = newtime
		opt = i
print opt