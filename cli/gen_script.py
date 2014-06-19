#! /bin/python2


#default values
num_sweeps = 50
match_window = 400
reject_offset = 3024
bunch_offset = 3064
coarse_offset = 0
coarse_rollover = 4095
thresh_start = 85
thresh_stop = 165
thresh_delta = 2
channel = 0
filename = "longsweep"

sweep_params = {"-n" : num_sweeps,      "-d" : thresh_delta, "-m" : match_window,
                "-r" : reject_offset,   "-b" : bunch_offset, "-c" : coarse_offset,
                "-x" : coarse_rollover, "-s" : thresh_start, "-t" : thresh_stop}

outfile = open("./bin/fullsweep", "w")

for i in range(0, 5):
    mystr = ''
    mystr += "tsweep "
    for flag, arg in sweep_params.iteritems():
        mystr += flag + " " + str(arg) + " "
    mystr += "-p " + str(i) + " "
    mystr += "-f " + filename + str(i) + ".txt"
    print >>outfile, mystr

outfile.close()
