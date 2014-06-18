#! /bin/python2


#default values
num_sweeps = 20
match_window = 1000
match_window = 1000
reject_offset = 2000
bunch_offset = 2008
coarse_offset = 0
coarse_rollover = 3500
thresh_start = 95
thresh_stop = 155
thresh_delta = 1
channel = 0
filename = "/longsweep"

sweep_params = {"-n" : num_sweeps,      "-d" : thresh_delta, "-m" : match_window,
                "-r" : reject_offset,   "-b" : bunch_offset, "-c" : coarse_offset,
                "-x" : coarse_rollover, "-s" : thresh_start, "-t" : thresh_stop}

outfile = open("./bin/fullsweep", "w")

for i in range(0, 24):
    mystr = ''
    mystr += "tsweep "
    for flag, arg in sweep_params.iteritems():
        mystr += flag + " " + str(arg) + " "
    mystr += "-p " + str(i) + " "
    mystr += "-f " + filename + str(i) + ".txt"
    print >>outfile, mystr

outfile.close()
