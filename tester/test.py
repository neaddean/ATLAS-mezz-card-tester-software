#! /usr/bin/python2
import csv

adclog = []
            # get adc log data
with open("/tmp/MezzTool.tmp/adc.log") as logfile:
    logreader = csv.DictReader(logfile, delimiter="\t", quotechar='"')
    adclog = logreader.next()

print "Analog voltage:\t %.2f V" % float(adclog["AV"])
print "Digital voltage: %.2f V" % float(adclog["DV"])
print "Analog current:\t %.2f ma" % (float(adclog["AC"])*1000)
print "Digital current: %.2f ma" % (float(adclog["DC"])*1000)
print "Temperature:  \t %.2f C" % float(adclog["TEMP"])
