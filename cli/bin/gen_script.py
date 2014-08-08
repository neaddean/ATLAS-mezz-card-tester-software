#! /bin/python2

#default values
num_sweeps = 1000
match_window = 1999
thresh_start = 90
thresh_stop = 160
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

asd_regs = ["0x00", "0x00", "0x6C",
            "0x01", # disc2 thresh 0x01
            "0x00", # disc1 hysteresis 0x02
            "0x06", # integration gate 0x06
            "0x05", # rundown current 0x05
            "0x07", # deadtime 0x07
            "0x00",
            "0x00",
            "0x1"]



filename = "single/tsweep/channel"

sweep_params = {"-n" : num_sweeps,      "-d" : thresh_delta, "-m" : match_window,
                "-s" : thresh_start,    "-t" : thresh_stop}

outfile = open("singlesweep", "w")

#print >>outfile, "jtw 10 %d" % 0xAF1
# print >>outfile, "sp 0"
#print >>outfile, "update"

for i in range(len(asd_regs)):
    print >>outfile, "jaw " + str(i) + " " + asd_regs[i]

for i in range(0, 4, 1):
    mystr = ''
    mystr += "fsweep "
    for flag, arg in sweep_params.iteritems():
        mystr += flag + " " + str(arg) + " "
    mystr += "-p " + str(i) + " "
    mystr += "-f " + filename + str(i)
    print >>outfile, mystr
    print mystr

print >>outfile, "quit"
outfile.close()
