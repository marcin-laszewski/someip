#if !defined(SOMEIP_UTILS_H)
#define	SOMEIP_UTILS_H

#include <sys/types.h>
#include <someip.h>

#define	arg_UDP	(1	<< 0)
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
#define	arg_UNIX_DGRAM	(1	<< 14)

#define	arg_PRINT	(arg_PRINT_DATA | arg_PRINT_HDR | arg_PRINT_RECV)

#define	arg_NET	(arg_UDP | arg_UNIX_DGRAM)

#if defined(__cplusplus)
extern "C" {
#endif

void	someip_print_data(struct someip * data);
void	someip_print_hdr(struct someip * data);
void	someip_print_msg(struct someip * o, unsigned arg_mask);
void	someip_print_recv(ssize_t i, unsigned arg_mask);

char const *	someip_type_text(unsigned char type);

#if defined(__cplusplus)
}
#endif


#endif
