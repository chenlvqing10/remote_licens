/*
* interval.h
*
* Copyright (C) 2019, LomboTech Co.Ltd.
* Author: lomboswer <lomboswer@lombotech.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef TINYALSA_INTERVAL_H
#define TINYALSA_INTERVAL_H

#include <stdlib.h>

/** A closed range signed interval. */

struct tinyalsa_signed_interval {
	/** The maximum value of the interval */
	ssize_t max;
	/** The minimum value of the interval */
	ssize_t min;
};

/** A closed range unsigned interval. */

struct tinyalsa_unsigned_interval {
	/** The maximum value of the interval */
	size_t max;
	/** The minimum value of the interval */
	size_t min;
};

#endif /* TINYALSA_INTERVAL_H */

