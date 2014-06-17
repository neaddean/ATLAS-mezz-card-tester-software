#ifndef SERIAL_IO_H
#define SERIAL_IO_H

#include <termios.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> //for close

#include <cstring> // for bzero
#include <cstdio> //for perror

#include <string>

#include <FDBlocking.h>


/*
  ==============================================================================
  SerialIO C++ class
  ==============================================================================
  This class can be used to open, close, read and write to a serial device.
  By default it assumes we are using a 8N1 9600 serial port in blocking mode. 
 
  Blocking can be toggled by SetBlocking(true/false).
  Changes made to FD and terminal flags only take effect only on a new Open call.
  
*/



class SerialIO
{
public:
  SerialIO();
  ~SerialIO();

  //FD options
  //Must call close+open to have an effect
  int  GetFD() const {return fd;}
  bool SetBlocking(bool state) {return SetFDBlocking(fd,state);}

  void SetFDFlags(int flag);
  void AddFDFlag(int flag);
  int  GetFDFlags() const {return fdFlags;}

  //Set device
  void SetDevice(char * str) {device = str;}
  void SetDevice(const std::string & str){device = str;}
  std::string GetDevice() const {return device;}

  //Terminal IO settings
  //Must call close+open to have an effect
  void SetBaudRate(int baud) {baudRate = baud;}
  void SetCFlags(int flags) {cFlags = flags;}
  void AddCFlag(int flag) {cFlags |= flag;}
  tcflag_t GetCFlags() const {return cFlags;}

  void SetOFlags(int flags) {oFlags = flags;}
  void AddOFlag(int flag){oFlags |= flag;}
  tcflag_t GetOFlags() const {return oFlags;}

  void SetIFlags(int flags) {iFlags = flags;}
  void AddIFlag(int flag) {iFlags |= flag;}
  tcflag_t GetIFlags() const {return iFlags;}

  void SetLFlags(int flags){lFlags=flags;}
  void AddLFlag(int flag){lFlags |= flag;}
  tcflag_t GetLFlags() const {return lFlags;}
  

  //IO open/close
  bool Open();
  bool Close();
  //IO operations
  int  Write(const char * buffer,size_t bufferSize) const;
  int  Write(const char * buffer) const;
  int  Writeln(const char * str, bool CompleteLine = true) const;
  int  Read(char * buffer,size_t bufferSize) const;
  int  Readln(char * buffer, int bufsize, char delim = '\n') const;
private:
  //System control variables 
  // (only for sys interface, assume they have been messed with!)
  std::string device;
  int fd; //File descriptor for the serial port.
  struct termios oldTermios;  //port properties before changes
  struct termios newTermios;  //current port properties

  //Serial properties variables
  int fdFlags;
  tcflag_t cFlags;
  tcflag_t iFlags;
  tcflag_t oFlags;
  tcflag_t lFlags;
  int baudRate;
  
  //No copying of class
  SerialIO(const SerialIO & rhs);
  SerialIO & operator=(const SerialIO & rhs);

  //
  void Reset(int level);
};
#endif
