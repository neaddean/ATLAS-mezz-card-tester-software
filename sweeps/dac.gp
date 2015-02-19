clear
reset
set key left

if (!exists("filename")) filename='dean'

set terminal pdf
set output filename.'.pdf'


f1(x) = m*x+b
m = .75; b = 120;
fit f1(x) filename using ($2*2.5/4095):($1) via m, b
set xlabel 'DAC setting (V)'
set ylabel 'Threshold settings (mV)
set title filename

plot filename using ($2*2.5/4095):($1) with linespoints lt 3 pt 0 title 'data', \
     m*x + b title sprintf("fit:\nm=%g\nb=%g", m, b)

