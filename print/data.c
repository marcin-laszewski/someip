#include <ctype.h>
#include <stdio.h>

#include <someip/utils.h>

#define	type_UNKNOWN	0
#define	type_TEXT	1
#define	type_BIN	0

void
someip_print_data(struct someip * d)
{
	unsigned i;
	unsigned len = someip_u4(d->len);
	unsigned char type = type_UNKNOWN;

	if(len < someip_len_HDR2)
	{
		printf("data-len-incorrect: %u\n", len);
		return;
	}

	fputs("data:	", stdout);

	len -= someip_len_HDR2;

	for(i = 0; i < len; i++)
	{
		if(isprint(d->data[i]))
		{
			if(type != type_TEXT)
			{
				if(i)
					putchar(' ');
				putchar('"');
				type = type_TEXT;
			}
			putchar(d->data[i]);
		}
		else
		{
			switch(type)
			{
				case type_TEXT:
					putchar('"');
				case type_UNKNOWN:
					type = type_BIN;
			}

			if(i)
				putchar(' ');

			printf("%02X", d->data[i]);
		}
	}

	if(type == type_TEXT)
		putchar('"');

	putchar('\n');
}
