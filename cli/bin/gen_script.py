#! /bin/python2


#default values
num_sweeps = 25
match_window = 3999
reject_offset = 458
bunch_offset = 464
coarse_offset = 0
coarse_rollover = 4095
thresh_start = 100
thresh_stop = 150
thresh_delta = 1
#channel = 0
filename = "longsweep"

sweep_params = {"-n" : num_sweeps,      "-d" : thresh_delta, "-m" : match_window,
                "-r" : reject_offset,   "-b" : bunch_offset, "-c" : coarse_offset,
                "-x" : coarse_rollover, "-s" : thresh_start, "-t" : thresh_stop}

outfile = open("fullsweep", "w")

print >>outfile, "jtw 10 %d" % 0xAF1
print >>outfile, "update"

for i in range(0, 1):
    mystr = ''
    mystr += "tsweep "
    for flag, arg in sweep_params.iteritems():
        mystr += flag + " " + str(arg) + " "
    mystr += "-p " + str(i) + " "
    mystr += "-f " + filename + str(i) + ".txt"
    print >>outfile, mystr
    print mystr

print >>outfile, "quit"
outfile.close()
