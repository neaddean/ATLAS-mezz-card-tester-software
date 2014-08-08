#! /usr/bin/python2
from gen_script import gen_script
import data_parse
import sqlite3 as lite
import subprocess
import os, errno
import numpy as np

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else: raise

user = raw_input ("Please enter user: ")
loc  = raw_input ("Please enter location: ")

con = lite.connect('mezz.db')
cur = con.cursor()

cur.execute("CREATE TABLE IF NOT EXISTS tsweep"
            "(MezzID INT, Chan INT, Ro INT, Voff INT, sigma INT, user TEXT, loc TEXT, "
            "date TEXT default current_timestamp);")
cur.execute("CREATE TABLE IF NOT EXISTS sweep_data"
            "(MezzID INT, Chan INT, thresh INT, hits INT, freq INT, window INT, error INT);")

running = True
while (running):
    mezzID = raw_input("Please enter serial number of board: ")
    if mezzID == "q":
        break

    gen_script(mezzID)
    mkdir_p("mezz_data/" + mezzID + "/tsweep")
    mkdir_p("mezz_data/" + mezzID + "/dsweep")

    subprocess.call(["./MezzTool", "-X test_script.sh"])

    sweep_params, sweep_data = data_parse.data_parse(mezzID)

    sweep_params_list = []
    for x in range(len(sweep_params)):
        sweep_params_list.append((mezzID, x, sweep_params[x][0],  sweep_params[x][1],  sweep_params[x][2], user, loc))

    sweep_data_list = []
    for x in range(len(sweep_data)):
        sweep_data[x] = np.transpose(sweep_data[x])
        for y in range(len(sweep_data[x])):
            sweep_data_list.append((mezzID, x, sweep_data[x][y][0],  sweep_data[x][y][1],
                                    sweep_data[x][y][2], sweep_data[x][y][3], sweep_data[x][y][4]))

    cur.executemany("INSERT INTO tsweep (MezzID, Chan, Ro, Voff, sigma, user, loc) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?)", sweep_params_list)
    cur.executemany("INSERT INTO sweep_data (MezzID, Chan, thresh, freq, hits, window, error) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?)" , sweep_data_list)
    con.commit()
    
con.close()

    
        
# popt_arr = np.array(popt_list)
# # make sure offsets are within 12 mV of each other
# # if not they are not, try to pick the outlier
# if np.ptp(popt_arr, axis=0)[1] > 12:
#     bad_channel = -1
#     off_min = np.min(popt_arr, axis=0)[1]
#     off_max = np.max(popt_arr, axis=0)[1]
#     off_mean = np.mean(popt_arr, axis=0)[1]
#     if np.abs(off_min-off_mean) > np.abs(off_max-off_mean):
#         bad_channel = np.where(popt_arr==off_min)[0][0]
#     elif np.abs(off_min-off_mean) < np.abs(off_max-off_mean):
#         bad_channel = np.where(popt_arr==off_max)[0][0]
#     print "Channel %d is out of the 12 mV offset range"%bad_channel
