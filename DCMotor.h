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

#include <Encoder.h>

/* These defines are the same as the Adafruit MotorShield library
 */
#define FORWARD         1
#define BACKWARD        2
#define BRAKE           3
#define RELEASE         4

class DCMotor {
    public:
        virtual void run(int8_t cmd)  = 0;
        virtual void setSpeed(uint8_t pwm) = 0;
};

class DCMotor_Encoder {
    private:
        uint16_t _ms_per_pos;

        Encoder *_encoder;
        DCMotor *_motor;

        uint8_t _pwmMin, _pwmMax;
        unsigned long _next_msec;
        int32_t _next_pos;

        uint8_t _pwm;
        uint8_t _status;
        int _dir;

    public:
        DCMotor_Encoder(DCMotor *m, Encoder *e, uint8_t pwmMin, uint8_t pwmMax)
        {
            _motor = m;
            _encoder = e;
            _pwmMin = pwmMin;
            _pwmMax = pwmMax;
            _dir = BRAKE;
        }

        void begin()
        {
            idle();
        }

        void ms_per_position(uint16_t ms_per_pos)
        {
            _ms_per_pos = ms_per_pos + 1;
        }

        int read()
        {
            /* Convert from encoder coordingate to dots (96 DPI) */
            return map(_encoder->read(), 0, 5000, 0, 96 * 9);
        }

        void idle()
        {
            _motor->setSpeed(0);
            _motor->run(BRAKE);
            _status &= ~STATUS_MOTOR_ON;
        }

        void run(int dir)
        {
            _next_msec = 0;
            _pwm = _pwmMin;
            _dir = dir;
            _motor->setSpeed(_pwm);
            _motor->run(_dir);
            _status &= ~(STATUS_MOTOR_MIN | STATUS_MOTOR_STALL);
            _status |= STATUS_MOTOR_ON;
        }

        uint8_t update(int pos, unsigned long msec)
        {
            /* Not enough time has elapsed.. */
            if (msec < _next_msec)
                return (_dir != BRAKE);

            /* Stall detection */
            uint8_t pwm = _pwm;

Serial.print("p:");Serial.print(pos);Serial.print(" n:");Serial.println(_next_pos);
            int delta = (_dir == FORWARD) ? 1 : -1;

            /* Right on time */
            if (pos == _next_pos)
                delta = 0;
            else if (pos > _next_pos)
                delta *= -1;

            if (delta < 0) {
                /* Crap! Too fast! */
                if (pwm > _pwmMin)
                    pwm--;
            } else if (delta > 0) {
                /* Agh. Not fast enough to reach the position... */
                if (pwm < _pwmMax) {
                    pwm++;
                } else {
Serial.println("motor: STALL!\n");
                    if (_dir == BACKWARD) {
                        _encoder->write(0);
                        _status |= STATUS_MOTOR_MIN;
                    } else {
                        _status |= STATUS_MOTOR_STALL;
                    }
                    idle();
                    return _status;
                }
            }

            if (pwm != _pwm) {
                _pwm = pwm;
                _motor->setSpeed(_pwm);
            }

            _next_msec = msec + _ms_per_pos;
            _next_pos = pos + ((_dir == FORWARD) ? 1 : -1);

            return _status;
        }
};


#endif /* DCMOTOR_H */
/* vim: set shiftwidth=4 expandtab:  */
