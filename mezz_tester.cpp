#include <iostream>
#include <mezz_tester.h>
#include <stdlib.h>


MezzTester::MezzTester(char* device_name, int ChannelMask) 
                  : Board(device_name, ChannelMask)
{
}

MezzTester::MezzTester(int * TDC, int ASD[10], int DAC[4], char* device_name, 
   int ChannelMask) : Board(TDC, ASD, DAC,device_name, ChannelMask)
{
}

MezzTester::~MezzTester()
{
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
  TDCStatus->error_flags = this->TDCStatus.error_flags;
  TDCStatus->tfifo = this->TDCStatus.tfifo;
  TDCStatus->tfifo_occ = this->TDCStatus.tfifo_occ;
  TDCStatus->tfifo_occ = this->TDCStatus.coarse_counter;
  TDCStatus->running = this->TDCStatus.running;
}

int MezzTester::getReadout()
{
  return Board.ReadFIFO(&(this->HitReadout));
}


void MezzTester::printTDCError()
{
  int errmask = TDCStatus.error_flags;

  if(errmask & COARSE_ERROR)
    printf("TDC status error: coarse counter parity error.\n");
  if(errmask & CHANNEL_SEL_ERROR)
    printf("TDC status error: channel select error (more than 1 channel are selected).\n");
  if(errmask & L1_BUFFER_PAR_ERROR)
    printf("TDC status error: l1 buffer parity error.\n");
  if(errmask & TRIGGER_FIFO_PAR_ERROR)
    printf("TDC status error: trigger FIFO parity error.\n");
  if(errmask & TRIGGER_MATCHING_ERROR)
    printf("TDC status error: trigger matching error (state error).\n");
  if(errmask & READOUT_FIFO_PARITY_ERROR)
    printf("TDC status error: readout FIFO parity error.\n");
  if(errmask & READOUT_STATE_ERROR)
    printf("TDC status error: readout FIFO parity error.\n");
  if(errmask & CONTROL_PARITY_ERROR)
    printf("TDC status error: control parity error.\n");
  if(errmask & JTAG_PARITY_ERROR)
    printf("TDC status error: JTAG instruction parity error.\n");
}

void MezzTester::printTDCStatus()
{
  getTDCStatus();
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
  //printf("coarse counter: %d.\n", TDCStatus.coarse_counter);
}

void MezzTester::printTDCHits()
{
  printf("Number of hits: %0d\n", HitReadout.numHits);
  printf("Event ID: %0d\n", HitReadout.eventID);
  printf("Bunch ID: %0d\n", HitReadout.bunchID);
  printf("Errorflags: %04X\n", HitReadout.errorflags);
  printf("\thit#\tchannel\tedge\terror\tcoarse\tfine\n");
  for (int i=0; i<HitReadout.numHits; i++)
    {
      printf("\t%0d\t%0d\t%0d\t%0d\t%0d\t%0d\n",
	     i, HitReadout.hits[i].channel, HitReadout.hits[i].edge, HitReadout.hits[i].error, 
	     HitReadout.hits[i].coarseTime, HitReadout.hits[i].fineTime);
    } 
}
