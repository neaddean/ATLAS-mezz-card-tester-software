#include <iostream>
#include <mezz_tester.h>
#include <stdlib.h>


MezzTester::MezzTester(char* device_name, bool shouldSave, int ChannelMask) 
                  : Board(device_name, ChannelMask)
{
  totalhits = 0;
  savedhits = 0;
  shouldSaveHits = shouldSave;
  if(shouldSaveHits)
    initFile();
}

MezzTester::MezzTester(int * TDC, int ASD[10], int DAC[4], char* device_name, 
		       bool shouldSave, int ChannelMask) 
                  : Board(TDC, ASD, DAC,device_name, ChannelMask)
{
  totalhits = 0;
  savedhits = 0;
  shouldSaveHits = shouldSave;
  if(shouldSaveHits)
    initFile();
}

void MezzTester::initFile()
{
  hitFile = fopen("hits.txt", "w");
  fprintf(hitFile, "thit#\teventID\thit#\tchannel\tedge\terror\tcoarse\tfine\ttime\n");
}

MezzTester::~MezzTester()
{
  fclose(hitFile);
}

void MezzTester::getTDCStatus()
{
  Board.GetStatus(&TDCStatus);
}

int MezzTester::TDCRunning()
{
  getTDCStatus();
  return TDCStatus.running;
} 

void MezzTester::getTDCStatus(TDCStatus_s * TDCStatus)
{
  Board.GetStatus(&(this->TDCStatus));

  TDCStatus->rfifo = this->TDCStatus.rfifo;
  TDCStatus->errorflags = this->TDCStatus.errorflags;
  TDCStatus->tfifo = this->TDCStatus.tfifo;
  TDCStatus->tfifo_occ = this->TDCStatus.tfifo_occ;
  TDCStatus->tfifo_occ = this->TDCStatus.coarse_counter;
  TDCStatus->running = this->TDCStatus.running;
}

int MezzTester::getReadout()
{
  int ret = Board.ReadFIFO(&(this->HitReadout));
  if (ret > NO_HITS)
    totalhits += HitReadout.numHits;
  if (shouldSaveHits)
    saveHits();
  return ret;
}


void MezzTester::printTDCError(int errmask)
{
  // int errmask = TDCStatus.errorflags;

  if(errmask & COARSE_ERROR)
    printf("TDC error: coarse counter parity error.\n");
  if(errmask & CHANNEL_SEL_ERROR)
    printf("TDC error: channel select error (more than 1 channel are selected).\n");
  if(errmask & L1_BUFFER_PAR_ERROR)
    printf("TDC error: l1 buffer parity error.\n");
  if(errmask & TRIGGER_FIFO_PAR_ERROR)
    printf("TDC error: trigger FIFO parity error.\n");
  if(errmask & TRIGGER_MATCHING_ERROR)
    printf("TDC error: trigger matching error (state error).\n");
  if(errmask & READOUT_FIFO_PARITY_ERROR)
    printf("TDC error: readout FIFO parity error.\n");
  if(errmask & READOUT_STATE_ERROR)
    printf("TDC error: readout FIFO parity error.\n");
  if(errmask & CONTROL_PARITY_ERROR)
    printf("TDC error: control parity error.\n");
  if(errmask & JTAG_PARITY_ERROR)
    printf("TDC error: JTAG instruction parity error.\n");
}

void MezzTester::printTDCStatus()
{
  getTDCStatus();
  // int fifoflag = Board.FIFOFlags();
  // switch(fifoflag)
  //   {
  //   case FIFO_EMPTY: printf("fifo is empty.\n"); break;
  //   case FIFO_NOT_EMPTY: printf("fifo is not empty.\n"); break;
  //   case FIFO_FULL: printf("fifo is full.\n"); break;
  //   default: printf("fifo is invalid. Register value: %04X\n", fifoflag);
  //   }
  // switch(TDCStatus.rfifo)
  //   {
  //   case FIFO_EMPTY: printf("rfifo is empty.\n"); break;
  //   case FIFO_NOT_EMPTY: printf("rfifo is not empty.\n"); break;
  //   case FIFO_FULL: printf("rfifo is full.\n"); break;
  //   default: printf("rfifo is invalid. Register value: %04X\n", TDCStatus.tfifo);
  //   }
  // switch(TDCStatus.tfifo)
  //   {
  //   case FIFO_EMPTY: printf("tfifo is empty.\n"); break;
  //   case FIFO_NOT_EMPTY: printf("tfifo is not empty.\n"); break;
  //   case FIFO_NEARLY_FULL: printf("tfifo is nearly full.\n"); break;
  //   case FIFO_FULL: printf("tfifo is full.\n"); break;
  //   default: printf("tfifo is invalid. Register value: %04X.\n", TDCStatus.tfifo);
  //   }
  //printf("tfifo occupancy: %d.\n", TDCStatus.tfifo_occ);
  //printf("coarse counter: %d.\n", TDCStatus.coarse_counter);
  // if (TDCStatus.running != 0)
  //   printf("ERROR: TDC RUNNING\n");
  if (TDCStatus.errorflags != 0)
      printTDCError(TDCStatus.errorflags);
}

void MezzTester::printTDCHits()
{
  printf("Number of hits: %0d\n", HitReadout.numHits);
  printf("Event ID: %0d\n", HitReadout.eventID);
  printf("Bunch ID: %0d\n", HitReadout.bunchID);
  if (HitReadout.errorflags != 0)
    printf("Errorflags: %04X\n", HitReadout.errorflags);
  if (HitReadout.numHits == 0)
    return;
  printf("thit#\tchannel\tedge\terror\tcoarse\tfine\ttime\n");
  for (int i=0; i<HitReadout.numHits; i++)
    {
      printf("%0d\t%0d\t%0d\t%0d\t%0d\t%0d\t%fns\n",
	     i, HitReadout.hits[i].channel, 
	     HitReadout.hits[i].edge, HitReadout.hits[i].error, 
	     HitReadout.hits[i].coarseTime, HitReadout.hits[i].fineTime, 
	     HitReadout.hits[i].hitTime);
    } 
  if (HitReadout.errorflags != 0)
    printTDCError(HitReadout.errorflags);
}

void MezzTester::saveHits()
{
  if (HitReadout.numHits==0)
    return;
  for (int i = 0; i < HitReadout.numHits; i++)
    {
      fprintf(hitFile, "%0d\t%0d\t%0d\t%0d\t%0d\t%0d\t%0d\t%0d\t%fns\n",
	      ++savedhits, HitReadout.eventID, i, HitReadout.hits[i].channel, 
	      HitReadout.hits[i].edge, HitReadout.hits[i].error,
	      HitReadout.hits[i].coarseTime, HitReadout.hits[i].fineTime, 
	      HitReadout.hits[i].hitTime);
    }
  if ((savedhits != totalhits) && shouldSaveHits)
    printf("ERROR: not saving all recorded hits: total: %d saved: %d\n", totalhits, savedhits);
}
