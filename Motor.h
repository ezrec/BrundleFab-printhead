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

#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

#include <AFMotor.h>
#include <Encoder.h>

class Motor {
    private:
        uint16_t _ms_per_pos;

        Encoder *_encoder;
        AF_DCMotor *_motor;

        uint8_t _pwmMin, _pwmMax;
        unsigned long _next_msec;
        int32_t _next_pos;

        uint8_t _pwm;
        int _dir;

    public:
        Motor(AF_DCMotor *m, Encoder *e, uint8_t pwmMin, uint8_t pwmMax)
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

        void idle()
        {
            _motor->setSpeed(0);
            _motor->run(BRAKE);
        }

        void home()
        {
            if (_dir != BACKWARD) {
                _pwm = _pwmMax;
                _dir = BACKWARD;
                _motor->setSpeed(_pwm);
                _motor->run(_dir);
            }
        }

        void run()
        {
            _pwm = 0;
            _dir = FORWARD;
            _motor->setSpeed(_pwm);
            _motor->run(_dir);
        }

        bool update(int32_t pos, unsigned long msec)
        {
            if (digitalRead(ENDSTOP_MIN) == 0) {
                /* We are home */
                idle();
                delay(100);
                _encoder->write(0);
                return false;
            }

            /* Not enough time has elapsed.. */
            if (msec < _next_msec)
                return (_dir != BRAKE);

            /* Stall detection */
            if (_pwm == _pwmMax &&
                (msec - _next_msec) > (_ms_per_pos * 10)) {
                idle();
                return false;
            }

            uint8_t pwm = _pwm;

            if (pos < _next_pos) {
                /* Agh. Not fast enough to reach the position... */
                if (pwm < _pwmMax)
                    pwm++;
            } else if (pos > _next_pos) {
                /* Crap! Too fast! */
                if (pwm > _pwmMin)
                    pwm--;
            }

            if (pwm != _pwm)
                _motor->setSpeed(_pwm);

            _next_msec = msec + _ms_per_pos;
            _next_pos = pos + 1;

            return true;
        }
};


#endif /* MOTOR_H */
/* vim: set shiftwidth=4 expandtab:  */
