#include <stdio.h>

#include <someip/utils.h>

void
someip_print_data(struct someip * d)
{
	unsigned i;
	unsigned len = someip_u4(d->len);

	if(len < someip_len_HDR2)
	{
		printf("data-len-incorrect: %u\n", len);
		return;
	}

	fputs("data:	", stdout);

	len -= someip_len_HDR2;

	for(i = 0; i < len; i++)
		putchar(d->data[i]);

	putchar('\n');
}
