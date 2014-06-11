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
  GoodLuck.Board.SetHitPeriod(900);
  GoodLuck.Board.SetChannel(12);
  running = true;

  for (int i=0; i < 10000; i++)
    {
      // GoodLuck.Board.TDCcmd(ECR);
      while(GoodLuck.Board.FIFOFlags() == FIFO_EMPTY)
      	{
      	  GoodLuck.Board.TDCcmd(TRIGGER);
	}
      if (GoodLuck.getReadout() > NO_HITS)
	{
	  // printf("\n\n-----------------------------------------"
	  // 	 "-------------------------------------------\n");
	  //GoodLuck.printTDCHits();
	  //GoodLuck.saveHits();
	} 
      if (i%100 == 0)
	printf("%d%% done with %d/%d hits\n", i/100, GoodLuck.getTotalHits(), i);
      if (!running)
      	break;
    }
  printf("%d total hits\n", GoodLuck.getTotalHits());
  return 0;
}
