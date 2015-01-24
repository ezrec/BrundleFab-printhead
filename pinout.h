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

#define AFMOTOR_PWM_MIN         98
#define AFMOTOR_PWM_MAX         200

#endif /* PINOUT_H */
/* vim: set shiftwidth=4 expandtab:  */
