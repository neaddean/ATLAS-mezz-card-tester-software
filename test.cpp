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
  //Setup serial port
  //==============================================================                      

  // SerialIO serial;
  // serial.SetDevice(argv[1]);
  // if(!serial.Open())
  //   {
  //     printf("Error opening device\n");
  //     return -1;
  //   }
  

  // size_t bufferSize = 256;
  // char buffer[bufferSize+1];
  // buffer[bufferSize] = '\0';


  //==============================================================                      
  //Start main loop
  //==============================================================                      
  MezzTester GoodLuck(argv[1], 0xFFFFFF);
  FILE * hitFile;
  hitFile = fopen("hits.txt", "w");
  HitReadout_s *hitz = NULL;
  fprintf(hitFile, "\thit#\tchannel\tedge\terror\tcoarse\tfine\tabs\n");
  GoodLuck.Board.SetHitPeriod(790);
  running = true;
  int totalhits = 0;
  for (int i=0; i < 1000; i++)
    {
      GoodLuck.Board.TDCcmd(BCR);
      while(GoodLuck.Board.FIFOFlags() == FIFO_EMPTY)
	{
	  GoodLuck.Board.TDCcmd(TRIGGER);
	}
      if (GoodLuck.getReadout() > NO_HITS)
	{
	  // printf("\n\n-----------------------------------------"
	  // 	 "-------------------------------------------\n");    
	  // GoodLuck.printTDCHits();
	  hitz = GoodLuck.retReadout();
	  for (int k = 0; k < hitz->numHits; k++)
	    {
	      fprintf(hitFile, "\t%0d\t%0d\t%0d\t%0d\t%0d\t%0d\t%d\n",
		      i, hitz->hits[i].channel, hitz->hits[i].edge, 
		      hitz->hits[i].error,hitz->hits[i].coarseTime, 
		      hitz->hits[i].fineTime, 
		      hitz->hits[i].coarseTime*32 + hitz->hits[i].fineTime);
	      totalhits++;
	    }
	} 
      sleep(.005);
      if (i%100 == 0)
	printf("%d%% done with %d hits\n", i/10, totalhits);
      if (!running)
      	break;
    }
  printf("total hits: %d\n", totalhits);
  fclose(hitFile);




  //GoodLuck->Board.serial.Writeln("tc 2");
  //GoodLuck->Board.TDCcmd(BCR);
  //GoodLuck->Board.SetHitPeriod(period);
  //GoodLuck->Board.UpdateInjector();
  //GoodLuck->printTDCStatus();

  return 0;
}
