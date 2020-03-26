#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "net.h"
#include <someip/net.h>
#include <someip/utils.h>

#include "config.h"

#define	BUFLEN	4096

#define	SR_DATA	"Hello World"
#define	SR_DATA_LEN	(sizeof(SR_DATA) - 1)

#define	arg_MANDATORY	(arg_UDP | arg_UNIX_DGRAM)

static	void
usage_exit(char const * progname)
{
	fputs(progname, stderr);
	fputs(" \\\n"
		"\t[--client] [--method] [--service] [--session] [--protocol] \\\n"
		"\t[--print-data] [--print-hdr] [--print-recv] \\\n"
		"\t{--udp port | --unix-dgram path}"
		"\n", stderr);
	exit(1);
}

void set_error(struct someip * o, char const * msg)
{
	someip_set_type(o, someip_type_RESPONSE);
	someip_set_code(o, 1);
	someip_set_data(o, msg, strlen(msg));
}

void set_result(struct someip * o, double result)
{
	someip_set_code(o, someip_code_OK);
	someip_set_type(o, someip_type_RESPONSE);
	someip_set_data(o, &result, sizeof result);
}

int
main(int argc, char *argv[])
{
	char const *	port;
	struct sockaddr_in addr_srv_in;
	struct sockaddr_in addr_cli_in;

	char const *	unix_dgram;
	struct sockaddr_un addr_srv_un;
	struct sockaddr_un addr_cli_un;

	struct sockaddr *	addr_srv;
	socklen_t	addr_srv_len;

	struct sockaddr *	addr_cli;
	socklen_t	addr_cli_len;

	int s;
	unsigned char	buf[BUFLEN];
	ssize_t	i;
	struct someip *	o	= (struct someip *)buf;
	size_t	len = someip_len_HDR1 + SR_DATA_LEN;
	unsigned	arg_mask = 0;
	unsigned short	client;
	unsigned char	code	= someip_code_OK;
	unsigned char	iface	= someip_IFACE;
	unsigned short	service;
	unsigned short	method;
	unsigned char	proto;
	unsigned short	session;
	unsigned char	type	= someip_type_RESPONSE;

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
			}
			else if(!strcmp(argv[i], "--method"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);

				method = strtold(argv[i], NULL);
			}
			else if(!strcmp(argv[i], "--print-data"))
				arg_mask |= arg_PRINT_DATA;
			else if(!strcmp(argv[i], "--print-hdr"))
				arg_mask |= arg_PRINT_HDR;
			else if(!strcmp(argv[i], "--print-recv"))
				arg_mask |= arg_PRINT_RECV;
			else if(!strcmp(argv[i], "--protocol"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);

				proto = strtold(argv[i], NULL);
			}
			else if(!strcmp(argv[i], "--service"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);

				service = strtold(argv[i], NULL);
			}
			else if(!strcmp(argv[i], "--session"))
			{
				i++;
				if(i >= argc)
					usage_exit(argv[0]);

				session = strtold(argv[i], NULL);
			}
			else if(!strcmp(argv[i], "--udp"))
			{
				if(i + 1 >= argc)
					usage_exit(argv[0]);

				port = argv[++i];
				arg_mask |= arg_UDP;
			}
			else if(!strcmp(argv[i], "--unix-dgram"))
			{
				if(i + 1 >= argc)
					usage_exit(argv[0]);

				unix_dgram = argv[++i];
				arg_mask |= arg_UNIX_DGRAM;
			}
			else
				usage_exit(argv[0]);
		}
	}

	switch(arg_mask & arg_MANDATORY)
	{
		case arg_UDP:
			s = net_udp_socket();
			if(s < 0)
			{
				fputs("socket(UDP)\n", stderr);
				return 3;
			}


			addr_cli = (struct sockaddr *)&addr_cli_in;
			addr_cli_len = sizeof(addr_cli_in);

			net_inet_addr_any(&addr_srv_in, atoi(port));

			addr_srv = (struct sockaddr *)&addr_srv_in;
			addr_srv_len = sizeof(addr_srv_in);

			break;

		case arg_UNIX_DGRAM:
			s = net_unix_dgram_socket();
			if(s < 0)
			{
				fputs("socket(UNIX)\n", stderr);
				return 3;
			}

			if(unlink(unix_dgram) < 0
			&& errno != ENOENT)
			{
				perror(unix_dgram);
				return 5;
			}

			{
				int i;

				i = net_unix_addr(&addr_srv_un, unix_dgram);
				if(i < 0)
				{
					fputs("Incorrect path: ", stderr);
					fputs(unix_dgram, stderr);
					fputc('\n', stderr);
					return 2;
				}

				addr_srv = (struct sockaddr *)&addr_srv_un;
				addr_srv_len = sizeof(addr_srv_un);

				addr_cli = (struct sockaddr *)&addr_cli_un;
				addr_cli_len = sizeof(addr_cli_un);
			}

			break;

		default:
			usage_exit(argv[0]);
	}

	if(bind(s, addr_srv, addr_srv_len) < 0)
	{
		perror("bind");
		return 4;
	}

	while(1)
	{
		i = someip_recv(s, (struct someip *)buf, sizeof(buf),
					addr_cli, &addr_cli_len, 0);
		if(i < 0)
		{
			fputs("someip_recv()\n", stderr);
			return 5;
		}

		someip_print_recv(i, arg_mask);

		if(i > someip_len_HDR1 + someip_len_HDR2)
		{
			double * d = (double *)&(o->data);

			someip_print_msg((struct someip *)buf, arg_mask);

			switch(someip_u2(o->method))
			{
				case	method_POW2:	set_result(o, pow(*d, 2));	break;
				case	method_SIN:	set_result(o, sin(*d));	break;
				case	method_COS:	set_result(o, cos(*d));	break;
				case	method_ABS:	set_result(o, fabs(*d));	break;
				case	method_SUM:	set_result(o, d[0] + d[1]);	break;
				case	method_SUB:	set_result(o, d[0] - d[1]);	break;
				case	method_MUL:	set_result(o, d[0] * d[1]);	break;
				case	method_DIV:
					if(d[1])
						set_result(o, d[0] / d[1]);
					else
						set_error(o, "Div by zero.");
					break;

				default:
					if(arg_mask & arg_SERVICE)
						o->service	= someip_u2(service);
					if(arg_mask & arg_METHOD)
						o->method		= someip_u2(method);
					o->len	= someip_u4(len);
					if(arg_mask & arg_METHOD)
						o->client	= someip_u2(client);
					if(arg_mask & arg_METHOD)
						o->session	= someip_u2(session);
					if(arg_mask & arg_PROTO)
						o->proto	= proto;
					if(arg_mask & arg_IFACE)
						o->iface	= iface;
					if(arg_mask & arg_TYPE)
						o->type	= type;
					else
						o->type	= someip_type_RESPONSE;
					if(arg_mask & arg_CODE)
						o->code	= code;
					memcpy(o->data, SR_DATA, SR_DATA_LEN);
			}

			someip_print_msg(o, arg_mask);

			if(file_select_write(s, 1) <= 0)
			{
				fputs("timeout(SEND)\n", stderr);
				return 6;
			}

			someip_print_msg(o, arg_mask);

			i = someip_send(s, o, addr_cli, addr_cli_len);
			if(i < 0)
			{
				perror("someip_send()");
				return 4;
			}
		}
	}

	return 0;
}
