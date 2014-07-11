#! /bin/python2

#default values
num_sweeps = 100
dac_delta = 50

filename = "fsweep1/dac"

outfile = open("dacsweep", "w")

#print >>outfile, "jtw 10 %d" % 0xAF1
# print >>outfile, "sp 0"
#print >>outfile, "update"

for i in range(0, 24, 1):
    mystr = ''
    mystr += "fdac_sweep "

    mystr += "-p " + str(i) + " "
    mystr += "-f " + filename + str(i)
    print >>outfile, mystr
    print mystr

print >>outfile, "quit"
outfile.close()
