#include <FDBlocking.h>

bool SetFDBlocking(int fd,bool value)
{
  //Get the previous flags
  int currentFlags = fcntl(fd,F_GETFL,0);
  if(currentFlags < 0)
    {
      return false;
    }
    //Make the socket non-blocking
  if(!value)
    {
      currentFlags |= O_NONBLOCK;
    }
  else
    {
      currentFlags &= ~O_NONBLOCK;
    }
  int currentFlags2 = fcntl(fd,F_SETFL,currentFlags);
  if(currentFlags2 < 0)
    {
      return false ;
    }
  return true ;
}    
