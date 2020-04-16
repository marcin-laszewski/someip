#if !defined(UTILS_H)

#include <sys/socket.h>
#include <sys/un.h>

#if defined(__cplusplus)
extern "C" {
#endif

int	unix_addr(struct sockaddr ** addr, socklen_t * addr_len,
			struct sockaddr_un * addr_un, char const * path);
int	unix_open(struct sockaddr ** addr, socklen_t * addr_len, int type,
			struct sockaddr_un * addr_un, char const * path);
int	unix_unlink(char const * path);

#if defined(__cplusplus)
}
#endif

#endif
