#if !defined(SOMEIP_H)
#define	SOMEIP_H

#include <conv.h>

#define	someip_len_HDR1	8
#define	someip_len_HDR2	8

#define	someip_PROTO	0x1
#define	someip_IFACE	0x0

#define	someip_type_REQUEST						0x00
#define	someip_type_REQUEST_NO_RETURN	0x01
#define	someip_type_NOTIFICATION			0x02
#define	someip_type_REQUEST_ACK				0x40
#define	someip_type_REQUEST_NO_RETURN_ACK	0x41
#define	someip_type_NOTIFICATION_ACK	0x42
#define	someip_type_RESPONSE					0x80
#define	someip_type_ERROR							0x81
#define	someip_type_RESPONSE_ACK			0xC0
#define	someip_type_ERROR_ACK					0xC1

#define	someip_code_OK	0x00

typedef	unsigned	u4_t;
typedef	unsigned short	u2_t;
typedef	unsigned char	u1_t;

struct someip
{
	union
	{
		u4_t	msgid;
		struct
		{
			u2_t	service;
			u2_t	method;
		};
	};
	u4_t	len;
	union
	{
		u4_t	reqid;
		struct
		{
			u2_t	client;
			u2_t	session;
		};
	};
	u1_t	proto;
	u1_t	iface;
	u1_t	type;
	u1_t	code;
	unsigned char	data[0];
};

#define	someip_u2(a)	conv_u2(a)
#define	someip_u4(a)	conv_u4(a)

#define	someip_set_u1(o, f, value)	(((o)->f) = (unsigned char)(value))
#define	someip_set_u2(o, f, value)	(((o)->f) = someip_u2(value))
#define	someip_set_u4(o, f, value)	(((o)->f) = someip_u4(value))

#define	someip_set_service(o,	value)	someip_set_u2(o, service, value)
#define	someip_set_method(o,	value)	someip_set_u2(o, method, value)
#define	someip_set_len(o,	value)	someip_set_u4(o, len, value)
#define	someip_set_client(o,	value)	someip_set_u2(o, client, value)
#define	someip_set_session(o,	value)	someip_set_u2(o, session, value)
#define	someip_set_proto(o,	value)	someip_set_u1(o, proto, value)
#define	someip_set_iface(o,	value)	someip_set_u1(o, iface, value)
#define	someip_set_type(o,	value)	someip_set_u1(o, type, value)
#define	someip_set_code(o,	value)	someip_set_u1(o, code, value)

#define	someip_set_code_ok(o)	someip_set_code(o, someip_code_OK)

#define	someip_set_data(o, src, len)	{ \
		memcpy((o)->data, src, len); \
		someip_set_len(o, (len) + someip_len_HDR2); \
	}

#define	someip_set_default(o)	{ \
		someip_set_proto(o, someip_PROTO); \
		someip_set_iface(o, someip_IFACE); \
	}

#define	someip_set_request(o)	{ \
		someip_set_default(o); \
		someip_set_type(o, someip_type_REQUEST); \
	}

#define	someip_set_request_ok(o)	{ \
		someip_set_request(o); \
		someip_set_code_ok(o); \
	}

#define	someip_set_response(o)	{ \
		someip_set_default(o); \
		someip_set_type(o, someip_type_RESPONSE); \
	}

#define	someip_set_response_ok(o)	{ \
		someip_set_response(o); \
		someip_set_code_ok(o); \
	}

#define	someip_set_msg(o, method, service)	{ \
		someip_set_method(o, method); \
		someip_set_service(o, service); \
	}

#define	someip_set_req(o, client, session)	{ \
		someip_set_client(o, client); \
		someip_set_session(o, session); \
	}

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__cplusplus)
}
#endif

#endif
