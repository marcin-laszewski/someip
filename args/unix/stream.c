#include <string.h>

#include <someip/utils.h>

int
someip_args_unix_stream(int argc, char * const * argv, unsigned * i,
	void (* usage_exit)(char const *), unsigned * mask, char const **path)
{
	if(strcmp(argv[*i], "--unix-stream"))
		return 1;

	if(*i + 1 >= argc)
		usage_exit(argv[0]);

	*path = argv[++(*i)];
	*mask |= arg_UNIX_STREAM;

	return 0;
}
