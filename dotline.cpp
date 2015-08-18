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

#include <stdint.h>

#include "dotline.h"

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

#define BUFFER_POS(x)	  (((x) / 2 * 3) + ((x)&1))
#define BUFFER_SHIFT(x)   (((x) & 1) ? 0 : 4)

static uint16_t pos_max;

void dotline_reset(uint16_t position_max)
{
    pos_max = position_max;
}

uint16_t dotline_get(uint16_t pos)
{
    uint16_t i = BUFFER_POS(pos);
    uint16_t line;

    if (pos > pos_max)
        return 0;

    line = ((uint16_t)dotline_buffer[i] << 8) | dotline_buffer[i+1];
    line >>= BUFFER_SHIFT(pos);

    return line & 0xfff;
}

void dotline_set(uint16_t pos, uint16_t line)
{
    uint16_t i = BUFFER_POS(pos);
    uint16_t mask = 0xfff;

    if (pos > pos_max)
        return;

    line <<= BUFFER_SHIFT(pos);
    mask <<= BUFFER_SHIFT(pos);

    dotline_buffer[i+0] &= ~((mask>>8) & 0xff);
    dotline_buffer[i+0] |= (line>>8);

    dotline_buffer[i+1] &= ~((mask>>0) & 0xff);
    dotline_buffer[i+1] |= (line>>0);
}

int dotline_set_range(uint16_t pos, int len, uint16_t line)
{
    uint8_t lmr[3];
    int done = 0;
    int index;

    if (pos >= pos_max)
        return done;

    if (pos + len > pos_max)
        len = pos_max - pos;

    if (pos & 1) {
        dotline_set(pos, line);
        pos++;
        done++;
    }

    if (len == 1)
        return done;

    lmr[0] = line >> 4;
    lmr[1] = ((line << 4) | (line >> 4)) & 0xff;
    lmr[2] = (line << 4) & 0xff;

    index = BUFFER_POS(pos);
    for (; done < len; done += 2, index += 3) {
        dotline_buffer[index + 0] = lmr[0];
        dotline_buffer[index + 1] = lmr[1];
        dotline_buffer[index + 2] = lmr[2];
    }

    if (done > len) {
        dotline_set(pos + len - 1, line);
        done--;
    }

    return done;
}

#ifdef STANDALONE

#include <stdio.h>
#include <stdlib.h>

uint8_t dotline_buffer[DOTLINE_SIZE(4096)];

int main(int argc, char **argv)
{
    uint16_t tbuff[4096];
    int i, j;
    int rc = EXIT_SUCCESS;
    int lines = 0;

    dotline_reset(4096);

    for (i = 0; i < 4096; i++) {
        int j, l = rand() & 0xf;
        uint16_t val = rand() & 0xfff;
        if (j == 0) {
            tbuff[i] = val;
            dotline_set(lines, val);
            lines++;
        } else {
            lines += dotline_set_range(lines, j, val);
            for (j = 0; j < l; j++)
                tbuff[i + j] = val;
            i += j - 1;
        }
    }

    for (i = 0; i < 4096; i++) {
        if (tbuff[i] != dotline_get(i)) {
            printf("ERROR: %d: ex 0x%03x != dl 0x%03x\n", i, tbuff[i], dotline_get(i));
            rc = EXIT_FAILURE;
        }
    }

    return rc;
}

#endif /* STANDALONE */

/* vim: set shiftwidth=4 expandtab:  */
