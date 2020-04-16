#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <net.h>

#include "utils.h"

int
unix_addr(struct sockaddr ** addr, socklen_t * addr_len,
	struct sockaddr_un * addr_un, char const * path)
{
	int i;

	i = net_unix_addr(addr_un, path);
	if(i < 0)
	{
		fputs("Incorrect path: ", stderr);
		fputs(path, stderr);
		fputc('\n', stderr);
		return -1;
	}

	*addr = (struct sockaddr *)addr_un;
	*addr_len = i;

	return 0;
}
