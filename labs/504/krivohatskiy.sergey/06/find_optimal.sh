if [ $# != '1' ]
then
echo "usage: ./find_optimal.sh N"
echo "recomended N ~ 10^7"
exit
fi

n=$1

results=[]

#run 5 times and acummulate result times
for j in {1..5}
do
for i in {1..20}
do
if [ $i -eq 1 ]
then
ts=$(date +%s%N) ; ./sieve_unith $n > /dev/null; tt=$((($(date +%s%N) - $ts)/1000000))
else
ts=$(date +%s%N) ; ./sieve_multith $n $i > /dev/null; tt=$((($(date +%s%N) - $ts)/1000000))
fi
results[i]=$((${results[i]} + $tt))
echo -ne "\r$(($j * 10 - 10 + i / 2))% complited"
done
done
echo ""

min=1
minValue=100
echo "1 thread result is"
echo "${results[1]} for 5 runs"
for i in {2..20}
do
results[i]=$((100 * ${results[i]} / ${results[1]}))
if [ ${results[i]} -lt $minValue ]
then
minValue=${results[i]}
min=$i
fi
echo "$i thread result is"
echo "${results[i]}% from 1 thread result"
done

echo "Recomented threads count is $min"
echo "It has this result: $minValue "
