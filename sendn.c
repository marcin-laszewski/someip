#include <stdio.h>

#include <net.h>

#include <someip/net.h>

static ssize_t
sendn_msg(int f, void * buf, size_t len, unsigned to)
{
	ssize_t i;

	i = sendn_timeout(f, buf, len, 0, to);
	if(i < 0)
	{
		perror("sendn");
		return 5;
	}

	if(i != len)
		fprintf(stderr, "send: Not enought [%lu]: %ld\n", len, i);

	return i;
}

ssize_t
someip_sendn(int f, struct someip * o, unsigned to)
{
	size_t n = someip_u4(o->len) + someip_len_HDR;

	if(sendn_msg(f, o, n, to) != n)
		return -1;

	return 0;
}
