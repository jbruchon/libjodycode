/* libjodycode: datetime string to UNIX epoch conversion
 *
 * Copyright (C) 2020-2026 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#include <errno.h>
#include <string.h>
#include <time.h>
#include "likely_unlikely.h"
#include "libjodycode.h"

#define NTTIME_CONSTANT 116444736000000000
#define NTTIME_NSEC 10000000

#define ATONUM(a,b) (a = b - '0')


static int twodigit_atoi(const char * restrict p)
{
	int i, val;

	if (unlikely(*p < '0' || *p > '9')) return -1;
	ATONUM(i, *p); val = i * 10; p++;
	if (unlikely(*p < '0' || *p > '9')) return -1;
	ATONUM(i, *p); val += i; p++;
	return val;
}

/* Accepts date[time] strings "YYYY-MM-DD" or "YYYY-MM-DD HH:MM:SS"
 * and returns the number of seconds since the Unix Epoch a la mktime()
 * or returns -1 on any error */
time_t jc_strtoepoch(const char * const datetime)
{
	time_t secs = 0;  /* 1970-01-01 00:00:00 */
	const char * restrict p = datetime;
	int i;
	struct tm tm;

	if (unlikely(datetime == NULL || *datetime == '\0')) goto error_null;
	memset(&tm, 0, sizeof(struct tm));

	/* Process year */
	i = twodigit_atoi(p); if (unlikely(i < 19)) goto error_datetime;
	tm.tm_year += i * 100; p += 2;
	i = twodigit_atoi(p); if (unlikely(i < 0)) goto error_datetime;
	tm.tm_year += i;
	tm.tm_year -= 1900;  /* struct tm year is since 1900 */
	p += 2;
	if (unlikely(*p != '-')) goto error_datetime;
	p++;
	/* Process month (0-11, not 1-12) */
	i = twodigit_atoi(p); if (unlikely(i < 1 || i > 12)) goto error_datetime;
	tm.tm_mon = i - 1;
	p += 2;
	if (unlikely(*p != '-')) goto error_datetime;
	p++;
	/* Process day */
	i = twodigit_atoi(p); if (unlikely(i < 1 || i > 31)) goto error_datetime;
	tm.tm_mday = i;
	p += 2;
	/* If YYYY-MM-DD is specified only, skip the time part */
	if (*p == '\0') goto skip_time;
	if (unlikely(*p != ' ')) goto error_datetime; else p++;
	/* Process hours */
	i = twodigit_atoi(p); if (unlikely(i < 0 || i > 23)) goto error_datetime;
	tm.tm_hour = i;
	p += 2;
	if (unlikely(*p != ':')) goto error_datetime;
	p++;
	/* Process minutes */
	i = twodigit_atoi(p); if (unlikely(i < 0 || i > 59)) goto error_datetime;
	tm.tm_min = i;
	p += 2;
	if (unlikely(*p != ':')) goto error_datetime;
	p++;
	/* Process seconds */
	i = twodigit_atoi(p); if (unlikely(i < 0 || i > 59)) goto error_datetime;
	tm.tm_sec = i;
	p += 2;
	/* Junk after datetime string should cause an error */
	if (unlikely(*p != '\0')) goto error_datetime;
skip_time:
	tm.tm_isdst = -1;  /* Let the host library decide if DST is in effect */
	errno = 0;
	secs = mktime(&tm);
	if (secs == -1) jc_errno = errno;
	return secs;
error_null:
	jc_errno = JC_ENULL;
	return -1;
error_datetime:
	jc_errno = JC_EDATETIME;
	return -1;
}


#ifdef ON_WINDOWS
int jc_nttime_to_unixtime(const FILETIME * const restrict filetime, struct JC_TIMESPEC * const restrict unixtime)
{
	uint64_t nttime;
	if (unlikely(filetime == NULL || unixtime == NULL)) return -1;
	nttime = ((uint64_t)(filetime->dwHighDateTime) << 32) + filetime->dwLowDateTime;
	if (unlikely(nttime <= NTTIME_CONSTANT)) return -1;
	unixtime->tv_sec = (time_t)((nttime - NTTIME_CONSTANT) / NTTIME_NSEC);
	unixtime->tv_nsec = (long)(((nttime - NTTIME_CONSTANT) % NTTIME_NSEC) * 100);
	return 0;
}


int jc_unixtime_to_nttime(const struct JC_TIMESPEC * const restrict unixtime, FILETIME * const restrict filetime)
{
	uint64_t nttime;
	if (unlikely(filetime == NULL || unixtime == NULL)) return -1;
	nttime = (uint64_t)((unixtime->tv_sec * NTTIME_NSEC) + (unixtime->tv_nsec / 100) + NTTIME_CONSTANT);
	if (unlikely(nttime <= NTTIME_CONSTANT)) return -1;
	filetime->dwHighDateTime = (DWORD)(nttime >> 32);
	filetime->dwLowDateTime = (DWORD)nttime;
	return 0;
}
#endif  /* ON_WINDOWS */
