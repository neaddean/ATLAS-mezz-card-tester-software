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
  
  int ASD[] = {0xFF, 0x07, 0x6C, 0x01, 0x02, 0x06, 0x05, 0x07, 0x00, 0x00, 0x00}; 
  int TDC[] = {0x000, 0xA00, 0x027, 0x01F, 0xD60, 0x000, 0xD75, 0x000, 0xDEB, 
  	       0xC0A, 0xA71, 0xE11, 0x1FF, 0xFFF, 0xFFF};			
  int DAC[] = {0xFFF, 0xFFF, 0xFFF, 0xFFF};

  MezzTester GoodLuck(TDC, ASD, DAC, argv[1]);

  // running = true;
  // while(running)
  //   {      
  //     serial.Writeln("jts");
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

