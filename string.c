/* libjodycode: non-standard string functions
 * Copyright (C) 2015-2025 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "libjodycode.h"
#include "likely_unlikely.h"


/* Like strncasecmp() but only tests for equality */
int jc_strncaseeq(const char *s1, const char *s2, const size_t len)
{
	size_t i = 0;

	while (i < len) {
		if (likely(*s1 != *s2)) {
			unsigned char c1, c2;

			c1 = *(const unsigned char *)s1;
			c2 = *(const unsigned char *)s2;
			/* Transform upper case to lower case */
			if (c1 == '\0' || c2 == '\0') return 1;
			if (c1 >= 'A' && c1 <= 'Z') c1 |= 0x20;
			if (c2 >= 'A' && c2 <= 'Z') c2 |= 0x20;
			if (c1 != c2) return 1;
		} else {
			if (*s1 == '\0') return 0;
		}
		s1++; s2++; i++;
	}
	return 0;
}

/* Like strcasecmp() but only tests for equality */
int jc_strcaseeq(const char *s1, const char *s2)
{
	while (1) {
		if (likely(*s1 != *s2)) {
			unsigned char c1, c2;

			c1 = *(const unsigned char *)s1;
			c2 = *(const unsigned char *)s2;
			/* Transform upper case to lower case */
			if (c1 == '\0' || c2 == '\0') return 1;
			if (c1 >= 'A' && c1 <= 'Z') c1 |= 0x20;
			if (c2 >= 'A' && c2 <= 'Z') c2 |= 0x20;
			if (c1 != c2) return 1;
		} else {
			if (*s1 == '\0') return 0;
		}
		s1++; s2++;
	}
	return 1;
}


/* Like strncmp() but only tests for equality */
int jc_strneq(const char *s1, const char *s2, const size_t len)
{
	size_t i = 0;

	while (likely(*s1 != '\0' && *s2 != '\0')) {
		if (*s1 != *s2) return 1;
		s1++; s2++; i++;
		if (i == len) return 0;
	}
	if (*s1 != *s2) return 1;
	return 0;
}


/* Like strcmp() but only tests for equality */
int jc_streq(const char *s1, const char *s2)
{
	while (likely(*s1 != '\0' && *s2 != '\0')) {
		if (*s1 != *s2) return 1;
		s1++; s2++;
	}
	if (*s1 != *s2) return 1;
	return 0;
}


/* Convert a plain C string into a jc_str with size prefix */
jc_str_t *jc_str_init(const char * const string, size_t len)
{
	jc_str_t *jstr;

	if (string == NULL) return NULL;

	if (len == 0) len = strlen(string);
	if (len > UINT16_MAX) return NULL;
	jstr = (jc_str_t *)malloc(sizeof(jc_str_t) + len + 1);
	if (jstr == NULL) return NULL;
	jstr->len = (uint16_t)len;
	memcpy(jstr->str, string, len + 1);

	return jstr;
}

#if 0
/* Like strcmp() but only tests for equality */
int jc_streq_fast(const jc_str_t *s1, const jc_str_t *s2)
{
	while (likely(*s1 != '\0' && *s2 != '\0')) {
		if (*s1 != *s2) return 1;
		s1++; s2++;
	}
	if (*s1 != *s2) return 1;
	return 0;
}
#endif
