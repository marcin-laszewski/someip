#include <net.h>

struct sockaddr_in *
net_addr_name(const char * host, int port, struct sockaddr_in * addr)
{
  struct hostent * h;

  h = gethostbyname(host);
  if(h == NULL)
      return NULL;

  return net_addr_hostent(h, port, addr);
}
