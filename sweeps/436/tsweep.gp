clear
reset

set terminal pdf
set output 'long_sweeps.pdf'

set xlabel 'Threshold setting (mV)'
set ylabel 'Hit rate (Hz)'

f1(x) = Ro*exp(-.5*((Voff-x)/sigma)**2)
set fit quiet

set yrange [100:3e7]
set xrange [-50:50]

do for [i=0:23:1] {
unset arrow
unset logscale y
stats [1:3] 'longsweep'.i nooutput
set logscale y
Ro = 1.3e+07; Voff = STATS_pos_max_y; sigma = 10;
set fit logfile 'logs/fit'.i.'.log'
fit f1(x) 'longsweep'.i using 1:3 via Ro, Voff, sigma
set title sprintf("Channel %d w/ fit\n Ro=%g Voff=%g sigma=%g", i, Ro, Voff, sigma)
set arrow from Voff,100 to Voff, 3e7 nohead lc rgb 'blue'
plot 'longsweep'.i using 1:3:5 with errorbars linecolor rgb "#C71585" title 'data', \
     Ro*exp(-.5*((Voff-x)/sigma)**2) title 'fit'
     }