//#include <CLI.hh>
#include "hcal/amc13/AMC13_CLI.hh"

AMC13_CLI::AMC13_CLI()
{
  // the include command
  includeString = "include";
  
  //The prompt style
  prompt = "$ ";
  
  //Filelevel (for recursion control)
  fileLevel = 0;
}


AMC13_CLI::~AMC13_CLI(){};

int AMC13_CLI::ProcessFile(std::string filename)
{
  if(fileLevel > 4)
    {
      fprintf(stderr,"Too many file include levels.\n");
      return -1;
    }
  fileLevel++;

  int foundCommands = 0;
  //open script file
  std::ifstream inFile(filename.c_str(),std::ifstream::in);
  //Check that the file opened and fail if it didn't
  if(inFile.fail())
    {
      fprintf(stderr,"Bad file: %s\n",filename.c_str());
      fileLevel--;
      return -1;
    }
  //read file until we are at the end
  while(!inFile.eof())
    {
      std::string line;
      //Read one line of the file
      std::getline(inFile,line);

      //Process this line
      int lineCommandCount = ProcessLine(line);
      //Check for a parse error
      if(lineCommandCount == -1)
	{
	  fileLevel--;
	  return -1;
	}
      //Update the command count
      foundCommands+=lineCommandCount;
    }
  fileLevel--;
  return foundCommands;
}

int AMC13_CLI::ProcessLine(std::string line)
{
  //count of found commads in this string
  int foundCommands = 0;

  //eat white space at the beginning of the line
  while((line.size() > 0) && (line[0] == ' '))
    {
      line.erase(0,1); //remove the first char
    }
  
  //Check that we still have a line
  if(line.empty())
    {
      //move on if we don't
      return 0;
    }
  
  //check if this line is commented out
  if(line[0] == '#')
    {
      return 0;  // move on to the next line
    }
  
  //Check for an include
  if(line.find(includeString) == 0) //if we find includeString at the beginning of the file
    {
      //extract the filename and pass it to ProcessFile for parsing
      std::string includeFilename = line.substr(includeString.size());

      //remove any white space
      while((includeFilename.size() > 0) && (includeFilename[0] == ' '))
	{
	  includeFilename.erase(0,1); //remove the first char
	}      
      
      //process the include file
      int foundSubCommands = ProcessFile(includeFilename);
      
      //Check if we found anything or got an error
      if(foundSubCommands < 0)
	{
	  fprintf(stderr,"Bad subfile(%d): %s, bailing out!\n",fileLevel,line.c_str());
	  return -1;
	}
      foundCommands+=foundSubCommands;
    }
  else  //We have some other text, so treat it as a command
    {
      //Add a command
      Commands.push_back(line);
      foundCommands++;     
    }
  
  return foundCommands;
}


std::vector<std::string> AMC13_CLI::GetInput()
{
  //Command string
  std::string currentCommand("");


  //Load a command

  //Check if we have a command from a loadedfile
  if(Commands.size())
    {
      //load the first command into currentCommand
      currentCommand = Commands.front();
      //delete that first command
      Commands.pop_front();
    }
  else
    {
      //std::getline(std::cin,currentCommand);
      //Get user input from gnu readline
      char * rlLine = readline(prompt.c_str());
      if( rlLine == NULL)
	{
	  fprintf(stderr,"EOF on prompt!\n");	  
	  exit(0);
	}
      else
	{
	  //convert readline string to std::string
	  currentCommand = std::string(rlLine);      
	  free(rlLine);  //Free readline's string
	  
	  //Parse the command string
	  ProcessLine(currentCommand);
	  if(Commands.size())			      
	    {
	      //load the first command into currentCommand
	      currentCommand = Commands.front();
	      //delete that first command
	      Commands.pop_front();
	    }
	}
    }

  if(!currentCommand.empty())
    {
      //Add the command to the history
      add_history(currentCommand.c_str());
    }

  //Parse the command
  return ParseInput(currentCommand);  
}


std::vector<std::string> AMC13_CLI::ParseInput(std::string command)
{
  std::vector<std::string> commandArgv;
  while(!command.empty())
    {
      //Find first split char
      size_t pos = command.find(' ');
      if(pos != std::string::npos)
	{
	  //Add this string to our list
	  commandArgv.push_back(command.substr(0,pos));
	  //Remove it from the command
	  command = command.substr(pos+1,std::string::npos);
	}
      else
	{
	  //This is the last command, so add it to the list
	  commandArgv.push_back(command);
	  //Empty the string
	  command.clear();
	}
    }
  //return this command
  return commandArgv;
}

int AMC13_CLI::ProcessString(std::string command)
{
  int foundCommands = 0;

  while(!command.empty())
    {
      //Find first split char
      size_t pos = command.find('\n');
      if(pos != std::string::npos)
	{
	  //Process this line
	  int foundSubCommands = ProcessLine(command.substr(0,pos));
	  if(foundSubCommands == -1)
	    {
	      return -1;
	    }
	  foundCommands += foundSubCommands;

	  //Remove it from the command
	  command = command.substr(pos+1,std::string::npos);
	}
      else
	{
	  //Process this line
	  int foundSubCommands = ProcessLine(command);
	  if(foundSubCommands == -1)
	    {
	      return -1;
	    }
	  foundCommands += foundSubCommands;

	  //Empty the string
	  command.clear();
	}
    }
  return ProcessLine(command);
}
