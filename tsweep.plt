clear
reset
set key off

set logscale y
plot "sweep_file.txt" using 1:3 with points pt 11 linecolor rgb "#C71585"
pause 1

