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

#ifndef PINOUT_ATMEGA328P_H
#define PINOUT_ATMEGA328P_H

#define ENCODER_A               3       /* D3 */

#define ENCODER_B               4       /* D4 */

#define INKSHIELD_PULSE         5       /* D5 */

#define HEATER_ENABLE           6       /* D6 */

                                        /* D7 - NC */

#define MOTOR_B_BRAKE           8       /* D8 */

#define MOTOR_A_BRAKE           9       /* D9 */
        
#define MOTOR_A_PWM             10      /* D10 */

#define MOTOR_B_PWM             11      /* D11 */

#define MOTOR_A_DIR             12      /* D12 */

#define MOTOR_B_DIR             13      /* D13 */
                
#define MOTOR_A_SENSE           A0      /* A0 */

#define MOTOR_B_SENSE           A1      /* A1 */

#define INKSHIELD_ABCD          A2      /* A2/3/4/5 */

#define HEATER_TEMP             A6      /* A6 - input only */

                                        /* A7 - input only */

#define INKSHIELD_CLASS         InkShieldA2A5


#endif /* PINOUT_ATMEGA328P_H */
/* vim: set shiftwidth=4 expandtab:  */
