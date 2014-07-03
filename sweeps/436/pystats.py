#! /usr/bin/python2
import numpy as np
import matplotlib.pyplot as plt

print "hello"
dean = 5

mydata = np.loadtxt(open("longsweep0","r"), delimiter="\t", unpack = True)

plt.plot(mydata[0], mydata[2])
plt.show()
