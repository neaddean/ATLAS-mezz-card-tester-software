clear
reset
set key off

set style fill solid border -1

set xrange [0:50000]

bin_width=200
bin_number(x) = floor(x/bin_width)

rounded(x) = bin_width * ( bin_number(x) + 0.5 )

#set terminal pdf
#set output "histo.pdf"

plot 'hits.txt' using (($5==0) ?  (rounded($10)): 1/0):(1) smooth frequency with \
     boxes linecolor rgb "#C71585"

pause -1
