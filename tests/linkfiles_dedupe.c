#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libjodycode.h"

int main(void)
{
	struct jc_fileinfo_batch *batch;
	const int filecnt = 28;

	batch = jc_fileinfo_batch_alloc(filecnt, 0, 3);
	if (batch == NULL) return -1;

	strcpy(batch->files[0].dirent->d_name, "a");
	strcpy(batch->files[1].dirent->d_name, "b");
	strcpy(batch->files[2].dirent->d_name, "c");
	strcpy(batch->files[3].dirent->d_name, "d");
	strcpy(batch->files[4].dirent->d_name, "e");
	strcpy(batch->files[5].dirent->d_name, "f");
	strcpy(batch->files[6].dirent->d_name, "g");
	strcpy(batch->files[7].dirent->d_name, "h");
	strcpy(batch->files[8].dirent->d_name, "i");
	strcpy(batch->files[9].dirent->d_name, "j");
	strcpy(batch->files[10].dirent->d_name, "k");
	strcpy(batch->files[11].dirent->d_name, "l");
	strcpy(batch->files[12].dirent->d_name, "m");
	strcpy(batch->files[13].dirent->d_name, "n");
	strcpy(batch->files[14].dirent->d_name, "o");
	strcpy(batch->files[15].dirent->d_name, "p");
	strcpy(batch->files[16].dirent->d_name, "q");
	strcpy(batch->files[17].dirent->d_name, "r");
	strcpy(batch->files[18].dirent->d_name, "s");
	strcpy(batch->files[19].dirent->d_name, "t");
	strcpy(batch->files[20].dirent->d_name, "u");
	strcpy(batch->files[21].dirent->d_name, "v");
	strcpy(batch->files[22].dirent->d_name, "w");
	strcpy(batch->files[23].dirent->d_name, "x");
	strcpy(batch->files[24].dirent->d_name, "y");
	strcpy(batch->files[25].dirent->d_name, "z");
	strcpy(batch->files[26].dirent->d_name, "0");
	strcpy(batch->files[27].dirent->d_name, "1");

	if (jc_linkfiles(batch, REFLINK) != 0) printf("failure\n");
	else printf("success\n");

	jc_fileinfo_batch_free(batch);
	return 0;
}
