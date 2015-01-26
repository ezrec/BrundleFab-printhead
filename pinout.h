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

#ifndef PINOUT_H
#define PINOUT_H

#if defined(__AVR_ATmega2560__)
#include "pinout-ATmega2560.h"
#endif

#if defined(__AVR_ATmega328P__)
#include "pinout-ATmega328P.h"
#endif

#define MOTOR_SELECT            1       /* Arduino Motor Shield 'A' */
#define MOTOR_PWM_MIN           98
#define MOTOR_PWM_MAX           150

/**
 * Global status bits
 */
#define STATUS_MOTOR_ON         (1 << 0)
#define STATUS_MOTOR_STALL      (1 << 1)
#define STATUS_MOTOR_MIN        (1 << 2)
#define STATUS_MOTOR_MAX        (1 << 3)
#define STATUS_HEATER_ON        (1 << 4)
#define STATUS_HEATER_STABLE    (1 << 5)
#define STATUS_INK_ON           (1 << 6)
#define STATUS_INK_EMPTY        (1 << 7)

#define STATUS_MASK_MOTOR       (STATUS_MOTOR_ON | \
                                 STATUS_MOTOR_STALL | \
                                 STATUS_MOTOR_MIN | \
                                 STATUS_MOTOR_MAX)

#endif /* PINOUT_H */
/* vim: set shiftwidth=4 expandtab:  */
