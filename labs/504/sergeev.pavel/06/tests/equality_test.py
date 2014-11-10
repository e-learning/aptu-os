#!/usr/bin/env python3

import os
import filecmp


def main():
	N = 1000000000
	M = 8
	os.system("../bin/sieve_unith -p {} > unith.txt".format(N))
	os.system("../bin/sieve_multith -p {} {} > multith.txt".format(N, M))
	print(filecmp.cmp("unith.txt", "multith.txt"))


if __name__ == "__main__":
	main()