#include <stdio.h>

#include <someip/utils.h>

void someip_print_hdr(struct someip *d)
{
	printf("service:\t0x%04X\n"
	       "method:\t0x%04X\n"
	       "length:\t%u\n"
	       "client:\t0x%04X\n"
	       "session:\t0x%04X\n"
	       "proto:\t0x%02X\n"
	       "iface:\t0x%02X\n"
	       "type:\t0x%02X\t%s\n"
	       "code:\t0x%02X\n",
	       someip_u2(d->service),
	       someip_u2(d->method),
	       someip_u4(d->len),
	       someip_u2(d->client),
	       someip_u2(d->session),
	       d->proto,
	       d->iface,
	       d->type, someip_type_text(d->type),
	       d->code);
}
