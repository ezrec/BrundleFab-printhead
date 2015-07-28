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

#ifndef AMSMOTOR_H
#define AMSMOTOR_H

#include "DCMotor.h"

#define MOTOR_A         1
#define MOTOR_B         2

#ifndef MOTOR_A_DIR
#define MOTOR_A_DIR     12
#endif
#ifndef MOTOR_B_DIR
#define MOTOR_B_DIR     13
#endif
#ifndef MOTOR_A_PWM
#define MOTOR_A_PWM     3
#endif
#ifndef MOTOR_B_PWM
#define MOTOR_B_PWM     11
#endif
#ifndef MOTOR_A_BRAKE
#define MOTOR_A_BRAKE   9
#endif
#ifndef MOTOR_B_BRAKE
#define MOTOR_B_BRAKE   8
#endif
#ifndef MOTOR_A_SENSE
#define MOTOR_A_SENSE   A0
#endif
#ifndef MOTOR_B_SENSE
#define MOTOR_B_SENSE   A1
#endif

class AMS_DCMotor : public DCMotor {
    private:
        int8_t _dir, _pwm, _brake, _sense;
        uint8_t _speed;
    public:
        AMS_DCMotor(int8_t select)
        {
            if (select == MOTOR_A) {
                _dir = MOTOR_A_DIR;
                _pwm = MOTOR_A_PWM;
                _brake = MOTOR_A_BRAKE;
                _sense = MOTOR_A_SENSE;
            } else {
                _dir = MOTOR_B_DIR;
                _pwm = MOTOR_B_PWM;
                _brake = MOTOR_B_BRAKE;
                _sense = MOTOR_B_SENSE;
            }

            digitalWrite(_dir, LOW);
            analogWrite(_pwm, 0);
            digitalWrite(_brake, LOW);

            pinMode(_dir, OUTPUT);
            pinMode(_pwm, OUTPUT);
            pinMode(_brake, OUTPUT);
        }

        void run(uint8_t cmd)
        {
            if (cmd == FORWARD) {
                digitalWrite(_dir, LOW);
                analogWrite(_pwm, _speed);
                digitalWrite(_brake, LOW);
            } else if (cmd == BACKWARD) {
                digitalWrite(_dir, HIGH);
                analogWrite(_pwm, _speed);
                digitalWrite(_brake, LOW);
            } else if (cmd == RELEASE) {
                analogWrite(_pwm, _speed);
                digitalWrite(_brake, LOW);
            } else { /* cmd == BRAKE */
                analogWrite(_pwm, 0);
                digitalWrite(_brake, HIGH);
            }
        }

        void setSpeed(uint8_t speed)
        {
            _speed = speed;
            analogWrite(_pwm, speed);
        }

        int sense(void)
        {
            return analogRead(_sense);
        }
};

#endif /* AMSMOTOR_H */
/* vim: set shiftwidth=4 expandtab:  */
