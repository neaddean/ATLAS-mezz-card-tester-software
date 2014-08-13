#! /usr/bin/python2
from gen_script import gen_script
import data_parse
import sqlite3 as lite
import subprocess
import os, errno
import numpy as np
import warnings
import datetime
import csv

warnings.simplefilter("ignore")

def errorCheckOffsetRange(sweep_params):
    # make sure offsets are within 12 mV of each other
    # if not they are not, try to pick the outlier
    if np.ptp(sweep_params, axis=0)[1] > 1:
        bad_channel = -1
        off_min = np.min(sweep_params, axis=0)[1]
        off_max = np.max(sweep_params, axis=0)[1]
        off_mean = np.mean(sweep_params, axis=0)[1]
        if np.abs(off_min-off_mean) > np.abs(off_max-off_mean):
            bad_channel = np.where(sweep_params==off_min)[0][0]
        elif np.abs(off_min-off_mean) < np.abs(off_max-off_mean):
            bad_channel = np.where(sweep_params==off_max)[0][0]
        print "Channel %d is out of the 12 mV offset range"%bad_channel
        return "fail", (str(bad_channel),)
    return "pass", None

def errorCheckSigma(sweep_params):
    passed = True
    widelist = []
    for sweep in sweep_params:
        if sweep[2] > 15:
            passed = False
            widelist.append(str(sweep_params.index(sweep)))
            print "Warning: Channel %s has a large width"%sweep_params.index(sweep)
    if passed:
        return "pass", None
        pass
    else:
        return "fail", widelist

user = raw_input ("Please enter user: ")
loc  = raw_input ("Please enter location: ")

con = lite.connect('mezz.db')
con.row_factory = lite.Row # why doesn't this work???!!
cur = con.cursor()

cur.execute("CREATE TABLE IF NOT EXISTS cards "
            "(MezzID INT, user TEXT, loc TEXT, pass TEXT, comments TEXT, date TEXT default current_timestamp, "
            "tdc_jtag_test TEXT, tdc_id_test TEXT, asd_jtag_test TEXT, asd_tdc TEXT, strobe_test TEXT, trig_test TEXT);")
cur.execute("CREATE TABLE IF NOT EXISTS channels "
            "(MezzID INT, Chan INT, Ro INT, Voff INT, sigma INT, m_fit INT, b_fit INT, r INT, std_err INT);")
cur.execute("CREATE TABLE IF NOT EXISTS sweep_data "
            "(MezzID INT, Chan INT, thresh INT, hits INT, freq INT, window INT, error INT);")
cur.execute("CREATE TABLE IF NOT EXISTS dac_data "
            "(MezzID INT, Chan INT, thresh INT, dac INT);")

running = True
while (running):
    mezzID = raw_input("Please enter serial number of board (q to quit): ")
    if mezzID == "q":
        break

    cur.execute("SELECT * FROM cards WHERE MezzID=?", (mezzID,))
    rows = cur.fetchall()

    if len(rows) > 0:
        rows.sort(key=lambda rows: datetime.datetime.strptime(rows["date"], '%Y-%m-%d %H:%M:%S'))

        print "This card was most recently tested by %s at %s on %s (utc)" % (rows[-1]["user"], rows[-1]["loc"], rows[-1]["date"])
        print "Result: "
        in_char = raw_input("Would you like to continue? (y/n) ")
        if in_char == "n":
            continue    
        
    # generate script to pass to MezzTool
    gen_script()

    # run MezzTool
    #subprocess.call(["./MezzTool", "-X test_script.sh"])

    # get test results from MezzTool
    testresults = []
    with open("/tmp/MezzTool.tmp/mezztool.log") as logfile:
        logreader = csv.DictReader(logfile, fieldnames=("test", "result"),
                                   restkey="comments", delimiter="\t", quotechar='"')
        for row in logreader:
            testresults.append(row)

    # parse collected data
    sweep_params, sweep_data = data_parse.parse_tsweep()
    dac_params, dac_data = data_parse.parse_dsweep()

    result, comments = errorCheckOffsetRange(sweep_params)
    testresults.append({"test" : "off_range", "result" : result, "comments" : comments})
    result, comments = errorCheckSigma(sweep_params)
    testresults.append({"test" : "sigma_width", "result" : result, "comments" : comments})

    sweep_params_list = []
    for x in range(len(sweep_params)):
        sweep_params_list.append((mezzID, x, sweep_params[x][0],  sweep_params[x][1],  sweep_params[x][2],
                                  dac_params[x][0], dac_params[x][1], dac_params[x][2], dac_params[x][3]))

    sweep_data_list = []
    for x in range(len(sweep_data)):
        sweep_data[x] = np.transpose(sweep_data[x])
        for y in range(len(sweep_data[x])):
            sweep_data_list.append((mezzID, x, sweep_data[x][y][0],  sweep_data[x][y][1],
                                    sweep_data[x][y][2], sweep_data[x][y][3], sweep_data[x][y][4]))

    dac_data_list = []
    for x in range(len(dac_data)):
       dac_data[x] = np.transpose(dac_data[x])
       for y in range(len(dac_data[x])):
           dac_data_list.append((mezzID, x, dac_data[x][y][0],  dac_data[x][y][1]))

    card_data_list = []
    for x in testresults:
        if x["test"] == "off_range":
            break
        if x["result"] == "pass":
            card_data_list.append("pass")
        else:
            card_data_list.append("fail: " + ", ".join(x["comments"]))
        
    cur.execute("INSERT INTO cards (MezzID, user, loc, pass, comments, tdc_jtag_test, tdc_id_test, asd_jtag_test, asd_tdc, strobe_test, trig_test) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", (mezzID, user, loc, "pass", "none",
                                           card_data_list[0], card_data_list[1], card_data_list[2],
                                                          card_data_list[3], card_data_list[4], card_data_list[5]))
    cur.executemany("INSERT INTO channels (MezzID, Chan, Ro, Voff, sigma, m_fit, b_fit, r, std_err)"
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", sweep_params_list)
    cur.executemany("INSERT INTO sweep_data (MezzID, Chan, thresh, freq, hits, window, error)"
                    "VALUES (?, ?, ?, ?, ?, ?, ?)" , sweep_data_list)
    cur.executemany("INSERT INTO dac_data (MezzID, Chan, thresh, dac)"
                    "VALUES (?, ?, ?, ?)" , dac_data_list)
    con.commit()

    passed = True
    for result in testresults:
        if result["result"] == "pass":
            continue
        passed = False
        print "ERROR: test %s failed with comments: %s" % (result["test"], ", ".join(result["comments"]))
    if not passed:
        in_char = raw_input("Would you like to produce graphs? (y/n) ")
        if in_char=="y":
            data_parse.should_plot = True
            data_parse.parse_tsweep(mezzID)
            data_parse.parse_dsweep(mezzID)
            data_parse.should_plot = False
                
con.close()
