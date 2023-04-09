/*
 * Jody Bruchon's string function library
 *
 * Copyright (C) 2015-2023 by Jody Bruchon <jody@jodybruchon.com>
 * See jody_string.c for license information
 */

#ifndef JODY_STRING_H
#define JODY_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <unistd.h>

extern int jc_strncaseeq(const char *s1, const char *s2, size_t len);
extern int jc_strcaseeq(const char *s1, const char *s2);
extern int jc_strneq(const char *s1, const char *s2, size_t len);
extern int jc_streq(const char *s1, const char *s2);

#ifdef __cplusplus
}
#endif

#endif /* JODY_STRING_H */
