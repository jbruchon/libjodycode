/* libjodycode: jc_str_t string functions
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


/* jc_strncaseeq() for jc_str_t strings */
int jc_strtncaseeq(const jc_str_t *s1, const jc_str_t *s2, const size_t len)
{
	if (len >= s1->len && len >= s2->len) return jc_strtcaseeq(s1, s2);
	return jc_strncaseeq(s1->str, s2->str, len);
}


/* strcmp() for jc_str_t strings */
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
