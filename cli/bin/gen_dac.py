#! /bin/python2

#default values
num_sweeps = 25
dac_delta = 50
thresh = 140

filename = "fsweep1/dsweeps/dac"

outfile = open("dacsweep", "w")

#print >>outfile, "jtw 10 %d" % 0xAF1
# print >>outfile, "sp 0"
#print >>outfile, "update"

for i in range(0, 24, 1):
    mystr = ''
    mystr += "fdac_sweep "
    mystr += "-n " + str(num_sweeps) + " "
    mystr += "-x " + str(dac_delta) + " "
    mystr += "-t "  + str(thresh) + " "
    mystr += "-p 0xFFFFFF "
    mystr += "-c " + str(i) + " "
    mystr += "-f " + filename + str(i)
    print >>outfile, mystr
    print mystr

print >>outfile, "quit"
outfile.close()
