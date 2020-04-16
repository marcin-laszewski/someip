#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.h"

int
unix_unlink(char const * path)
{
	if(unlink(path) < 0
	&& errno != ENOENT)
	{
		perror(path);
		return -1;
	}

	return 0;
}

