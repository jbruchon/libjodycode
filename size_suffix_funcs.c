/* libjodycode: functions for using size suffixes
 *
 * Copyright (C) 2023-2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include "libjodycode.h"

#define MAX_SIZE_DECIMALS 9

enum {
	SIZE_UNITS_K = 0,
	SIZE_UNITS_KIB = 1,
	SIZE_UNITS_KB = 2
};


int jc_sizestr_to_int64(const char *str, int64_t *out_val)
{
	const struct jc_size_suffix *ss = jc_size_suffix;
	char *end = NULL;
	int64_t val;
	uint64_t mult = 1;

	if (!str || !out_val || *str < '0' || *str > '9') return -EINVAL;
	jc_errno = 0;
	val = jc_strtoll(str, &end, 10);
	if (jc_errno != 0 || val < 0 || end == str) return -EINVAL;

	if (*end != '\0') {
		while (ss->suffix != NULL && jc_strcaseeq(ss->suffix, end) != 0) ss++;
		if (ss->suffix == NULL || ss->multiplier <= 0) return JC_ESIZESUFFIX;
		mult = (uint64_t)ss->multiplier;
	}

	if (mult > 0 && (uint64_t)val > (uint64_t)INT64_MAX / mult) return JC_EOUFLOW;
	*out_val = val * (int64_t)mult;

	return 0;
}


int jc_int64_to_sizestr(int64_t bytes, char *buf, size_t bufsz, int units, int decimals)
{
	const struct jc_size_suffix *best = NULL;
	uint64_t ubytes, unit = 0, whole, frac = 0;
	size_t idx, step;

	if (!buf || bufsz == 0 || bytes < 0 || decimals < 0 || decimals > MAX_SIZE_DECIMALS) return -1;
	if (units < SIZE_UNITS_K || units > SIZE_UNITS_KB) return -1;
	ubytes = (uint64_t)bytes;

	if (units == SIZE_UNITS_KB) {
		idx = 13U;
		step = 1U;
		while (jc_size_suffix[idx].suffix != NULL && jc_size_suffix[idx].multiplier > 0) {
			uint64_t scale = (uint64_t)jc_size_suffix[idx].multiplier;
			if (scale > ubytes) break;
			best = &jc_size_suffix[idx];
			unit = scale;
			idx += step;
		}
	} else {
		idx = 1U + (size_t)units;
		step = 2U;
		while (jc_size_suffix[idx].suffix != NULL && jc_size_suffix[idx].shift >= 0) {
			uint64_t scale = 1ULL << jc_size_suffix[idx].shift;
			if (scale > ubytes) break;
			best = &jc_size_suffix[idx];
			unit = scale;
			idx += step;
		}
	}

	if (!best || unit == 0) {
		snprintf(buf, bufsz, "%" PRIu64 " B", ubytes);
		return 0;
	}

	whole = ubytes / unit;
	if (decimals > 0) {
		uint64_t rem = ubytes % unit;
		for (int i = 0; i < decimals; i++) {
			rem *= 10ULL;
			frac = frac * 10ULL + (rem / unit);
			rem %= unit;
		}
		snprintf(buf, bufsz, "%" PRIu64 ".%0*" PRIu64 " %s", whole, decimals, frac, best->suffix);
	} else {
		snprintf(buf, bufsz, "%" PRIu64 " %s", whole, best->suffix);
	}
	return 0;
}
