#ifndef TESTER_H
#define TESTER_H

#include "mezz_tester_board.h"
#include <fstream>

class MezzTester
{
 public:
  MezzTester(const char* device_name, bool shouldSave, int ChannelMask = 0x000000);
  MezzTester(int TDC[15], int ASD[10], int DAC[4], const char* device_name, 
	     bool shouldSave, int ChannelMask = 0x000000);
  ~MezzTester();

  void getTDCStatus();
  int  TDCRunning();
  void printTDCError(int errmask, int mask = 0);
  void printTDCStatus();
  void printTDCHits();
  int  getReadout();
  int  getTotalHits() {return totalhits;}
  void resetTotalHits() {totalhits = 0;}
  HitReadout_s * retReadout() {return &(HitReadout);} 
  void saveHits();

  MezzTesterBoard Board;
  TDCStatus_s TDCStatus;
  HitReadout_s HitReadout;
  int totalhits;
  bool shouldSaveHits;

 private:
  int savedhits;
  void initFile();
  FILE* hitFile;
};

#endif
