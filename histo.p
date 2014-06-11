clear
reset
set key off

set style fill solid border -1

bin_width=10
bin_number(x) = floor(x/bin_width)

rounded(x) = bin_width * ( bin_number(x) + 0.5 )

plot 'hits.txt' using (rounded($9)):(1) smooth frequency with boxes linecolor rgb "#C71585"

pause -1
