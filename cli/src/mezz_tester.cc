#include <iostream>
#include "mezz_tester.h"
#include <stdlib.h>


// default constructor is same scheme as MezzTesterBoard
MezzTester::MezzTester(const char* device_name, bool shouldSave, int ChannelMask) 
                  : Board(device_name, ChannelMask)
{
  totalhits = 0;
  savedhits = 0;
  shouldSaveHits = shouldSave;
  if(shouldSaveHits)
    initFile();
}

// same scheme as MezzTesterBoard
MezzTester::MezzTester(int * TDC, int ASD[10], int DAC[4], const char* device_name, 
		       bool shouldSave, int ChannelMask) 
                  : Board(TDC, ASD, DAC,device_name, ChannelMask)
{
  totalhits = 0;
  savedhits = 0;
  shouldSaveHits = shouldSave;
  if(shouldSaveHits)
    initFile();
}

// open hit saving file
void MezzTester::initFile()
{
  hitFile = fopen("../../sweeps/hits.txt", "w");
  fprintf(hitFile, "#thit\tthr\teventID\thit#\tchannel\tedge\terror\tcoarse\tfine\ttime\tetime\n");
}

// close init file in the destructor
MezzTester::~MezzTester()
{
  if (shouldSaveHits)
    fclose(hitFile);
}

// get and save TDC status into class
void MezzTester::getTDCStatus()
{
  Board.GetStatus(&TDCStatus);
}

// poll running flag
int MezzTester::TDCRunning()
{
  getTDCStatus();
  return TDCStatus.running;
} 

int MezzTester::getReadout(int maskflags)
{
  int ret = Board.ReadFIFO(&(this->HitReadout));
  if (ret > NO_HITS)
    totalhits += HitReadout.numHits;
  // if (shouldSaveHits)
  //   saveHits();
  if (HitReadout.errorflags != 0)
    printTDCError(HitReadout.errorflags, maskflags);
  return ret;
}


void MezzTester::printTDCError(int errmask, int mask)
{
  errmask &= ~mask;
  if (errmask == 0)
    return;
  printf("-----------------------------------------------------"
	 "---------------------------------------------------\n"
	 "Event ID: %d\n", HitReadout.eventID);
  if(errmask & COARSE_ERROR)
    printf("TDC error: coarse counter parity error.\n");
  if(errmask & CHANNEL_SEL_ERROR)
    printf("TDC error: channel select error (more than 1 channel are selected).\n");
  if(errmask & L1_BUFFER_PAR_ERROR)
    printf("TDC error: l1 buffer parity error.\n");
  if(errmask & TRIGGER_FIFO_PAR_ERROR)
    printf("TDC error: trigger FIFO parity error.\n");
  if(errmask & TRIGGER_MATCHING_ERROR)
    {
      printf("TDC error: trigger matching error (state error).\n");
      printf("Global reset sent.\n");
      int temp = Board.TDCRegs[0];
      Board.TDCRegs[0] |= 0x800;
      Board.UpdateTDC();
      Board.TDCRegs[0] = temp;
      Board.UpdateTDC();
    }
  if(errmask & READOUT_FIFO_PARITY_ERROR)
    printf("TDC error: readout FIFO parity error.\n");
  if(errmask & READOUT_STATE_ERROR)
    printf("TDC error: readout FIFO parity error.\n");
  if(errmask & CONTROL_PARITY_ERROR)
    printf("TDC error: control parity error.\n");
  if(errmask & JTAG_PARITY_ERROR)
    printf("TDC error: JTAG instruction parity error.\n");
  if(errmask & L1_BUFFER_OVERFLOW_ERROR)
      printf("TDC error: L1 buffer overflow error.\n");
  if(errmask & TRIGGER_FIFO_OVERFLOW_ERROR)
    printf("TDC error: trigger fifo overflow error.\n");
  if(errmask & READOUT_FIFO_OVERFLOW_ERROR)
      printf("TDC error: readout fifo overflow error.\n");
  if(errmask & HIT_ERROR)
    printf("TDC error: hit error.\n");
}

void MezzTester::printTDCStatus(bool printstatus)
{
  getTDCStatus();
  if (printstatus)
    {
      int fifoflag = Board.FIFOFlags();
      switch(fifoflag)
	{
	case FIFO_EMPTY: printf("fifo is empty.\n"); break;
	case FIFO_NOT_EMPTY: printf("fifo is not empty.\n"); break;
	case FIFO_FULL: printf("fifo is full.\n"); break;
	default: printf("fifo is invalid. Register value: %04X\n", fifoflag);
	}
      switch(TDCStatus.rfifo)
	{
	case FIFO_EMPTY: printf("rfifo is empty.\n"); break;
	case FIFO_NOT_EMPTY: printf("rfifo is not empty.\n"); break;
	case FIFO_FULL: printf("rfifo is full.\n"); break;
	default: printf("rfifo is invalid. Register value: %04X\n", TDCStatus.tfifo);
	}
      switch(TDCStatus.tfifo)
	{
	case FIFO_EMPTY: printf("tfifo is empty.\n"); break;
	case FIFO_NOT_EMPTY: printf("tfifo is not empty.\n"); break;
	case FIFO_NEARLY_FULL: printf("tfifo is nearly full.\n"); break;
	case FIFO_FULL: printf("tfifo is full.\n"); break;
	default: printf("tfifo is invalid. Register value: %04X.\n", TDCStatus.tfifo);
	}
      printf("tfifo occupancy: %d.\n", TDCStatus.tfifo_occ);
      printf("rfifo occupancy: %d.\n", TDCStatus.rfifo_occ);
      printf("coarse counter: %d.\n", TDCStatus.coarse_counter);
    }
  if (TDCStatus.errorflags != 0)
    printTDCError(TDCStatus.errorflags);
}

void MezzTester::printTDCHits(int maskflags)
{
  printf("Number of hits: %0d\n", HitReadout.numHits);
  printf("Event ID: %0d\n", HitReadout.eventID);
  printf("Bunch ID: %0d\n", HitReadout.bunchID);
  if (HitReadout.errorflags != 0)
    printf("Errorflags: %04X\n", HitReadout.errorflags);
  if (HitReadout.numHits == 0)
    return;
  printf("thit#\tchannel\tedge\terror\tcoarse\tfine\ttime\t\tetime\n");
  for (int i=0; i<HitReadout.numHits; i++)
    {
      printf("%0d\t%0d\t%0d\t%0d\t%0d\t%0d\t%fns\t",
	     i, HitReadout.hits[i].channel, 
	     HitReadout.hits[i].edge, HitReadout.hits[i].error, 
	     HitReadout.hits[i].coarseTime, HitReadout.hits[i].fineTime, 
	     HitReadout.hits[i].hitTime);
      if (i>0)
	  printf("%fns\n", HitReadout.hits[i].hitTime - HitReadout.hits[i-1].hitTime);
      else
	printf("\n");
    } 
  if (HitReadout.errorflags != 0)
    printTDCError(HitReadout.errorflags, maskflags);
}

void MezzTester::saveHits()
{
  if (HitReadout.numHits==0)
    return;
  for (int i = 0; i < HitReadout.numHits; i++)
    {
      savedhits+=1;
      fprintf(hitFile, "%d\t%d\t%0d\t%0d\t%0d\t%0d\t%0d\t%0d\t%0d\t%fns\t",
	      savedhits, 2*(Board.ASDRegs[DISC1_THR]-127),
	      HitReadout.eventID, i, HitReadout.hits[i].channel, 
	      HitReadout.hits[i].edge, HitReadout.hits[i].error,
	      HitReadout.hits[i].coarseTime, HitReadout.hits[i].fineTime, 
	      HitReadout.hits[i].hitTime);
      if (i>0)
	fprintf(hitFile, "%fns\n", HitReadout.hits[i].hitTime - HitReadout.hits[i-1].hitTime);
      else
	fprintf(hitFile, "\n");
    }
  // if ((savedhits != totalhits) && shouldSaveHits)
  //   printf("ERROR: not saving all recorded hits: total: %d saved: %d\n", totalhits, savedhits);
}

void MezzTester::SetWindow(int match_window)
{
  Board.SetTDCReg(MATCH_WINDOW, match_window);
  Board.SetTDCReg(SEARCH_WINDOW, match_window+8);
  Board.SetTDCReg(BUNCH_OFFSET, 4095-match_window);
  Board.SetTDCReg(REJECT_OFFSET, 4095-match_window-8);
  Board.SetTDCReg(COUNT_ROLLOVER, 4095);
}

void MezzTester::ResetTDC()
{
  Board.serial.Writeln("tc 2");
  Board.serial.Writeln("tc 1");
  Board.serial.Writeln("tc 3");
}
