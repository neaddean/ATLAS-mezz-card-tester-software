clear
reset
set key off

if (!exists("filename")) filename='longsweep0.txt'

set xlabel "Threshold setting (mv)"
set ylabel "Hit rate (hz)"

set logscale y

set terminal pdf
set output filename.".pdf"
set title "channel 0"
#plot filename using 1:3:5 with errorbars linecolor rgb "#C71585"
plot filename using 1:3 with points pt 11 linecolor rgb "#C71585"
