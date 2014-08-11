#include "AMC13_Launcher.hh"
#include <cmath>

#define save_dir ""

void AMC13_Launcher::LoadCommandList()
{
  AddCommand("help",&AMC13_Launcher::Help,"");
  AddCommandAlias( "h", "help");
  AddCommand("quit",&AMC13_Launcher::Quit,"Close program");
  AddCommandAlias("q", "quit");
  AddCommand("exit",&AMC13_Launcher::Quit,"Close program");
  AddCommand("echo",&AMC13_Launcher::Echo,"echo to screen");
  AddCommand("cli",&AMC13_Launcher::cli,"send command directly to pico blaze cli");
  AddCommand("tsweep_man",&AMC13_Launcher::tsweep_man,
	     "threshold sweep with manauly set parameters");
  AddCommand("tsweep",&AMC13_Launcher::tsweep,"threshold sweep");
  AddCommand("fsweep",&AMC13_Launcher::fsweep,"fast threshold sweep");
  AddCommand("trig",&AMC13_Launcher::Trigger,"trigger and read back TDC");
  AddCommand("jtw",&AMC13_Launcher::jtw,"write to TDC register\tjtw {reg} {value}");
  AddCommand("jaw",&AMC13_Launcher::jaw,"write to ASD register\tjaw {reg} {value}");
  AddCommand("jtr",&AMC13_Launcher::jtr,"read from TDC register\tjtr {reg}");
  AddCommand("jar",&AMC13_Launcher::jar,"read from ASD register\tjar {reg}");
  AddCommand("p",&AMC13_Launcher::SetChannelMask,"set injector channel mask");
  AddCommand("c",&AMC13_Launcher::SetChannel,"set TDC channel");
  AddCommand("hp",&AMC13_Launcher::SetHitPeriod,"set pulse delay");
  AddCommand("sp",&AMC13_Launcher::SetStrobePulsePeriod,"set ASD strobe period");
  AddCommand("update",&AMC13_Launcher::UpdateBoard,"flush settings to board");
  AddCommand("load_test",&AMC13_Launcher::load_test,"load asd strobe test settings");
  AddCommand("dump",&AMC13_Launcher::dump,
	     "display current (local) TDC registers, give any argument for other settings");
  AddCommand("mw",&AMC13_Launcher::mw,"set match window");
  AddCommand("bo",&AMC13_Launcher::bo,"set bunch offset");
  AddCommand("treset",&AMC13_Launcher::treset,
	     "send TDC global reset followed by event and bunch resets");
  AddCommand("d",&AMC13_Launcher::d,"set all dacs");
  AddCommand("load_inject",&AMC13_Launcher::load_inject,"load settings for external injector");
  AddCommand("dac_sweep",&AMC13_Launcher::dac_sweep,"sweep threshold and dac");
  AddCommand("fdac_sweep",&AMC13_Launcher::fdac_sweep,"fast sweep threshold and dac");
  AddCommand("jtag_test",&AMC13_Launcher::jtag_test,"perform jtag tests, run with any"
	     " argument for verbose");
  AddCommand("strobe_test",&AMC13_Launcher::strobe_test,"perform strobe test on all channels");
  AddCommand("trig_test",&AMC13_Launcher::trig_test,"perform trigger/reset test on all channels");
  AddCommand("jts",&AMC13_Launcher::TDC_status,"print the status of the tdc");
}

int AMC13_Launcher::Quit(std::vector<std::string>,
			 std::vector<uint64_t>)
{
  //Quit CLI so return -1
  return -1;
}

int AMC13_Launcher::Echo(std::vector<std::string> strArg,
			 std::vector<uint64_t> intArg)
{
  for(size_t iArg = 0; iArg < strArg.size();iArg++)
    {
      printf("%s ",strArg[iArg].c_str());
    }
  printf("\n");
  return 0;
}


int AMC13_Launcher::Help(std::vector<std::string> strArg,
			 std::vector<uint64_t> intArg)
{
  printf("Commands:\n");  
  for(size_t i = 0; i < commandName.size();i++)
    {
      printf(" %-12s:   %s\n",commandName[i].c_str(),commandHelp[i].c_str());
    }
  return 0;
}

int AMC13_Launcher::cli(std::vector<std::string> strArg,
			std::vector<uint64_t> intArg)
{
  (void) intArg;
  static char buffer[8*64];
  for(size_t iArg = 0; iArg < strArg.size();iArg++)
    {
      mezzTester->Board.serial.Write(strArg[iArg].c_str());
    }
  // false argument means do not read entire line back
  mezzTester->Board.serial.Writeln("", false);
  mezzTester->Board.serial.Readln(buffer,8*64,'>');
  printf("%s\n", buffer);

  return 0;
}

void printManSweepHelp()
{
  printf("usage (default):\n"
	 "-n\tnumber of sweeps (100)\n"
	 "-p\tchannel (0)\n"
	 "-m\tmatch window (1000)\n"
	 "-r\treject offset (2000)\n"
	 "-b\tbunch offset (2008)\n"
	 "-c\tcoarse offset (0)\n"
	 "-x\tcoarse rollover (3500)\n"
	 "-s\tthreshold start (90)\n"
	 "-t\tthreshold stop (154)\n"
	 "-d\tthreshold delta (2)\n"
	 "-f\tsave file in sweep folder *\n"
	 "-h\tdisplays this message\n"
	 "*required\n");
}

int AMC13_Launcher::tsweep_man(std::vector<std::string> strArg,
			       std::vector<uint64_t> intArg)
{
  int num_sweeps = 100;
  int match_window = 1000;
  int reject_offset = 2000;
  int bunch_offset = 2008;
  int coarse_offset = 0;
  int coarse_rollover = 3500;
  int thresh_start = 90;
  int thresh_stop = 140;
  int thresh_delta = 2;
  int channel = 0;
  char file_name_buffer[100];
  FILE* sweep_file = NULL;
  int runhits;
  float rate;

  // can only check for "-h" if there are actually arguments
  // so this is split into two checks
  if ((strArg.size() == 0) ||
      (strArg.size()%2==1))
    {
      printManSweepHelp();
      return 0;
    }
  
  if (strArg[0].compare("-h")==0)
    {
      printManSweepHelp();
      return 0;
    }

  for (size_t arg=0; arg < strArg.size(); arg+=2)
    {
      if(strArg[arg].compare("-n")==0)
	num_sweeps = intArg[arg+1];
      else if(strArg[arg].compare("-m")==0)
	match_window = intArg[arg+1];
      else if(strArg[arg].compare("-r")==0)
	reject_offset = intArg[arg+1];
      else if(strArg[arg].compare("-b")==0)
	bunch_offset = intArg[arg+1];
      else if(strArg[arg].compare("-c")==0)
	coarse_offset = intArg[arg+1];
      else if(strArg[arg].compare("-x")==0)
	coarse_rollover = intArg[arg+1];
      else if(strArg[arg].compare("-s")==0)
	thresh_start = intArg[arg+1];
      else if(strArg[arg].compare("-t")==0)
	thresh_stop = intArg[arg+1];
      else if(strArg[arg].compare("-d")==0)
	thresh_delta = intArg[arg+1];
      else if(strArg[arg].compare("-p")==0)
        channel = intArg[arg+1];
      else if(strArg[arg].compare("-f")==0)
	{
	  sprintf(file_name_buffer,"%s%s", save_dir, strArg[arg+1].c_str());
	  sweep_file = fopen(file_name_buffer, "w");
	}
      else
	{
	  printf("Error: invalid argument: %s %s\n", strArg[arg].c_str(), strArg[arg+1].c_str());
	  if (sweep_file!= NULL)
	    fclose(sweep_file);
	  return 0;
	}
    }
	      
  if (sweep_file == NULL)
    {
      printf("ERROR: must give a file destination\n");
      return 0;
    }

  fprintf(sweep_file,
	  "# Number of sweeps:%d\n"
	  "# Match window:%d\n"
	  "# Search window:%d\n"
	  "# Reject offset:%d\n"
	  "# Bunch offset:%d\n"
	  "# Coarse offset:%d\n"
	  "# Coarse rollover:%d\n"
	  "# Threshold: [%d, %d, %d]\n"
	  "# Channel:%d\n",
	  num_sweeps, match_window, match_window+8, reject_offset,
	  bunch_offset, coarse_offset, coarse_rollover,
	  thresh_start, thresh_stop, thresh_delta, channel);
  
  fprintf(sweep_file,"#thr\thits\tfreq\twindow\terror\n");
  
  mezzTester->Board.SetTDCReg(MATCH_WINDOW, match_window);
  mezzTester->Board.SetTDCReg(SEARCH_WINDOW, match_window+8);
  mezzTester->Board.SetTDCReg(REJECT_OFFSET, reject_offset);
  mezzTester->Board.SetTDCReg(BUNCH_OFFSET, bunch_offset);
  mezzTester->Board.SetTDCReg(COARSE_OFFSET, coarse_offset);
  mezzTester->Board.SetTDCReg(COUNT_ROLLOVER, coarse_rollover);
  mezzTester->Board.SetChannel(channel);
  mezzTester->Board.SetASDReg(CAL_INJECT_MASK, 0);
  mezzTester->Board.SetASDReg(CAL_INJ_CAPS, 0);
  mezzTester->Board.SetStrobePulsePeriod(0);
  mezzTester->Board.SetChannelMask(0);
  mezzTester->Board.UpdateBoard();

  printf("Channel: %d\n", channel);

  int match_og = match_window;
  int i_max;
  for (int thresh=thresh_start; thresh<thresh_stop; thresh+=thresh_delta)
    {
      mezzTester->Board.SetASDReg(DISC1_THR, thresh);
      mezzTester->Board.UpdateASD();

      runhits = 0;

      match_window = match_og;
      i_max = num_sweeps;
      
      mezzTester->Board.SetTDCReg(MATCH_WINDOW, match_window);
      mezzTester->Board.SetTDCReg(SEARCH_WINDOW, match_window+8);
      mezzTester->Board.UpdateTDC();      

      for (int i=0; i<i_max;)
  	{
  	  while(mezzTester->Board.FIFOFlags() == FIFO_EMPTY)
  	    {
  	      mezzTester->Board.TDCcmd(TRIGGER);
  	    }
  	  mezzTester->getReadout();
	  if ((mezzTester->HitReadout.errorflags & 
	       READOUT_FIFO_OVERFLOW_ERROR) ||
	      (mezzTester->HitReadout.errorflags & 
	       L1_BUFFER_OVERFLOW_ERROR))
	    {
	      match_window /= 2;
	      if (match_window == 0)
		mezzTester->Board.SetTDCReg(SEARCH_WINDOW, match_window);
	      else
		mezzTester->Board.SetTDCReg(SEARCH_WINDOW, match_window+8);
	      mezzTester->Board.SetTDCReg(MATCH_WINDOW, match_window);
	      mezzTester->Board.UpdateTDC();   

	      // if (match_window < 10)
	      // 	{
	      // 	  printf("ERROR: match window <10\n");
	      // 	  printf("Match window was set to %d\n", match_window);
	      // 	  printf("index: %d\n", i);
	      // 	  break;
	      // 	}
	      // if (match_window > 399)
	      // 	match_window = 399;
	      // else 
	      printf("Match window set to %d\n", match_window);
	      if (match_window == 0)
		break;
	      // i_max *= 2;
	      i = 0;
	      runhits = 0;
	      sleep(.001);
	      continue;
	    }	 
	  runhits += mezzTester->HitReadout.numHits;
	  mezzTester->saveHits();
	  i++;
	}
      // runhits = mezzTester->getTotalHits();
      rate = (float)runhits/(TDC_CLK*(match_window+1)*num_sweeps);

      fprintf(sweep_file, "%0d\t%0d\t%g\t%d\t", 2*(thresh-127), runhits, 
	      rate, match_window);

      if(runhits != 0)
	fprintf(sweep_file,"%g\n", sqrt(runhits)/runhits*rate);
      else
	fprintf(sweep_file,"0\n");

      printf("%0d\t%0d\t%g\t%d\t", 2*(thresh-127), runhits, 
	     rate, match_window);

      if(runhits != 0)
	printf("%g\n", sqrt(runhits)/runhits*rate);
      else
	printf("0\n");
    }
  fclose(sweep_file);

  return 0;
}

int AMC13_Launcher::tsweep(std::vector<std::string> strArg,
			   std::vector<uint64_t> intArg)
{
  int num_sweeps = 100;
  int match_window = 1999;
  int thresh_start = 100;
  int thresh_stop = 140;
  int thresh_delta = 2;
  int channel = 0;
  char file_name_buffer[100];
  FILE* sweep_file = NULL;
  bool recording = false;
  int runhits;
  float rate;
 
  if ((strArg.size()%2==1) && (strArg.size() > 2))
    {
      printf("Error: unmatched arguments\n");
      return 0;
    }

  for (size_t arg=0; arg < strArg.size(); arg+=2)
    {
      if (strArg[arg].compare("-h")==0) {
	printf("usage (default):\n"
	       "-h\tdisplays this message\n"
	       "-n\tnumber of sweeps (100)\n"
	       "-p\tchannel (0)\n"
	       "-m\tmatch window (1999)\n"
	       "-s\tthreshold start (100)\n"
	       "-t\tthreshold stop (140)\n"
	       "-d\tthreshold delta (2)\n"
	       "-f\tsave file in sweep folder \n");
	return 0;
      }
      else if(strArg[arg].compare("-n")==0)
	num_sweeps = intArg[arg+1];
      else if(strArg[arg].compare("-m")==0)
	match_window = intArg[arg+1];
      else if(strArg[arg].compare("-s")==0)
	thresh_start = intArg[arg+1];
      else if(strArg[arg].compare("-t")==0)
	thresh_stop = intArg[arg+1];
      else if(strArg[arg].compare("-d")==0)
	thresh_delta = intArg[arg+1];
      else if(strArg[arg].compare("-p")==0)
        channel = intArg[arg+1];
      else if(strArg[arg].compare("-f")==0)
	{
	  sprintf(file_name_buffer,"%s%s", save_dir, strArg[arg+1].c_str());
	  sweep_file = fopen(file_name_buffer, "w");
	  recording = true;
	}
      else
	{
	  printf("Error: invalid argument: %s %s\n", strArg[arg].c_str(), strArg[arg+1].c_str());
	  if (sweep_file!= NULL)
	    fclose(sweep_file);
	  return 0;
	}
    }

  if (recording)
    fprintf(sweep_file,
	    "# Number of sweeps:%d\n"
	    "# Match window:%d\n"
	    "# Search window:%d\n"
	    "# Reject offset:%d\n"
	    "# Bunch offset:%d\n"
	    "# Coarse rollover:%d\n"
	    "# Threshold: [%d, %d, %d]\n"
	    "# Channel:%d\n",
	    num_sweeps, 
	    mezzTester->Board.GetTDCReg(MATCH_WINDOW), 
	    mezzTester->Board.GetTDCReg(SEARCH_WINDOW),
	    mezzTester->Board.GetTDCReg(REJECT_OFFSET),
	    mezzTester->Board.GetTDCReg(BUNCH_OFFSET),
	    mezzTester->Board.GetTDCReg(COUNT_ROLLOVER),
	    thresh_start, thresh_stop, thresh_delta, channel);
  
  if (recording)
    fprintf(sweep_file,"#thr\thits\tfreq\twindow\terror\n");
  
  mezzTester->SetWindow(match_window);
  mezzTester->Board.SetASDReg(CAL_INJECT_MASK, 0);
  mezzTester->Board.SetASDReg(CAL_INJ_CAPS, 0);
  mezzTester->Board.SetStrobePulsePeriod(0);
  mezzTester->Board.SetChannelMask(0);
  mezzTester->Board.SetChannel(channel);
  mezzTester->Board.UpdateBoard();
  mezzTester->ResetTDC();

  printf("Channel: %d\n", channel);

  int match_og = match_window;

  for (int thresh=thresh_start; thresh<thresh_stop; thresh+=thresh_delta)
    {
      mezzTester->Board.SetASDReg(DISC1_THR, thresh);
      mezzTester->Board.UpdateASD();

      runhits = 0;

      match_window = match_og;
      
      mezzTester->SetWindow(match_window);
      mezzTester->Board.UpdateTDC();    
      mezzTester->ResetTDC();  

      int i = 0;
      while(i<num_sweeps)
  	{
  	  while(mezzTester->Board.FIFOFlags() == FIFO_EMPTY)
  	    {
  	      mezzTester->Board.TDCcmd(TRIGGER);
  	    }
  	  mezzTester->getReadout(READOUT_FIFO_OVERFLOW_ERROR | L1_BUFFER_OVERFLOW_ERROR);
	  if ((mezzTester->HitReadout.errorflags & 
	       READOUT_FIFO_OVERFLOW_ERROR) ||
	      (mezzTester->HitReadout.errorflags & 
	       L1_BUFFER_OVERFLOW_ERROR))
	    {
	      if (match_window == 0)
		{
		  printf("Error: match window = 0\n");
		  sleep(3);
		  break;
		}
	      //match_window /= 2;
	      match_window > 199 ? (match_window = 199) : (match_window /= 2);
	      mezzTester->SetWindow(match_window);
	      mezzTester->Board.UpdateTDC();   
	      mezzTester->ResetTDC();  

	      //printf("Match window set to %d\n", match_window);

	      i = 0;
	      runhits = 0;
	      sleep(.001);
	      // this break is for only recording sweeps of the full window
	      // break;
	    }	 
	  runhits += mezzTester->HitReadout.numHits;
	  mezzTester->saveHits();
	  i++;
	}

      rate = (float)runhits/(TDC_CLK*(match_window+1)*num_sweeps);

      if (recording)
	{
	  fprintf(sweep_file, "%0d\t%0d\t%g\t%d\t", 2*(thresh-127), runhits, 
		  rate, match_window);
	  if(runhits != 0)
	    fprintf(sweep_file,"%g\n", sqrt(runhits)/runhits*rate);
	  else
	    fprintf(sweep_file,"0\n");
	}

      printf("%0d\t%0d\t%g\t%d\t", 2*(thresh-127), runhits, 
	     rate, match_window);
      if(runhits != 0)
	printf("%g\n", sqrt(runhits)/runhits*rate);
      else
	printf("0\n");
    }
  if (recording)
    fclose(sweep_file);

  return 0;
}

int AMC13_Launcher::fsweep(std::vector<std::string> strArg,
			   std::vector<uint64_t> intArg)
{
  int num_sweeps = 1000;
  int match_window = 1999;
  int thresh_start = 90;
  int thresh_stop = 160;
  int thresh_delta = 1;
  int channel = 0;
  char file_name_buffer[100];
  FILE* sweep_file = NULL;
  bool recording = false;
  int runhits;
  float rate;
  bool verbose = false;
 
  if ((strArg.size()%2==1) && (strArg.size() > 2))
    {
      printf("Error: unmatched arguments\n");
      return 0;
    }

  for (size_t arg=0; arg < strArg.size(); arg+=2)
    {
      if (strArg[arg].compare("-h")==0) {
	printf("usage (default):\n"
	       "-h\tdisplays this message\n"
	       "-n\tnumber of sweeps (1000)\n"
	       "-p\tchannel (0)\n"
	       "-m\tmatch window (1999)\n"
	       "-s\tthreshold start (90)\n"
	       "-t\tthreshold stop (160)\n"
	       "-d\tthreshold delta (1)\n"
	       "-f\tsave file in sweep folder \n"
	       "-v\tverbose (must give dummy argument)\n");
	return 0;
      }
      else if(strArg[arg].compare("-n")==0)
	num_sweeps = intArg[arg+1];
      else if(strArg[arg].compare("-m")==0)
	match_window = intArg[arg+1];
      else if(strArg[arg].compare("-s")==0)
	thresh_start = intArg[arg+1];
      else if(strArg[arg].compare("-t")==0)
	thresh_stop = intArg[arg+1];
      else if(strArg[arg].compare("-d")==0)
	thresh_delta = intArg[arg+1];
      else if(strArg[arg].compare("-p")==0)
        channel = intArg[arg+1];
      else if(strArg[arg].compare("-v")==0)
        verbose = true;
      else if(strArg[arg].compare("-f")==0)
	{
	  sprintf(file_name_buffer,"%s%s", save_dir, strArg[arg+1].c_str());
	  sweep_file = fopen(file_name_buffer, "w");
	  recording = true;
	}
      else
	{
	  printf("Error: invalid argument: %s %s\n", strArg[arg].c_str(), strArg[arg+1].c_str());
	  if (sweep_file!= NULL)
	    fclose(sweep_file);
	  return 0;
	}
    }
  
  if (recording)
    fprintf(sweep_file,"#thr\thits\tfreq\twindow\terror\n");
  
  mezzTester->SetWindow(match_window);
  mezzTester->Board.SetChannel(channel);
  mezzTester->Board.SetASDReg(CAL_INJECT_MASK, 0);
  mezzTester->Board.SetASDReg(CAL_INJ_CAPS, 0);
  mezzTester->Board.SetASDReg(CHIP_MODE, 1);
  mezzTester->Board.SetStrobePulsePeriod(0);
  mezzTester->Board.SetChannelMask(0);
  mezzTester->Board.SetAllDAC(0);
  mezzTester->Board.UpdateBoard();
  mezzTester->ResetTDC();

  if (recording)
    fprintf(sweep_file,
	    "# Number of swees:%d\n"
	    "# Fast sweep mode\n"
	    "# Match window:%d\n"
	    "# Search window:%d\n"
	    "# Reject offset:%d\n"
	    "# Bunch offset:%d\n"
	    "# Coarse rollover:%d\n"
	    "# Threshold: [%d, %d, %d]\n"
	    "# Channel:%d\n",
	    num_sweeps, 
	    mezzTester->Board.GetTDCReg(MATCH_WINDOW), 
	    mezzTester->Board.GetTDCReg(SEARCH_WINDOW),
	    mezzTester->Board.GetTDCReg(REJECT_OFFSET),
	    mezzTester->Board.GetTDCReg(BUNCH_OFFSET),
	    mezzTester->Board.GetTDCReg(COUNT_ROLLOVER),
	    thresh_start, thresh_stop, thresh_delta, channel);

  if (verbose)
    printf("# Number of sweeps:%d\n"
	   "# Fast sweep mode\n"
	   "# Match window:%d\n"
	   "# Search window:%d\n"
	   "# Reject offset:%d\n"
	   "# Bunch offset:%d\n"
	   "# Coarse rollover:%d\n"
	   "# Threshold: [%d, %d, %d]\n"
	   "# Channel:%d\n",
	   num_sweeps, 
	   mezzTester->Board.GetTDCReg(MATCH_WINDOW), 
	   mezzTester->Board.GetTDCReg(SEARCH_WINDOW),
	   mezzTester->Board.GetTDCReg(REJECT_OFFSET),
	   mezzTester->Board.GetTDCReg(BUNCH_OFFSET),
	   mezzTester->Board.GetTDCReg(COUNT_ROLLOVER),
	   thresh_start, thresh_stop, thresh_delta, channel);

  printf("Channel %d threshold sweep\n", channel);

  int match_og = match_window;
  char inbuf[25];
  char outbuf[10];
  int token;

  int thresh=thresh_start;
  while (thresh < thresh_stop)
    {
      mezzTester->Board.SetASDReg(DISC1_THR, thresh);
      mezzTester->Board.UpdateASD();

      runhits = 0;

      mezzTester->SetWindow(match_window);
      mezzTester->Board.UpdateTDC();    
      mezzTester->ResetTDC();  

      sprintf(outbuf, "ts %04X", num_sweeps);
      mezzTester->Board.serial.Writeln(outbuf,false);
      mezzTester->Board.serial.Readln(inbuf, 25);
      token = strtol(inbuf+1, NULL, 16);

      if (inbuf[0]=='E')
	{
	  mezzTester->printTDCError(token, READOUT_FIFO_OVERFLOW_ERROR | 
				    L1_BUFFER_OVERFLOW_ERROR);
	  if ((token & READOUT_FIFO_OVERFLOW_ERROR) ||
  	      (token & L1_BUFFER_OVERFLOW_ERROR))
  	    {
  	      if (match_window == 0)
  		{
  		  printf("Error: match window = 0\n");
  		  sleep(3);
  		  continue;
  		}
	      match_window > 199 ? (match_window = 199) : (match_window /= 2);
  	      continue;
  	    }	 
	}
      else if (inbuf[0]=='I')
	{
	  printf("Error: event ID mismatch (4 LSBits)\n"
		 "Header: %d Trailer: %d\n", 
		 (token & 0xFF00) >> 8,
		 token & 0x00FF);
	}
      else if (inbuf[0]=='N')
	  runhits = token;
      else
	  printf("Error: invalid repsonse from \"%s\": %s\n", outbuf, inbuf);

      rate = (float)runhits/(TDC_CLK*(match_window+1)*num_sweeps);

      if (recording)
	{
	  fprintf(sweep_file, "%0d\t%0d\t%g\t%d\t", 2*(thresh-127), runhits, 
		  rate, match_window);
	  if(runhits != 0)
	    fprintf(sweep_file,"%g\n", sqrt(runhits)/runhits*rate);
	  else
	    fprintf(sweep_file,"0\n");
	}
      
      if (verbose)
	{
	  printf("%0d\t%0d\t%g\t%d\t", 2*(thresh-127), runhits, 
		 rate, match_window);
	  if(runhits != 0)
	    printf("%g\n", sqrt(runhits)/runhits*rate);
	  else
	    printf("0\n");
	}

      match_window = match_og;
      
      thresh+=thresh_delta;
    }
  if (recording)
    fclose(sweep_file);

  return 0;
}

int AMC13_Launcher::dac_sweep(std::vector<std::string> strArg,
			      std::vector<uint64_t> intArg)
{
  char file_name_buffer[100];
  int num_sweeps = 50;
  int match_window = 1999;
  int thresh = 180;
  int thresh_delta = 1;
  int thresh_stop = 255;
  int dac = 0;
  int dac_delta = 50;
  int channel = 0;
  int channelmask = 1;
  int hit_period = 100;
  bool recording = false;
  
  FILE * dacfile = NULL;

  if ((strArg.size()%2==1) && (strArg.size() > 2))
    {
      printf("Error: unmatched arguments\n");
      return 0;
    }

  for (size_t arg=0; arg < strArg.size(); arg+=2)
    {
      if (strArg[arg].compare("-h")==0) {
	printf("usage (default):\n"
	       "-h\tdisplays this message\n"
	       "-n\tnumber of empty triggers required (50)\n"
	       "-i\thit delay (100)\n"
	       "-p\tchannel mask (1)\n"
	       "-c\tchannel (0)\n"
	       "-m\tmatch window (1999)\n"
	       "-t\tthreshold initial (180)\n"
	       "-s\tthreshold step (1)\n"
	       "-d\tdac initial (0)\n"
	       "-x\tdac step (50)\n"
	       "-f\tsave file in sweep folder(dac.txt)\n");
	return 0;
      }
      else if(strArg[arg].compare("-n")==0)
	num_sweeps = intArg[arg+1];
      else if(strArg[arg].compare("-i")==0)
	hit_period = intArg[arg+1];
      else if(strArg[arg].compare("-m")==0)
	match_window = intArg[arg+1];
      else if(strArg[arg].compare("-t")==0)
	thresh = intArg[arg+1];
      else if(strArg[arg].compare("-s")==0)
	thresh_delta = intArg[arg+1];
      else if(strArg[arg].compare("-e")==0)
	thresh_stop = intArg[arg+1];
      else if(strArg[arg].compare("-d")==0)
	dac = intArg[arg+1];
      else if(strArg[arg].compare("-x")==0)
	dac_delta = intArg[arg+1];
      else if(strArg[arg].compare("-p")==0)
        channelmask = intArg[arg+1];
      else if(strArg[arg].compare("-c")==0)
	channel = intArg[arg+1];
      else if(strArg[arg].compare("-f")==0)
	{
	  sprintf(file_name_buffer, "%s%s", save_dir, strArg[arg+1].c_str());
	  dacfile = fopen(file_name_buffer, "w");
	  fprintf(dacfile, "#num_sweeps:%d\n", num_sweeps);
	  fprintf(dacfile, "#thresh\tdac\n");
	  recording = true;
	}
      else
	{
	  printf("Error: invalid argument: %s\n", strArg[arg].c_str());
	  if (dacfile != NULL)
	    fclose(dacfile);
	  return 0;
	}
    }

  printf("Channel: %d\n", channel);
  printf("thresh\tdac\n");

  mezzTester->Board.SetHitPeriod(hit_period);
  mezzTester->Board.SetASDReg(CAL_INJECT_MASK, 0);
  mezzTester->Board.SetASDReg(CAL_INJ_CAPS, 0);
  mezzTester->Board.SetStrobePulsePeriod(0);
  mezzTester->SetWindow(match_window);
  mezzTester->Board.SetChannel(channel);
  mezzTester->Board.SetChannelMask(channelmask);

  mezzTester->Board.UpdateBoard();
  mezzTester->ResetTDC();

  int i;
  while(thresh < thresh_stop)
    {
      mezzTester->Board.SetAllDAC(dac);
      mezzTester->Board.UpdateDAC();
      i = 0;
      while (i < num_sweeps)
	{
	  mezzTester->Board.SetASDReg(DISC1_THR, thresh);
	  mezzTester->Board.UpdateASD();

	  while(mezzTester->Board.FIFOFlags() == FIFO_EMPTY)
	    mezzTester->Board.TDCcmd(TRIGGER_W_PULSE);
	  if (mezzTester->getReadout() > NO_HITS)
	    {
	      thresh += thresh_delta;
	      if (thresh >255)
	        break;
	      i=0;
	      continue;
	    }

	  i++;
	}
      if (recording)
	fprintf(dacfile, "%d\t%d\n", 2*(thresh-127), dac);
      printf("%d\t%d\n", thresh, dac);
      dac += dac_delta;
      if (dac > 4095)
	{
	  printf("Error: dac = %d, value to high\n", dac);
	  break;
	}
    }
  if (recording)
    fclose(dacfile);
  return 0;
}

int AMC13_Launcher::fdac_sweep(std::vector<std::string> strArg,
			       std::vector<uint64_t> intArg)
{
  char file_name_buffer[100];
  int num_sweeps = 50;
  int match_window = 1999;
  int thresh = 170;
  int thresh_delta = 1;
  int thresh_stop = 255;
  int dac = 0;
  int dac_delta = 50;
  int channel = 0;
  int channelmask = 1;
  int hit_period = 100;
  bool recording = false;
  
  FILE * dacfile = NULL;

  if ((strArg.size()%2==1) && (strArg.size() > 2))
    {
      printf("Error: unmatched arguments\n");
      return 0;
    }

  for (size_t arg=0; arg < strArg.size(); arg+=2)
    {
      if (strArg[arg].compare("-h")==0) {
	printf("usage (default):\n"
	       "-h\tdisplays this message\n"
	       "-n\tnumber of empty triggers required (50)\n"
	       "-i\thit delay (100)\n"
	       "-p\tchannel mask (1)\n"
	       "-c\tchannel (0)\n"
	       "-m\tmatch window (1999)\n"
	       "-t\tthreshold initial (180)\n"
	       "-s\tthreshold step (1)\n"
	       "-d\tdac initial (0)\n"
	       "-x\tdac step (50)\n"
	       "-f\tsave file in sweep folder(dac.txt)\n");
	return 0;
      }
      else if(strArg[arg].compare("-n")==0)
	num_sweeps = intArg[arg+1];
      else if(strArg[arg].compare("-i")==0)
	hit_period = intArg[arg+1];
      else if(strArg[arg].compare("-m")==0)
	match_window = intArg[arg+1];
      else if(strArg[arg].compare("-t")==0)
	thresh = intArg[arg+1];
      else if(strArg[arg].compare("-s")==0)
	thresh_delta = intArg[arg+1];
      else if(strArg[arg].compare("-e")==0)
	thresh_stop = intArg[arg+1];
      else if(strArg[arg].compare("-d")==0)
	dac = intArg[arg+1];
      else if(strArg[arg].compare("-x")==0)
	dac_delta = intArg[arg+1];
      else if(strArg[arg].compare("-p")==0)
        channelmask = intArg[arg+1];
      else if(strArg[arg].compare("-c")==0)
	channel = intArg[arg+1];
      else if(strArg[arg].compare("-f")==0)
	{
	  sprintf(file_name_buffer, "%s%s", save_dir, strArg[arg+1].c_str());
	  dacfile = fopen(file_name_buffer, "w");
	  fprintf(dacfile, "#num_sweeps:%d\n", num_sweeps);
	  fprintf(dacfile, "#thresh\tdac\n");
	  recording = true;
	}   
      else
	{
	  printf("Error: invalid argument: %s\n", strArg[arg].c_str());
	  if (dacfile != NULL)
	    fclose(dacfile);
	  return 0;
	}
    }
  
  printf("Channel dac sweep: %d\t", channel);
  //printf("thresh\tdac\n");
  
  mezzTester->Board.SetHitPeriod(hit_period);
  mezzTester->Board.SetASDReg(CAL_INJECT_MASK, 0);
  mezzTester->Board.SetASDReg(CAL_INJ_CAPS, 0);
  mezzTester->Board.SetStrobePulsePeriod(0);
  mezzTester->SetWindow(match_window);
  mezzTester->Board.SetChannel(channel);
  mezzTester->Board.SetChannelMask( channelmask);
  (void) channelmask;

  mezzTester->Board.UpdateBoard();
  mezzTester->ResetTDC();

  mezzTester->Board.SetAllDAC(dac);
  mezzTester->Board.UpdateDAC();

  int runhits;
  char outbuf[25];
  char inbuf[25];
  int token;
  while(thresh < thresh_stop)
    {
      mezzTester->Board.SetASDReg(DISC1_THR, thresh);
      mezzTester->Board.UpdateASD();

      runhits = 0;

      sprintf(outbuf, "ts %04X 4", num_sweeps);
      mezzTester->Board.serial.Writeln(outbuf,false);
      mezzTester->Board.serial.Readln(inbuf, 25);
      token = strtol(inbuf+1, NULL, 16);
      if (inbuf[0]=='E')
	{
	  mezzTester->printTDCError(token);
	  continue; 
	}
      else if (inbuf[0]=='I')
	{
	  printf("Error: event ID mismatch (4 LSBits)\n"
		 "Header: %d Trailer: %d\n", 
		 (token & 0xFF00) >> 8,
		 token & 0x00FF);
	}
      else if (inbuf[0]=='N')
	  runhits = token;
      else
	  printf("Error: invalid repsonse from \"ts\": %s\n", inbuf);
      
      if (runhits > 0)
	{
	  thresh += thresh_delta;
	  continue;
	}
      if (recording)
	fprintf(dacfile, "%d\t%d\n", 2*(thresh-127), dac);
      // printf("%d\t%d\n", thresh, dac);
      dac += dac_delta;
      mezzTester->Board.SetAllDAC(dac);
      mezzTester->Board.UpdateDAC();

      if (dac > 4095)
	{
	  //printf("Error: dac = %d, value to high\n", dac);
	  printf("Error: %d\t%d\n", thresh, dac);
	  break;
	}
    } // end while loop
  if (dac < 4095)
    printf("%d\t%d\n", thresh, dac);
  if (recording)
    fclose(dacfile);
  return 0;
}

int AMC13_Launcher::Trigger(std::vector<std::string> strArg,
			    std::vector<uint64_t> intArg)
{
  if (strArg.size() > 1)
    {
      printf("usage:\n"
	     "trig [n]\n"
	     "where n in number of triggers (default 1)\n");
      return 0;
    }

  int i_max;
  if (intArg.size() == 0)
    i_max = 1;
  else
    i_max = intArg[0];

  for (int i=0; i<i_max; i++)
    {
      while(mezzTester->Board.FIFOFlags() == FIFO_EMPTY)
	mezzTester->Board.TDCcmd(TRIGGER_W_PULSE);
      mezzTester->getReadout();
      printf("--------------------------------------"
	     "------------------------------------\n");
      mezzTester->printTDCHits();
      mezzTester->printTDCStatus();
      mezzTester->saveHits();
    }
  return 0; 
}

int AMC13_Launcher::jtw(std::vector<std::string> strArg,
			std::vector<uint64_t> intArg)
{
  if (intArg.size() != 2)
    {
      printf("usage:\n"
	     "jtw n xxx\n"
	     "n = tdc register to read\n"
	     "xxx = register value\n");
      return 0;
    }
  // int arg1 = strtol(strArg[0].c_str(), NULL, 16);
  // int arg2 = strtol(strArg[1].c_str(), NULL, 16);
  // mezzTester->Board.SetTDCReg(arg1, arg2);
  mezzTester->Board.SetTDCReg(intArg[0], intArg[1]);
  return 0;
}

int AMC13_Launcher::jaw(std::vector<std::string> strArg,
			std::vector<uint64_t> intArg)
{
  if (intArg.size() != 2)
    {
      printf("usage:\n"
	     "jaw n xxx\n"
	     "n = asd register to write\n"
	     "xxx = register value\n");
      return 0;
    }
  // int arg1 = strtol(strArg[0].c_str(), NULL, 16);
  // int arg2 = strtol(strArg[1].c_str(), NULL, 16);
  // mezzTester->Board.SetASDReg(arg1, arg2);
  mezzTester->Board.SetASDReg(intArg[0], intArg[1]);
  return 0;
}

int AMC13_Launcher::jtr(std::vector<std::string> strArg,
			std::vector<uint64_t> intArg)
{
  if (intArg.size() != 1)
    {
      printf("usage:\n"
	     "jtr n\n"
	     "n = tdc register to read\n");
      return 0;
    }
  // int arg1 = strtol(strArg[0].c_str(), NULL, 16);
  // printf("%03X\n",mezzTester->Board.GetTDCReg(arg1));
  printf("%d\n", mezzTester->Board.GetTDCReg(intArg[0]));
  return 0;
}

int AMC13_Launcher::jar(std::vector<std::string> strArg,
			std::vector<uint64_t> intArg)
{
  if (intArg.size() != 1)
    {
      printf("usage:\n"
	     "jar n\n"
	     "n = asd register to read\n");
      return 0;
    }
  // int arg1 = strtol(strArg[0].c_str(), NULL, 16);
  // printf("%03X\n",mezzTester->Board.GetASDReg(arg1));
  printf("%d\n", mezzTester->Board.GetASDReg(intArg[0]));
  return 0;
}

int AMC13_Launcher::UpdateBoard(std::vector<std::string> strArg,
				std::vector<uint64_t> intArg)
{
  mezzTester->Board.UpdateBoard();
  return 0;
}

int AMC13_Launcher::SetChannelMask(std::vector<std::string> strArg,
				   std::vector<uint64_t> intArg)
{
  if (intArg.size() > 0)
    mezzTester->Board.SetChannelMask(intArg[0]);
  return 0;
}

int AMC13_Launcher::SetChannel(std::vector<std::string> strArg,
			       std::vector<uint64_t> intArg)
{
  if (intArg.size() > 0)
    mezzTester->Board.SetChannel(intArg[0]);
  return 0;
}

int AMC13_Launcher::SetHitPeriod(std::vector<std::string> strArg,
				 std::vector<uint64_t> intArg)
{
  if (intArg.size() > 0)
    mezzTester->Board.SetHitPeriod(intArg[0]);
  return 0;
}

int AMC13_Launcher::SetStrobePulsePeriod(std::vector<std::string> strArg,
					 std::vector<uint64_t> intArg)
{
  if (intArg.size() > 0)
    mezzTester->Board.SetStrobePulsePeriod(intArg[0]);
  return 0;
}

int AMC13_Launcher::load_test(std::vector<std::string> strArg,
			      std::vector<uint64_t> intArg)
{
  mezzTester->Board.SetASDReg(CHIP_MODE, 1);
  mezzTester->Board.SetASDReg(CAL_INJECT_MASK, 0xFF);
  mezzTester->Board.SetASDReg(CAL_INJ_CAPS, 0x07);
  mezzTester->Board.SetASDReg(DISC1_THR, 40);

  mezzTester->Board.SetChannelMask(0x00);
  mezzTester->Board.SetChannel(0);
  mezzTester->Board.SetStrobePulsePeriod(19);

  mezzTester->Board.UpdateBoard();

  return 0;
}

int AMC13_Launcher::dump(std::vector<std::string> strArg,
			 std::vector<uint64_t> intArg)
{
  int i;
  printf("TDC:\n");
  for (i = 0; i<9; i++)
    printf("\t%d:\t%d\n", i, mezzTester->Board.GetTDCReg(i));
  for (i = 9; i<15; i++)
    printf("\t%d:\t%03X\n", i, mezzTester->Board.GetTDCReg(i));
  if (strArg.size() > 0)
    {
      printf("ASD:\n");
      for (i = 0; i<ASD_REG_NUM; i++)
	printf("\t%d:\t%d\n", i, mezzTester->Board.GetASDReg(i));
      printf("DAC:%d\n"
	     "HP :%d\n"
	     "SP :%d\n"
	     "channel mask: %06X\n",
	     mezzTester->Board.GetDAC(0),
	     mezzTester->Board.GetHitPeriod(),
	     mezzTester->Board.GetStrobePulsePeriod(),
	     mezzTester->Board.GetChannelMask());
    }
  return 0;
}

int AMC13_Launcher::mw(std::vector<std::string> strArg,
		       std::vector<uint64_t> intArg)
{
  if (intArg.size() > 0)
    mezzTester->SetWindow(intArg[0]);
  return 0;
}

int AMC13_Launcher::bo(std::vector<std::string> strArg,
		       std::vector<uint64_t> intArg)
{
  if (intArg.size() > 0)  
    {
      mezzTester->Board.SetTDCReg(BUNCH_OFFSET, intArg[0]);
      mezzTester->Board.SetTDCReg(REJECT_OFFSET, intArg[0] - 8);
    }
  return 0;
}

int AMC13_Launcher::treset(std::vector<std::string> strArg,
			   std::vector<uint64_t> intArg)
{
  mezzTester->ResetTDC();
  return 0;
}

int AMC13_Launcher::d(std::vector<std::string> strArg,
		      std::vector<uint64_t> intArg)
{
  if (intArg.size() > 0)
    mezzTester->Board.SetAllDAC(intArg[0]);
  return 0;
}

int AMC13_Launcher::load_inject(std::vector<std::string> strArg,
				std::vector<uint64_t> intArg)
{
  mezzTester->Board.SetStrobePulsePeriod(0x00);
  mezzTester->Board.SetHitPeriod(100);
  mezzTester->SetWindow(1999);
  mezzTester->Board.SetChannel(0);
  mezzTester->Board.SetChannelMask(1);
  mezzTester->Board.SetASDReg(DISC1_THR, 170);
  mezzTester->Board.UpdateBoard();
  mezzTester->ResetTDC();
  return 0;
}

int AMC13_Launcher::jtag_test(std::vector<std::string> strArg,
			      std::vector<uint64_t> intArg)
{
  bool verbose = false;
  if (intArg.size() > 0)
    verbose = true;

  puts("------------------------------------------------------");
  puts("Testing JTAG");

  if (mezzTester->Board.TDC_JTAG_test(verbose))
    printf("TDC JTAG test....passed!\n");
  else 
    printf("TDC JTAG test....FAILED!\n");

  if (mezzTester->Board.TDC_ID_test(verbose))
    printf("TDC ID test....passed!\n");
  else 
    printf("TDC ID test....FAILED!\n");

  if (mezzTester->Board.ASD_JTAG_test(verbose))
    printf("ASD JTAG test....passed!\n");
  else 
    printf("ASD JTAG test....FAILED!\n");

  if (mezzTester->Board.ASD_TDC_test(verbose))
    printf("ASD TDC connectivity test....passed!\n");
  else 
    printf("ASD JTAG connectivity....FAILED!\n");

  return 0;
}

int AMC13_Launcher::TDC_status(std::vector<std::string> strArg,
			       std::vector<uint64_t> intArg)
{
  mezzTester->printTDCStatus(true);
  return 0;
}

int AMC13_Launcher::strobe_test(std::vector<std::string> strArg,
				std::vector<uint64_t> intArg)
{
  int num_sweeps = 1000;
  int match_window = 1999;
  int thresh = 0;
  int strobe_period = 99; 

  if ((strArg.size()%2==1) && (strArg.size() > 2))
    {
      printf("Error: unmatched arguments\n");
      return 0;
    }

  for (size_t arg=0; arg < strArg.size(); arg+=2)
    {
      if (strArg[arg].compare("-h")==0) 
	{
	  printf("usage (default):\n"
		 "-h\tdisplays this message\n"
		 "-n\tnumber of triggers run (1000)\n"
		 "-m\tmatch window (1999)\n"
		 "-t\tthreshold (0)\n"
		 "-p\tstrobe pulse period(99)\n");
	  return 0;
	}
      else if(strArg[arg].compare("-n")==0)
	num_sweeps = intArg[arg+1];
      else if(strArg[arg].compare("-m")==0)
	match_window = intArg[arg+1];
      else if(strArg[arg].compare("-t")==0)
	thresh = intArg[arg+1];
      else if(strArg[arg].compare("-p")==0)
        strobe_period = intArg[arg+1];
      else
	{
	  printf("Error: invalid argument: %s\n", strArg[arg].c_str());
	  return 0;
	}
    }
      
  mezzTester->Board.SetStrobePulsePeriod(strobe_period);
  mezzTester->SetWindow(match_window);
  mezzTester->Board.SetChannelMask(0);
  mezzTester->Board.SetASDReg(DISC1_THR, thresh);
  mezzTester->Board.SetASDReg(CAL_INJECT_MASK, 0xFF);
  mezzTester->Board.SetASDReg(CAL_INJ_CAPS, 0x07);
  mezzTester->Board.SetAllDAC(0);
  mezzTester->Board.UpdateBoard();
  mezzTester->ResetTDC();

  puts("------------------------------------------------------");
  puts("Testing ASD strobe");

  int runhits;
  char outbuf[25];
  char inbuf[25];
  int token;
  int expected_hits = (int) floor ( (match_window + 1) / (4 * (strobe_period + 1)) * num_sweeps );
  printf("Expected hits: %d\n", expected_hits);

    for (int chan=0; chan < 24; chan++)
      {
	mezzTester->Board.SetChannel(chan);
	mezzTester->Board.UpdateTDC();
	runhits = 0;

	printf("Channel %d....", chan);

	sprintf(outbuf, "ts %04X 4", num_sweeps);
	mezzTester->Board.serial.Writeln(outbuf,false);
	mezzTester->Board.serial.Readln(inbuf, 25);
	token = strtol(inbuf+1, NULL, 16);
	if (inbuf[0]=='E')
	  {
	    mezzTester->printTDCError(token);
	    continue; 
	  }
	else if (inbuf[0]=='I')
	  {
	    printf("Error: event ID mismatch (4 LSBits)\n"
		   "Header: %d Trailer: %d\n", 
		   (token & 0xFF00) >> 8,
		   token & 0x00FF);
	  }
	else if (inbuf[0]=='N')
	  runhits = token;
	else
	  printf("Error: invalid repsonse from \"ts\": %s\n", inbuf);


	if (runhits != expected_hits)
	  printf("FAILED! Expected hits: %d Counted hits: %d\n", expected_hits, runhits);
	else
	  printf("Passed!\n");
      } // end for loop
  return 0;
}

int AMC13_Launcher::trig_test(std::vector<std::string> strArg,
			      std::vector<uint64_t> intArg)
{
  int num_sweeps = 3;

  if ((strArg.size()%2==1) && (strArg.size() > 2))
    {
      printf("Error: unmatched arguments\n");
      return 0;
    }

  for (size_t arg=0; arg < strArg.size(); arg+=2)
    {
      if (strArg[arg].compare("-h")==0) 
	{
	  printf("usage (default):\n"
		 "-h\tdisplays this message\n"
		 "-n\tnumber of triggers run (3)\n");
	  return 0;
	}
      else if(strArg[arg].compare("-n")==0)
	num_sweeps = intArg[arg+1];
      else
	{
	  printf("Error: invalid argument: %s\n", strArg[arg].c_str());
	  return 0;
	}
    }
      
  mezzTester->Board.SetStrobePulsePeriod(0);
  mezzTester->SetWindow(1999);
  mezzTester->Board.SetChannelMask(0);
  mezzTester->Board.SetASDReg(DISC1_THR, 0);
  mezzTester->Board.SetASDReg(CAL_INJECT_MASK, 0);
  mezzTester->Board.SetASDReg(CAL_INJ_CAPS, 0);
  mezzTester->Board.SetAllDAC(0);
  mezzTester->Board.UpdateBoard();

  puts("------------------------------------------------------");
  puts("Testing trigger resets");

  int firstID, lastID;

  for (int chan=0; chan < 24; chan++)
    {
      mezzTester->Board.SetChannel(chan);
      mezzTester->Board.UpdateTDC();
      mezzTester->ResetTDC();

      printf("Channel %d....", chan);

      // make sure first event id i 0
      while(mezzTester->Board.FIFOFlags() == FIFO_EMPTY)
	mezzTester->Board.TDCcmd(TRIGGER_W_PULSE);
      mezzTester->getReadout();

      firstID = mezzTester->HitReadout.eventID;

      for (int i=0; i<num_sweeps; i++)
	{
	  while(mezzTester->Board.FIFOFlags() == FIFO_EMPTY)
	    mezzTester->Board.TDCcmd(TRIGGER_W_PULSE);
	  mezzTester->getReadout();
	}

      lastID = mezzTester->HitReadout.eventID;

      if ((firstID != 0) || (lastID != num_sweeps))
	printf("FAILED!\n\tfirst ID (should be 0): %d\n\tlast ID (should be %d): %d\n",
	       firstID, num_sweeps, lastID);
      else
	printf("passed!\n");
    }
  return 0;
}
