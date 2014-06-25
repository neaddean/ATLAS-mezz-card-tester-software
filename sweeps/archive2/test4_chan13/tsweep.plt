clear
reset
set key off

set xlabel "Threshold setting (mv)"
set ylabel "Hit rate (hz)"

set logscale y

set terminal pdf
set output "long_sweep.pdf"
#pt 11
plot "longsweep13.txt" using 1:3:4 with errorbars linecolor rgb "#C71585"
