#include <serial_io.h>
#include <errno.h>

SerialIO::SerialIO()
{
  Reset(1);
}

SerialIO::~SerialIO()
{
  //make sure the port is closed.
  Close();
}

//Reset internals
//   Level 0 is just fd and term strucutres
//   Level 1+ is all internal values
void SerialIO::Reset(int level)
{
  //Connection values
  fd = -1;
  bzero(&oldTermios,sizeof(oldTermios));
  bzero(&newTermios,sizeof(newTermios));
  
  if(level > 0)
    {
      fdFlags = O_RDWR | O_NOCTTY | O_NDELAY;
  
      //Default values
      lFlags = 0;
      oFlags = 0;
      iFlags = 0;
      cFlags = CS8 | CLOCAL | CREAD;
      baudRate = B500000;
      
      device.clear();  
    }
}

bool SerialIO::Open()
{
  //Close the device if it is already opened
  if(fd < 0)
    {
      Close();
    }

  //Open device
  fd = open(device.c_str(),fdFlags);
  if(fd < 0)
    {
      perror(device.c_str());
      Close();
      return false;
    }
  SetBlocking(true);

  //Get old termios and build newone
  tcgetattr(fd,&oldTermios);
  bzero(&newTermios,sizeof(newTermios));

  newTermios.c_cflag = baudRate | cFlags;
  newTermios.c_iflag = iFlags;
  newTermios.c_oflag = oFlags;
  newTermios.c_lflag = lFlags;

  //Flush the current line
  tcflush(fd,TCIFLUSH);
  //Apply new settings
  struct termios tempTermios=newTermios;
  if(tcsetattr(fd,TCSANOW,&newTermios) < 0)
    {
      perror("Serial tcsetattr:");
      Close();
      return false;
    }
  //Check that newTermios is the same as tempTermios
  if( (newTermios.c_cflag != tempTermios.c_cflag) ||
      (newTermios.c_iflag != tempTermios.c_iflag) ||
      (newTermios.c_oflag != tempTermios.c_oflag) ||
      (newTermios.c_lflag != tempTermios.c_lflag) )
    {
      printf("Not all settings are correct!\n");
      //Report what didn't work
      printf("cflag: 0x%08X %c= 0x%08X\n",
	     newTermios.c_cflag,
	     (newTermios.c_cflag == tempTermios.c_cflag ? '=' : '!' ),
	     tempTermios.c_cflag);
      printf("iflag: 0x%08X %c= 0x%08X\n",
	     newTermios.c_iflag,
	     (newTermios.c_iflag == tempTermios.c_iflag ? '=' : '!' ),
	     tempTermios.c_iflag);
      printf("oflag: 0x%08X %c= 0x%08X\n",
	     newTermios.c_oflag,
	     (newTermios.c_oflag == tempTermios.c_oflag ? '=' : '!' ),
	     tempTermios.c_oflag);
      printf("lflag: 0x%08X %c= 0x%08X\n",
	     newTermios.c_lflag,
	     (newTermios.c_lflag == tempTermios.c_lflag ? '=' : '!' ),
	     tempTermios.c_lflag);
      Close();
      return false;
    }
  return true;
}

bool SerialIO::Close()
{
  if(fd >= 0)
    {
      SetBlocking(true);
      close(fd);  
      Reset(0);
    }
  return true;
}

int  SerialIO::Write(const char * buffer,size_t bufferSize) const
{
  //If there is a valid FD, call write and return the result
  if(fd > 0)
    {
      return write(fd,buffer,bufferSize);
    }
  //if there is a bad fd, return -2 
  //(we don't return -1, since that would mix this with a system error with an errno)
  return -2;
}

int SerialIO::Write(const char * str) const
{ 
  int num = 0;
  if(fd > 0)
    {
      while(*str != 0x00)
	{
	  if (write(fd, str++, 1)==-1)
	    printf("Write error in %s line %d: %s\n", __FILE__, __LINE__, strerror(errno));
	  num++;
	}
      return num;
    }
  return -2;
}

int SerialIO::Writeln(const char * str, bool CompleteLine) const
{ 
  int num = 0;
  char tempc = ' ';
  if(fd > 0)
    {
      while(*str != 0x00)
	{
	  if (write(fd, str++,1)==-1)
	    printf("Write error in %s line %d: %s\n", __FILE__, __LINE__, strerror(errno));
	  num++;
	}
      if (write(fd, "\n", 1)==-1)
	printf("Write error in %s line %d: %s\n", __FILE__, __LINE__, strerror(errno));
      num++;
      do 
	{
	  if (read(fd, &tempc, 1)==-1)
	    printf("Read error in %s line %d: %s\n",__FILE__, __LINE__, strerror(errno));
	} while (tempc != '\n');
      if (CompleteLine)
	{
	  do {
	    if (read(fd, &tempc, 1)==-1)
	      printf("Read error in %s line %d: %s\n",__FILE__, __LINE__, strerror(errno));
	  } while (tempc != '>');
	}
      return num;
    }
  return -2;
}


int  SerialIO::Read(char * buffer,size_t bufferSize) const
{
  //If there is a valid FD, call write and return the result
  if(fd > 0)
    {
      return read(fd,buffer,bufferSize);
    }
  //if there is a bad fd, return -2 
  //(we don't return -1, since that would mix this with a system error with an errno)
  return -2;
}

int  SerialIO::Readln(char * buffer, int bufsize, char delim) const 
{
  char tempc = '`';
  int i = 0;
  ssize_t ret = 0;
  memset(buffer, 0, bufsize);
  if(fd > 0)
    {
      while (1) {
	ret = read(fd, &tempc, 1);
	if (ret == -1)
	  {
	    printf("Read error in %s line %d: %s\n", __FILE__, __LINE__, strerror(errno));
	    return i;
	  }
	buffer[i++] = tempc;
	if (ret == 0)
	  {
	    if (i>0)
	      i--;
	  }
	if (i == bufsize-1)
	  {
	    buffer[i] = '\0';
	    printf("ERROR: Buffer overflow on Readln().\n"
		   "Size: %d\n"
		   "----------------------\n"
		   "***%s***\n"
		   "----------------------\n", i, buffer);
	    return i;
	  }
	if(tempc == delim)
	  {
	    if (delim=='>')
	      i-=3;
	    else if (delim=='\n')
	      i=i-2;
	    buffer[i] = '\0';
	    return i;
	  }
      }
    }
  return -2;
}
