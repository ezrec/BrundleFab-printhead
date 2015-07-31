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
 *   The encoder is 600dpi, over 8.75" of travel, for 5250 possible
 *   dot lines per spray bar. However, we will only ink at 100 DPI, so
 *   we will only use 875 of those possible dotlines.
 *
 *   The Atmel323 only has 2K of RAM, so we use the same ink pattern
 *   twice - the pattern in read forwards at 2x the dot rate, then
 *   in reverse as the head returns.
 *
 *   The maximum Y axis "feed rate" of the printhead is therefore:
 *
 *     v(mm/min) = in/96dots * 25.4mm/in * 1dot/1ms * 1000ms/sec * 60sec/min
 *               = 15875 mm/min
 *
 *   Covering a swatch of 3.175mm (12/96")
 *
 *   Just the time needed to ink a 100mm solid cube (without accounting for
 *   the X, Z, or extruder movements), assuming a 1mm extruder, would be:
 *
 *   t = (100mm / 1mm) * (100mm / 3.175mm) * (100mm * min/15875mm)
 *     = 19.8 minutes
 *   
 */

#include "pinout.h"

#include <Encoder.h>

/* We use a private InkShield class, that is interrupt-safe
 */
#include <InkShield.h>

#include "AMSMotor.h"
#include "Axis_DCEncoder.h"

AMS_DCMotor dcmotor = AMS_DCMotor(MOTOR_SELECT);
Encoder encoder = Encoder(ENCODER_A, ENCODER_B);
INKSHIELD_CLASS ink(INKSHIELD_PULSE);

#define SCAN_WIDTH_CI	875L	/* 8.75" in ceniinches */
#define SCAN_WIDTH_MM	((float)SCAN_WIDTH_CI * 0.254)
#define SCAN_WIDTH_ENC 	(SCAN_WIDTH_CI * 600 / 100)
#define SCAN_WIDTH_DOT  (SCAN_WIDTH_CI * 96 / 100)

/* NOTE: This encoder is 600DPI */
Axis_DCEncoder motor = Axis_DCEncoder(&dcmotor, MOTOR_PWM_MIN, MOTOR_PWM_MAX,
                                      &encoder,
				      SCAN_WIDTH_MM,
				      -10, SCAN_WIDTH_ENC,
                                      -1, -1);

#define BUFFER_POS(x)	  (((x) * 3)/2)

uint16_t line_index;
uint16_t line_total;
uint8_t line_buffer[BUFFER_POS(SCAN_WIDTH_DOT)+1];

static inline uint16_t line_get(uint16_t pos)
{
    uint16_t i = BUFFER_POS(pos);
    uint16_t line;

    if (pos > line_total)
        return 0;

    line = ((uint16_t)line_buffer[i] << 8) | line_buffer[i+1];
    line >>= (i & 1) ? 0 : 4;
    
    return line & 0xfff;
}

static inline void line_set(uint16_t pos, uint16_t line)
{
    uint16_t i = BUFFER_POS(pos);
    uint16_t mask = 0xfff;

    if (pos > SCAN_WIDTH_DOT)
        return;

    line <<= (i & 1) ? 0 : 4;
    mask <<= (i & 1) ? 0 : 4;

    line_buffer[i+0] &= (mask>>8) & 0xff;
    line_buffer[i+0] |= (line>>8);

    line_buffer[i+1] &= (mask>>0) & 0xff;
    line_buffer[i+1] |= (line>>0);
}

static enum {
    STATE_BOGUS,        /* Out of sync */
    STATE_IDLE,         /* No spray, no move */
    STATE_ZERO,         /* Expecting a 0x00 */
    STATE_DOTLINE,      /* Got the first half of a buffer read */
    STATE_SYNC_1,
    STATE_SYNC_2,
    STATE_PULSES,
} state;

static uint8_t status;

/* Next response character */

void setup(void)
{
        Serial.begin(115200);

        pinMode(ENCODER_A, INPUT_PULLUP);
        pinMode(ENCODER_B, INPUT_PULLUP);

        state = STATE_BOGUS;
        line_index = 0;
        line_total = 0;
}

void update_ink(void)
{
    /* Sprayer on? */
    if ((status & STATUS_INK_ON)) {
        /* Convert from mm to dotline */
        int32_t pos = motor.position_get() / 25.4 * 96.0;

        ink.spray_ink(line_get(pos));
    }
}

void update_motor(unsigned long now)
{
    if (!motor.update(now)) {
        if (motor.endstop_min())
            status |= STATUS_MOTOR_MIN;
        else
            status &= ~STATUS_MOTOR_MIN;

        if (motor.endstop_max())
            status |= STATUS_MOTOR_MAX;
        else
            status &= ~STATUS_MOTOR_MAX;

        if (status & STATUS_MOTOR_ON) {
	    motor.motor_enable(false);
            status &= ~(STATUS_MOTOR_ON | STATUS_INK_ON);
	}
    }
}

uint16_t sprays;

void inkto(uint16_t line_target)
{
    unsigned long ms;
    uint16_t pos = motor.position_get() / 25.4 * 96.0;

    /* ms = 1 dotline/ms * sprays/dotline * dotlines-to-travel
     */
    ms = sprays * abs((int)pos - (int)line_target);

    motor.motor_enable(true);
    motor.target_set(line_target / 96.0 * 25.4, ms);
    status |= STATUS_INK_ON | STATUS_MOTOR_ON;
}

void home(void)
{
    line_index = 0;
    line_total = 0;
    sprays = 1;

    motor.home();
    status |= STATUS_MOTOR_ON;
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

    update_ink();

    update_motor(now);

    if (Serial.available()) {
        uint8_t c = Serial.read();

        if (c == '\n') {
            /* Do nothing */
        } else if (isxdigit(c)) {
            arg <<= 4;
            arg |= (c >= '0' && c <= '9') ? (c - '0') :
                   ((c | 0x20) - 'a' + 10);
        } else if (!isspace(c)) {
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
		if (line_total >= SCAN_WIDTH_DOT)
			break;
                line_total++;
                line_set(line_index++, arg & 0xfff);
                break;
            case 'r':
		if (line_total >= SCAN_WIDTH_DOT)
			break;
                line_total+= (arg & 0x7fff);
		if (line_total > SCAN_WIDTH_DOT)
			line_total = SCAN_WIDTH_DOT;
                arg = (line_index > 0) ? line_get(line_index-1) : 0;
                while (line_index < line_total)
                        line_set(line_index++, arg);
                break;
            case 'h':
                home();
                break;
            case 'i':
                inkto(line_total);
                break;
            case 'j':
                inkto(0);
                break;
	    case 'k':
		line_total=0;
		line_index=0;
		motor.motor_enable(false);
		break;
            case 's':
                sprays = arg + 1;
                break;
            case 't':
                //heater_setpoint = arg;
                break;
            default:
                ok = "? ";
            }
            cmd = 0;
            if (ok) {
		int16_t pos = motor.position_get() / 25.4 * 96.0;
		if (pos < 0)
		    pos = 0;
                Serial.print(ok);
                Serial.print(status, HEX);
                Serial.print(" ");
                Serial.print(sprays, HEX);
                Serial.print(" ");
                Serial.print((uint16_t)(SCAN_WIDTH_DOT - line_index), HEX);
                Serial.print(" ");
                Serial.print((uint8_t)0x55, HEX);
                Serial.print(" ");
                Serial.print(pos, HEX);
                Serial.println();
            }
        }
    }
}

/* vim: set shiftwidth=4 expandtab:  */
