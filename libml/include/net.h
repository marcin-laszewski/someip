#if !defined(NET_H)
#define	NET_H

#include <convdata.h>
#include <stddef.h>

#if defined(__MINGW32__)
#	include <winsock2.h>
#	include <sys/types.h>
typedef	u_long	in_addr_t;
typedef int	socklen_t;
#define	IP_TTL	4 /* old value 7 */
#else
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netdb.h>
#	include <unistd.h>
#endif

#define	net_tcp_connect_ERR	(-6)


#if defined(__MINGW32__)
#	define	EINPROGRESS	WSAEINPROGRESS
#endif

#if defined(__cplusplus)
extern "C" {
#endif

struct sockaddr_in * net_addr(const char * hostname, int port,
                        struct sockaddr_in * addr);
struct sockaddr_in * net_addr_hostent(struct hostent * h, int port,
                                      struct sockaddr_in * addr);
struct sockaddr_in * net_addr_ip(in_addr_t ad, int port,
                        struct sockaddr_in * addr);
struct sockaddr_in * net_addr_name(const char * host, int port,
                        struct sockaddr_in * addr);
struct sockaddr_in * net_addr_name_port(const char * hostname,
                        int default_port, struct sockaddr_in * addr);

#if defined(__MINGW32__)
#	define	net_close(s)	socketclose(s)
int	net_init(void);
void	net_end(void);
#else
#	define	net_close(s)	close(s)
#	define	net_end()
#	define	net_init()
#endif

int net_error(int f);

int net_inet_bind_any(int s, int port);

int net_ip_ttl(int f, unsigned ttl);
int net_tcp_connect(const char * host, int port, unsigned * peer_addr);
int net_tcp_connect_timeout(const char * host, int port, unsigned * peer_addr,
      unsigned timeout);
int net_tcp_connect_addr(struct sockaddr_in * addr, unsigned * peer_addr);
int net_tcp_connect_addr_timeout(struct sockaddr_in * addr,
      unsigned * peer_addr, unsigned timeout);
int net_tcp_connect_name(const char * host, int default_port,
      unsigned * peer_addr);
int net_tcp_connect_to(int s, struct sockaddr_in * addr, unsigned timeout);

int net_tcp_listen(int port);
int net_tcp_reuse(int f);

int net_udp_client(const char * host, int port, struct sockaddr_in * serv);
int net_udp_socket(void);
ssize_t net_udp_send(int s, const struct sockaddr_in * addr,
		const void * data, size_t len);
ssize_t net_udp_recv(int s, struct sockaddr_in * addr,
		void * data, size_t len);

int recv1(int f, int flag);
int send1(int f, unsigned char c, int flags);

int recvn(int f, const void * data, int len, int flag);
int sendn(int f, const void * data, int len, int flag);

ssize_t recv_timeout(int f, void * data, size_t len, int flag,
            unsigned timeout);
ssize_t send_timeout(int f, const void * data, size_t len, int flag,
            unsigned timeout);

int sendn_timeout(int f, const void * data, int len, int flag,
      unsigned timeout);
int recvn_timeout(int f, const void * data, int len, int flag,
      unsigned timeout);

int recv1_timeout(int f, int flag, unsigned timeout);

int net_send_text(int f, const char * text, int flags);
int net_send_text_timeout(int f, const char * text, int flags,
      unsigned timeout);

int net_rexec(const char * host, int port,
	const char * user, const char * passwd, const char * cmd,
	unsigned to_conn, unsigned to_recv);

int net_unix_connect(const char * path);
int net_unix_listen(const char * path);

ssize_t	net_write(int f, const void * a, size_t n);
ssize_t	net_read(int f, void * a, size_t n);

int net_cmd_code(int s, char c[3], unsigned timeout, int v);
int net_cmd_resp(int s, char c[3], unsigned timeout, int v);
int net_cmd_send(int s, const char * cmd, const char * value, int v);

int net_packet_socket(const char * ip);
int net_packet_recv(int s, unsigned char * buf, size_t buf_len,
      struct timeval * tv, u4_t * src, u4_t * dst, size_t * len);

void net_print_ip(const char * msg, struct in_addr addr);

#if !defined(__MINGW32__)
void net_print_icmp(u4_t addr, icmp_t * icmp, size_t len);
#endif

#if defined(__cplusplus)
}
#endif

#endif

#define	icmp_echo(icmp)		((icmp)->data.echo_reply)
#define	icmp_echo_id(icmp)	(icmp_echo(icmp).id)
#define	icmp_echo_seq(icmp)	(icmp_echo(icmp).seq)

#define	icmp_te(icmp)		((icmp)->data.reply)
/*#define	icmp_te(icmp)		((icmp)->data.time_exceeded)*/
#define	icmp_te_ttl(icmp)	(icmp_te(icmp).ip.ip_ttl)
#define	icmp_te_ip_p(icmp)	(icmp_te(icmp).ip.ip_p)
#define	icmp_te_ip_src(icmp)	(icmp_te(icmp).ip.ip_src)
#define	icmp_te_ip_dst(icmp)	(icmp_te(icmp).ip.ip_dst)
#define	icmp_te_orig(icmp)	((icmp_t *)(icmp_te(icmp).orig))
#define	icmp_te_id(icmp)	icmp_echo_id(icmp_te_orig(icmp))
#define	icmp_te_seq(icmp)	icmp_echo_seq(icmp_te_orig(icmp))
