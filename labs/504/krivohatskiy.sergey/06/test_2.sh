for n in 12345 123456 1234567 12345678 123456789 1234567890
do
ts=$(date +%s%N) ; ./sieve_unith $n > /dev/null; tt1=$((($(date +%s%N) - $ts)/1000000))
ts=$(date +%s%N) ; ./sieve_multith $n 1 > /dev/null; tt2=$((($(date +%s%N) - $ts)/1000000))
echo "$tt1 | $tt2; $n"
done
