#if !defined(CONVDATA_H)
#define	CONVDATA_H

#include <time.h>

#include <limits.h>

#define	i2_MAX	SHRT_MAX
#define	i2_MIN	SHRT_MIN

#define	u2_MAX	USHRT_MAX
#define	u2_MIN	0u

typedef	char			i1_t;
typedef	short int		i2_t;
typedef	int			i4_t;
typedef	long int		i8_t;

typedef	unsigned char		u1_t;
typedef	unsigned short int	u2_t;
typedef	unsigned int		u4_t;
typedef	unsigned long int	u8_t;

typedef	float			f4_t;
typedef	double			f8_t;

typedef	time_t			time4_t;

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__TURBOC__)
#	define	i386
#endif

#define	ext_byte(nr, n)		(((nr) >> ((n) << 3)) & 0xFF)
#define set_byte(nr, n)		((nr) << ((n) << 3))
#define mov_byte(nr, a, b)	set_byte(ext_byte(nr, a), b)

#define  orden_bytes_2(x, n1, n0)			\
         (i2_t)(mov_byte(x, n0, 0) + mov_byte(x, n1, 1))
#define  orden_bytes_4(x, n3, n2, n1, n0)		\
	 (i4_t)(mov_byte(x, n0, 0) + mov_byte(x, n1, 1) +	\
         mov_byte(x, n2, 2) + mov_byte(x, n3, 3))
         
#define	orden_revers_2(x)	orden_bytes_2((i2_t)x, 0, 1)
#define	orden_revers_4(x)	orden_bytes_4((i4_t)x, 0, 1, 2, 3)

#define	revers_var_2(x)		((x) = orden_revers_2(x))
#define	revers_var_4(x)		((x) = orden_revers_4(x))

/*--- Little endian --------------------------------------------------*/
#	define	u2_little(x)	i2_little(x)
#	define	u4_little(x)	i4_little(x)

#if defined(__hppa) || defined(sparc)
#	define	i2_little(x)	revers_var_2(x)
#	define	i4_little(x)	revers_var_4(x)
#else
#	define i2_little(x)
#	define i4_little(x)
#endif
/*-------------------------------------------------------------------*/

/*--- Big endian ----------------------------------------------------*/
#define	u2_big(x)		i2_big(x)
#define	u4_big(x)		i4_bin(x)

#if 0
#define	u2_ptr_big(x)		i2_ptr_big(x)
#endif
#define	u2_ptr_big(x) 	((((u2_t)(((u1_t *)(x))[0])) << 8) + (((u1_t *)(x))[1]))
#define	u4_ptr_big(x)		i4_ptr_big(x)

#if defined(__hppa) || defined(sparc)
#	define i2_big(x)
#	define i4_big(x)
#elif defined(__alpha) || defined(i386) || defined(__amd64)
#	define	i2_big(x)	revers_var_2(x)
#	define	i4_big(x)	revers_var_4(x)

#	define	i2_ptr_big(x)	orden_bytes_2(*(i2_t *)(x), 0, 1)
#	define	i4_ptr_big(x)	orden_bytes_4(*(i4_t *)(x), 0, 1, 2, 3)
#endif

#if 0
#if defined(sparc)
#	define	f4_ptr_ieee(p)	(*(p))
#else
double	f4_ptr_ieee(void * p);
#endif
#endif
/*-------------------------------------------------------------------*/


/*--- i386 ----------------------------------------------------------*/
#	define	u2_i386(x)	i2_i386(x)
#	define	u4_i386(x)	i4_i386(x)

#if defined(__hppa) || defined(sparc)
#	define	i2_i386(x)	((x) = orden_bytes_2((i2_t)x, 0, 1))
#	define	i4_i386(x)	((x) = orden_bytes_4((i4_t)x, 0, 1, 2, 3))
#elif defined(__alpha) || defined(i386)
#	define i2_i386(x)
#	define i4_i386(x)
#endif
/*-------------------------------------------------------------------*/

/*--- sparc ----------------------------------------------------------*/
#define	i2_sparc(x)	i2_big(x)
#define	i4_sparc(x)	i4_big(x)

#define	u2_sparc(x)	u2_big(x)
#define	u4_sparc(x)	u4_big(x)

#define	i2_ptr_sparc(x)	i2_ptr_big(x)
#define	i4_ptr_sparc(x)	i4_ptr_big(x)

#define	u2_ptr_sparc(x)	u2_ptr_big(x)
#define	u4_ptr_sparc(x)	u4_ptr_big(x)

#define	f4_ptr_sparc(p)	f4_ptr_ieee(p)

#if 0
#if defined(sparc)
#	define	f4_ptr_sparc(p)	(*(p))
#else
double	f4_ptr_sparc(void * p);
#endif
#endif


#if 0
#define	u2_sparc(x)	i2_sparc(x)
#define	u4_sparc(x)	i4_sparc(x)

#define	u2_ptr_sparc(x)	i2_ptr_sparc(x)
#define	u4_ptr_sparc(x)	i4_ptr_sparc(x)

#if defined(sparc)
#	define	f4_ptr_sparc(p)	(*(p))
#else
double	f4_ptr_sparc(void * p);
#endif
#endif
/*-------------------------------------------------------------------*/

/*--- time4_t (big, little ------------------------------------------*/
#define	time4_big(x)		i4_big(x)
#define	time4_ptr_big(x)	i4_ptr_big(x)

#define	time4_little(x)		i4_little(x)
#define	time4_ptr_little(x)	i4_ptr_little(x)
/*-------------------------------------------------------------------*/

/*--- f4 ------------------------------------------------------------*/
#if defined(sparc)
#	define	f4_ptr_ieee(p)	(*(p))
#else
double	f4_ptr_ieee(void * p);
#endif
/*-------------------------------------------------------------------*/

/*--- f8 ------------------------------------------------------------*/
#define	f8_ptr_ieee(p)	(*(p))
#if 0
#if defined(sparc)
#	define	f4_ptr_ieee(p)	(*(p))
#else
double	f4_ptr_ieee(void * p);
#endif
#endif
/*-------------------------------------------------------------------*/


#if defined(__cplusplus)
}
#endif

#endif
