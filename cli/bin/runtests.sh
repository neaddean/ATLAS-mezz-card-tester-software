#! /usr/bin/zsh 
./MezzTool -X ./runscript.sh

cd ../../sweeps/test/
./dstat.py -p -f dsweep/dac
./pystats.py -p -f tsweep/channel
