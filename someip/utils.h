#if !defined(SOMEIP_UTILS_H)
#define	SOMEIP_UTILS_H

#include <sys/types.h>
#include <someip.h>

#define	arg_NET			(1 << 0)
#define	arg_SERVICE	(1 << 1)
#define	arg_METHOD	(1 << 2)
#define	arg_CLIENT	(1 << 3)
#define	arg_SESSION	(1 << 4)
#define	arg_SESSION	(1 << 4)
#define	arg_PROTO		(1 << 5)
#define	arg_IFACE		(1 << 6)
#define	arg_TYPE	(1 << 7)
#define	arg_CODE	(1 << 8)
#define	arg_PRINT_DATA	(1 << 9)
#define	arg_PRINT_HDR	(1 << 10)
#define	arg_PRINT_RECV	(1 << 11)
#define	arg_TIMEOUT	(1 << 12)
#define	arg_STDIN	(1 << 13)
#define	arg_net_DOMAIN	(1	<< 14)
#define	arg_net_TYPE		(1 << 15)

#define	arg_PRINT	(arg_PRINT_DATA | arg_PRINT_HDR | arg_PRINT_RECV)

#define	arg_net_domain_UNIX	arg_net_DOMAIN
#define	arg_net_domain_IP		0

#define	is_UNIX(m)	((m) & arg_net_domain_UNIX)
#define	is_IP(m)		(!is_UNIX(m))

#define	arg_net_type_DGRAM	arg_net_TYPE
#define	arg_net_type_STREAM	0

#define	is_DGRAM(m)		((m) & arg_net_type_DGRAM)
#define	is_STREAM(m)	(!is_DGRAM(m))

#if defined(__cplusplus)
extern "C" {
#endif

int	someip_args_print(int argc, char * const * argv, unsigned * i,
        unsigned * mask);
int someip_args_someip(int args, char * const * argv, unsigned * i,
        unsigned * mask, void (* usage_exit)(char const *),
        unsigned short * service, unsigned short * method,
        unsigned short * client, unsigned short * session);
int someip_args_udp(int argc, char * const * argv, unsigned * i,
        void (* usage_exit)(char const *), unsigned * mask,
        const char **host, const char **port);
int	someip_args_unix_dgram(int argc, char * const * argv, unsigned * i,
        void (* usage_exit)(char const *), unsigned * mask,
        char const **remote);
int	someip_args_unix_stream(int argc, char * const * argv, unsigned * i,
        void (* usage_exit)(char const *), unsigned * mask, char const **path);

void	someip_print_data(struct someip * data);
void	someip_print_hdr(struct someip * data);
void	someip_print_msg(struct someip * o, unsigned arg_mask);
void	someip_print_recv(ssize_t i, unsigned arg_mask);

char const *	someip_type_text(unsigned char type);

#if defined(__cplusplus)
}
#endif

#endif
