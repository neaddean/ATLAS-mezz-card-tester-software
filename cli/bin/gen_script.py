#! /bin/python2


#default values
num_sweeps = 50
match_window = 31
reject_offset = 2456
bunch_offset = 2464
coarse_rollover = 2563
coarse_offset = 0
thresh_start = 100
thresh_stop = 150
thresh_delta = 1

# num_sweeps = 50
# match_window = 1999
# reject_offset = 2040
# bunch_offset = 2048
# coarse_rollover = 4095
# coarse_offset = 0
# thresh_start = 100
# thresh_stop = 150
# thresh_delta = 1

filename = "longsweep"

sweep_params = {"-n" : num_sweeps,      "-d" : thresh_delta, "-m" : match_window,
                "-r" : reject_offset,   "-b" : bunch_offset, "-c" : coarse_offset,
                "-x" : coarse_rollover, "-s" : thresh_start, "-t" : thresh_stop}

outfile = open("fullsweep", "w")

print >>outfile, "jtw 10 %d" % 0xAF1
# print >>outfile, "sp 0"
print >>outfile, "update"

for i in range(0, 1, 3):
    mystr = ''
    mystr += "tsweep "
    for flag, arg in sweep_params.iteritems():
        mystr += flag + " " + str(arg) + " "
    mystr += "-p " + str(i) + " "
    mystr += "-f " + filename + str(i) + ".txt"
    print >>outfile, mystr
    print mystr

#print >>outfile, "quit"
outfile.close()
