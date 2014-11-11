for i in {1..20}
do
for n in 12345 123456 1234567 12345678 123456789 1234567890
do
if [ $i -eq 1 ]
then
ts=$(date +%s%N) ; ./sieve_unith $n > /dev/null; tt=$((($(date +%s%N) - $ts)/1000000))
else
ts=$(date +%s%N) ; ./sieve_multith $n $i > /dev/null; tt=$((($(date +%s%N) - $ts)/1000000))
fi
echo "$i; $n; $tt"
done
done
