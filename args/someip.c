#include <stdlib.h>
#include <string.h>

#include <someip/utils.h>

int
someip_args_someip(int argc, char * const * argv, unsigned * i, unsigned * mask,
	void (* usage_exit)(char const *), unsigned short * service,
	unsigned short * method, unsigned short * client, unsigned short * session)
{
	if(!strcmp(argv[*i], "--service"))
	{
		(*i)++;
		if(*i >= argc)
			usage_exit(argv[0]);

		*service = strtold(argv[*i], NULL);
		*mask |= arg_SERVICE;
	}
	else if(!strcmp(argv[*i], "--method"))
	{
		(*i)++;
		if(*i >= argc)
			usage_exit(argv[0]);

		*method = strtold(argv[*i], NULL);
		*mask |= arg_METHOD;
	}
	else if(!strcmp(argv[*i], "--client"))
	{
		(*i)++;
		if(*i >= argc)
			usage_exit(argv[0]);

		*client = strtold(argv[*i], NULL);
		*mask |= arg_CLIENT;
	}
	else if(!strcmp(argv[*i], "--session"))
	{
		(*i)++;
		if(*i >= argc)
			usage_exit(argv[0]);
		
		*session = strtold(argv[*i], NULL);
		*mask |= arg_SESSION;
	}
	else
		return 1;

	return 0;
}
