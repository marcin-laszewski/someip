#include <net.h>

struct sockaddr_in * net_addr(const char * hostname, int port,
                              struct sockaddr_in * addr)
{
#if defined(DONTUSE_GETHOSTBYNAME)
  in_addr_t ad;
    
  ad = inet_addr(hostname);
  if(ad == INADDR_NONE)
      return NULL;
  return net_addr_ip(ad, port, addr);
#else
  return net_addr_name(hostname, port, addr);
#endif
}
