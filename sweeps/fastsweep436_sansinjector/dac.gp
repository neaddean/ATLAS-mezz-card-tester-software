clear
reset
set key off

set xlabel 'DAC setting (V)'
set ylabel 'Threshold settings (mV)

plot 'dac.txt' using ($2*2.5/4095):($1) with linespoints lt 3

pause -1