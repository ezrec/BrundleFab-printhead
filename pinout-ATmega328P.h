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

#define AFMOTOR                 1

#define ENCODER_A               3       /* D3 */

                                        /* D4 - DIR_CLK */

#define ENCODER_B               5       /* D5 */

#define ENDSTOP_MIN             6       /* D6 */

                                        /* D7 - NC */

                                        /* D8 - DIR_SET */

                                        /* D9 - PWM2A */
        
#define HEATER_ENABLE           10      /* D10 */
                
#define MOTORENABLE             11      /* D11 - Moved from D7 */

                                        /* D12 - DIR_LATCH */

#define INKSHIELD_PULSE         13      /* D13 */

#define INKSHIELD_ABCD          0       /* A0/1/2/3 */

#define HEATER_TEMP             5       /* A5 */


#endif /* PINOUT_ATMEGA328P_H */
/* vim: set shiftwidth=4 expandtab:  */
