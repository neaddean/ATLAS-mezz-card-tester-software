#ifndef HCAL_AMC13_COMMANDS
#define HCAL_AMC13_COMMANDS

#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include "mezztester/mezz_tester.h"

class AMC13_Launcher {
public:
  AMC13_Launcher(const char * device_name);
  ~AMC13_Launcher();

  int EvaluateCommand(std::vector<std::string> command);

private:
  //A mapping between command string and function to call
  //The called function returns an int and takes an AMC13 class, a vector of argument strings and a vector of uint64_t conversions of those strings
  std::map<std::string,
	   int (AMC13_Launcher::*)(std::vector<std::string>,
				   std::vector<uint64_t>)> List;

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
  int cli (std::vector<std::string>,std::vector<uint64_t>);
  int tsweep (std::vector<std::string>,std::vector<uint64_t>);
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
  int load_default(std::vector<std::string>,std::vector<uint64_t>);
};
#endif
