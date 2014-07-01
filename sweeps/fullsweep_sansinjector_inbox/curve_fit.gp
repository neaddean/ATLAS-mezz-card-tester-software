clear
reset
set key off

set xlabel "Threshold setting (mv)"
set ylabel "Hit rate (hz)"

set logscale y
set yrange [100:3e7]
set xrange [-50:50]

f1(x) = Ro*exp(-.5*((Voff-x)/sigma)**2)
Ro = 1.3e+07; Voff = -6; sigma = 10;
fit f1(x) 'longsweep0' using 1:3 via Ro, Voff, sigma

set title sprintf("Channel 0 w/ fit\n Ro=%g Voff=%g sigma=%g", Ro, Voff, sigma)
plot 'longsweep0' using 1:3:5 with errorbars linecolor rgb "#C71585", \
     Ro*exp(-.5*((Voff-x)/sigma)**2) title 'fit curve'
#plot 'longsweep0' using 1:3 with points pt 11 linecolor rgb "#C71585"

pause -1