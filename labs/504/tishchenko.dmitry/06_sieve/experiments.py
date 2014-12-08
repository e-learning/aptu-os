#!/usr/bin/env python3
__author__ = 'Dmitry V. Tishchenko'
import optparse
import argparse
import sys
import time
import subprocess

multprog = "./sieve_multith"
uniprog = "./sieve_unith"

def perform_iters(progname, argsline, iterations):
    args = [progname,]
    args.extend(argsline)
    exectime = 0
    for i in range(iterations):
        start = time.time()
        subprocess.call(args)
        end = time.time()
        exectime += end - start
    exectime/= iterations
    return exectime

def findopt(iterations, outfile):
    #M; N; time M[1,20]
    outformat = "{:d}; {:d}; {:.3f}\n"
    test_intervals = (100000, 1000000, 10000000, 100000000, 1000000000)

    with open(outfile, 'w') as out:
        for interval in test_intervals:
            result = perform_iters(uniprog, [str(interval),], iterations)
            out.write(str.format(outformat, 1, interval, result*1000))
        for threads_count in range(2,21):
            for interval in test_intervals:
                result = perform_iters(multprog, [str(threads_count), str(interval)], iterations)
                out.write(str.format(outformat, threads_count, interval, result*1000))

def compare(iterations, outfile):
    outformat = "{:s}; {:d}; {:.3f}\n"
    with open(outfile, 'w') as out:
        for interval in range(1000000, 1000000001, 1000000):
            uniresult = perform_iters(uniprog, [str(interval),], iterations)
            out.write(str.format(outformat, "UNI", interval, uniresult*1000))
            multresult = perform_iters(multprog, ["1", str(interval)], iterations)
            out.write(str.format(outformat, "MULT", interval, multresult*1000))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="experiments")
    subparsers = parser.add_subparsers(help='subcommands:', dest="utility")

    parser_findopt = subparsers.add_parser('findopt', help='find optimal threads count for this machine')
    parser_findopt.add_argument("--out", dest="outfile", help="output filename")
    parser_findopt.add_argument('-i', type=int, default=3, dest="iterations", help='iterations for experiment measurement')

    parser_report1 = subparsers.add_parser('compare', help='compares unithreaded and multithreaded programs')
    parser_report1.add_argument("--out", dest="outfile", help="output filename")
    parser_report1.add_argument('-i', type=int, default=3, dest="iterations", help='iterations for experiment measurement')

    args = parser.parse_args()
    if args.utility == "findopt":
        findopt(args.iterations,args.outfile)
    elif args.utility == "compare":
        compare(args.iterations,args.outfile)



