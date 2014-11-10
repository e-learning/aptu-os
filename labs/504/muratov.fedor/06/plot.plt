set ylabel "Время, мс"
set xlabel "Количество потоков" 
set xrange [1:20]
set xtics 1
set title "Зависимость времени работы алгоритма от количества потоков"
plot 'plotfile' using 1:3 w li lw 3 title "Время(потоки)"
pause -1