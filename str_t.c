/* libjodycode: JC_STR_T string functions
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
JC_STR_T *jc_str_init(const char *string, uint32_t len)
{
	JC_STR_T *jstr;

	if (unlikely(string == NULL || len == 0)) {
		len = 0;
		string = jc_emptystring;
	} else {
		if (len == 0) len = (uint32_t)strlen(string);
		if (len > UINT32_MAX) return NULL;
	}

	jstr = (JC_STR_T *)malloc(sizeof(JC_STR_T) + len + 1);
	if (jstr == NULL) return NULL;
	jstr->len = len;
	memcpy(jstr->str, string, len + 1);

	return jstr;
}


/* jc_streq() for JC_STR_T strings */
int jc_strteq(const JC_STR_T *s1, const JC_STR_T *s2)
{
	if (s1->len == s2->len) return jc_streq(s1->str, s2->str);
	return 1;
}


/* jc_strneq() for JC_STR_T strings */
int jc_strtneq(const JC_STR_T *s1, const JC_STR_T *s2, const size_t len)
{
	if (len >= s1->len && len >= s2->len) return jc_strteq(s1, s2);
	return jc_strneq(s1->str, s2->str, len);
}


/* jc_strtcaseeq() for JC_STR_T strings */
int jc_strtcaseeq(const JC_STR_T *s1, const JC_STR_T *s2)
{
	if (s1->len == s2->len) return jc_strcaseeq(s1->str, s2->str);
	return 1;
}


/* jc_strncaseeq() for JC_STR_T strings */
int jc_strtncaseeq(const JC_STR_T *s1, const JC_STR_T *s2, const size_t len)
{
	if (len >= s1->len && len >= s2->len) return jc_strtcaseeq(s1, s2);
	return jc_strncaseeq(s1->str, s2->str, len);
}


/* strcmp() for JC_STR_T strings */
int jc_strtcmp(const JC_STR_T *s1, const JC_STR_T *s2)
{
	int cmp;
	size_t max = s1->len;

	if (s2->len < max) max = s2->len;
	cmp = memcmp(s1->str, s2->str, max);
	if (cmp == 0 && s1->len != s2->len)
		return s1->len > s2->len ? 1 : -1;
	return cmp;
}


int jc_strtncmp(const JC_STR_T *s1, const JC_STR_T *s2, size_t len)
{
	int cmp;
	int limited = 0;
	size_t max = len;

	if (s1->len < max) { max = s1->len; limited = 1; }
	if (s2->len < max) { max = s2->len; limited = 1; }
	cmp = memcmp(s1->str, s2->str, max);
	if (cmp == 0 && limited == 1 && s1->len != s2->len)
		return s1->len > s2->len ? 1 : -1;

	return cmp;
}


int jc_strtcasecmp(const JC_STR_T *s1, const JC_STR_T *s2)
{
	return strcasecmp(s1->str, s2->str);
}


int jc_strtncasecmp(const JC_STR_T *s1, const JC_STR_T *s2, size_t len)
{
	return strncasecmp(s1->str, s2->str, len);
}


int jc_numeric_strtcmp(const JC_STR_T *c1, const JC_STR_T *c2, const int insensitive)
{
	return jc_numeric_strcmp(c1->str, c2->str, insensitive);
}
