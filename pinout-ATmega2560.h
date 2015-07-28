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

#ifndef PINOUT_ATMEGA2560_H
#define PINOUT_ATMEGA2560_H

#define INKSHIELD_PULSE         2       /* D2 */

                                        /* D7 - DIR_EN */

                                        /* D8 - DIR_SET */

                                        /* D9 - PWM2A */
        
                                        /* D12 - DIR_LATCH */

#define INKSHIELD_ABCD          0       /* A0/1/2/3 */

#define INKSHIELD_CLASS         InkShieldA0A3

#define HEATER_TEMP             5       /* A5 */

#define ENCODER_A               19      /* D3 */

                                        /* D4 - DIR_CLK */

#define ENCODER_B               29      /* D5 */

#define ENDSTOP_MIN             35      /* D6 */

#define HEATER_ENABLE           43      /* D43 */

/**
 * Other configs
 */
#if defined(__AVR_ATmega323__)
#define AFMOTOR                 1
#endif

#if defined(__AVR_ATmega2560__)
#define AFMOTOR                 4
#endif

#define AFMOTOR_PWM_MIN         98
#define AFMOTOR_PWM_MAX         200

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

#endif /* PINOUT_ATMEGA2560_H */
/* vim: set shiftwidth=4 expandtab:  */
