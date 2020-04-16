#if !defined(HELLO_CONFIG_H)
#define	HELLO_CONFIG_H

#define	method_POW2	0x3334
#define	method_SIN	0x3335
#define	method_COS	0x3336
#define	method_ABS	0x3337
#define	method_SUM	0x3338
#define	method_SUB	0x3339
#define	method_MUL	0x3340
#define	method_DIV	0x3341

#define	method_is_CALC(m)	(((m) >= method_POW2) && ((m) <= method_DIV))

#endif
