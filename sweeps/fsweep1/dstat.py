#! /usr/bin/python2
from scipy import stats, polyval
import matplotlib.pyplot as plt
import numpy as np

# load the data
mydata = np.loadtxt(open("dac.txt","r"), delimiter="\t", unpack=True)
thresh = mydata[0]
dac = mydata[1]*2.5/4095

fig = plt.figure()
ax = fig.add_subplot(111)

# Linear regression using stats.linregress
m_fit, b_fit, r, p, stderr = stats.linregress(dac,thresh)
# create the fitted curve
fit=polyval([m_fit,b_fit],dac)
print('parameters:\n\tslope: %g\tintercept:%g\nr:%g\tr^2:%g\tstd_err:%g' % (m_fit, b_fit, r, r**2, stderr))

# text box with fit parameters
props = dict(boxstyle='round', facecolor='wheat', alpha=0.7)
textstr='slope= %g\nintercept=%g\nr=%g\nr^2=%g\nstd_err=%g' % (m_fit, b_fit, r, r**2, stderr)
# make text appear always in the top left
ax.text(0.05, 0.95, textstr, transform=ax.transAxes, fontsize=14,
        verticalalignment='top', bbox=props)

#matplotlib ploting
ax.set_title('Linear Regression of dac sweep')
ax.plot(dac,thresh,'r.')
ax.plot(dac,fit,'b.-')
ax.legend(['data', 'fit'], loc=4)

plt.show()
