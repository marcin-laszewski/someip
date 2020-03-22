#include <file.h>
#include <someip/net.h>

ssize_t
someip_recv(int s, struct  someip * o, size_t len, struct sockaddr * addr, socklen_t addr_len, unsigned to)
{
	if(to && file_select_read(s, to) <= 0)
		return -2;

	{
		ssize_t i;

		i = recvfrom(s, o, len, 0, addr, &addr_len);
		if(i < 0)
			return i;

		if(i <= someip_len_HDR1 + someip_len_HDR2)
			return -3;

		return i;
	}
}
