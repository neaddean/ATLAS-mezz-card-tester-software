#! /usr/bin/python2
from gen_script import gen_script
import data_parse
import sqlite3 as lite
import subprocess
import os, errno
import numpy as np
import warnings
import datetime

warnings.simplefilter("ignore")

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

        
def errorCheckOffsetRange(sweep_params):
    # make sure offsets are within 12 mV of each other
    # if not they are not, try to pick the outlier
    if np.ptp(sweep_params, axis=0)[1] > 12:
        bad_channel = -1
        off_min = np.min(sweep_params, axis=0)[1]
        off_max = np.max(sweep_params, axis=0)[1]
        off_mean = np.mean(sweep_params, axis=0)[1]
        if np.abs(off_min-off_mean) > np.abs(off_max-off_mean):
            bad_channel = np.where(sweep_params==off_min)[0][0]
        elif np.abs(off_min-off_mean) < np.abs(off_max-off_mean):
            bad_channel = np.where(sweep_params==off_max)[0][0]
        print "Channel %d is out of the 12 mV offset range"%bad_channel

def errorCheckSigma(sweep_params):
    for sweep in sweep_params:
        if sweep[2] > 15:
            print "Warning: Channel %s has a large width"%sweep_params.index(sweep) 


user = raw_input ("Please enter user: ")
loc  = raw_input ("Please enter location: ")

con = lite.connect('mezz.db')
con.row_factory = lite.Row # why doesn't this work???!!
cur = con.cursor()


cur.execute("CREATE TABLE IF NOT EXISTS cards"
            "(MezzID INT, user TEXT, loc TEXT, pass TEXT, comments TEXT, date TEXT default current_timestamp);")
cur.execute("CREATE TABLE IF NOT EXISTS channels"
            "(MezzID INT, Chan INT, Ro INT, Voff INT, sigma INT, m_fit INT, b_fit INT, r INT, std_err INT);")
cur.execute("CREATE TABLE IF NOT EXISTS sweep_data"
            "(MezzID INT, Chan INT, thresh INT, hits INT, freq INT, window INT, error INT);")
cur.execute("CREATE TABLE IF NOT EXISTS dac_data"
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

        print "This card was most recently tested by %s at %s on %s" % (rows[-1]["user"], rows[-1]["loc"], rows[-1]["date"])
        print "Result: "
        in_char = raw_input("Would you like to continue? (y/n) ")
        if in_char == "n":
            continue    
        
    # generate script to pass to MezzTool
    gen_script(mezzID)
    mkdir_p("mezz_data/" + mezzID + "/tsweep")
    mkdir_p("mezz_data/" + mezzID + "/dsweep")

    # run MezzTool
    subprocess.call(["./MezzTool", "-X test_script.sh"])

    # parse collected data
    sweep_params, sweep_data = data_parse.parse_tsweep(mezzID)
    dac_params, dac_data = data_parse.parse_dsweep(mezzID)

    errorCheckOffsetRange(sweep_params)
    errorCheckSigma(sweep_params)

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
           

    cur.execute("INSERT INTO cards (MezzID, user, loc, pass, comments) "
                "VALUES (?, ?, ?, ?, ?)", (mezzID, user, loc, "pass", "none"))
    cur.executemany("INSERT INTO channels (MezzID, Chan, Ro, Voff, sigma, m_fit, b_fit, r, std_err)"
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)", sweep_params_list)
    cur.executemany("INSERT INTO sweep_data (MezzID, Chan, thresh, freq, hits, window, error)"
                    "VALUES (?, ?, ?, ?, ?, ?, ?)" , sweep_data_list)
    cur.executemany("INSERT INTO dac_data (MezzID, Chan, thresh, dac)"
                    "VALUES (?, ?, ?, ?)" , dac_data_list)
    con.commit()
    
con.close()
