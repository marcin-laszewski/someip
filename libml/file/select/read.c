#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <file.h>

#if defined(__MINGW32__)
#	include <winsock2.h>
#endif

int file_select_read(int f, unsigned timeout)
{
  fd_set		fd;
  struct timeval	tv;
  int			i;
  
  FD_ZERO(&fd);
  FD_SET(f, &fd);

  if(timeout != file_select_INF)
  {
    tv.tv_sec	= timeout;
    tv.tv_usec	= 0;
  }
  
  i = select(f + 1, &fd, NULL, NULL, timeout == file_select_INF ? NULL : &tv);
  
/*  printf("fdset = %d\n", FD_ISSET(f, &fd));*/
  
  if(i > 0)
    return 1;
    
  return i;
}
