#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <net.h>

#include "utils.h"

int
unix_open(struct sockaddr ** addr, socklen_t * addr_len, int type,
	struct sockaddr_un * addr_un, char const * path)
{
	int s;
	
	s = socket(AF_UNIX, type, 0);
	if(s < 0)
	{
		fputs("socket(UNIX)\n", stderr);
		return -1;
	}

	if(unix_addr(addr, addr_len, addr_un, path) < 0)
		return -2;

	/*
	{
		int i;

		i = net_unix_addr(addr_un, path);
		if(i < 0)
		{
			fputs("Incorrect path: ", stderr);
			fputs(path, stderr);
			fputc('\n', stderr);
			return -3;
		}

		*addr = (struct sockaddr *)addr_un;
		*addr_len = i;
	}
	*/

	return s;
}
