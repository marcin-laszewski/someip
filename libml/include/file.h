#ifndef FILE_H
#define FILE_H

#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

#if !defined(__MSDOS__) && !defined(__MINGW32__) && !defined(__CYGWIN__)
#	define	O_BINARY	0
#endif

#if defined(__MINGW32__)
#	define	S_IRGRP	0
#	define	S_IWGRP	0
#	define	S_IXGRP	0

#	define	S_IROTH	0
#	define	S_IWOTH	0
#	define	S_IXOTH	0

#	include <sys/locking.h>
#	define	file_lock_LOCK		LK_LOCK
#	define	file_lock_LOCK_NB	LK_NBLCK
#	define	file_lock_UNLOCK	LK_UNLOCK
#endif

#define	file_select_INF	UINT_MAX

int	file_block_off(int f);
int	file_block_on(int f);
int	file_cp(int src, int dest);
ssize_t	file_cp_n(int src, int dest, size_t len);
ssize_t	file_cp_timeout(int src, int dest,
          unsigned timeout_read, unsigned timeout_write);
char *	file_read(int f, size_t addbytes, long * l);
int	file_lock(int f, int mode);
int	file_pid(const char * filename);
int	file_select_read(int f, unsigned timeout);
int	file_select_write(int f, unsigned timeout);
off_t	file_size(int f);

int	write_c(int f, char c);
int	write_nl(int f);
int	write_tab(int f);
int	write_text(int f, char const * text);
int	write_text_nl(int f, char const * text);

#ifdef __cplusplus
  }
#endif

#endif
