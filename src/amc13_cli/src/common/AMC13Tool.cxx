#include <string>
#include <vector>
#include <iostream>

#include "hcal/amc13/AMC13_Launcher.hh"
#include "hcal/amc13/AMC13_CLI.hh"


int main(int argc, char* argv[]) 
{
  (void) argc;
  (void) argv;
  //Create CLI
  AMC13_CLI cli;
  //============================================================================
  //Main loop
  //============================================================================

  //Create Command launcher
  AMC13_Launcher launcher;

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
