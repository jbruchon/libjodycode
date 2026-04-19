/* libjodycode: numerically correct string comparison
 * that "sorts" symbols and spaces AFTER alphanumeric characters
 *
 * Copyright (C) 2014-2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <stdlib.h>
#include <stdint.h>
#include "likely_unlikely.h"
#include "libjodycode.h"

#define IS_NUM(a) (((a >= '0') && (a <= '9')) ? 1 : 0)
#define IS_LOWER(a) (((a >= 'a') && (a <= 'z')) ? 1 : 0)


/* sensitive: 0 = case-sensitive, 1 = case-insensitive */
int jc_numeric_strcmp(const char * restrict c1, const char * restrict c2, const int insensitive)
{
	int precompare;
	uintptr_t len1, len2;
	const char *rewind1, *rewind2;
	const char *start1, *start2;

	if (unlikely(c1 == NULL || c2 == NULL)) return JC_ENUMSTRCMP;

	start1 = c1;
	start2 = c2;

	while (unlikely(*c1 != '\0' && *c2 != '\0')) {
		/* Reset string rewind points */
		rewind1 = c1; rewind2 = c2;

		/* Skip all sequences of zeroes */
		while (*c1 == '0') c1++;
		while (*c2 == '0') c2++;

		/* If both chars are numeric, do a numeric comparison */
		if (IS_NUM(*c1) && IS_NUM(*c2)) {
			precompare = 0;

			/* Scan numbers and get preliminary results */
			while (IS_NUM(*c1) && IS_NUM(*c2)) {
				if (*c1 < *c2) precompare = -1;
				if (*c1 > *c2) precompare = 1;
				c1++; c2++;

				/* Skip remaining digit pairs after any difference is found */
				if (precompare != 0) {
					for (; IS_NUM(*c1) && IS_NUM(*c2); c1++, c2++);
					break;
				}
			}

			/* One numeric and one non-numeric means the numeric one is larger and sorts later */
			if (IS_NUM(*c1) ^ IS_NUM(*c2)) {
				if (IS_NUM(*c1) != 0) return 1;
				else return -1;
			}

			/* If the last test fell through, numbers are of equal length.
			Use the precompare result as the result for this number comparison. */
			if (precompare != 0) return precompare;
		} else {
			/* Zeroes aren't followed by a digit; rewind the streams */
			c1 = rewind1; c2 = rewind2;
		}

		/* Do normal comparison */
		if (likely(*c1 == *c2 && *c1 != '\0' && *c2 != '\0')) {
			c1++; c2++;
		/* Put symbols and spaces after everything else */
		} else if (*c2 < '.' && *c1 >= '.') {
			return -1;
		} else if (*c1 < '.' && *c2 >= '.') {
			return 1;
		} else {
			/* Normal strcasecmp() style compare */
			char s1 = *c1, s2 = *c2;
			/* Convert lowercase into uppercase */
			if (insensitive == 1) {
				if (IS_LOWER(s1)) s1 = (char)(s1 - 32);
				if (IS_LOWER(s2)) s2 = (char)(s2 - 32);
			}
			if (s1 > s2) return 1;
			if (s1 < s2) return -1;
			return 0;
		}
	}

	len1 = (uintptr_t)start1 - (uintptr_t)c1;
	len2 = (uintptr_t)start2 - (uintptr_t)c2;
	/* Longer strings generally sort later */
	if (len1 < len2) return -1;
	if (len1 > len2) return 1;

	/* Normal comparison - FIXME? length check should already handle these */
	if (unlikely(*c1 == '\0' && *c2 != '\0')) return -1;
	if (unlikely(*c1 != '\0' && *c2 == '\0')) return 1;

	/* Fall through: the strings are equal */
	return 0;
}
