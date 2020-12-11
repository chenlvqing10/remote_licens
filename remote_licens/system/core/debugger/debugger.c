/*
 * debugger.c
 *
 * Copyright (C)
 * Author:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
#include <stdlib.h>

#define LOG_TAG "debugger"
#include <log/log.h>

// add-s by wenweiwei
#include <unistd.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <syscall.h>
#define gettidv1() syscall(__NR_gettid)
// add-e by wenweiwei

#define STACK_DEPTH_MAX 128

static const char *get_signame(int sig)
{
    switch(sig) {
    case SIGILL:     return "SIGILL";
    case SIGABRT:    return "SIGABRT";
    case SIGBUS:     return "SIGBUS";
    case SIGFPE:     return "SIGFPE";
    case SIGSEGV:    return "SIGSEGV";
    case SIGSTKFLT:  return "SIGSTKFLT";
    case SIGPIPE:    return "SIGPIPE";
    default:         return "?";
    }
}

static void debugger_signal_handler(int sig)
{
#if 1
    //add-s by wenweiwei
	unsigned pid;
    unsigned tid;
	int s;
	signal(SIGUSR1, SIG_IGN);
	pid = getpid();
    tid = gettidv1();
	ALOGI(" thread Signal pid: %d , tid: %d\n", pid,(long int)tid);
	s = socket_local_client("virgo:debuggerd", ANDROID_SOCKET_NAMESPACE_ABSTRACT, 1/*SOCK_STREAM*/);
	if (s > 0) {
		write(s, &tid, sizeof(unsigned));
		read(s, &tid, 1);
		close(s);
	}
    signal(sig,SIG_IGN);
    //add-e by wenweiwei
#else
	void *stack_trace[STACK_DEPTH_MAX] = {0};
	char **stack_strings = NULL;
	int stack_depth = 0;
	int i = 0;

	ALOGI(" Signal: %d (%s)\n", sig, get_signame(sig));
	stack_depth = backtrace(stack_trace, STACK_DEPTH_MAX);
	ALOGI(" Stack depth: %d\n", stack_depth);

	stack_strings = (char **)backtrace_symbols(stack_trace, stack_depth);
	if (NULL == stack_strings) {
		ALOGI(" Memory is not enough while dump stack trace\n");
		exit(EXIT_FAILURE);
	}

	ALOGI(" Stack trace:\n");
	for (i = 0; i < stack_depth; i++) {
		ALOGI(" [%d] %s\n", stack_depth - i, stack_strings[i]);
	}
	ALOGI("\n");
	free(stack_strings);
	stack_strings = NULL;
	exit(EXIT_FAILURE);
#endif
}


static void __attribute__ ((constructor)) debugger_init()
{
    signal(SIGILL, debugger_signal_handler);
    signal(SIGABRT, debugger_signal_handler);
    signal(SIGBUS, debugger_signal_handler);
    signal(SIGFPE, debugger_signal_handler);
    signal(SIGSEGV, debugger_signal_handler);
    signal(SIGSTKFLT, debugger_signal_handler);
    signal(SIGPIPE, debugger_signal_handler);
}

#endif /* _DEBUGGER_H_ */
