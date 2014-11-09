#!/bin/bash
if (( $# < 1))
then
	echo "Usage : $0 <Number>"
	exit 1
fi
if (( $1 < 100000))
then
	echo "Number is too small for checking. Should be greater than 100000."
	exit 1
fi
number=$1
mtime=`./sieve_unith $number`
threadn=1
for i in {1..20..1}
do
	echo "Current checking thread number is $i"
	time=`./sieve_multith $number $i`
	if (($time < $mtime))
	then
		mtime=$time
		threadn=$i
	fi
done
echo "Optimal number of threads for $number is $threadn"
echo "Time record: $mtime msecs"
