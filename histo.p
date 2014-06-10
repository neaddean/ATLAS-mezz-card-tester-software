clear
reset
set key off
#set border 3

#set boxwidth 0.05 absolute
set style fill solid noborder

bin_width=25
bin_number(x) = floor(x/bin_width)

rounded(x) = bin_width * ( bin_number(x) + 0.5 )

plot 'hits.txt' using (rounded($1)):(1) smooth frequency with boxes

pause -1
