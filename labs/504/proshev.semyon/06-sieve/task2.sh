#!/bin/bash
echo "" > TASK2_DATA
for n in 1000 10000 100000 1000000 10000000
do
		let start1=`date +%s%N`
        `./sieve_unith $n`
		let finish1=`date +%s%N`
		let time1=$finish1-$start1

		let start2=`date +%s%N`
        `./sieve_multith $n 1`
		let finish2=`date +%s%N`
		let time2=$finish2-$start2

        echo "$time1 | $time2; $n" >> TASK2_DATA
done