#include "hcal/amc13/AMC13_Launcher.hh"
#include <cmath>

void AMC13_Launcher::LoadCommandList()
{
  //Help
  List["help"] = &AMC13_Launcher::Help;
  //Quit
  List["quit"] = &AMC13_Launcher::Quit;
  List["exit"] = &AMC13_Launcher::Quit;
  
  //Echo
  List["echo"] = &AMC13_Launcher::Echo;

  // direct access to board cli
  List["cli"]  = &AMC13_Launcher::cli;
  // threshold sweep
  List["tsweep"] = &AMC13_Launcher::tsweep;
  // trigger
  List["trig"] = &AMC13_Launcher::Trigger;
  // register commands
  List["jtw"] = &AMC13_Launcher::jtw;
  List["jaw"] = &AMC13_Launcher::jaw;
  List["jtr"] = &AMC13_Launcher::jtr;
  List["jar"] = &AMC13_Launcher::jar;
  // injector commands1
  List["p"] = &AMC13_Launcher::SetChannelMask;
  List["c"] = &AMC13_Launcher::SetChannel;
  List["hp"] = &AMC13_Launcher::SetHitPeriod;
  List["sp"] = &AMC13_Launcher::SetStrobePulsePeriod;
  // update board
  List["update"] = &AMC13_Launcher::UpdateBoard;
  List["load_test"] = &AMC13_Launcher::load_test;
  List["load_default"] = &AMC13_Launcher::load_default;
 
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
  (void) intArg;
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
  (void) strArg;
  (void) intArg;
  std::map<std::string,
	   int (AMC13_Launcher::*)(std::vector<std::string>,
				   std::vector<uint64_t>)>:: iterator itList;

  printf("Commands:\n");  
  for(itList = List.begin();itList != List.end();itList++)
    {
      printf("   %s\n",itList->first.c_str());
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

void printSweepHelp()
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

int AMC13_Launcher::tsweep(std::vector<std::string> strArg,
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
      printSweepHelp();
      return 0;
    }
  
  if (strArg[0].compare("-h")==0)
    {
      printSweepHelp();
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
	  sprintf(file_name_buffer,"../../sweeps/%s", strArg[arg+1].c_str());
	  sweep_file = fopen(file_name_buffer, "w");
	  fprintf(sweep_file,"thr\thits\tfreq\twindow\terror\n");
	}
    }
	      
  if (sweep_file == NULL)
    {
      printf("ERROR: must give a file destination\n");
      return 0;
    }
  
  mezzTester->Board.SetTDCReg(MATCH_WINDOW, match_window);
  mezzTester->Board.SetTDCReg(SEARCH_WINDOW, match_window+8);
  mezzTester->Board.SetTDCReg(REJECT_OFFSET, reject_offset);
  mezzTester->Board.SetTDCReg(BUNCH_OFFSET, bunch_offset);
  mezzTester->Board.SetTDCReg(COARSE_OFFSET, coarse_offset);
  mezzTester->Board.SetTDCReg(COUNT_ROLLOVER, coarse_rollover);
  mezzTester->Board.SetChannel(channel);
  mezzTester->Board.UpdateBoard();

  printf("Channel: %d\n", channel);

  int match_og = match_window;
  
  for (int thresh=thresh_start; thresh<thresh_stop; thresh+=thresh_delta)
    {
      mezzTester->Board.SetASDReg(DISC1_THR, thresh);
      mezzTester->Board.UpdateASD();

      runhits = 0;

      match_window = match_og;
      mezzTester->Board.SetTDCReg(MATCH_WINDOW, match_window);
      mezzTester->Board.SetTDCReg(SEARCH_WINDOW, match_window+8);
      mezzTester->Board.UpdateTDC();      

      for (int i=0; i<num_sweeps;)
  	{
  	  while(mezzTester->Board.FIFOFlags() == FIFO_EMPTY)
  	    {
  	      mezzTester->Board.TDCcmd(TRIGGER);
  	    }
  	  mezzTester->getReadout();
  	  mezzTester->printTDCStatus();
	  if ((mezzTester->HitReadout.errorflags & 
	       READOUT_FIFO_OVERFLOW_ERROR) ||
	      (mezzTester->HitReadout.errorflags & 
	       L1_BUFFER_OVERFLOW_ERROR))
	    {
	      mezzTester->printTDCError(mezzTester->HitReadout.errorflags);
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

// int AMC13_Launcher::tsweep(std::vector<std::string> strArg,
// 			   std::vector<uint64_t> intArg)
// {
//   int num_sweeps = 100;
//   int match_window = 1000;
//   int reject_offset = 2000;
//   int bunch_offset = 2008;
//   int coarse_offset = 0;
//   int coarse_rollover = 3500;
//   int thresh_start = 90;
//   int thresh_stop = 140;
//   int thresh_delta = 2;
//   int channel = 0;
//   char file_name_buffer[100];
//   FILE* sweep_file = NULL;
//   int runhits;
//   float rate;

//   // can only check for "-h" if there are actually arguments
//   // so this is split into two checks
//   if ((strArg.size() == 0) ||
//       (strArg.size()%2==1))
//     {
//       printSweepHelp();
//       return 0;
//     }
  
//   if (strArg[0].compare("-h")==0)
//     {
//       printSweepHelp();
//       return 0;
//     }

//   for (size_t arg=0; arg < strArg.size(); arg+=2)
//     {
//       if(strArg[arg].compare("-n")==0)
// 	num_sweeps = intArg[arg+1];
//       else if(strArg[arg].compare("-m")==0)
// 	match_window = intArg[arg+1];
//       else if(strArg[arg].compare("-r")==0)
// 	reject_offset = intArg[arg+1];
//       else if(strArg[arg].compare("-b")==0)
// 	bunch_offset = intArg[arg+1];
//       else if(strArg[arg].compare("-c")==0)
// 	coarse_offset = intArg[arg+1];
//       else if(strArg[arg].compare("-x")==0)
// 	coarse_rollover = intArg[arg+1];
//       else if(strArg[arg].compare("-s")==0)
// 	thresh_start = intArg[arg+1];
//       else if(strArg[arg].compare("-t")==0)
// 	thresh_stop = intArg[arg+1];
//       else if(strArg[arg].compare("-d")==0)
// 	thresh_delta = intArg[arg+1];
//       else if(strArg[arg].compare("-p")==0)
//         channel = intArg[arg+1];
//       else if(strArg[arg].compare("-f")==0)
// 	{
// 	  sprintf(file_name_buffer,"../../sweeps/%s", strArg[arg+1].c_str());
// 	  sweep_file = fopen(file_name_buffer, "w");
// 	  fprintf(sweep_file,"thr\thits\tfreq\twindow\terror\n");
// 	}
//     }
	      
//   if (sweep_file == NULL)
//     {
//       printf("ERROR: must give a file destination\n");
//       return 0;
//     }
  
//   mezzTester->Board.SetTDCReg(MATCH_WINDOW, match_window);
//   mezzTester->Board.SetTDCReg(SEARCH_WINDOW, match_window+8);
//   mezzTester->Board.SetTDCReg(REJECT_OFFSET, reject_offset);
//   mezzTester->Board.SetTDCReg(BUNCH_OFFSET, bunch_offset);
//   mezzTester->Board.SetTDCReg(COARSE_OFFSET, coarse_offset);
//   mezzTester->Board.SetTDCReg(COUNT_ROLLOVER, coarse_rollover);
//   mezzTester->Board.SetChannel(channel);
//   mezzTester->Board.UpdateBoard();

//   printf("Channel: %d\n", channel);

//   int match_og = match_window;
//   float window_time;
//   float full_window = TDC_CLK*match_window*num_sweeps;
//   for (int thresh=thresh_start; thresh<thresh_stop; thresh+=thresh_delta)
//     {
//       mezzTester->Board.SetASDReg(DISC1_THR, thresh);
//       mezzTester->Board.UpdateASD();
//       mezzTester->resetTotalHits();
//       window_time = 0;
//       match_window = match_og;
//       mezzTester->Board.SetTDCReg(MATCH_WINDOW, match_window);
//       mezzTester->Board.SetTDCReg(SEARCH_WINDOW, match_window+8);
//       mezzTester->Board.UpdateTDC();      
//       //for (int i=0; i<num_sweeps; i++)
//       while (window_time < full_window)
//   	{
//   	  while(mezzTester->Board.FIFOFlags() == FIFO_EMPTY)
//   	    {
//   	      //mezzTester->Board.TDCcmd(BCR);
//   	      mezzTester->Board.TDCcmd(TRIGGER);
//   	    }
//   	  mezzTester->getReadout();
//   	  mezzTester->printTDCStatus();
// 	  if ((mezzTester->HitReadout.errorflags & 
// 	       READOUT_FIFO_OVERFLOW_ERROR) ||
// 	      (mezzTester->HitReadout.errorflags & 
// 	       L1_BUFFER_OVERFLOW_ERROR))
// 	    {
// 	      if (match_window < 10)
// 		{
// 		  printf("ERROR: match window <10\n");
// 		  printf("Match window set to %d\n", match_window);
// 		  break;
// 		  //	  printf("index: %d\n", i);
// 		}
// 	      match_window /= 2;
// 	      if (match_window == 0)
// 	      	match_window = 1;
// 	      mezzTester->Board.SetTDCReg(MATCH_WINDOW, match_window);
// 	      mezzTester->Board.SetTDCReg(SEARCH_WINDOW, match_window+8);
// 	      mezzTester->Board.UpdateTDC();
// 	      //mezzTester->Board.serial.Writeln("jtr 2"
// 	      // printf("Match window set to %d\n", match_window);
// 	      // printf("Throwing out packet.\n");
// 	      // printf("total hits before sub:%d\n", mezzTester->totalhits);
// 	      //i -= 1;;
// 	      mezzTester->totalhits -= mezzTester->HitReadout.numHits;
// 	      // printf("total hits after sub:%d\n", mezzTester->totalhits);
// 	      sleep(.001);
// 	      continue;
// 	    }
// 	  window_time += (float)match_window*TDC_CLK;
// 	  // if (match_window != match_og)
// 	  //   printf("new window_time =%f\n"
// 	  // 	   "match window=%d\n"
// 	  // 	   "total hits:%d\n", 
// 	  // 	   window_time, match_window, mezzTester->totalhits);
// 	  mezzTester->saveHits();
// 	}
//       runhits = mezzTester->getTotalHits();
//       rate = (float)runhits/(window_time);

//       fprintf(sweep_file, "%0d\t%0d\t%g\t%d\t", 2*(thresh-127), runhits, 
// 	      rate, match_window);

//       if(runhits != 0)
// 	fprintf(sweep_file,"%g\n", sqrt(runhits)/runhits*rate);
//       else
// 	fprintf(sweep_file,"0\n");

//       printf("%0d\t%0d\t%g\t%d\t", 2*(thresh-127), runhits, 
// 	     rate, match_window);

//       if(runhits != 0)
// 	printf("%g\n", sqrt(runhits)/runhits*rate);
//       else
// 	printf("0\n");
//       printf("window time: %f\n", window_time);
//     }
//   fclose(sweep_file);

//   return 0;
// }

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
	mezzTester->Board.TDCcmd(TRIGGER);
      mezzTester->getReadout();
      printf("--------------------------------------"
	     "------------------------------------\n");
      mezzTester->printTDCHits();
      mezzTester->printTDCStatus();
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
  mezzTester->Board.SetChannelMask(intArg[0]);
  return 0;
}

int AMC13_Launcher::SetChannel(std::vector<std::string> strArg,
			       std::vector<uint64_t> intArg)
{
  mezzTester->Board.SetChannel(intArg[0]);
  return 0;
}

int AMC13_Launcher::SetHitPeriod(std::vector<std::string> strArg,
				 std::vector<uint64_t> intArg)
{
  mezzTester->Board.SetHitPeriod(intArg[0]);
  return 0;
}

int AMC13_Launcher::SetStrobePulsePeriod(std::vector<std::string> strArg,
					 std::vector<uint64_t> intArg)
{
  mezzTester->Board.SetStrobePulsePeriod(intArg[0]);
  return 0;
}

int AMC13_Launcher::load_test(std::vector<std::string> strArg,
			      std::vector<uint64_t> intArg)
{
  mezzTester->Board.SetASDReg(CHIP_MODE, 1);
  mezzTester->Board.SetASDReg(CAL_INJECT_MASK, 0xFF);
  mezzTester->Board.SetASDReg(CAL_INJ_CAPS, 0x07);
  mezzTester->Board.SetASDReg(DISC1_THR, 80);

  mezzTester->Board.SetChannelMask(0xFFFFFF);
  mezzTester->Board.SetChannel(0);
  mezzTester->Board.UpdateBoard();

  mezzTester->Board.SetStrobePulsePeriod(01);

  return 0;
}

int AMC13_Launcher::load_default(std::vector<std::string> strArg,
				 std::vector<uint64_t> intArg)
{
  int ASD[] = {0x00, 0x00, 108,   1,   2,   6,   5,   7, 0x00, 0x00, 0x00};
  /*              0     1    2    3    4    5    6    7     8     9     A  */
  int TDC[] = {0x000,        0,    39,    31,  3424,     0,  3464,     0,  3563, 
	       /*               0      1      2      3      4      5      6      7      8 */
  	       0xC0A, 0xAF1, 0xF19, 0x1FF, 0x000, 0x000};		
  /*               9      A      B      C      D      E */ 
  int i = 0;
  for (i = 0; i<TDC_REG_NUM; i++)
    mezzTester->Board.SetTDCReg(i, TDC[i]);
  for (i = 0; i<ASD_REG_NUM; i++)
    mezzTester->Board.SetASDReg(i, ASD[i]);

  return 0;
}
