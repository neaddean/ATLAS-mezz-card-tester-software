#ifndef AMC13_CLI_HH
#define AMC13_CLI_HH

#include <string>
#include <deque>
#include <vector>

#include <fstream>
#include <iostream>
#include <stdlib.h>

//gnu readline
#include <readline/readline.h>
#include <readline/history.h>


class AMC13_CLI
{
public:
  AMC13_CLI();
  ~AMC13_CLI();

  //Load a script file
  int ProcessFile(std::string filename);

  //Inject a command string
  int ProcessString(std::string command);

  //Get the next command (std::cin or from file)
  //From file first, then parse cin
  std::vector<std::string> GetInput();

private:
  //Get a command string
  int ProcessLine(std::string line);
  //Parse the command string
  std::vector<std::string> ParseInput(std::string command);

  //Interally stored commands
  std::deque<std::string> Commands;

  //include file recusion overflow stopper
  int fileLevel;

  //include command
  std::string prompt;
  std::string includeString;
};

#endif
