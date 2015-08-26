/*
 * Copyright (C) 2015, Netronome, Inc.
 * All right reserved.
 * Author: Jason McMullan <jason.mcmullan@netronome.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef TIMECMP_H
#define TIMECMP_H

static inline long time_diff(unsigned long past, unsigned long future)
{
	return ((long)future - (long)past);
}

static inline int time_before(unsigned long past, unsigned long future)
{
	return time_diff(past, future) > 0;
}

static inline int time_before_eq(unsigned long past, unsigned long future)
{
	return time_diff(past, future) >= 0;
}

#define time_after(f, p)	time_before(p, f)
#define time_after_eq(f, p)	time_before_eq(p, f)

#endif /* TIMECMP_H */
/* vim: set shiftwidth=8 noexpandtab:  */
