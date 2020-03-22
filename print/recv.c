#include <stdio.h>

#include <someip/utils.h>

void
someip_print_recv(ssize_t i, unsigned arg_mask)
{
	if(arg_mask & arg_PRINT_RECV)
		printf("recv:\t%ld\n", i);
}
