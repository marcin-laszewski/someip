#include <someip/net.h>

ssize_t
someip_send(int s, struct someip * o, struct sockaddr * addr, socklen_t addr_len)
{
	return sendto(s, o, someip_u4(o->len) + someip_len_HDR1, 0, addr, addr_len);
}

