#ifndef TESTER_H
#define TESTER_H

#include <mezz_tester_board.h>
#include <fstream>

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
  void printTDCError(int errmask);
  void printTDCStatus();
  void printTDCHits();
  int  getReadout();
  int getTotalHits() {return totalhits;}
  void resetTotalHits() {totalhits = 0;}
  HitReadout_s * retReadout() {return &(HitReadout);} 
  void saveHits();
  
  MezzTesterBoard Board;
 private:
  void initFile();
  HitReadout_s HitReadout;
  TDCStatus_s TDCStatus;
  int totalhits;
  int savedhits;
  FILE* hitFile;
};

#endif
