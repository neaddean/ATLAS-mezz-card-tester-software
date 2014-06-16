#include <iostream>
#include <mezz_tester.h>
#include <cstring>
#include <stdlib.h>
#include <errno.h>

// This is the default constructor
MezzTesterBoard::MezzTesterBoard(char* device_name, int ChannelMask)
{
  int ASD[] = {0x00, 0x00, 108,   1,   2,   6,   5,   7, 0x00, 0x00, 0x00};
  /*              0     1    2    3    4    5    6    7     8     9     A  */
  // int TDC[] = {0x000,   32,     39,    31,  3424,     0,  3464,     0,  3563, 
  // /*               0      1      2      3      4      5      6      7      8 */
  // 	       0xC0A, 0xAF1, 0xE11, 0x1FF, 0xfff, 0xfff};		
  // /*            9      A      B      C      D      E */
  
  int TDC[] = {0x000,     0,   808,   800,  3024,     0,  3064,     0,  4000,
  /*               0      1      2      3      4      5      6      7      8 */
  // int TDC[] = {0x000,     0,   400,   408,  2000,     0,  2000,     0,  4095, 
  // /*               0      1      2      3      4      5      6      7      8 */
  	       0xC0A, 0xAF1, 0xF11, 0x1FF, 0x000, 0x000};		
  /*               9      A      B      C      D      E */ 
  int DAC[] = {0xFFF, 0xFFF, 0xFFF, 0xFFF};

  int i;
  for (i = 0; i<TDC_REG_NUM; i++)
    TDCRegs[i] = TDC[i];
  for (i = 0; i<ASD_REG_NUM; i++)
    ASDRegs[i] = ASD[i];
  for (i = 0; i<DAC_REG_NUM; i++)
    DACRegs[i] = DAC[i];

  this->ChannelMask = ChannelMask;
  StrobePulsePeriod = 0x01;
  HitPeriod = 0x9B;
  EnabledChannel = ALL_OFF;

  serial.SetDevice(device_name);
  if(!serial.Open())
    printf("ERROR: cannot opening device\n"); 

  BoardReset();
 
  printf("MezzTesterBoard Online.\n");
}

// This constructor takes the registers as parameters to override the default settings
MezzTesterBoard::MezzTesterBoard(int * TDC, int ASD[11], int DAC[4], char* device_name, 
		       int ChannelMask)
{
  int i;
  for (i = 0; i<15; i++)
    TDCRegs[i] = TDC[i];
  for (i = 0; i<11; i++)
    ASDRegs[i] = ASD[i];
  for (i = 0; i<4; i++)
    DACRegs[i] = DAC[i];

  this->ChannelMask = ChannelMask;
  StrobePulsePeriod = 0x01; 
  HitPeriod = 0x9B;
  EnabledChannel = ALL_OFF;


  serial.SetDevice(device_name);
  if(!serial.Open())
    printf("ERROR: cannot opening device\n"); 
 
  BoardReset();
  
  printf("MezzTesterBoard Online.\n");
}

// Powers off the board and closes the serial port
MezzTesterBoard::~MezzTesterBoard()
{
  Power(OFF);
  serial.Close();
  printf("MezzTesterBoard Offline.\n");
}

// Takes a power command as a parameter. 
// Further initialization takes place elsewhere
void MezzTesterBoard::Power(int pwr)
{
  switch (pwr)
    {
    case ON: serial.Writeln("power_on"); break;
    case OFF: serial.Writeln("power_off"); break;
    case RESET: Power(OFF); sleep(2); Power(ON); sleep(2); break;
    }
}

// Resets the CLI, makes all GPIO outputs and writes all registers
void MezzTesterBoard::Init()
{
  serial.Writeln(" ");
  serial.Writeln("reset");
  serial.Writeln("gpio");
  TDCcmd(GR);
  TDCcmd(BCR);
  TDCcmd(ECR);
  ResetFIFO();
}

// Reset/init the board
void MezzTesterBoard::BoardReset()
{
  Power(RESET);
  Init();
  UpdateBoard();
}

// Reads the TDC status register and fills in the struct
void MezzTesterBoard::GetStatus(TDCStatus_s * TDCStatus)
{
  char buffer[20];
  int statusbuf[6];
  
  // read status registers into a buffer
  serial.Writeln(" ");
  serial.Writeln("jts", false);
  // printf("-----ASCII : HEX-----\n");
  for (int i=0; i<6; i++)
    {
      serial.Readln(buffer, 20);
      statusbuf[i] = (int)strtoul(buffer, NULL, 16);
      // printf("   %d : %s : %04X\n", i, buffer, statusbuf[i]);
    }
  
  int tempmask = 0;
  tempmask = statusbuf[0];

  // readout fifo status
  tempmask &= 0xC00;
  if (tempmask==0x800)
    TDCStatus->rfifo = FIFO_EMPTY;
  else if (tempmask==0x400)
    TDCStatus->rfifo = FIFO_FULL;
  else
    TDCStatus->rfifo = FIFO_INVALID;

  tempmask = statusbuf[0];
  // error flags
  tempmask &= 0x1FF;
  TDCStatus->errorflags = tempmask;

  tempmask = statusbuf[2];

  // trigger fifo status
  tempmask &= 0xE00;
  if (tempmask==0x800)
    TDCStatus->tfifo = FIFO_EMPTY;
  else if (tempmask==0x400)
    TDCStatus->tfifo = FIFO_NEARLY_FULL;
  else if (tempmask==0x200)
    TDCStatus->tfifo = FIFO_FULL;
  else
    TDCStatus->tfifo = FIFO_INVALID;

  // running flags
  TDCStatus->running = ((statusbuf[2] & 0x100) >> 8);

  // trigger fifo occupancy
  TDCStatus->tfifo_occ = ((statusbuf[3] & 0x700) >> 8);

  // coarse counter 
  int coarse_counter = 0;
  coarse_counter = (statusbuf[3] & 0x800) >> 11;
  coarse_counter |= (statusbuf[4] << 1);

  TDCStatus->coarse_counter = coarse_counter;

  // readout fifo occupancy
  TDCStatus->rfifo_occ = statusbuf[5] & 0x03F;
}

// flush all register
void MezzTesterBoard::UpdateBoard()
{
  UpdateTDC();
  UpdateASD();
  UpdateInjector();
}

// update only relevant pulse injector registers
void MezzTesterBoard::UpdateInjector()
{
  char outbuf[20];

  sprintf(outbuf, "p %06X", ChannelMask);
  serial.Writeln(outbuf);

  sprintf(outbuf, "pt %02X", HitPeriod);
  serial.Writeln(outbuf);
}

// send either TRIGGER, ECR, GR, or BCR
void MezzTesterBoard::TDCcmd(int cmd)
{
  switch (cmd)
    {
    case TRIGGER: serial.Writeln("tc 0"); break;
    case BCR: serial.Writeln("tc 1"); break;
    case GR: serial.Writeln("tc 2"); break;
    case ECR: serial.Writeln("tc 3"); break;
    }
  //sleep(.0001);
}

// periodic bunch count reset has not been implemented yet
// except for what is already on TDC
void MezzTesterBoard::TDCBCR(int n)
{
  (void) n;
}

int MezzTesterBoard::ReadFIFO(HitReadout_s * HitReadout)
{
  unsigned int readbuf[RFIFO_DEPTH];
  int readsize = 0;
  // temporary error flag
  int errortemp = 0;
  // cannot get stuck reading empty fifo or program will freeze
  if (FIFOFlags() == FIFO_EMPTY)
    {
      //printf("ERROR: There is nothing to read out...??....\n");
      readsize = NO_READOUT;
      return NO_READOUT;
    }
  //printf("\n----------------Readout-----------------\n");
  //printf("\t#\tASCII\t\tHEX\n");
  char buffer[20];
  serial.Writeln(" ");
  // read command
  serial.Writeln("tr", false);
  for (readsize = 0; readsize<RFIFO_DEPTH; )
    {
      serial.Readln(buffer, 20);
      readbuf[readsize] = (unsigned int)strtoul(buffer, NULL, 16);
      // check for error word, if it exists then save to temporary error flag
      // and do not increment the write pointer
      if ((readbuf[readsize] & 0x60000000) == 0x60000000)
	{
	  errortemp |= readbuf[readsize];
	  //printf("\t%-d\t%s\t%08X\n", readsize, buffer, readbuf[readsize]);
	  continue;
	}
      //printf("\t%-d\t%s\t%08X\n", readsize, buffer, readbuf[readsize]);
      if ((readbuf[readsize] & 0xC0000000) == 0xC0000000)
	break;
      readsize++;
    }
  // check that packet began with header
  if ((readbuf[0] & 0xA0000000) != 0xA0000000)
    printf("ERROR: packet did not begin with header.\n");

  // check number of words written to number received (including header)
  int expected_read = (readbuf[readsize] & 0x000000FFF);
  // if hard error flag recieved, subtract from expected words
  if ((errortemp & 0x00003E00) != 0)
    expected_read--;
  // if temporal error flag recieved
  if ((errortemp & 0x000001FF) != 0)
    expected_read--;
  // must account for the trailer because quantity starts at 1, not 0
  readsize++;
  if (expected_read != readsize)
    {
      printf("ERROR: fifo read size mismatch error, expected %d but read %d\n", 
	     expected_read, readsize);
      for (int i=0; i<readsize; i++)
	printf("\t%d\t%08X\n", i, readbuf[i]);
      if (errortemp != 0)
	printf("\tE\t%08X\n", errortemp);
      // allow time for user interaction
      sleep(20);
    }
  
  // check that packet ended with trailer

  if ((readbuf[readsize-1] & 0xC0000000) != 0xC0000000)
    printf("ERROR: packet did not end with trailer\n");

  if (readsize == RFIFO_DEPTH)
    printf("ERROR: readsize > RFIFO_DEPTH,\treadsize:%d\tRFIFO_DEPTH:%d\n", 
	   readsize, RFIFO_DEPTH);

  // if ((readsize == RFIFO_DEPTH) && ((readbuf[readsize-1] & 0xC0000000) != 0xC0000000))
  //   printf("ERROR: packet did not end with trailer\n");
  
  if ((readbuf[readsize-1] & 0x00FFF000) != (readbuf[0] & 0x00FFF000))
    printf("ERROR: header and trailer event ID do not match\n");

  //==============================
  // Parsing
  //==============================

  // must have read at least a header and trailer
  // this code should never execute 
  if (readsize < 2)
    {
      printf("ERROR: readsize= %d\n", readsize);
      return READSIZE_ERROR;
    }

  // save error flags
  HitReadout->errorflags = errortemp & 0x00003FFF;
  // write error flags if present
  if (HitReadout->errorflags != 0)
    printf("ERROR: error flags: %04X\n", HitReadout->errorflags);
  // number of hits is packet size - errorflags - header/trailer;
  HitReadout->numHits = readsize - 2;
  // mask and save event ID
  HitReadout->eventID = (readbuf[0] & 0x00FFF000) >> 12;
  // mask and save bunch count ID
  HitReadout->bunchID = (readbuf[0] & 0x00000FFF);

  // if there were no hits
  if (readsize == 2)
    {
      //   HitReadout->numHits = 0;
      // printf("No hits\n");
      return NO_HITS;
    }

  // if we had hits and all channels were disabled, then produce error
  if (EnabledChannel == ALL_OFF)
    {
      printf("ERROR: got hits with all channels on TDC disabled\n");
      // return READSIZE_ERROR;
    }

  // parse hit data
  for (int i=1; i < readsize-1; i++)
    {
      // make sure we are only reading measurement data
      if ((readbuf[i] & 0x30000000) != 0x30000000)
	{
	  printf("ERROR: word is not measurement data: %08X\n", readbuf[i]);
	  continue;
	}
      // record the channel
      HitReadout->hits[i-1].channel = (readbuf[i] & 0x00F80000) >> 19;
      // make sure it was the enabled channel
      if (HitReadout->hits[i-1].channel != EnabledChannel)
	printf("ERROR: Hit on channel %d, but only channel %d enabled\n",
	       HitReadout->hits[i-1].channel, EnabledChannel);
      HitReadout->hits[i-1].edge = (readbuf[i] & 0x00040000) >> 18;
      HitReadout->hits[i-1].error = (readbuf[i] & 0x00020000) >> 17;
      HitReadout->hits[i-1].coarseTime = (readbuf[i] & 0x0001FFE0) >> 5;
      HitReadout->hits[i-1].fineTime = (readbuf[i] & 0x0000001F);
      // calcualte trigger matched time in ns
      HitReadout->hits[i-1].hitTime = (HitReadout->hits[i-1].coarseTime*32 +
				       HitReadout->hits[i-1].fineTime)*.78125;
    }

  return 1;

}

int MezzTesterBoard::FIFOFlags()
{
  serial.Writeln(" ");
  char buffer[20];
  serial.Writeln("tf", false);
  serial.Readln(buffer, 20);
  if (buffer[0]==0x30 && buffer[1]==0x31)
    return FIFO_EMPTY;
  else if (buffer[0]==0x31 && buffer[1]==0x30)
    return FIFO_FULL;
  else if (buffer[0]==0x30 && buffer[1]==0x30)
    return FIFO_NOT_EMPTY;
   
  printf("ERROR: Fifo flags in invalid state, %s\n", buffer);
  return FIFO_INVALID;

}

void MezzTesterBoard::ResetFIFO()
{
  serial.Writeln("tz");
}

void MezzTesterBoard::WriteReg(int Reg[], int RegSize, const char * cmd)
{
  char outbuf[30];
  for (int i=0; i<RegSize; i++)
    {
      sprintf(outbuf, "%s %01X %03X", cmd, i, Reg[i]);
      serial.Writeln(outbuf);
    }
}

void MezzTesterBoard::SetChannel(int set_channel)
{
  EnabledChannel = set_channel;
  if (set_channel == ALL_OFF)
    {
      TDCRegs[13] = 0;
      TDCRegs[14] = 0;
    }    
  if (set_channel < 12)
    {
      TDCRegs[13] = (1 << (set_channel));
      TDCRegs[14] = 0;
    }  
  else
    {
      TDCRegs[13] = 0;
      TDCRegs[14] = (1 << (set_channel - 12));
    }
  UpdateBoard();
}
