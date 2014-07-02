#! /usr/bin/gnuplot

clear
reset
set key off

set style fill solid border -1

set xrange [0:50000]

set terminal pdf
set output 'histo.pdf'

bin_width=300
bin_number(x) = floor(x/bin_width)

rounded(x) = bin_width * ( bin_number(x) + 0.5 )

set title 'All channels'
plot 'hits.txt' using (rounded($10)):(1) smooth frequency with \
     boxes linecolor rgb "#C71585"

do for [i=0:23:1] {
set title 'Channel '.i
plot 'hits.txt' using (($5==i) ?  (rounded($10)): 1/0):(1) smooth frequency with \
     boxes linecolor rgb "#C71585"
     }
