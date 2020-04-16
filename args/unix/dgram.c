#include <string.h>

#include <someip/utils.h>

int
someip_args_unix_dgram(int argc, char * const * argv, unsigned * i,
	void (* usage_exit)(char const *), unsigned * mask, char const **remote)
{
	if(strcmp(argv[*i], "--unix-dgram"))
		return 1;

	if(*i + 1 >= argc)
		usage_exit(argv[0]);

	*remote = argv[++(*i)];
	*mask |= (arg_NET | arg_net_domain_UNIX | arg_net_type_DGRAM);

	return 0;
}
