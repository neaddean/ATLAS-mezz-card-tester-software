#! /usr/bin/python2
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from matplotlib.backends.backend_pdf import PdfPages
import glob
import re
import argparse
import sqlite3 as lite


parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("-f", help="name of data file w/o number", default = "tsweep/channel")
parser.add_argument("-p", "--plot", help="plot sweep data flag", action="store_true",
                    default = False)
args = parser.parse_args()
should_plot = args.plot

def gauss(x, *p):
    Ro, Voff, sig = p
    return Ro*np.exp(-.5*((Voff-x)/sig)**2)

def process_file(filename):
    # open the file
    mydata = np.loadtxt(open(filename,"r"), delimiter="\t", unpack = True)

    # fit the gaussian function
    # inital guesses: Ro=max y value, Voff= x value of y max 
    maxR = np.max(mydata[2])
    p0 = [maxR, mydata[0][np.where(mydata[2]==maxR)[0][0]], 10]
    # fit the curve
    popt, pcov = curve_fit(gauss, mydata[0], mydata[2], p0=p0)

    # if we aren't plotting, then end here
    if not should_plot:
        return popt
    
    # make figure instance
    fig = plt.figure()
    ax = fig.add_subplot(111)

    ax.errorbar(mydata[0], mydata[2], yerr=mydata[4], xlolims=True,
                fmt="g+", ms=10, mew=1.5, label='data')
    ax.axis([-60, 50, 1e2, 3e7])
    ax.set_yscale("log", nonposy="clip")
    ax.set_xlabel("Threshold setting $mV$", fontsize = 18)
    ax.set_ylabel("Hit rate $Hz$", fontsize =18)
    ax.set_title("Channel "+re.findall('\d+',filename)[-1], fontsize = 22)

    ax.plot(mydata[0], gauss(mydata[0], *popt), label='curve fit')

    # add a wheat colored rounded box with the stats in it
    props = dict(boxstyle='round', facecolor='wheat', alpha=0.7)
    textstr="$R_0=%g$\n$V_{off}=%f$\n$\sigma=%f$" %(popt[0], popt[1], popt[2])
    # make text appear always in the top left
    ax.text(0.05, 0.95, textstr, transform=ax.transAxes, fontsize=14,
        verticalalignment='top', bbox=props)

    ax.grid()
    ax.legend()
    return fig, popt

popt_list = []
# only save plots if we are plotting
if should_plot:
    pdf_file = PdfPages('python.pdf')
    for i in range(0, 24):
        temp_fig, temp_popt = process_file(args.f+str(i))
        pdf_file.savefig(temp_fig)
        plt.close(temp_fig)
        popt_list.append(list(temp_popt))
    pdf_file.close()
else:
    for i in range(0, 24):
        popt_list.append(process_file(args.f+str(i)))

popt_arr = np.array(popt_list)
# make sure offsets are within 12 mV of each other
# if not they are not, try to pick the outlier
if np.ptp(popt_arr, axis=0)[1] > 12:
    bad_channel = -1
    off_min = np.min(popt_arr, axis=0)[1]
    off_max = np.max(popt_arr, axis=0)[1]
    off_mean = np.mean(popt_arr, axis=0)[1]
    if np.abs(off_min-off_mean) > np.abs(off_max-off_mean):
        bad_channel = np.where(popt_arr==off_min)[0][0]
    elif np.abs(off_min-off_mean) < np.abs(off_max-off_mean):
        bad_channel = np.where(popt_arr==off_max)[0][0]
    print "Channel %d is out of the 12 mV offset range"%bad_channel

np.savetxt("stat_log.txt",popt_arr, fmt="%g", delimiter="\t", header="Ro\tVoff\tsigma")

con = lite.connect('mezz.db')

with con:
    cur = con.cursor()

    cur.execute("DROP TABLE IF EXISTS Channels")
    cur.execute("CREATE TABLE Channels (Id INTEGER PRIMARY KEY AUTOINCREMENT, Ro INT, Voff INT, sigma INT)")
    cur.executemany("INSERT INTO Channels(Ro, Voff, sigma) VALUES (?, ?, ?)", popt_list)

print "done"




