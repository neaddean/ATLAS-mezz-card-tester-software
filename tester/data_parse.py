#! /usr/bin/python2
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from matplotlib.backends.backend_pdf import PdfPages
import re
import sqlite3 as lite
import getpass


should_plot = False;

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
        return popt, mydata
    
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
    return fig, popt, mydata

def data_parse(mezzID):
    popt_list = []
    mydata_list = []
# only save plots if we are plotting
    if should_plot:
        pdf_file = PdfPages('python.pdf')
        for i in range(0, 24):
            temp_fig, temp_popt, temp_mydata = process_file("mezz_data/"+mezzID+"/tsweep/channel"+str(i))
            pdf_file.savefig(temp_fig)
            plt.close(temp_fig)
            popt_list.append(list(temp_popt))
        pdf_file.close()
    else:
        for i in range(0, 24):
            temp_popt, temp_mydata = process_file("mezz_data/"+mezzID+"/tsweep/channel"+str(i))
            popt_list.append(list(temp_popt))
            mydata_list.append(list(temp_mydata))
    return popt_list, mydata_list
        
