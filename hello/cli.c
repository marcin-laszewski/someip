#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "net.h"
#include <someip.h>
#include <someip/net.h>
#include <someip/utils.h>

#include "config.h"

#define	BUFLEN	4096
#define	STDIN_BUFLEN	256

#define	CL_DATA	"World"
#define	CL_DATA_LEN	(sizeof(CL_DATA) - 1)

/*
	Message ...
	... service:		0x11, 0x11,
	...	method:			0x33, 0x33,
	Length = 13:		0x00, 0x00, 0x00, 0x0d,
	Request ...
	...	AppID:			0x55, 0x55,
	...	Session:		0x00, 0x01,
	Proto ID:				0x01,
	Iface:					0x00,
	Type (REQUEST):	0x00
	Retcode (OK):		0x00,
	Data (World):		0x57, 0x6f, 0x72, 0x6c, 0x64
*/

void usage_exit(char const * progname)
{
	fputs(progname, stderr);
	fputs(
		" [--loop ms] [--print-hdr] [--print-data] [--print-recv] [--timeout sec]"
		" {--udp inet-addr port} {--service id} {--method id} {--client id} {--session id}\n",
		stderr);
	exit(1);
}

int set_data_n(struct someip * o, unsigned short method, unsigned n, int i, double * tab)
{
	if(--i != n)
	{
		fprintf(stderr, "Incorrect nunmber of expected arguments [%u]: %d\n", n, i);
		return -1;
	}

	someip_set_method(o, method);
	someip_set_data(o, tab, n * sizeof(*tab));

	return 0;
}


#define	arg_MANDATORY	(arg_UDP | arg_SERVICE | arg_METHOD | arg_CLIENT | arg_SESSION)

int
main(int argc, char *argv[])
{
	char const	*host	= NULL;
	char const	*port	= NULL;
	unsigned loop	= UINT_MAX;
	struct sockaddr_in	addr;
	struct sockaddr_in	addr_sender;
	int s;
	unsigned char	buf[BUFLEN];
	ssize_t	i;
	struct someip *	o	= (struct someip *)buf;
	unsigned	arg_mask = 0;
	unsigned short	client	= 0;
	unsigned short	service	= 0;
	unsigned short	method	= 0;
	unsigned short	session	= 0;
	unsigned	to	= 0;

	{
		unsigned i;

		for(i = 1; i < argc; i++)
		{
			if(!strcmp(argv[i], "--client"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);

				client = strtold(argv[i], NULL);
				arg_mask |= arg_CLIENT;
			}
			else if(!strcmp(argv[i], "--stdin"))
				arg_mask |= arg_STDIN;
			else if(!strcmp(argv[i], "--loop"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);

				loop = atoi(argv[i]);
			}
			else if(!strcmp(argv[i], "--method"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);

				method = strtold(argv[i], NULL);
				arg_mask |= arg_METHOD;
			}
			else if(!strcmp(argv[i], "--print-data"))
				arg_mask |= arg_PRINT_DATA;
			else if(!strcmp(argv[i], "--print-hdr"))
				arg_mask |= arg_PRINT_HDR;
			else if(!strcmp(argv[i], "--print-recv"))
				arg_mask |= arg_PRINT_RECV;
			else if(!strcmp(argv[i], "--service"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);

				service = strtold(argv[i], NULL);
				arg_mask |= arg_SERVICE;
			}
			else if(!strcmp(argv[i], "--session"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);
				session = strtold(argv[i], NULL);
				arg_mask |= arg_SESSION;
			}
			else if(!strcmp(argv[i], "--timeout"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);

				to = strtold(argv[i], NULL);
			}
			else if(!strcmp(argv[i], "--udp"))
			{
				if(i + 2 >= argc)
					usage_exit(argv[0]);

				host = argv[++i];
				port = argv[++i];
				arg_mask |= arg_UDP;
			}
			else
				usage_exit(argv[0]);
		}
	}

	if((arg_mask & arg_MANDATORY) != arg_MANDATORY)
		usage_exit(argv[0]);

	if(net_addr(host, atoi(port), &addr) == NULL)
	{
		fputs("Incorrect adress/port: ", stderr);
		fputs(argv[1], stderr);
		fputc(':', stderr);
		fputs(argv[2], stderr);
		fputc('\n', stderr);
		return 2;
	}

	s = net_udp_socket();
	if(s < 0)
	{
		fputs("socket(UDP)\n", stderr);
		return 3;
	}

	for(;;)
	{
		char buf[STDIN_BUFLEN];

		if(arg_mask & arg_STDIN)
		{
			if(isatty(fileno(stdin)) > 0)
			{
				fputs("cmd> ", stdout);
				fflush(stdout);
			}

			if(fgets(buf, sizeof(buf), stdin) == NULL)
				break;

			{
				char cmd[STDIN_BUFLEN];
				double arg[2];
				int i;

				i = sscanf(buf, "%s %lf%lf", cmd, arg, arg + 1);
				if(i > 0)
				{
					if(!strcmp(cmd, "pow2"))
					{
						if(set_data_n(o, method_POW2, 1, i, arg))
							continue;
					}
					else if(!strcmp(cmd, "sin"))
					{
						if(set_data_n(o, method_SIN, 1, i, arg))
							continue;
					}
					else if(!strcmp(cmd, "cos"))
					{
						if(set_data_n(o, method_COS, 1, i, arg))
							continue;
					}
					else if(!strcmp(cmd, "abs"))
					{
						if(set_data_n(o, method_ABS, 1, i, arg))
							continue;
					}
					else if(!strcmp(cmd, "sum"))
					{
						if(set_data_n(o, method_SUM, 2, i, arg))
							continue;
					}
					else if(!strcmp(cmd, "sub"))
					{
						if(set_data_n(o, method_SUB, 2, i, arg))
							continue;
					}
					else if(!strcmp(cmd, "mul"))
					{
						if(set_data_n(o, method_MUL, 2, i, arg))
							continue;
					}
					else if(!strcmp(cmd, "div"))
					{
						if(set_data_n(o, method_DIV, 2, i, arg))
							continue;
					}
					else
					{
						fprintf(stderr, "Unknown command: %s\n", cmd);
						continue;
					}

					someip_set_request_ok(o);
					someip_set_service(o, service);
					someip_set_req(o, client, session);
				}
				else
					continue;
			}
		}
		else
		{
			someip_set_request_ok(o);
			someip_set_msg(o, method, service);
			someip_set_req(o, client, session);
			someip_set_data(o, CL_DATA, CL_DATA_LEN);
		}

		someip_print_msg(o, arg_mask);

		i = someip_send(s, o, (struct sockaddr *)&addr, sizeof(addr));
		if(i < 0)
		{
			perror("someip_send()");
			return 4;
		}

		i = someip_recv(s, (struct someip *)buf, sizeof(buf), (struct sockaddr *)&addr_sender, sizeof(addr_sender), to);
		if(i < 0)
		{
			fputs("someip_recv()\n", stderr);
			return 5;
		}

		someip_print_recv(i, arg_mask);
		someip_print_msg((struct someip *)buf, arg_mask);

		{
			struct someip * o = (struct someip *)buf;
			unsigned short m = someip_u2(o->method);

			if(method_is_CALC(m))
			{
				printf("[%04X.%04X] ", someip_u2(o->service), m);
				if(o->code == someip_code_OK)
					printf("Result: %.3g", *(double *)o->data);
				else
				{
					fflush(stdout);
					fputs("Error: ", stderr);
					fwrite(o->data, someip_u4(o->len), 1, stdout);
				}
			}

			putchar('\n');
		}

		if(loop)
		{
			if(loop != UINT_MAX)
				usleep(1000 * (useconds_t)loop);
			else
				if(!(arg_mask & arg_STDIN))
					break;
		}
	}
}
