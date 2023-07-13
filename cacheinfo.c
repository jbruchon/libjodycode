/* Detect and report size of CPU caches
 *
 * Copyright (C) 2017-2023 by Jody Bruchon <jody@jodybruchon.com>
 * Distributed under The MIT License
 *
 * If an error occurs or a cache is missing, zeroes are returned
 * Unified caches populate l1/l2/l3; split caches populate lXi/lXd instead
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "likely_unlikely.h"
#include "libjodycode.h"

/* None of this code is useful outside of Linux */
#ifdef __linux__

static char *pathidx;
static char buf[16];
static char path[64] = "/sys/devices/system/cpu/cpu0/cache/index";


/*** End declarations, begin code ***/


/* Linux sysfs */
static size_t read_procfile(const char * const restrict name)
{
	FILE *fp;
	size_t i;

	if (unlikely(name == NULL)) return 0;

	memset(buf, 0, 16);
	/* Create path */
	*pathidx = '\0';
	strcpy(pathidx, name);
	fp = fopen(path, "rb");
	if (fp == NULL) return 0;
	i = fread(buf, 1, 16, fp);
	if (ferror(fp)) return 0;
	fclose(fp);
	return i;
}


extern void jc_get_proc_cacheinfo(struct jc_proc_cacheinfo *pci)
{
	char *idx;
	size_t i;
	size_t size;
	int level;
	char type;
	char index;

	if (unlikely(pci == NULL)) return;

	memset(pci, 0, sizeof(struct jc_proc_cacheinfo));
	i = strlen(path);
	if (i > 48) return;
	idx = path + i;
	pathidx = idx + 1;
	*pathidx = '/'; pathidx++;

	for (index = '0'; index < '9'; index++) {
		*idx = index;

		/* Get the level for this index */
		if (read_procfile("level") == 0) break;
		if (*buf < '1' || *buf > '3') break;
		else level = (*buf) + 1 - '1';

		/* Get the size */
		if (read_procfile("size") == 0) break;
		size = (size_t)atoi(buf) * 1024;
		if (size == 0) break;

		/* Get the type */
		if (read_procfile("type") == 0) break;
		if (*buf != 'U' && *buf != 'I' && *buf != 'D') break;
		type = *buf;

		/* Act on it */
		switch (type) {
			case 'D':
			switch (level) {
				case 1: pci->l1d = size; break;
				case 2: pci->l2d = size; break;
				case 3: pci->l3d = size; break;
				default: return;
			};
			break;
			case 'I':
			switch (level) {
				case 1: pci->l1i = size; break;
				case 2: pci->l2i = size; break;
				case 3: pci->l3i = size; break;
				default: return;
			};
			break;
			case 'U':
			switch (level) {
				case 1: pci->l1 = size; break;
				case 2: pci->l2 = size; break;
				case 3: pci->l3 = size; break;
				default: return;
			};
			break;
			default: return;
		}

		/* Continue to next index */
	}
	return;
}

#endif /* __linux__ */


/* This is for testing only */
#ifdef JC_TEST
int main(void)
{
	static struct jc_proc_cacheinfo pci;
	jc_get_proc_cacheinfo(&pci);

	printf("Cache info:\n\n");
	printf("L1  I %4luK, D %4luK, U %4luK\n", pci.l1i >> 10, pci.l1d >> 10, pci.l1 >> 10);
	printf("L2  I %4luK, D %4luK, U %4luK\n", pci.l2i >> 10, pci.l2d >> 10, pci.l2 >> 10);
	if ((pci.l3d | pci.l3i | pci.l3) != 0) printf("L3  I %4luK, D %4luK, U %4luK\n", pci.l3i >> 10, pci.l3d >> 10, pci.l3 >> 10);
	else printf("L3  does not exist\n");
	return 0;
}
#endif
