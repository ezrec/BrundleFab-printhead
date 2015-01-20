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

#ifndef INKSHIELD_H
#define INKSHIELD_H

#ifndef INKSHIELD_PULSE
#define INKSHIELD_PULSE 2       /* D2 */
#endif

#ifndef INKSHIELD_ABCD
#define INKSHIELD_ABCD  14      /* A0..A3 */
#endif

#define PULSE_PIN  INKSHIELD_PULSE
#define PULSE_PORT portOutputRegister(digitalPinToPort(INKSHIELD_PULSE))
#define PULSE_MASK digitalPinToBitMask(INKSHIELD_PULSE)

#define ABCD_PIN   INKSHIELD_ABCD
#define ABCD_PORT  portOutputRegister(digitalPinToPort(INKSHIELD_ABCD))
#define ABCD_BIT   analogInPinToBit(INKSHIELD_ABCD)
    
class InkShield {
    public:
        InkShield()
        {
            pinMode(PULSE_PIN, OUTPUT);

            pinMode(ABCD_PIN + 0, OUTPUT);
            pinMode(ABCD_PIN + 1, OUTPUT);
            pinMode(ABCD_PIN + 2, OUTPUT);
            pinMode(ABCD_PIN + 3, OUTPUT);
        }

        static inline void setABCD(int nozzle)
        {
            uint8_t tmp = *ABCD_PORT;
            tmp &= ~(0xf << ABCD_BIT);
            *ABCD_PORT = tmp | (nozzle << ABCD_BIT);
        }

        static inline void pulse(void)
        {
            /* No interrupts while spraying,
             * or you can burn out a nozzle!
             */
            noInterrupts();

                *PULSE_PORT |= PULSE_MASK;
                delayMicroseconds(5);
                *PULSE_PORT &= PULSE_MASK;

            interrupts();
        }

        void spray_ink(uint16_t strip)
        {
            for (uint8_t nozzle = 0; nozzle < 12; nozzle++) {
                if (strip & (1 << nozzle)) {
                    setABCD(nozzle);
                    pulse();
                }
            }
            delayMicroseconds(800);
        }
};

#endif /* INKSHIELD_H */
/* vim: set shiftwidth=4 expandtab:  */
