#include <string>
#include <vector>
#include <iostream>

#include "AMC13_Launcher.hh"
#include "AMC13_CLI.hh"
#include "tclap/CmdLine.h"


int main(int argc, char* argv[]) 
{
  //Create CLI
  AMC13_CLI cli;

  TCLAP::CmdLine cmd("Tool for testing ATLAS MDT boards.",
		     ' ',
		     "MezzBoardTester v1");
  
  TCLAP::ValueArg<std::string> device_name("D",
					   "device",			   
					   "port of the MezzTester Board",
					   false,
					   "/dev/ttyUSB0",
					   "string",
					   cmd);
 

  TCLAP::MultiArg<std::string> scriptFiles("X",                   //one char flag
					   "script",              // full flag name
					   "script filename",     //description
					   false,                 //required
					   "string",              // type
					   cmd);

  // parse the arguments
  try {
    cmd.parse(argc, argv);
  } catch (TCLAP::ArgException &e) {
    fprintf(stderr, "Error %s for arg %s\n",
  	    e.error().c_str(), e.argId().c_str());
    return 0;
  }

  //Load scripts
  for(std::vector<std::string>::const_iterator it = scriptFiles.getValue().begin(); 
      it != scriptFiles.getValue().end();
      it++)
    {
      cli.ProcessFile(*it);
    }


  //============================================================================
  //Main loop
  //============================================================================

  //Create Command launcher
  AMC13_Launcher launcher (device_name.getValue().c_str());

  while (true) 
    {
      //Get parsed command
      std::vector<std::string> command = cli.GetInput();

      //Launch command function
      if(launcher.EvaluateCommand(command) < 0)
	{
	  //Shutdown tool
	  break;
	}      
    }

  return 0;
}
