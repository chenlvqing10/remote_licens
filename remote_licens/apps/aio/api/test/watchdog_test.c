/*
 * watchdog_test.c - Get the private data from private partition
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define LOG_TAG		"watchdog_test"
#include <log/log.h>

#include "watchdog.h"

static sig_atomic_t s_signal_received = 0;

static void signal_handler(int sig_num)
{
	signal(sig_num, signal_handler);
	s_signal_received = sig_num;
}

int main(int argc, char *argv[])
{
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	watchdog_init();

	while (s_signal_received == 0) {
		printf("Ctrl-c cancel feed watchdog device will reboot after 1...16 sec\n");
		watchdog_feed();
		sleep(1);
	}
	// should not use watchdog_deinit
	//watchdog_deinit();
	return 0;
}

