#include <net.h>

int net_udp_socket(void)
{
  return socket(AF_INET, SOCK_DGRAM, 0);
}
