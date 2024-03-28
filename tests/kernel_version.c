#include <stdio.h>
#include "libjodycode.h"

int main(void)
{
	int ver = jc_get_kernel_version();

	printf("Kernel version: ");
	if (ver < 0) printf("failed\n");
	else printf("%d\n", ver);
	return 0;
}
