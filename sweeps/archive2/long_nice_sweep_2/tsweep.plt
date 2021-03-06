clear
reset
set key off

set xlabel "Threshold setting (mv)"
set ylabel "Hit rate (hz)"

set logscale y

set terminal pdf
set output "long_sweeps.pdf"

do for [i=0:23:3] {
   titlebuf = sprintf("Channel #%d", i)
   set title titlebuf
   infile = sprintf("longsweep%d.txt", i)
   plot infile using 1:3:4 with errorbars linecolor rgb "#C71585"
   #plot infile using 1:3 with points pt 11 linecolor rgb "#C71585"
}
