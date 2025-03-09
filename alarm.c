/* libjodycode: cross-platform alarms
 *
 * Copyright (C) 2023-2025 by Jody Bruchon <jody@jodybruchon.com>
 * Released under The MIT License
 */

#ifdef ON_WINDOWS
 #define _WIN32_WINNT 0x0500
 #define WIN32_LEAN_AND_MEAN
 #include <windows.h>
#else
 #include <errno.h>
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef ON_WINDOWS
 #include <unistd.h>
#endif

#include "likely_unlikely.h"
#include "libjodycode.h"

int jc_alarm_ring = 0;

#ifdef ON_WINDOWS
static HANDLE hTimer;
#else
static int jc_alarm_repeat = 0;
#endif /* ON_WINDOWS */


#ifdef ON_WINDOWS
void CALLBACK jc_catch_alarm(PVOID arg1, BOOLEAN arg2)
{
	(void)arg1; (void)arg2;
	jc_alarm_ring++;
	return;
}


int jc_start_alarm(const unsigned int seconds, const int repeat)
{
	unsigned int secs = seconds * 1000;
	unsigned int period = 0;

	if (repeat != 0) period = secs;
	if (!CreateTimerQueueTimer(&hTimer, NULL, (WAITORTIMERCALLBACK)jc_catch_alarm, 0, secs, period, 0)) {
		jc_errno = jc_GetLastError();
		return JC_EALARM;
	}
	jc_alarm_ring++;
	return 0;
}


int jc_stop_alarm(void)
{
	if (CloseHandle(hTimer) == 0) {
		jc_errno = jc_GetLastError();
		return JC_EALARM;
	}
	return 0;
}

#else /* not ON_WINDOWS */
void jc_catch_alarm(const int signum)
{
	(void)signum;

	jc_alarm_ring++;
	if (jc_alarm_repeat != 0) alarm(1);
	return;
}


int jc_start_alarm(const unsigned int seconds, const int repeat)
{
	struct sigaction sa_run;

	memset(&sa_run, 0, sizeof(struct sigaction));
	sa_run.sa_handler = jc_catch_alarm;
	if (repeat != 0) jc_alarm_repeat = 1;
	if (sigaction(SIGALRM, &sa_run, NULL) != 0) {
		jc_errno = errno;
		return JC_EALARM;
	}
	alarm(seconds);
	return 0;
}


int jc_stop_alarm(void)
{
	struct sigaction sa_stop;

	alarm(0);
	memset(&sa_stop, 0, sizeof(struct sigaction));
	sa_stop.sa_handler = SIG_IGN;
	jc_alarm_repeat = 0;
	if (sigaction(SIGALRM, &sa_stop, NULL) != 0) {
		jc_errno = errno;
		return JC_EALARM;
	}
	return 0;
}
#endif /* ON_WINDOWS */
