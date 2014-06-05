#ifndef MEZZ_TESTER_H
#define MEZZ_TESTER_H

#include <serial_io.h>
#include <mezz_tester_cmds.h>

#define ON 1
#define OFF 2
#define RESET 3

#define FIFO_EMPTY 1
#define FIFO_FULL 2
#define FIFO_ALMOST_FULL 3

#define RFIFO_EMPTY 1
#define RFIFO_FULL 2

#define TFIFO_EMPTY 1 
#define TFIFO_NEARLY_FULL 2
#define TFIFO_FULL 3

#define TRIGGER 0
#define BCR 1
#define GR 2
#define ECR 3

#define TDC_REG_NUM 15
#define ASD_REG_NUM 10
#define DAC_REG_NUM 4


typedef struct TDCStatus_s 
{
int rfifo;
int error_flags;
int tfifo;
int tfifo_occupancy;
int coarse_counter;
} TDCStatus_s;

class MezzTester
{
public:
MezzTester(char* device_name);
MezzTester(int TDC[15], int ASD[10], int DAC[4], char* device_name, 
	     int ChannelMask = 0x000000);
~MezzTester();

void Power(int pwr);
  
void SetTDCReg(int n, int value) {TDCRegs[n]=value;}
int  GetTDCReg(int n) {return TDCRegs[n];}
void SetASDReg(int n, int value) {ASDRegs[n]=value;}
int  GetASDReg(int n) {return ASDRegs[n];}
void SetDAC(int n, int value) {DACRegs[n]=value;}
void SetAllDAC(int value) {for (int i=0; i<DAC_REG_NUM; i++)
			     DACRegs[i]=value;}
int  GetDAC(int n) {return DACRegs[n];}
int  GetChannelMask() {return ChannelMask;}
void SetChannelMask(int mask) {ChannelMask=mask;};

TDCStatus_s GetStatus();
void Update();

void TDCcmd(int cmd);
void TDCBCR(int n);
void ReadFIFO(char * buffer);
int FIFOFlags();
void ResetFIFO();
  
private:
void WriteReg(int Reg[], int RegSize, const char * cmd);
  
SerialIO serial;

int TDCRegs[TDC_REG_NUM];
int ASDRegs[ASD_REG_NUM];
int DACRegs[DAC_REG_NUM];
int ChannelMask;
};


#endif
