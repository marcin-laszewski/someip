#include <string.h>

#include <someip/utils.h>

int
someip_args_print(int args, char * const * argv, unsigned * i, unsigned * mask)
{
	if(!strcmp(argv[*i], "--print-data"))
		*mask |= arg_PRINT_DATA;
	else if(!strcmp(argv[*i], "--print-hdr"))
		*mask |= arg_PRINT_HDR;
	else if(!strcmp(argv[*i], "--print-recv"))
		*mask |= arg_PRINT_RECV;
	else
		return 1;

	return 0;
}
