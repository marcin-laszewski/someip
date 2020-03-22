#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "net.h"
#include <someip/net.h>
#include <someip/utils.h>

#define	BUFLEN	4096

#define	SR_DATA	"Hello World"
#define	SR_DATA_LEN	(sizeof(SR_DATA) - 1)

#define	arg_MANDATORY	(arg_UDP)

static	void
usage_exit(char const * progname)
{
	fputs(progname, stderr);
	fputs(" {--udp port}\n", stderr);
	exit(1);
}

int
main(int argc, char *argv[])
{
	int s;
	unsigned char	buf[BUFLEN];
	ssize_t	i;
	char const *	port;
	struct someip *	o	= (struct someip *)buf;
	struct sockaddr_in addr_sender;
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
			else
				usage_exit(argv[0]);
		}
	}

	if((arg_mask & arg_MANDATORY) != arg_MANDATORY)
		usage_exit(argv[0]);

	s = net_udp_socket();
	if(s < 0)
	{
		fputs("socket(UDP)\n", stderr);
		return 3;
	}

	if(net_inet_bind_any(s, atoi(port)) < 0)
	{
		fputs("Cannot bind port: ", stderr);
		fputs(argv[1], stderr);
		fputc('\n', stderr);
		return 2;
	}

	while(1)
	{
		i = someip_recv(s, (struct someip *)buf, sizeof(buf), (struct sockaddr *)&addr_sender, sizeof(addr_sender), 0);
		if(i < 0)
		{
			fputs("someip_recv()\n", stderr);
			return 5;
		}

		someip_print_recv(i, arg_mask);

		if(i > someip_len_HDR1 + someip_len_HDR2)
		{
			someip_print_msg((struct someip *)buf, arg_mask);

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

			someip_print_msg(o, arg_mask);

			if(file_select_write(s, 1) <= 0)
			{
				fputs("timeout(SEND)\n", stderr);
				return 6;
			}

			someip_print_msg(o, arg_mask);

			i = someip_send(s, o, (struct sockaddr *)&addr_sender, sizeof(addr_sender));
			if(i < 0)
			{
				perror("someip_send()");
				return 4;
			}
		}
	}

	return 0;
}
