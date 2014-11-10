#!/bin/bash
echo "" > TASK1_DATA
for m in 1 2 4 8 16
do
	for n in 1000 10000 100000 1000000 10000000
	do
		let start=`date +%s%N`
		if (( $m == 1))
		then
			`./sieve_unith $n`
		else
			`./sieve_multith $n $m`
		fi
		let finish=`date +%s%N`
		let time=$finish-$start
		echo "$m; $n; $time" >> TASK1_DATA
	done
done