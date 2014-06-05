#include <iostream>
#include <mezz_tester.h>
#include <cstring>

MezzTester::MezzTester(char* device_name)
{
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
  return 0;
}

void MezzTester::ResetFIFO()
{
}

void MezzTester::WriteReg(int Reg[], int RegSize, const char * cmd)
{
  char outbuf[15];
  for (int i=0; i<RegSize; i++)
    {
      serial.Write(cmd);
      serial.Write(" ");
      // memset(outbuf, 0, 15);
      sprintf(outbuf, "%01X", i);
      serial.Write(outbuf);
      serial.Write(" ");
      // memset(outbuf, 0, 15);
      sprintf(outbuf, "%03X", Reg[i]);
      serial.Writeln(outbuf);
    }
}
