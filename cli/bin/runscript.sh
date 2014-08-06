# run elementary tests
jtag_test
strobe_test
trig_test

# run the full sweep
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 0 -f test/tsweep/channel0
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 1 -f test/tsweep/channel1
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 2 -f test/tsweep/channel2
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 3 -f test/tsweep/channel3
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 4 -f test/tsweep/channel4
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 5 -f test/tsweep/channel5
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 6 -f test/tsweep/channel6
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 7 -f test/tsweep/channel7
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 8 -f test/tsweep/channel8
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 9 -f test/tsweep/channel9
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 10 -f test/tsweep/channel10
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 11 -f test/tsweep/channel11
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 12 -f test/tsweep/channel12
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 13 -f test/tsweep/channel13
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 14 -f test/tsweep/channel14
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 15 -f test/tsweep/channel15
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 16 -f test/tsweep/channel16
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 17 -f test/tsweep/channel17
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 18 -f test/tsweep/channel18
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 19 -f test/tsweep/channel19
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 20 -f test/tsweep/channel20
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 21 -f test/tsweep/channel21
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 22 -f test/tsweep/channel22
fsweep -m 1999 -d 1 -n 1000 -s 90 -t 160 -p 23 -f test/tsweep/channel23

# run the dac sweeps
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 0 -f test/dsweep/dac0
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 1 -f test/dsweep/dac1
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 2 -f test/dsweep/dac2
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 3 -f test/dsweep/dac3
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 4 -f test/dsweep/dac4
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 5 -f test/dsweep/dac5
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 6 -f test/dsweep/dac6
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 7 -f test/dsweep/dac7
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 8 -f test/dsweep/dac8
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 9 -f test/dsweep/dac9
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 10 -f test/dsweep/dac10
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 11 -f test/dsweep/dac11
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 12 -f test/dsweep/dac12
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 13 -f test/dsweep/dac13
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 14 -f test/dsweep/dac14
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 15 -f test/dsweep/dac15
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 16 -f test/dsweep/dac16
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 17 -f test/dsweep/dac17
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 18 -f test/dsweep/dac18
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 19 -f test/dsweep/dac19
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 20 -f test/dsweep/dac20
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 21 -f test/dsweep/dac21
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 22 -f test/dsweep/dac22
fdac_sweep -n 25 -x 500 -t 140 -p 0xFFFFFF -c 23 -f test/dsweep/dac23


quit
