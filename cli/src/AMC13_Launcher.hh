#ifndef HCAL_AMC13_COMMANDS
#define HCAL_AMC13_COMMANDS

#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <stdint.h>
#include "mezz_tester.h"

class AMC13_Launcher {
public:
  AMC13_Launcher(const char * device_name);
  ~AMC13_Launcher();

  int EvaluateCommand(std::vector<std::string> command);

private:
  //A mapping between command string and function to call
  //The called function returns an int and takes an AMC13 class, a vector of argument strings and a vector of uint64_t conversions of those strings

  void AddCommand(std::string name, 
		  int (AMC13_Launcher::*)(std::vector<std::string>,std::vector<uint64_t>),
		  std::string help, 
		  std::string (AMC13_Launcher::*)(std::string,int)=NULL);
  void AddCommandAlias( std::string alias, std::string existingCommand );
  int FindCommand(std::string command);

  std::vector<std::string> commandName;
  std::vector<int (AMC13_Launcher::*)(std::vector<std::string>,std::vector<uint64_t>)> commandPtr;
  std::vector<std::string> commandHelp;
  std::vector<std::string (AMC13_Launcher::*)(std::string,int)> commandAutoComplete;
  //====================================================
  //Mezzanine Board Tester Class
  //====================================================
  MezzTester * mezzTester;
  //====================================================
  //Add your commands here
  //====================================================

  //Here is where you update the map between string and function
  void LoadCommandList();

  //Add new command functions here
  int Help(std::vector<std::string>,std::vector<uint64_t>);	   
  int Quit(std::vector<std::string>,std::vector<uint64_t>);	   
  int Echo(std::vector<std::string>,std::vector<uint64_t>);
  int cli(std::vector<std::string>,std::vector<uint64_t>);
  int tsweep_man(std::vector<std::string>,std::vector<uint64_t>);
  int tsweep(std::vector<std::string>,std::vector<uint64_t>);
  int fsweep(std::vector<std::string>,std::vector<uint64_t>);
  int Trigger(std::vector<std::string>,std::vector<uint64_t>);
  int jtw(std::vector<std::string>,std::vector<uint64_t>);
  int jaw(std::vector<std::string>,std::vector<uint64_t>);
  int jtr(std::vector<std::string>,std::vector<uint64_t>);
  int jar(std::vector<std::string>,std::vector<uint64_t>);
  int UpdateBoard(std::vector<std::string>,std::vector<uint64_t>);
  int SetChannelMask(std::vector<std::string>,std::vector<uint64_t>);
  int SetChannel(std::vector<std::string>,std::vector<uint64_t>);
  int SetHitPeriod(std::vector<std::string>,std::vector<uint64_t>);
  int SetStrobePulsePeriod(std::vector<std::string>,std::vector<uint64_t>);
  int load_test(std::vector<std::string>,std::vector<uint64_t>);
  int dump(std::vector<std::string>,std::vector<uint64_t>);
  int mw(std::vector<std::string>,std::vector<uint64_t>);
  int bo(std::vector<std::string>,std::vector<uint64_t>);
  int treset(std::vector<std::string>,std::vector<uint64_t>);
  int d(std::vector<std::string>,std::vector<uint64_t>);
  int load_inject(std::vector<std::string>,std::vector<uint64_t>);
  int dac_sweep(std::vector<std::string>,std::vector<uint64_t>);
  int fdac_sweep(std::vector<std::string>,std::vector<uint64_t>);
  int jtag_test(std::vector<std::string>,std::vector<uint64_t>);
  int TDC_status(std::vector<std::string>,std::vector<uint64_t>);
};
#endif
