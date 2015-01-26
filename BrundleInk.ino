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

/* Theory of operation:
 *   The ink ejector (HP C6602A via InkShield) takes 5us x 12 to
 *   emit a dot line, but requires 800us cool down between dot lines.
 *   For sake of simplicity, 1ms is allocated for a dot line.
 *   
 *   For every dot line, we will need to overspray by N to apply enough
 *   binder to the powder. So, N x 1ms per dot line.
 *
 *   The encoder is 600dpi, over 8.75" of travel, for 5250 dot lines
 *   per spray bar.
 *
 *   The maximum velocity of the printhead is therefore:
 *
 *     v(lines/sec) = 5250 / (N x 1ms);
 *
 *   The Atmel323 only has 2K of RAM, so ink patterns are delta compressed:
 *       0x0nnn = Emit pattern
 *       0x1nnn = Repeat previous pattern nnn times
 *   
 */

#include "pinout.h"

#include <Encoder.h>

/* We use a private InkShield class, that is interrupt-safe
 */
#include <InkShield.h>

#include "DCMotor.h"
#include "AMSMotor.h"

AMS_DCMotor dcmotor = AMS_DCMotor(MOTOR_SELECT);
Encoder encoder = Encoder(ENCODER_A, ENCODER_B);
INKSHIELD_CLASS ink(INKSHIELD_PULSE);

DCMotor_Encoder motor = DCMotor_Encoder(&dcmotor, &encoder, MOTOR_PWM_MIN, MOTOR_PWM_MAX);

#define BUFFER_MAX      512
uint16_t line_offset;
uint16_t line_index;
uint16_t line_buffer[BUFFER_MAX];

#define LINE_IS_REPEAT(line)    ((line) & 0x8000)
#define LINE_COUNT(line)        ((line) & 0x7fff)

static enum {
    STATE_BOGUS,        /* Out of sync */
    STATE_IDLE,         /* No spray, no move */
    STATE_ZERO,         /* Expecting a 0x00 */
    STATE_DOTLINE,      /* Got the first half of a buffer read */
    STATE_SYNC_1,
    STATE_SYNC_2,
    STATE_PULSES,
} state;

static bool active;
static uint8_t status;
/* Sprays (1ms) per encoder position,
 * used to determine motor velocity
 */
static uint16_t sprays;

/* Next response character */
static uint16_t dotline, count, spray;
static int next_position;
static int last_position;

void setup()
{
        Serial.begin(115200);
        state = STATE_BOGUS;
        line_index = 0;
        line_offset = 0;
        active = true;
}

void run(void)
{
Serial.println("motor: on\n");
    last_position = -1;
    next_position = 0;
    line_offset = 0;
    dotline = line_buffer[0];
    count   = 1;
    spray   = 0;
    active = true;

    motor.ms_per_position(sprays);
    motor.run(FORWARD);
}

void home(void)
{
    last_position = -1;
    next_position = 0;
    line_index = 0;
    line_offset = 0;
    count = 0;
    active = true;
    motor.ms_per_position(0);
    motor.run(BACKWARD);
}

char cmd;
int arg;
unsigned long next_time;

void loop()
{
    unsigned long now = millis();
    if (now <= next_time)
        return;
    next_time  = now + 1;

    int32_t pos = motor.read();

    if (active) {
        /* Skip positions until we match up with reality */
        if (last_position != pos) {
            while (next_position < pos) {
                uint16_t tmp;

                if (line_offset >= line_index) {
                    home();
                    break;
                }

                tmp = line_buffer[++line_offset];
                if (!LINE_IS_REPEAT(tmp)) {
                    dotline = tmp;
                    count = 1;
                    spray = 0;
                } else {
                    count = LINE_COUNT(tmp);
                    spray = 0;
                }

                next_position += count;
            }

            /* Update control, and spray */
            if (count) {
                if (spray < sprays) {
                    ink.spray_ink(dotline);
                    spray++;
                } else {
                    count--;
                    spray=0;
                }
            }

            last_position = pos;
        }
    
        status = (status & ~STATUS_MASK_MOTOR);
        status |= motor.update(pos, now);
        if (!(status & STATUS_MOTOR_ON)) {
            active = false;
        }
    }

    if (Serial.available()) {
        uint8_t c = Serial.read();

        if (c == '\n') {
            /* Do nothing */
        } else if (isxdigit(c)) {
            arg <<= 4;
            arg |= (c >= '0' && c <= '9') ? (c - '0') :
                   ((c | 0x20) - 'a' + 10);
        } else if (isspace(c)) {
            cmd = c;
            arg = 0;
        } else {
            const char *ok = "ok ";
            switch (cmd) {
            case 0:
                ok = NULL;
                break;
            case '?':
                break;
            case 'l':
                line_buffer[line_index++] = (arg & 0x7fff);
                break;
            case 'r':
                line_buffer[line_index++] = (arg & 0x7fff) | 0x8000;
                break;
            case 'h':
                home();
                break;
            case 'i':
                run();
                break;
            case 's':
                sprays = arg;
                break;
            case 't':
                //heater_setpoint = arg;
                break;
            default:
                ok = "? ";
            }
            cmd = 0;
            if (ok) {
                Serial.print(ok);
                Serial.print(status, HEX);
                Serial.print(" ");
                Serial.print(sprays, HEX);
                Serial.print(" ");
                Serial.print((uint16_t)(BUFFER_MAX - line_index), HEX);
                Serial.print(" ");
                Serial.print((uint8_t)0x55, HEX);
                Serial.println();
            }
        }
    }
}

/* vim: set shiftwidth=4 expandtab:  */
