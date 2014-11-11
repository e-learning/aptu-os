#!/bin/bash
echo "" > TASK1_DATA
for threadn in 1 4 8 16 20
do
	echo "Test for $threadn thread(s)..."
	for number in 50000 500000 5000000 50000000
	do
	if (($threadn == 1))	
	then
		time=`./sieve_unith $number`
	else
		time=`./sieve_multith $number $threadn`
	fi
	echo "$threadn; $number; $time" >> TASK1_DATA
	done
done
echo "Done"
