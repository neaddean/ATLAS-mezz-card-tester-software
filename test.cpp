#include <iostream>
#include <fstream>

#include <signal.h>
#include <errno.h>

#include <serial_io.h>

#include <mezz_tester.h>

//==============================================================                      
//Signal handler and loop control                            
//==============================================================                      
volatile bool running;
void signalHandler(int signum)
{
  (void) signum;
  printf("Shutting down\n");
  running = false;
}

int main(int argc, char ** argv)
{
  //==============================================================                      
  //Parse CLI
  //==============================================================                      
  if(argc < 2)
    {
      printf("Usage: %s device\n",argv[0]);
      return -1;
    }

  //==============================================================                      
  //Setup signal handler                            
  //==============================================================                      
  sigset_t signalMask;
  struct sigaction sSigAction;                      
  memset (&sSigAction, 0, sizeof(sSigAction));      
                
                
  //Setup a signal handler & sigmask to capture CTRL-C                                  
  sSigAction.sa_handler = signalHandler;            
  if ( sigaction(SIGINT, &sSigAction, NULL)  < 0 )  
    {           
      printf("Error in sigaction (%d): %s\n",errno,strerror(errno));                    
      return -2;
    }           
  sigemptyset(&signalMask);                        
  sigaddset (&signalMask, SIGINT);                  
  

  //==============================================================                      
  //Start main loop
  //==============================================================                      
  MezzTester GoodLuck(argv[1], 0x000000, true);
  running = true;
  
  FILE* sweep_file;
  sweep_file = fopen("sweep.txt", "w");
  fprintf(sweep_file, "#thresh\trate\n");
  float rate = 0;
  int runhits = 0;

  const int num_sweeps = 100;
  GoodLuck.Board.SetChannel(0);
  for (int thresh=0; thresh<255; thresh+=2)
    {
      GoodLuck.Board.SetASDReg(DISC1_THR, thresh);
      GoodLuck.Board.UpdateASD();
      GoodLuck.resetTotalHits();
      //GoodLuck.Board.TDCcmd(GR);
      for (int i=0; i<num_sweeps; i++)
  	{
  	  while(GoodLuck.Board.FIFOFlags() == FIFO_EMPTY)
	    {
	      GoodLuck.Board.TDCcmd(BCR);
	      GoodLuck.Board.TDCcmd(TRIGGER);
	    }
  	  GoodLuck.getReadout();
  	  if (!running)
  	    break;
  	}
      runhits = GoodLuck.getTotalHits();
      rate = (float)runhits/(.000025*num_sweeps);
      fprintf(sweep_file, "%0d\t%0d\t%g\n", 2*(thresh-127), runhits, rate);
      printf("%0d\t%0d\t%g\n", 2*(thresh-127), runhits, rate);
      if (!running)
  	break;
    }

  fclose(sweep_file);

  // GoodLuck.Board.SetASDReg(DISC1_THR, 80);
  // GoodLuck.Board.SetChannel(0);
  // GoodLuck.Board.SetHitPeriod(25);
  // GoodLuck.Board.UpdateBoard();
  // int i ;
  // for(i = 0; i<500; i++)
  //   {
  //     // GoodLuck.Board.SetHitPeriod(i);
  //     // GoodLuck.Board.UpdateInjector();
  //     while(GoodLuck.Board.FIFOFlags() == FIFO_EMPTY)
  // 	GoodLuck.Board.TDCcmd(TRIGGER);
  //     //      GoodLuck.getReadout();
  //     if (GoodLuck.getReadout() > NO_HITS)
  // 	{
  // 	  // printf("-------------------------------------\n");
  // 	  // GoodLuck.printTDCHits();
  // 	}
  //     if (!running)
  // 	break;
  //   }

  // printf("total hits:%d/%d\n", GoodLuck.getTotalHits(), i);

  return 0;
}
