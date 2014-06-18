#ifndef MEZZ_TESTER_H
#define MEZZ_TESTER_H

#include "serial_io.h"
#include "mezz_tester.h"

#define ON 1
#define OFF 2
#define RESET 3

#define FIFO_EMPTY 1
#define FIFO_NOT_EMPTY 2
#define FIFO_FULL 3
#define FIFO_NEARLY_FULL 4
#define FIFO_INVALID -1

#define RFIFO_DEPTH 8000

#define TRIGGER 0
#define BCR 1
#define GR 2
#define ECR 3

#define NO_READOUT -2
#define NO_HITS -1
#define READSIZE_ERROR -3

#define TDC_REG_NUM 15
#define ASD_REG_NUM 11
#define DAC_REG_NUM 4

#define COARSE_ERROR 1
#define CHANNEL_SEL_ERROR 1<<1
#define L1_BUFFER_PAR_ERROR 1<<2
#define TRIGGER_FIFO_PAR_ERROR 1<<3
#define TRIGGER_MATCHING_ERROR 1<<4
#define READOUT_FIFO_PARITY_ERROR 1<<5
#define READOUT_STATE_ERROR 1<<6
#define CONTROL_PARITY_ERROR 1<<7
#define JTAG_PARITY_ERROR 1<<8
#define L1_BUFFER_OVERFLOW_ERROR 1<<9
#define TRIGGER_FIFO_OVERFLOW_ERROR 1<<10
#define READOUT_FIFO_OVERFLOW_ERROR 1<<11
#define HIT_ERROR 1<<12

#define CAL_INJECT_MASK 0
#define CAL_INJ_CAPS 1
#define DISC1_THR 2
#define DISC2_THR 3
#define DISC1_HYSTERSIS 4
#define INTEGRATION_GATE 5
#define RUNDOWN_CURRENT 6
#define DEADTIME 7
#define CHANNEL_LOWER 8
#define CHANNEL_UPPER 9
#define CHIP_MODE 10

#define MASK_WINDOW 1
#define SEARCH_WINDOW 2
#define MATCH_WINDOW 3
#define REJECT_OFFSET 4
#define EVENT_OFFSET 5
#define BUNCH_OFFSET 6
#define COARSE_OFFSET 7
#define COUNT_ROLLOVER 8
#define CHANNEL_11_0 13
#define CHANNEL_23_12 14

#define ALL_OFF 24

typedef struct TDCStatus_s 
{
  int rfifo;
  int errorflags;
  int tfifo;
  int tfifo_occ;
  int rfifo_occ;
  int coarse_counter;
  int running;
} TDCStatus_s;

typedef struct TDCHit_s
{
  int channel;
  int edge;
  int error;
  int coarseTime;
  int fineTime;
  float hitTime;
} TDCHit_s;

typedef struct HitReadout_s
{
  TDCHit_s hits[RFIFO_DEPTH];
  int numHits; 
  int eventID;
  int bunchID;
  int errorflags;
} HitReadout_s;

class MezzTesterBoard
{
  friend class MezzTester;
 public:
  MezzTesterBoard(const char* device_name, int ChannelMask = 0x000000);
  MezzTesterBoard(int TDC[15], int ASD[10], int DAC[4], const char* device_name, 
	     int ChannelMask = 0x000000);
  ~MezzTesterBoard();

  void Power(int pwr);
  void Init(void);
  void BoardReset();
  
  void SetTDCReg(int n, int value) {TDCRegs[n]=value;}
  int  GetTDCReg(int n) {return TDCRegs[n];}
  void SetASDReg(int n, int value) {ASDRegs[n]=value;}
  int  GetASDReg(int n) {return ASDRegs[n];}
  void SetDAC(int n, int value) {DACRegs[n]=value;}
  void SetAllDAC(int value) {for (int i=0; i<DAC_REG_NUM; i++)
                             DACRegs[i]=value;}
  int  GetDAC(int n) {return DACRegs[n];}
  int  GetChannelMask() {return ChannelMask;}
  void SetChannelMask(int mask) {ChannelMask=mask;}
  void SetChannel(int set_channel);
  int  getChannel() {return EnabledChannel;}
  void SetStrobePulsePeriod(int value) {StrobePulsePeriod=value;}
  int  GetStrobePulsePeriod() {return StrobePulsePeriod;}
  void SetHitPeriod(int value) {HitPeriod=value;}
  int  GetHitPeriod() {return HitPeriod;}

  void GetStatus(TDCStatus_s * TDCStatus);
  void UpdateBoard();
  void UpdateTDC() {WriteReg(TDCRegs, TDC_REG_NUM, "jtw");serial.Writeln("jtu");}
  void UpdateASD() {WriteReg(ASDRegs, ASD_REG_NUM, "jaw");serial.Writeln("jau");}
  void UpdateInjector();

  void TDCcmd(int cmd);
  void TDCBCR(int n);
  int  ReadFIFO(HitReadout_s * HitReadout);
  int  FIFOFlags();
  void ResetFIFO();
  
  SerialIO serial;
 private:
  void WriteReg(int Reg[], int RegSize, const char * cmd);
  

  int TDCRegs[TDC_REG_NUM];
  int ASDRegs[ASD_REG_NUM];
  int DACRegs[DAC_REG_NUM];
  int ChannelMask;
  int StrobePulsePeriod;
  int HitPeriod;
  int EnabledChannel;
};


#endif
