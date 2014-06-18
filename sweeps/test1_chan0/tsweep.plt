clear
reset
set key off

set xlabel "Threshold setting (mv)"
set ylabel "Hit rate (hz)"

set logscale y

set terminal pdf
set output "long_sweep.pdf"
plot "longsweep0.txt" using 1:3:4 with points pt 11 linecolor rgb "#C71585"
#plot "longsweep0.txt" using 1:3:4 with errorbars linecolor rgb "#C71585"
