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

const char *jc_emptystring = '\0';


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


/* jc_streq() for jc_str_t strings */
int jc_strteq(const jc_str_t *s1, const jc_str_t *s2)
{
	if (s1->len == s2->len) return jc_streq(s1->str, s2->str);
	return 1;
}


/* jc_strneq() for jc_str_t strings */
int jc_strtneq(const jc_str_t *s1, const jc_str_t *s2, const size_t len)
{
	if (len >= s1->len && len >= s2->len) return jc_strteq(s1, s2);
	return jc_strneq(s1->str, s2->str, len);
}


/* jc_strtcaseeq() for jc_str_t strings */
int jc_strtcaseeq(const jc_str_t *s1, const jc_str_t *s2)
{
	if (s1->len == s2->len) return jc_strcaseeq(s1->str, s2->str);
	return 1;
}


int jc_strtncaseeq(const jc_str_t *s1, const jc_str_t *s2, const size_t len)
{
	if (len >= s1->len && len >= s2->len) return jc_strtcaseeq(s1, s2);
	return jc_strncaseeq(s1->str, s2->str, len);
}


int jc_strtcmp(const jc_str_t *s1, const jc_str_t *s2)
{
	int cmp;
	size_t max;

	max = s1->len;
	if (s2->len < max) max = s2->len;
	cmp = memcmp(s1->str, s2->str, max);
	if (cmp == 0) {
		if (s1->len != s2->len) return s1->len > max ? 1 : -1;
		return 0;
	}
	return cmp;
}


/* Convert a plain C string into a jc_str with size prefix */
jc_str_t *jc_str_init(const char *string, size_t len)
{
	jc_str_t *jstr;

	if (unlikely(string == NULL || len == 0)) {
		len = 0;
		string = jc_emptystring;
	} else {
		if (len == 0) len = strlen(string);
		if (len > UINT32_MAX) return NULL;
	}

	jstr = (jc_str_t *)malloc(sizeof(jc_str_t) + len + 1);
	if (jstr == NULL) return NULL;
	jstr->len = len;
	memcpy(jstr->str, string, len + 1);

	return jstr;
}
