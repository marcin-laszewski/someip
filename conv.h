#if !defined(CONV_H)
#define	CONV_H

#define	byte_bit(n)	((n) << 3)
#define	sl_byte(n, a)	((a) << byte_bit(n))
#define	sr_byte(n, a)	((a) >> byte_bit(n))
#define	mask_byte(n, a)	((a) & (0xFF << byte_bit(n)))
#define	ml_byte(n, m, a)	sl_byte(m, mask_byte(n, a))
#define	mr_byte(n, m, a)	sr_byte(m, mask_byte(n, a))

#define	conv_u2(a) \
    ( mr_byte(1, 1, (unsigned short)a) \
    | ml_byte(0, 1, (unsigned short)a))
#define	conv_u4(a) \
    ( mr_byte(3, 3, a) \
    | mr_byte(2, 1, a) \
    | ml_byte(1, 1, a) \
    | ml_byte(0, 3, a))

#endif
