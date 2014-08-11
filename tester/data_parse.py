#! /usr/bin/python2
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
from scipy import stats, polyval
from matplotlib.backends.backend_pdf import PdfPages
import re
import sqlite3 as lite
import getpass


should_plot = False;

def gauss(x, *p):
    Ro, Voff, sig = p
    return Ro*np.exp(-.5*((Voff-x)/sig)**2)

def process_threshold_file(filename):
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

def parse_tsweep(mezzID):
    popt_list = []
    mydata_list = []
# only save plots if we are plotting
    if should_plot:
        pdf_file = PdfPages('tsweep_%s.pdf'%mezzID)
        for i in range(0, 24):
            temp_fig, temp_popt, temp_mydata = process_threshold_file("mezz_data/"+mezzID+"/tsweep/channel"+str(i))
            pdf_file.savefig(temp_fig)
            plt.close(temp_fig)
            popt_list.append(list(temp_popt))
            mydata_list.append(list(temp_mydata))
        pdf_file.close()
    else:
        for i in range(0, 24):
            temp_popt, temp_mydata = process_threshold_file("mezz_data/"+mezzID+"/tsweep/channel"+str(i))
            popt_list.append(list(temp_popt))
            mydata_list.append(list(temp_mydata))
    return popt_list, mydata_list
        

def process_dac_file(filename):
    # load the data
    mydata = np.loadtxt(open(filename,"r"), delimiter="\t", unpack=True)
    try:
        thresh = mydata[0]/2+127
        dac = mydata[1]*2.5/4095
    except:
        thresh = [0,1]
        dac = [1, 2]

    # Linear regression using stats.linregress
    # m_fit, b_fit, r, p, stderr = stats.linregress(dac,thresh)
    popt = stats.linregress(dac,thresh)
    #get rid of p
    popt = popt[0], popt[1], popt[2], popt[4]
    # if we aren't plotting, then end here
    if not should_plot:
        return popt, mydata
    
    # make figure instance
    fig = plt.figure()
    ax = fig.add_subplot(111)

    # plot the fit
    fit=polyval([popt[0], popt[1]],dac)
    ax.plot(dac,fit,'bs-', ms=4)

    #plot original data (over fit)
    ax.plot(dac,thresh,'r.')

    # text box with fit parameters
    props = dict(boxstyle='round', facecolor='wheat', alpha=0.7)
    textstr='slope= %g\nintercept=%g\nr=%g\nr^2=%g\nstd_err=%g' % (popt[0], popt[1], popt[2],
                                                                   popt[2]**2, popt[3])
    # make text appear always in the top left
    ax.text(0.05, 0.95, textstr, transform=ax.transAxes, fontsize=14,
            verticalalignment='top', bbox=props)

    #matplotlib ploting
    ax.set_title('Dac sweep on channel '+ re.findall('\d+',filename)[-1],
                 fontsize=22)
    ax.set_xlabel("Dac setting (out of 0xFFF)")
    ax.set_ylabel("ASD Threshold setting")
    ax.legend(['data', 'fit'], loc=4)
    
    return fig, popt, mydata

def parse_dsweep(mezzID):
    popt_list = []
    mydata_list = []
    # only save plots if we are plotting
    if should_plot:
        pdf_file = PdfPages('dsweep_%s.pdf'%mezzID)
        for i in range(0, 24):
            temp_fig, temp_popt, temp_mydata = process_dac_file("mezz_data/"+mezzID+"/dsweep/dac"+str(i))
            pdf_file.savefig(temp_fig)
            plt.close(temp_fig)
            popt_list.append(list(temp_popt))
            mydata_list.append(list(temp_mydata))
        pdf_file.close()
    else:
        for i in range(0, 24):
            temp_popt, temp_mydata = process_dac_file("mezz_data/"+mezzID+"/dsweep/dac"+str(i))
            popt_list.append(list(temp_popt))
            mydata_list.append(list(temp_mydata))
    return popt_list, mydata_list
