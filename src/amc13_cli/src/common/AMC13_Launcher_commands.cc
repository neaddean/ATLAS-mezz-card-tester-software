#include "hcal/amc13/AMC13_Launcher.hh"

void AMC13_Launcher::LoadCommandList()
{
  //Help
  List["help"] = &AMC13_Launcher::Help;
  //Quit
  List["quit"] = &AMC13_Launcher::Quit;
  List["exit"] = &AMC13_Launcher::Quit;
  
  //Echo
  List["echo"] = &AMC13_Launcher::Echo;
}



int AMC13_Launcher::Quit(std::vector<std::string>,
			 std::vector<uint64_t>)
{
  //Quit CLI so return -1
  return -1;
}

int AMC13_Launcher::Echo(std::vector<std::string> strArg,
			 std::vector<uint64_t> intArg)
{
  (void) intArg;
  for(size_t iArg = 0; iArg < strArg.size();iArg++)
    {
      printf("%s ",strArg[iArg].c_str());
    }
  printf("\n");
  return 0;
}


int AMC13_Launcher::Help(std::vector<std::string> strArg,
			 std::vector<uint64_t> intArg)
{
  (void) strArg;
  (void) intArg;
  std::map<std::string,
	   int (AMC13_Launcher::*)(std::vector<std::string>,
				   std::vector<uint64_t>)>:: iterator itList;

  printf("Commands:\n");  
  for(itList = List.begin();itList != List.end();itList++)
    {
      printf("   %s\n",itList->first.c_str());
    }
  return 0;
}
