#ifndef TESTER_H
#define TESTER_H

#include <mezz_tester_board.h>

class MezzTester
{
 public:
  MezzTester(char* device_name, int ChannelMask = 0x000000);
  MezzTester(int TDC[15], int ASD[10], int DAC[4], char* device_name, 
		  int ChannelMask = 0x000000);
  ~MezzTester();

  void getTDCStatus();
  void getTDCStatus(TDCStatus_s * TDCStatus);
  int TDCRunning();
  void printTDCError();
  void printTDCStatus();
  void printTDCHits();
  int  getReadout();
  
  MezzTesterBoard Board;
 private:
  TDCStatus_s TDCStatus;
  HitReadout_s HitReadout;
};

#endif
