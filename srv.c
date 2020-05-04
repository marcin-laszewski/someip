#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "net.h"
#include <someip/net.h>
#include <someip/utils.h>

#include "utils.h"

#include "app/config.h"

#if !defined(STREAM_TIMEOUT)
#	define	STREAM_TIMEOUT	60
#endif

#define	BUFLEN	4096

#define	SR_DATA	"Hello World"
#define	SR_DATA_LEN	(sizeof(SR_DATA) - 1)

#define	arg_MANDATORY	arg_NET

static	void
usage_exit(char const * progname)
{
	fputs(progname, stderr);
	fputs(" \\\n"
		"\t[--client] [--method] [--service] [--session] [--protocol] \\\n"
		"\t[--print-data] [--print-hdr] [--print-recv] \\\n"
		"\t{--udp port | --unix-dgram path | --unix-stream path }"
		"\n", stderr);
	exit(1);
}

void set_error(struct someip * o, char const * msg)
{
	someip_set_type(o, someip_type_RESPONSE);
	someip_set_code(o, 1);
	someip_set_data(o, msg, strlen(msg));
}

#define	get_arg_double(o)	((double *)&((o)->data))

void set_result(struct someip * o, double result)
{
	someip_set_code(o, someip_code_OK);
	someip_set_type(o, someip_type_RESPONSE);
	someip_set_data(o, &result, sizeof result);
	someip_set_len(o, sizeof(result));
}


static int
srv_ip(
	struct sockaddr ** srv, socklen_t * srv_len,
	struct sockaddr ** cli, socklen_t * cli_len,
	struct sockaddr_in * srv_in,
	struct sockaddr_in * cli_in,
	int type, char const * port)
{
	int s;

	s = socket(AF_INET, type, 0);
	if(s < 0)
		return -1;

	net_inet_addr_any(srv_in, atoi(port));
	*srv = (struct sockaddr *)srv_in;
	*srv_len = sizeof(*srv_in);

	*cli = (struct sockaddr *)cli_in;
	*cli_len = sizeof(*cli_in);

	/*
	{
		unsigned char reuse_addr = 1;
		unsigned char linger_opt = 0;

		setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr));
		setsockopt(s, SOL_SOCKET, SO_LINGER, &linger_opt, sizeof(SO_LINGER));
	}
	*/

	return s;
}

static int
srv_unix(
	struct sockaddr ** srv, socklen_t * srv_len,
	struct sockaddr ** cli, socklen_t * cli_len,
	struct sockaddr_un * srv_un,
	struct sockaddr_un * cli_un,
	int type, char const * path)
{
	if(unix_unlink(path))
			return -1;

	{
		int s;

		s = unix_open(srv, srv_len, type, srv_un, path);
		if(s < 0)
			return 3;

		*cli = (struct sockaddr *)cli_un;
		*cli_len = sizeof(*cli_un);

		return s;
	}
}

static void
stream_close(int * f)
{
	if(close(*f) < 0)
		perror("close(stream)");
	*f = -1;
}

int
main(int argc, char *argv[])
{
	char const *	port = NULL;
	struct sockaddr_in addr_srv_in;
	struct sockaddr_in addr_cli_in;

	char const *	unix_path = NULL;
	struct sockaddr_un addr_srv_un;
	struct sockaddr_un addr_cli_un;

	struct sockaddr *	addr_srv;
	socklen_t	addr_srv_len;

	struct sockaddr *	addr_cli;
	socklen_t	addr_cli_len;

	int s;
	int f = -1;
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
			if(someip_args_someip(argc, argv, &i, &arg_mask, usage_exit,
				&service, &method, &client, &session))
			if(someip_args_print(argc, argv, &i, &arg_mask))
			if(someip_args_unix_dgram(argc, argv, &i, usage_exit, &arg_mask, &unix_path))
			if(someip_args_unix_stream(argc, argv, &i, usage_exit, &arg_mask, &unix_path))
			{
				if(!strcmp(argv[i], "--protocol"))
				{
					i++;
					if(i >= argc)
						usage_exit(argv[0]);

					proto = strtold(argv[i], NULL);
				}
				else if(!strcmp(argv[i], "--tcp"))
				{
					if(i + 1 >= argc)
						usage_exit(argv[0]);

					port = argv[++i];
					arg_mask |= (arg_NET | arg_net_domain_IP | arg_net_type_STREAM);
				}
				else if(!strcmp(argv[i], "--udp"))
				{
					if(i + 2 >= argc)
						usage_exit(argv[0]);

					port = argv[++i];
					arg_mask |= (arg_NET | arg_net_domain_IP | arg_net_type_DGRAM);
				}
				else
					usage_exit(argv[0]);
			}
		}
	}

	if(!(port || unix_path))
		usage_exit(argv[0]);

	if(is_IP(arg_mask))
		s = srv_ip(
					&addr_srv, &addr_srv_len,
					&addr_cli, &addr_cli_len,
					&addr_srv_in,
					&addr_cli_in,
					is_DGRAM(arg_mask)
					? SOCK_DGRAM
					: SOCK_STREAM,
					port);
	else
		s = srv_unix(
					&addr_srv, &addr_srv_len,
					&addr_cli, &addr_cli_len,
					&addr_srv_un,
					&addr_cli_un,
					is_DGRAM(arg_mask)
					? SOCK_DGRAM
					: SOCK_STREAM,
					unix_path);

	if(bind(s, addr_srv, addr_srv_len) < 0)
	{
		perror("bind");
		return 4;
	}

	if(is_STREAM(arg_mask))
		if(listen(s, 5) < 0)
		{
			perror("listen-stream");
			return 3;
		}

	while(1)
	{
		if(is_STREAM(arg_mask))
		{
			if(f < 0)
			{
				socklen_t len = addr_cli_len;

				f = accept(s, addr_cli, &len);
				if(f < 0)
				{
					perror("accept");
					return 4;
				}
			}

			i = someip_recvn(f, o, STREAM_TIMEOUT);
		}
		else
			i = someip_recv(s, (struct someip *)buf, sizeof(buf),
						addr_cli, &addr_cli_len, 0);

		if(i < someip_len_HDR)
		{
			if(i)
				fprintf(stderr, "someip_recv() = %ld\n", i);

			if(is_STREAM(arg_mask))
				stream_close(&f);

			continue;
		}

		someip_print_recv(i, arg_mask);

		i -= someip_len_HDR;

		if(i > sizeof(double))
		{
			someip_print_msg((struct someip *)buf, arg_mask);

			switch(someip_u2(o->method))
			{
				case	method_POW2:	set_result(o, pow(*get_arg_double(o), 2));	break;
				case	method_SIN:	set_result(o, sin(*get_arg_double(o)));	break;
				case	method_COS:	set_result(o, cos(*get_arg_double(o)));	break;
				case	method_ABS:	set_result(o, fabs(*get_arg_double(o)));	break;
				case	method_SUM:	set_result(o, get_arg_double(o)[0] + get_arg_double(o)[1]);	break;
				case	method_SUB:	set_result(o, get_arg_double(o)[0] - get_arg_double(o)[1]);	break;
				case	method_MUL:	set_result(o, get_arg_double(o)[0] * get_arg_double(o)[1]);	break;
				case	method_DIV:
					if(get_arg_double(o)[1])
						set_result(o, get_arg_double(o)[0] / get_arg_double(o)[1]);
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

			if(is_STREAM(arg_mask))
			{
				if(someip_sendn(f, o, STREAM_TIMEOUT))
				{
					fputs("send(stream) error.\n", stderr);
					stream_close(&f);
					continue;
				}
			}
			else
			{
				if(file_select_write(s, STREAM_TIMEOUT) <= 0)
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
	}

	return 0;
}
