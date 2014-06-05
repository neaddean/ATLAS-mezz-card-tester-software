#include <iostream>
#include <mezz_tester.h>
#include <cstring>



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
  serial.Close();
  printf("MezzTester Offline.\n");
}

void MezzTester::Power(int pwr)
{
  switch (pwr)
    {
    case ON: serial.Writeln("power_on"); break;
    case OFF: serial.Writeln("power_off"); break;
    case RESET: Power(OFF); sleep(1); Power(ON);
      serial.Writeln("gpio");
      ResetFIFO();  break;
    }
}

TDCStatus_s MezzTester::GetStatus()
{
  TDCStatus_s nothing;
  char buffer[256];
  buffer[255] = 0x00;

  serial.Write("jts");
  serial.Read(buffer, 255);
  printf("TDC status registers :\n %s", buffer);
  return nothing;
}

void MezzTester::Update()
{
  WriteReg(TDCRegs, TDC_REG_NUM, "jtw");
  WriteReg(ASDRegs, ASD_REG_NUM, "jaw");
  WriteReg(DACRegs, DAC_REG_NUM, "d");

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
  serial.Writeln("");
  char buffer[256];
  buffer[255] = '\0';
  serial.Writeln("tf", false);
  size_t sread = serial.Readln(buffer);
  printf("Read (%zu) : %s\n" , sread,buffer);
  return 0;
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
