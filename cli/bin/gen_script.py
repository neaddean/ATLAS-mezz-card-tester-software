#! /bin/python2

#default values
num_sweeps = 20
match_window = 1999
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

filename = "436/longsweep"

sweep_params = {"-n" : num_sweeps,      "-d" : thresh_delta, "-m" : match_window,
                "-s" : thresh_start,    "-t" : thresh_stop}

outfile = open("fullsweep", "w")

#print >>outfile, "jtw 10 %d" % 0xAF1
# print >>outfile, "sp 0"
#print >>outfile, "update"

for i in range(0, 24, 1):
    mystr = ''
    mystr += "tsweep "
    for flag, arg in sweep_params.iteritems():
        mystr += flag + " " + str(arg) + " "
    mystr += "-p " + str(i) + " "
    mystr += "-f " + filename + str(i)
    print >>outfile, mystr
    print mystr

print >>outfile, "quit"
outfile.close()
