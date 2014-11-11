#!/bin/bash
echo "" > TASK2_DATA
for number in 50000 500000 5000000 50000000
do
	echo "Test UNITH and MULTITH for $number..."
	time1=`./sieve_unith $number`
	time2=`./sieve_multith $number 1`
	echo "$time1 | $time2; $number" >> TASK2_DATA
done
echo "Done"
