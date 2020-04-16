#if !defined(SOMEIP_NET_H)
#define	SOMEIP_NET_H

#include <sys/types.h>
#include <sys/socket.h>

#include <someip.h>

ssize_t	someip_recv(int s, struct someip * o, size_t len,
    struct sockaddr * addr, socklen_t *addr_len, unsigned to);
ssize_t	someip_recvn(int f, struct someip * o, unsigned to);

ssize_t	someip_send(int s, struct someip * o,
    struct sockaddr * addr, socklen_t addr_len);
ssize_t	someip_sendn(int f, struct someip * o, unsigned to);

#endif
