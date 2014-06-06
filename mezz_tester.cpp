#include <iostream>
#include <mezz_tester.h>
#include <cstring>
#include <stdlib.h>
#include <errno.h>

int AtoH(char);

MezzTester::MezzTester(char* device_name)
{
  int ASD[] = {0xFF, 0x07, 0x6C, 0x01, 0x02, 0x06, 0x05, 0x07, 0x00, 0x00, 0x00}; 
  int TDC[] = {0x000, 0xA00, 0x027, 0x01F, 0xD60, 0x000, 0xD75, 0x000, 0xDEB, 
  	       0xC0A, 0xA71, 0xE11, 0x1FF, 0xFFF, 0xFFF};			
  int DAC[] = {0xFFF, 0xFFF, 0xFFF, 0xFFF};

  int i;
  for (i = 0; i<15; i++)
    TDCRegs[i] = TDC[i];
  for (i = 0; i<10; i++)
    ASDRegs[i] = ASD[i];
  for (i = 0; i<4; i++)
    DACRegs[i] = DAC[i];

  this->ChannelMask = 0x000000;

  serial.SetDevice(device_name);
  if(!serial.Open())
    printf("Error opening device\n"); 
 
  Power(RESET);
  Update();
  
  printf("MezzTester Online.\n");

  return;
}

MezzTester::MezzTester(int * TDC, int ASD[10], int DAC[4], char* device_name, 
		       int ChannelMask)
{
  int i;
  for (i = 0; i<15; i++)
    TDCRegs[i] = TDC[i];
  for (i = 0; i<10; i++)
    ASDRegs[i] = ASD[i];
  for (i = 0; i<4; i++)
    DACRegs[i] = DAC[i];

  this->ChannelMask = ChannelMask;


  serial.SetDevice(device_name);
  if(!serial.Open())
    printf("Error opening device\n"); 
 
  Power(RESET);
  Update();
  
  printf("MezzTester Online.\n");
}

MezzTester::~MezzTester()
{
  Power(OFF);
  serial.Close();
  printf("MezzTester Offline.\n");
}

void MezzTester::Power(int pwr)
{
  switch (pwr)
    {
    case ON: serial.Writeln("power_on"); break;
    case OFF: serial.Writeln("power_off"); break;
    case RESET: 
      serial.Writeln("reset");      
      Power(OFF); sleep(1); Power(ON);
      serial.Writeln("gpio");
      ResetFIFO();  break;
    }
}

void MezzTester::GetStatus(TDCStatus_s * TDCStatus)
{
  //char buffer[6][10];
  char buffer[10];
  int statusbuf[6];
  
  serial.Writeln(" ");
  serial.Writeln("jts", false);

  memset(statusbuf, 0, 6);
  printf("------ASCII : HEX------\n");
  for (int i=0; i<6; i++)
    {
      serial.Readln(buffer, 10, '\n');
      for (int k=0; k<3; k++)
	statusbuf[i] |= (AtoH(buffer[k]) << (2-k)*4);
      printf("%d : %s : %04X\n", i, buffer, statusbuf[i]);
    }

  // printf("------HEX------\n");
  // for(int i=0; i<6; i++)
  //   {
  //     for(int k =0; k<3; k++)
  // 	statusbuf[i] |= (AtoH(buffer[i][k]) << (2-k)*4);
  //     printf("%d : %04x\n", i, statusbuf[i]);
  //   }
  
  // printf("------ASCII------\n");
  // for (int i=0; i<6; i++)
  //   {
  //     serial.Readln(buffer[i], 10, '\n');
  //     printf("%d : %s\n", i, buffer[i]);
  //   }
  // memset(statusbuf, 0, 6);

  // printf("------HEX------\n");
  // for(int i=0; i<6; i++)
  //   {
  //     for(int k =0; k<3; k++)
  // 	statusbuf[i] |= (AtoH(buffer[i][k]) << (2-k)*4);
  //     printf("%d : %04x\n", i, statusbuf[i]);
  //   }
  
  int tempmask = 0;
  tempmask = statusbuf[0];

  tempmask &= 0xC00;
  if (tempmask==0x800)
    TDCStatus->rfifo = FIFO_EMPTY;
  else if (tempmask==0x400)
    TDCStatus->rfifo = FIFO_FULL;
  else
    TDCStatus->rfifo = FIFO_INVALID;

  tempmask = statusbuf[0];
  tempmask &= 0x1FF;
  TDCStatus->error_flags = tempmask;

  tempmask = statusbuf[2];

  tempmask &= 0xE00;
  if (tempmask==0x800)
    TDCStatus->tfifo = FIFO_EMPTY;
  else if (tempmask==0x400)
    TDCStatus->tfifo = FIFO_NEARLY_FULL;
  else if (tempmask==0x200)
    TDCStatus->tfifo = FIFO_FULL;
  else
    TDCStatus->tfifo = FIFO_INVALID;

  TDCStatus->tfifo_occ= ((statusbuf[3] & 0x700) >> 8);

  int coarse_counter = 0;
  printf("coarse_counter[0]: %04X\n", (statusbuf[3] & 0x800) >> 11);
  printf("coarse_counter[1]: %04X\n", statusbuf[4]);
  coarse_counter = (statusbuf[3] & 0x800) >> 11;
  coarse_counter |= (statusbuf[4] << 1);

  TDCStatus->coarse_counter = coarse_counter;
}

void MezzTester::Update()
{
  WriteReg(TDCRegs, TDC_REG_NUM, "jtw");
  WriteReg(ASDRegs, ASD_REG_NUM, "jaw");
  WriteReg(DACRegs, DAC_REG_NUM, "d");

  serial.Writeln("jtu");
  serial.Writeln("jau");

  char outbuf[10];
  serial.Write("p ");
  sprintf(outbuf, "%06X", ChannelMask);
}

void MezzTester::TDCcmd(int cmd)
{
}

void MezzTester::TDCBCR(int n)
{
}

void MezzTester::ReadFIFO(char * buffer)
{
}

int MezzTester::FIFOFlags()
{
  serial.Writeln(" ");
  char buffer[10];
  serial.Writeln("tf", false);
  serial.Readln(buffer, 10);
  if (buffer[0]==0x30 && buffer[1]==0x31)
    return FIFO_EMPTY;
    // printf("Buffer Empty!\n");
  else if (buffer[0]==0x31 && buffer[1]==0x30)
    return FIFO_FULL;
    //  printf("Buffer FUll!\n");
  
  return FIFO_INVALID;
  printf("ERROR: Fifo flags in invalid state, %s", buffer);

}

void MezzTester::ResetFIFO()
{
  serial.Writeln("tz");
}

void MezzTester::WriteReg(int Reg[], int RegSize, const char * cmd)
{
  char outbuf[15];
  for (int i=0; i<RegSize; i++)
    {
      serial.Write(cmd);
      serial.Write(" ");
      sprintf(outbuf, "%01X", i);
      serial.Write(outbuf);
      serial.Write(" ");
      sprintf(outbuf, "%03X", Reg[i]);
      serial.Writeln(outbuf);
    }
}

int AtoH (char n)
{
    if (n >= '0' && n <= '9') {
      return (n - '0');
    } else {
      return ((n + 10) - 'A');
    }
}
