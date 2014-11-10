#!/usr/bin/env python3

#!/usr/bin/env python3

import os
import filecmp
import datetime


def test_sieve_unith(N):
	start = datetime.datetime.now()
	os.system("../bin/sieve_unith {}".format(N))
	stop = datetime.datetime.now()
	return (stop - start).microseconds / 10

def test_sieve_multith(N, M):
	start = datetime.datetime.now()
	os.system("../bin/sieve_multith {} {}".format(N, M))
	stop = datetime.datetime.now()
	return (stop - start).microseconds / 10


def main():
	N = 2 * 10 ** 7
	M = 3
	unith_time = test_sieve_unith(N)
	multith_time = test_sieve_multith(N, M)
	print("unith_time:{}\nmultith_time:{}\n".format(unith_time, multith_time))
	print("speer up:{}".format(1.0 - multith_time / unith_time ))
	


if __name__ == "__main__":
	main()