/* libjodycode: fileinfo batch handlers
 *
 * Copyright (C) 2014-2024 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <stdlib.h>
#include <string.h>

#include "libjodycode.h"
#include "likely_unlikely.h"

/* allocs: 0 = just the batch, 1 = with dirents, 2 = with stats, 3 = with both */
struct jc_fileinfo_batch *jc_fileinfo_batch_alloc(const int filecnt, int stat, int namlen)
{
	struct jc_fileinfo_batch *batch;
	struct JC_STAT *stats;
	JC_DIRENT *dirents;
	int i;

	if (unlikely(filecnt <= 0)) return NULL;

	batch = calloc(1, (size_t)((int)sizeof(struct jc_fileinfo_batch) + ((int)sizeof(struct jc_fileinfo) * filecnt)));
	if (batch == NULL) return NULL;
	batch->count = filecnt;

	if (stat != 0) {
		stats = calloc(1, (size_t)(((int)sizeof(struct JC_STAT) * filecnt)));
		if (stats == NULL) goto error_cleanup;
		for (i = 0; i < filecnt; i++)
			batch->files[i].stat = (struct JC_STAT *)((uintptr_t)stats + (uintptr_t)(((int)sizeof(struct JC_STAT) * i)));
	}
	if (namlen != 0) {
		dirents = calloc(1, (size_t)(((int)sizeof(JC_DIRENT) + namlen) * filecnt));
		if (dirents == NULL) goto error_cleanup;
		for (i = 0; i < filecnt; i++)
			batch->files[i].dirent = (JC_DIRENT *)((uintptr_t)dirents + (uintptr_t)(((int)sizeof(JC_DIRENT) + namlen) * i));
	}

	return batch;

error_cleanup:
	jc_fileinfo_batch_free(batch);
	return NULL;
}


void jc_fileinfo_batch_free(struct jc_fileinfo_batch *batch)
{
	if (unlikely(batch == NULL)) return;

	if (batch->files[0].stat != NULL) free(batch->files[0].stat);
	if (batch->files[0].dirent != NULL) free(batch->files[0].dirent);
	free(batch);

	return;
}
