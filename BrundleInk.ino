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

/* We use a private AFMotor class, because D7 is not available
 * on the the Trinket
 */
#include <AFMotor.h>

#include "Motor.h"

AF_DCMotor dcmotor = AF_DCMotor(AFMOTOR);
Encoder encoder = Encoder(ENCODER_A, ENCODER_B);
InkShield ink;

Motor motor = Motor(&dcmotor, &encoder, AFMOTOR_PWM_MIN, AFMOTOR_PWM_MAX);

#define BUFFER_MAX      512
uint16_t line_offset;
uint16_t line_index;
uint16_t line_buffer[BUFFER_MAX];

#define LINE_IS_REPEAT(line)    ((line) & 0x4000)
#define LINE_COUNT(line)        ((line) & 0x3fff)

#define IS_COMMAND(c)   ((c) & 0x80)
#define COMMAND_SYNC    0x8f
#define COMMAND_RESET   0x80
#define COMMAND_RUN     0x81
#define COMMAND_PULSES  0x82

static enum {
    STATE_BOGUS,        /* Out of sync */
    STATE_IDLE,         /* No spray, no move */
    STATE_DOTLINE,      /* Got the first half of a buffer read */
    STATE_SYNC_1,
    STATE_SYNC_2,
    STATE_PULSES_1,
    STATE_PULSES_2
} state;

static uint16_t pulses;
static bool active;
/* Sprays (1ms) per encoder position,
 * used to determine motor velocity
 */
static uint16_t sprays;

/* Next response character */
static uint8_t next_out;
static uint16_t dotline, count, spray;
static int32_t last_position;


void setup()
{
        Serial.begin(1000000);
        state = STATE_BOGUS;
        line_index = 0;
        line_offset = 0;
        active = true;
}

void run(void)
{
    last_position = 0;
    line_offset = 0;
    dotline = line_buffer[0];
    count   = 1;
    spray   = 0;
    active = true;

    motor.ms_per_position(sprays);
    motor.run();
}

void loop()
{
    unsigned long now = micros();
    int32_t pos = encoder.read();

    if (active) {
        if (line_offset >= line_index && count == 0) {
            /* Out of things to spray, home the head */
            motor.home();
        } else {
            /* Skip positions until we match up with reality */
            while (last_position < pos) {
                uint16_t tmp;

                tmp = line_buffer[++line_offset];
                if (!LINE_IS_REPEAT(tmp)) {
                    dotline = tmp;
                    count = 1;
                    spray = 0;
                } else {
                    count = LINE_COUNT(tmp);
                    spray = 0;
                }

                last_position += count;
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
        }
    }
    
    active = motor.update(pos, now);

    if (Serial.available()) {
        uint8_t c = Serial.read();
        Serial.write(next_out);
        next_out = c;

        switch (state) {
        case STATE_BOGUS:
            if (c == COMMAND_SYNC) {
                state = STATE_SYNC_1;
                next_out = 0xBF;
            } else {
                next_out = '?';
            }
            active = false;
            break;
        case STATE_IDLE:
            if (IS_COMMAND(c)) {
                switch (c) {
                case COMMAND_SYNC:
                    state = STATE_SYNC_1;
                    next_out = 0xBF;
                    break;
                case COMMAND_RESET:
                    line_index = 0;
                    /* FALLTROUGH */
                case COMMAND_RUN:
                    run();
                    break;
                case COMMAND_PULSES:
                    state = STATE_PULSES_1;
                    break;
                default:
                    state = STATE_BOGUS;
                    break;
                }
            } else {
                state = STATE_DOTLINE;
                dotline = c << 8;
                int left = (BUFFER_MAX - line_index);
                next_out = (uint8_t)(left >= 255 ? 255 :  left);
            }
            break;
        case STATE_SYNC_1:
            if (c == 0xff) {
                state = STATE_SYNC_2;
                next_out = 0x01;
            } else {
                state = STATE_BOGUS;
            }
            break;
        case STATE_SYNC_2:
            if (c == 0)
                state = STATE_IDLE;
            else
                state = STATE_BOGUS;
            break;
        case STATE_PULSES_1:
            pulses = c;
            state = STATE_PULSES_2;
            break;
        case STATE_PULSES_2:
            pulses <<= 8;
            pulses |= c;
            state = STATE_IDLE;
            break;
        case STATE_DOTLINE:
            dotline |= c;
            if (line_index < (BUFFER_MAX-1))
                line_buffer[line_index] = dotline;
            break;
        }
    }
}

/* vim: set shiftwidth=4 expandtab:  */
