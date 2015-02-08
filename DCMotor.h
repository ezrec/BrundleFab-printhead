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

#ifndef DCMOTOR_H
#define DCMOTOR_H

#define FORWARD         1
#define BACKWARD        2
#define BRAKE           3
#define RELEASE         4

class DCMotor {
    public:
        virtual void run(uint8_t dir) = 0;
        virtual void setSpeed(uint8_t pwm) = 0;
};

#endif /* DCMOTOR_H */
/* vim: set shiftwidth=4 expandtab:  */
