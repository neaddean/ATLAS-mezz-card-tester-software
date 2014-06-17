#ifndef HCAL_AMC13_COMMANDS
#define HCAL_AMC13_COMMANDS

#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

class AMC13_Launcher {
public:
  AMC13_Launcher();
  ~AMC13_Launcher();

  int EvaluateCommand(std::vector<std::string> command);

private:
  //A mapping between command string and function to call
  //The called function returns an int and takes an AMC13 class, a vector of argument strings and a vector of uint64_t conversions of those strings
  std::map<std::string,
	   int (AMC13_Launcher::*)(std::vector<std::string>,
				   std::vector<uint64_t>)> List;

  //====================================================
  //Add your commands here
  //====================================================
  // std::vector<AMC13Simple *> AMCModgule;
  // size_t defaultAMC13;

  //====================================================
  //Add your commands here
  //====================================================

  //Here is where you update the map between string and function
  void LoadCommandList();

  //Add new command functions here
  int Help(std::vector<std::string>,std::vector<uint64_t>);	   
  int Quit(std::vector<std::string>,std::vector<uint64_t>);	   
  int Echo(std::vector<std::string>,std::vector<uint64_t>);	       
};
#endif
