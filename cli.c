#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "net.h"
#include <someip.h>
#include <someip/net.h>
#include <someip/utils.h>

#include "utils.h"

#include "app/config.h"

#if !defined(STREAM_TIMEOUT)
#	define	STREAM_TIMEOUT	1
#endif

#define	BUFLEN	4096
#define	STDIN_BUFLEN	256

#define	CL_DATA	"World"
#define	CL_DATA_LEN	(sizeof(CL_DATA) - 1)

#if !defined(UNIX_LOCAL)
#	define	UNIX_LOCAL	"/tmp/hello-cli"
#endif

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
	fputs(" \\\n"
		"\t[--loop ms] [--timeout sec] \\\n"
		"\t[--print-hdr] [--print-data] [--print-recv] \\\n"
		"\t[--stdin] \\\n"
		"\t{--udp inet-addr port | --unix-dgram path} [--unix-dgram-local path] \\\n"
		"\t{--service id} {--method id} {--client id} {--session id}"
		"\n",
		stderr);
	exit(1);
}

int
set_data_n(struct someip * o, unsigned short method,
	unsigned n, int i, double * tab)
{
	if(--i != n)
	{
		fprintf(stderr, "Incorrect nunmber of expected arguments [%u]: %d\n",
			n, i);
		return -1;
	}

	someip_set_method(o, method);
	someip_set_data(o, tab, n * sizeof(*tab));

	return 0;
}

struct cmd
{
	char const *	name;
	unsigned short	method;
	unsigned	argc;
};

static	int cmd_cmp(char const * name, struct cmd * cmd)
{
	return strcmp(name, cmd->name);
}

#define	arg_MANDATORY	(arg_NET | arg_SERVICE | arg_METHOD | arg_CLIENT | arg_SESSION)

static struct cmd cmds[] =
{
	{ "abs",	method_ABS, 1 },
	{ "cos",	method_COS, 1 },
	{ "div",	method_DIV, 2 },
	{ "mul",	method_MUL, 2 },
	{ "pow2",	method_POW2, 1 },
	{ "sin",	method_SIN, 1 },
	{ "sub",	method_SUB, 2 },
	{ "sum",	method_SUM, 2 },
};

#define	cmds_n	(sizeof(cmds) / sizeof(*cmds))

static int
cli_open(
	struct sockaddr ** srv, socklen_t * srv_len,
	struct sockaddr ** cli, socklen_t * cli_len,
	struct sockaddr_un * srv_un,
	struct sockaddr_un * cli_un,
	int type,
	char const * path_remote,
	char const * path_local)
{
	int s;

	s = unix_open(srv, srv_len, type, srv_un, path_remote);
	if(s < 0)
		return 3;

	{
		char path[UNIX_PATH_MAX];

		sprintf(path, "%s.%u", path_local, (unsigned)getpid());
		if(unix_unlink(path_local))
			return 2;

		{
			socklen_t i;

			i = net_unix_addr(cli_un, path);
			if(i < 0)
			{
				fputs("addr-cli(UNIX): ", stderr);
				fputs(path_local, stderr);
				fputc('\n', stderr);
				return 3;
			}

			*cli_len = i;
		}
	}

	*cli = (struct sockaddr *)cli_un;

	return s;
}

int
main(int argc, char *argv[])
{
	char const	*host	= NULL;
	char const	*port	= NULL;
	struct sockaddr_in	addr_cli_in;
	struct sockaddr_in	addr_srv_in;

	char const	*unix_local = UNIX_LOCAL;
	char const	*unix_remote = NULL;
	struct sockaddr_un	addr_cli_un;
	struct sockaddr_un	addr_srv_un;

	unsigned loop	= UINT_MAX;

	struct sockaddr *	addr_cli;
	socklen_t	addr_cli_len;

	struct sockaddr *	addr_srv;
	socklen_t	addr_srv_len;

	int s;
	ssize_t	i;
	unsigned	arg_mask = 0;
	unsigned short	client	= 0;
	unsigned short	service	= 0;
	unsigned short	method	= 0;
	unsigned short	session	= 0;
	unsigned	to	= 1;

	{
		unsigned i;

		for(i = 1; i < argc; i++)
		{
			if(someip_args_someip(argc, argv, &i, &arg_mask, usage_exit,
				&service, &method, &client, &session))
			if(someip_args_print(argc, argv, &i, &arg_mask))
			if(someip_args_unix_dgram(argc, argv, &i, usage_exit, &arg_mask, &unix_remote))
			if(someip_args_unix_stream(argc, argv, &i, usage_exit, &arg_mask, &unix_remote))
			{
				if(!strcmp(argv[i], "--tcp"))
				{
					if(i + 2 >= argc)
						usage_exit(argv[0]);

					host = argv[++i];
					port = argv[++i];
					arg_mask |= (arg_NET | arg_net_domain_IP | arg_net_type_STREAM);
				}
				else if(!strcmp(argv[i], "--udp"))
				{
					if(i + 2 >= argc)
						usage_exit(argv[0]);

					host = argv[++i];
					port = argv[++i];
					arg_mask |= (arg_net_domain_IP | arg_net_type_DGRAM);
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
				else if(!strcmp(argv[i], "--timeout"))
				{
					i++;
					if(i >= argc)
						usage_exit(argv[0]);

					to = strtold(argv[i], NULL);
				}
				else if(!strcmp(argv[i], "--unix-dgram-local"))
				{
					if(i + 1 >= argc)
						usage_exit(argv[0]);

					unix_local = argv[++i];
				}
				else if(!strcmp(argv[i], "--unix-stream-local"))
				{
					if(i + 1 >= argc)
						usage_exit(argv[0]);

					unix_local = argv[++i];
				}
				else
					usage_exit(argv[0]);
			}
		}
	}

	if((arg_mask & arg_MANDATORY) != arg_MANDATORY)
		usage_exit(argv[0]);

	if(is_IP(arg_mask))
	{
		if(net_inet_addr(&addr_srv_in, host, atoi(port)) == NULL)
		{
			fputs("Incorrect adress/port: ", stderr);
			fputs(argv[1], stderr);
			fputc(':', stderr);
			fputs(argv[2], stderr);
			fputc('\n', stderr);
			return 2;
		}

		if(is_DGRAM(arg_mask))
		{
			s = net_udp_socket();
			if(s < 0)
			{
				fputs("socket(UDP)\n", stderr);
				return 3;
			}
		}
		else
		{
			s = socket(AF_INET, SOCK_STREAM, 0);
			if(s < 0)
			{
				fputs("socket(TCP)\n", stderr);
				return 3;
			}
		}

		addr_srv = (struct sockaddr *)&addr_srv_in;
		addr_srv_len = sizeof(addr_srv_in);

		addr_cli = (struct sockaddr *)&addr_cli_in;
		addr_cli_len = sizeof(addr_cli_in);
	}
	else
		s = cli_open(
					&addr_srv, &addr_srv_len,
					&addr_cli, &addr_cli_len,
					&addr_srv_un,
					&addr_cli_un,
					is_DGRAM(arg_mask)
					? SOCK_DGRAM
					: SOCK_STREAM,
					unix_remote,
					unix_local);

	if(bind(s, addr_cli, addr_cli_len) < 0)
	{
		perror("bind");
		return 4;
	}

	if(is_STREAM(arg_mask))
	{
		if(connect(s, addr_srv, addr_srv_len) < 0)
		{
			perror("connect");
			return 5;
		}
	}

	for(;;)
	{
		unsigned char	buf[BUFLEN];
		struct someip *	o	= (struct someip *)buf;

		if(arg_mask & arg_STDIN)
		{
			char get_buf[STDIN_BUFLEN];

			if(isatty(fileno(stdin)) > 0)
			{
				fputs("cmd> ", stdout);
				fflush(stdout);
			}

			if(fgets(get_buf, sizeof(get_buf), stdin) == NULL)
				break;

			{
				char cmd[STDIN_BUFLEN];
				double arg[2];
				int i;

				i = sscanf(get_buf, "%s %lf%lf", cmd, arg, arg + 1);
				if(i > 0)
				{
					struct cmd * c;

					c = (struct cmd *)bsearch(cmd, cmds, cmds_n, sizeof(*cmds),
															(__compar_fn_t)cmd_cmp);

					if(c == NULL)
					{
						fprintf(stderr, "Unknown command: %s\n", cmd);
						continue;
					}
					else
						if(set_data_n(o, c->method, c->argc, i, arg))
							continue;

					someip_set_request_ok(o);
					someip_set_service(o, service);
					someip_set_req(o, client, session);
				}
				else
				{
					fputs("List of command:\n", stderr);
					for(i = 0; i < cmds_n; i++)
					{
						fputc('\t', stderr);
						fputs(cmds[i].name, stderr);
						fputc('\n', stderr);
					}
					continue;
				}
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

		if(is_STREAM(arg_mask))
		{
			if(someip_sendn(s, o, STREAM_TIMEOUT) < 0)
			{
				fputs("Connection lost.\n", stderr);
				break;
			}

			i = someip_recvn(s, o, to);
		}
		else
		{
			i = someip_send(s, o, addr_srv, addr_srv_len);
			if(i < 0)
			{
				perror("Cannot send.");
				break;
			}

			i = someip_recv(s, (struct someip *)buf, sizeof(buf), NULL, NULL, to);
		}

		if(i < 0)
		{
			fputs("Cannot recv.\n", stderr);
			break;
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

	if(is_UNIX(arg_mask))
		unix_unlink(unix_local);

	if(is_STREAM(arg_mask))
		close(s);

	return 0;
}
