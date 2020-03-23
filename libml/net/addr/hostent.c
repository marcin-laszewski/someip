#include <string.h>

#include <net.h>

struct sockaddr_in *
net_addr_hostent(struct hostent * h, int port, struct sockaddr_in * addr)
{
/*addr->sin_addr.s_addr	= htonl(((struct in_addr *)h->h_addr)->s_addr);*/
  addr->sin_family = h->h_addrtype;
  memcpy(&addr->sin_addr.s_addr, h->h_addr_list[0], h->h_length);
/*  addr->sin_addr.s_addr	= htonl(addr->sin_addr.s_addr);*/
  addr->sin_family	= AF_INET;
  addr->sin_port	= htons(port);

  return addr;
}
