if [ $# != 1 ]
then
echo "usage: generate_and_plot_data.sh START_N"
echo "N will be from START_N to START_N * 10"
echo "recomended START_N ~ 10^6"
exit
fi

rm -f data.txt
#10^7
start_n=$1

for mul in {1..10}
do
n=$(($mul * $start_n))
for i in {1..10}
do
if [ $i -eq 1 ]
then
ts=$(date +%s%N) ; ./sieve_unith $n > /dev/null; tt=$((($(date +%s%N) - $ts)/1000000))
else
ts=$(date +%s%N) ; ./sieve_multith $n $i > /dev/null; tt=$((($(date +%s%N) - $ts)/1000000))
fi
echo "$n $i $tt" >> data.txt
echo -ne "\r$(($mul * 10 - 10 + i))% complited"
done
echo "" >> data.txt
done
echo ""

gnuplot -p -e "set xlabel 'N';set ylabel 'M';set zlabel 't ms';
splot 'data.txt' using 1:2:3 with points palette pointsize 3 pointtype 7 title ''"

gnuplot -p -e "set xlabel 'N';set ylabel 'M';set zlabel 't ms';
set hidden3d;splot 'data.txt' using 1:2:3 with lines palette title 'ConcurrencyEratosthenesSieve'"

gnuplot -p -e "set xlabel 'N';set ylabel 'M';set zlabel 't ms';
set view map;
splot 'data.txt' using 1:2:3 with points pointtype 5 pointsize 3 palette linewidth 30  title ''"
