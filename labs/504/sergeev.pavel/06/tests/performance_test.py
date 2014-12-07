#!/usr/bin/env python3

import os
import filecmp
import time


def test_sieve_unith(N):
	start = int(round(time.time() * 1000))
	os.system("../bin/sieve_unith {}".format(N))
	stop = int(round(time.time() * 1000))
	return (stop - start)

def test_sieve_multith(N, M=1):
	start = int(round(time.time() * 1000))
	os.system("../bin/sieve_multith {} {}".format(N, M))
	stop = int(round(time.time() * 1000))
	return (stop - start)


def run_thread_test():
	NUMBER_LIMITS = (10**4, 10**5, 10**6, 10**7,10**8 )
	THREADS = (1, 2, 3, 4, 8, 12, 16, 20)
	TRY_NUMBER = 10

	for thread in THREADS:
		for number in NUMBER_LIMITS:
			print(str(thread) + " ; " + str(number) + " ; ", end="")
			time = 0
			for i in range(TRY_NUMBER):
				if thread == 1:
					time += test_sieve_unith(number)
				else:
					time += test_sieve_multith(number, thread)
			print(str(time / TRY_NUMBER))


def run_one_thread_test():
	NUMBER_LIMITS = (10**4, 10**5, 10**6, 10**7,10**8 )
	TRY_NUMBER = 10
	TESTS = (test_sieve_unith, test_sieve_multith)
	for i in TESTS:
		print(i.__name__, end = " ")
	print()

	for number in NUMBER_LIMITS:
		for test in TESTS:
			time = 0
			for i in range(TRY_NUMBER):
				time += test(number)
			print(time / TRY_NUMBER, end=" ")
		print()







def main():
	N = 1 * 10 ** 5
	M = 4
	K = 10
	unith_time = 0
	
	run_one_thread_test()
	print()
	print()
	run_thread_test()


if __name__ == "__main__":
	main()