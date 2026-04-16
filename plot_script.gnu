set terminal png size 1200,600 enhanced font 'Arial,12'
set output 'signal_plot.png'
set title 'AMI with HDB3 - Input: 1010110' font 'Arial,14'
set xlabel 'Time (bit period)' font 'Arial,12'
set ylabel 'Voltage Level' font 'Arial,12'
set yrange [-1.5:1.5]
set grid
set style line 1 lc rgb '#0060ad' lt 1 lw 2
plot 'signal_data.txt' with lines ls 1 notitle
