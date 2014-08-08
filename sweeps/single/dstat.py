#! /usr/bin/python2
from scipy import stats, polyval
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import numpy as np
import argparse
import re

parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("-f", help="name of dac file w/o number", default = "dac")
parser.add_argument("-p", "--plot", help="plot flag", action="store_true",
                    default = False)
parser.add_argument("--outfile", help="name of pdf to put plots in", default = "dac")
args = parser.parse_args()
should_plot = args.plot

def process_file(filename):
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
        return popt
    
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
    
    return fig, popt

popt_list = []
# only save plots if we are plotting
if should_plot:
    pdf_file = PdfPages(args.outfile + '.pdf')
    for i in range(0, 24):
        temp_fig, temp_popt = process_file(args.f+str(i))
        pdf_file.savefig(temp_fig)
        plt.close(temp_fig)
        popt_list.append(list(temp_popt))
    pdf_file.close()
else:
    for i in range(0, 24):
        popt_list.append(process_file(args.f+str(i)))

np.savetxt("dac_log.txt", popt_list, fmt="%g", delimiter="\t", header="slope\tintercept\tr\tstderr")
