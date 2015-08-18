/*
 * Copyright (C) 2015, Jason S. McMullan
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
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

#ifndef DOTLINE_H
#define DOTLINE_H

/*
 * This can get a bit confusing, with how the bits are packed:
 *
 *  |B A 9 8 7 6 5 4|3 2 1 0-B A 9 8|7 6 5 4 3 2 1 0|....
 *  | even-hi       |even-lo;odd-hi |   odd-lo      |
 *
 * So, if a position is even:
 *
 * bits = ((buff[pos/2*3]<<8) | (buff[pos/2*3+1])) >> 4)
 *
 * And if it is odd:
 *
 * bits = ((buff[pos/2*3+1]<<8) | (buff[pos/2*3+2])) & 0x3ff
 */

#define DOTLINE_SIZE(pos)     ((((pos) / 2 * 3) + ((pos)&1))+1)

extern uint8_t dotline_buffer[];

void dotline_reset(uint16_t pos_max);

uint16_t dotline_get(uint16_t pos);

void dotline_set(uint16_t pos, uint16_t line);

/* Returns total number of valid lines
 */
int dotline_set_range(uint16_t pos, int len, uint16_t dotline);

#endif /* DOTLINE_H */
/* vim: set shiftwidth=4 expandtab:  */
