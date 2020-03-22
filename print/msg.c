#include <stdio.h>

#include <someip/utils.h>

void
someip_print_msg(struct someip * o, unsigned arg_mask)
{
	if(arg_mask & arg_PRINT_HDR)
		someip_print_hdr(o);
	if(arg_mask & arg_PRINT_DATA)
		someip_print_data(o);
	if(arg_mask & arg_PRINT)
		putchar('\n');
}

