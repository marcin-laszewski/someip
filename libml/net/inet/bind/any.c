#include <net.h>

int net_inet_bind_any(int s, int port)
{
  struct sockaddr_in	addr;

  addr.sin_family		= AF_INET;
  addr.sin_addr.s_addr	= htonl(INADDR_ANY);
  addr.sin_port		= htons(port);
  
  if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    return -1;

  return s;
}
