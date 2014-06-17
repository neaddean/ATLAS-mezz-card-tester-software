clear
reset
set key off

set xlabel "Threshold setting (mv)"
set ylabel "Hit rate (hz)"

set logscale y
#set term x11 0
plot "sweep.txt" using 1:3 with points pt 11 linecolor rgb "#C71585"
#set term x11 1
#plot "sweep2.txt" using 1:3 with points pt 11 linecolor rgb "#0000FF"