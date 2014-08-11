#! /bin/python2

def gen_script(mezzID):
    #default values
    num_sweeps = 500
    match_window = 1999
    thresh_start = 90
    thresh_stop = 160
    thresh_delta = 1

    filename = "mezz_data/" + mezzID + "/tsweep/channel"

    sweep_params = {"-n" : num_sweeps,      "-d" : thresh_delta, "-m" : match_window,
                    "-s" : thresh_start,    "-t" : thresh_stop}

    outfile = open("test_script.sh", "w")

    print >>outfile, "jtag_test"
    print >>outfile, "strobe_test"
    print >>outfile, "trig_test"

    for i in range(0, 24):
        mystr = ''
        mystr += "fsweep "
        for flag, arg in sweep_params.iteritems():
            mystr += flag + " " + str(arg) + " "
        mystr += "-p " + str(i) + " "
        mystr += "-f " + filename + str(i)
        print >>outfile, mystr

        
    dac_num_sweeps = 20
    dac_dac_delta = 400
    dac_thresh = 160

    dac_filename = "mezz_data/" + mezzID + "/dsweep/dac"

    for i in range(0, 24, 1):
        mystr = ''
        mystr += "fdac_sweep "
        mystr += "-n " + str(dac_num_sweeps) + " "
        mystr += "-x " + str(dac_dac_delta) + " "
        mystr += "-t "  + str(dac_thresh) + " "
        mystr += "-p 0xFFFFFF "
        mystr += "-c " + str(i) + " "
        mystr += "-f " + dac_filename + str(i)
        print >>outfile, mystr    

    print >>outfile, "quit"
    outfile.close()
