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
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

int tty_open(const char *device)
{
    struct termios nterm;
    int io;
      
    io = open(device, O_RDWR);
    if (io < 0)
        return -1;

    tcgetattr(io, &nterm);
    cfmakeraw(&nterm);
    cfsetspeed(&nterm, B115200);
    tcsetattr(io, TCSANOW, &nterm);

    return io;
}

int docmd(int fd, const uint8_t *cmd, size_t len)
{
    int i, err;
  
    for (i = 0; i < len; i++)
        printf("%02x.", cmd[i]);
    printf("\n");

    for (i = 0; i < len; i++) {
        uint8_t resp;
        err = write(fd, &cmd[i], 1);
        if (err != 1)
            return -1;
        err = read(fd, &resp, 1);
        if (err != 1)
            return -1;
        printf("%02x ", resp);
    }
    printf("\n");
}

uint8_t cmd_sync[] = { 0x8f, 0xfe, 0x8f, 0xff, 0x00 };

uint8_t cmd_reset[] = { 0x80 };

uint8_t cmd_pulses[] = { 0x82, 0x00, 0x0a };

uint8_t cmd_dotline[] = { 0x01, 0x23, 0x45, 0x67 };

uint8_t cmd_ink[] = { 0x81 };

#define DO(cmd) if (docmd(fd, cmd, sizeof(cmd)) < 0) return 1

int main(int argc, char **argv)
{
    int fd;
    uint8_t buff[16];

    fd = tty_open(argv[1]);

    DO(cmd_sync);
    DO(cmd_reset);
    DO(cmd_pulses);
    DO(cmd_dotline);
    DO(cmd_ink);
    DO(cmd_ink);

    close(fd);

    return 0;
}

/* vim: set shiftwidth=4 expandtab:  */
