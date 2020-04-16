#include <stdio.h>

#include <net.h>

#include <someip/net.h>

ssize_t
someip_recvn(int f, struct someip * o, unsigned to)
{
	ssize_t i;

	i = recvn_timeout(f, o, someip_len_HDR, 0, to);
	if(i != someip_len_HDR)
		return i;

	i = someip_u4(o->len);
	if(i)
	{
		i = recvn_timeout(f, o->data, i, 0, to);
		if(i < 0)
			return i;
	}

	return i + someip_len_HDR;
}
