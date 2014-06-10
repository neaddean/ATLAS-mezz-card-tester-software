#include <iostream>

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

void quick_test(int, MezzTester*);

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
  GoodLuck.Board.SetHitPeriod(0x9B);
  GoodLuck.Board.SetStrobePulsePeriod(0x02);
  running = true;
  // while (running)
  // {
  for (int i=0; i!=0x0F; i++)
    {
      printf("\n\n-----------------------------------------"
	     "-------------------------------------------\n");
      quick_test(i, &GoodLuck);
      if (!running)
	break;
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


void quick_test(int period, MezzTester * GoodLuck)
{
  (void) period;
  // GoodLuck->Board.SetStrobePulsePeriod(period);
  //GoodLuck->printTDCStatus();
  GoodLuck->Board.TDCcmd(ECR);
  GoodLuck->Board.TDCcmd(TRIGGER);
  // printf("Period: %02X\n", period);
  if (GoodLuck->getReadout() >= NO_HITS)
    GoodLuck->printTDCHits();
  sleep(.001);
}

