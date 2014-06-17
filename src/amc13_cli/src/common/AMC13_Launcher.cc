#include "hcal/amc13/AMC13_Launcher.hh"

AMC13_Launcher::AMC13_Launcher()
{
  LoadCommandList(); //Load list
};

AMC13_Launcher::~AMC13_Launcher()
{
}

int AMC13_Launcher::EvaluateCommand(std::vector<std::string> command)
{
  //Handle no command
  if(command.size() == 0)
    {
      return 0;
    }

  //Search for command
  if(List.find(command[0]) != List.end())
    {
      //Command found!

      //Build the string & uint64_t argument vector
      std::vector<std::string> argListString;
      std::vector<uint64_t> argListUint64_t;
      for(size_t iArg = 1; iArg < command.size();iArg++)
	{
	  argListString.push_back(command[iArg]);
	  argListUint64_t.push_back(strtoul(command[iArg].c_str(),NULL,0));
	}
      
      //Call the function associated with this command
      //If the return value is negative, we will quit
      return (*this.*(List[command[0]]))(/*AMC13,*/
					 argListString,
					 argListUint64_t);

    }
  else //command not found
    {
      printf("Bad Command.\n");
      return 0;
    }
}

