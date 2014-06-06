#include <iostream>

#include <signal.h>
#include <errno.h>

#include <serial_io.h>

#include <mezz_tester.h>
#include <tester.h>

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

  // MezzTester * GoodLuck = new MezzTester(argv[1]);
  //GoodLuck->FIFOFlags();
  MezzTester GoodLuck(argv[1]);
  TDCStatus_s tempstat;
  running = true;
  while (running)
    {
      printf("\n\n--------------------------------------------\n");
      //GoodLuck.FIFOFlags();
      GoodLuck.GetStatus(&tempstat);
      switch(tempstat.rfifo)
	{
	case FIFO_EMPTY: printf("rfifo is empty.\n"); break;
	case FIFO_FULL: printf("rfifo is full.\n"); break;
	default: printf("rfifo is invalid. Register value: %04X\n", tempstat.tfifo);
	}
      switch(tempstat.tfifo)
	{
	case FIFO_EMPTY: printf("tfifo is empty.\n"); break;
	case FIFO_NEARLY_FULL: printf("tfifo is nearly full.\n"); break;
	case FIFO_FULL: printf("tfifo is full.\n"); break;
	default: printf("tfifo is invalid. Register value: %04X.\n", tempstat.tfifo);
	}
      printf("tfifo occupancy: %d.\n", tempstat.tfifo_occ);
      printf("coarse counter: %d, %4x.\n", tempstat.coarse_counter, tempstat.coarse_counter);
      check_tdc_err(&tempstat);
      sleep(1);
    }

  // delete GoodLuck;

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


  // running = true;
  // while(running)
  //   {      
  //     serial.Writeln("tf", false);
  //     size_t sread = serial.Read(buffer,bufferSize);
  //     if(sread < bufferSize)
  //     	{
  //     	  buffer[sread] = '\0';
  //     	  printf("Read (%zu) : %s\n" , sread,buffer);	  
  //     	}
  //     sleep(1);
  //   }
  // serial.Close();


  return 0;
}

