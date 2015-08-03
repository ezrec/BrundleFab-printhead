/*
 * Copyright (C) 2014, Jason S. McMullan
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

#ifndef AXIS_DCENCODER_H
#define AXIS_DCENCODER_H

#include <AMSMotor.h>
#include <Encoder.h>

#include "Axis.h"

class Axis_DCEncoder : public Axis {
    private:
        static const int _overshoot = 10;
        static const int DEBUG = 0;

        int _minPos;
        int _maxPos;
        int _pinStopMin;
        int _pinStopMax;
        uint8_t _pwmMinimum;
        uint8_t _pwmMaximum;

        float _mm_to_position;
        int32_t _target_pos;
        struct {
            int32_t pos;
	    unsigned long ms;
        } _last;

        DCMotor *_motor;
        Encoder *_encoder;

        enum {
            IDLE = 0,
            MOVING = 1,
            HOMING_STOP = 3,
            HOMING_STOP_QUIESCE = 4,
            HOMING_STOP_BACKOFF = 5,
            HOMING_STALL = 6,
            HOMING_STALL_QUIESCE = 7,
            HOMING_STALL_BACKOFF = 8
        } _mode;
        struct {
            unsigned long timeout;
            int overshoot;
        } _moving;
        struct {
            int32_t home, position, target;
            uint32_t timeout;
            int pin, dir;
            uint8_t pwm;
        } _homing;
        struct {
            unsigned long ms;
            int32_t pos;
        } _stall;

        uint8_t _pwm;

        int32_t _mm2pos(float mm)
        {
            return (mm * _mm_to_position) + _minPos;
        }

        float _pos2mm(int32_t pos)
        {
            return (pos - _minPos) / _mm_to_position;
        }

    public:
        Axis_DCEncoder(DCMotor *motor,
		       uint8_t pwm_min, uint8_t pwm_max,
                       Encoder *encoder,
                           float mm, int32_t minpos, int32_t maxpos,
                       int stop_min, int stop_max,
		       float vel_max_mm_per_ms = 0.0) :
            Axis(vel_max_mm_per_ms)
        {
            _motor = motor;
            _encoder = encoder;
            _minPos = minpos;
            _maxPos = maxpos;
            _pinStopMin = stop_min;
            _pinStopMax = stop_max;
            _pwmMinimum = pwm_min;
            _pwmMaximum = pwm_max;
            _pwm = pwm_min;
            _mm_to_position = (float)(_maxPos - _minPos)/mm;
        }

        virtual void begin()
        {
            _encoder->write(0);
            Axis::begin();
        }

        virtual void home(float mm = 0.0)
        {
            _homing.target = _mm2pos(mm);

            if (_pinStopMin >= 0) {
                _mode = HOMING_STOP;
                _homing.pin = _pinStopMin;
                _homing.dir = BACKWARD;
                _homing.home = _minPos - _overshoot;
                _homing.pwm = _pwmMaximum;
            } else if (_pinStopMax >= 0) {
                _mode = HOMING_STOP;
                _homing.pin = _pinStopMax;
                _homing.dir = FORWARD;
                _homing.home = _maxPos + _overshoot;
                _homing.pwm = _pwmMaximum;
            } else {
                _mode = HOMING_STALL;
                _homing.dir = BACKWARD;
                _homing.home = _minPos - _overshoot;
                _homing.pwm = (_pwmMinimum + _pwmMaximum) / 2;
            }

	    motor_enable(true);
            _motor->setSpeed(_homing.pwm);
            _motor->run(_homing.dir);
        }

        virtual float position_min()
        {
            return _pos2mm(_minPos);
        }

        virtual float position_max()
        {
            return _pos2mm(_maxPos);
        }

        virtual bool endstop_min()
        {
            if (_pinStopMin >= 0)
                return digitalRead(_pinStopMin) == 1;

            return Axis::endstop_min();
        }

        virtual bool endstop_max()
        {
            if (_pinStopMax >= 0)
                return digitalRead(_pinStopMax) == 1;

            return Axis::endstop_max();
        }

        virtual void motor_enable(bool enabled = true)
        {
            if (!enabled)
                _motor->run(RELEASE);
            Axis::motor_enable(enabled);
        }

        virtual void target_set(float mm, unsigned long ms = 0)
        {
            Axis::target_set(mm, ms);
            _target_pos = _mm2pos(_target.mm);
        }

        virtual bool motor_active()
        {
            return _mode != IDLE;
        }

        virtual float position_get(void)
        {
            return _pos2mm(_encoder->read());
        }

        virtual bool update(unsigned long ms_now)
        {
	    if (!motor_enabled())
		return false;

            int32_t pos = _encoder->read();
            int32_t tar = _target_pos;

if (DEBUG) {
    Serial.print("update: mode:");Serial.print(_mode);
    Serial.print(" now:");Serial.print(ms_now);
    Serial.print(" pos:");Serial.print(pos);
    Serial.print(" tar:");Serial.print(tar);
}
            if (tar >= _maxPos)
                tar = _maxPos - 1;

            if (tar < _minPos)
                tar = _minPos;

            switch (_mode) {
            case IDLE:
                if (tar != pos) {
                    _mode = MOVING;
	    	    _last.pos = _encoder->read();
		    _last.ms  = ms_now;
                    _stall.ms = ms_now;
                    _stall.pos = _last.pos;
if (DEBUG) {
    Serial.println(" MOVING");
}
                    return update(ms_now);
                }
                break;
            case HOMING_STALL:
                _homing.timeout = ms_now+10;
                _homing.position = pos;
                _mode = HOMING_STALL_QUIESCE;
                break;
            case HOMING_STALL_QUIESCE:
                if (ms_now >= _homing.timeout) {
                    if (_encoder->read() == _homing.position) {
                        _motor->setSpeed(0);
                        _motor->run(RELEASE);
                        _homing.timeout = ms_now + 100;
                        _mode = HOMING_STALL_BACKOFF;
                       break;
                    }
                    _mode = HOMING_STALL;
                }
                break;
            case HOMING_STALL_BACKOFF:
                if (ms_now >= _homing.timeout) {
                    _motor->setSpeed(_homing.pwm);
                    _encoder->write(_homing.home);
                    _mode = MOVING;
	    	    _last.pos = _encoder->read();
		    _last.ms  = ms_now;
                    _stall.ms = ms_now;
                    _stall.pos = _last.pos;
                    Axis::home(_homing.target);
                }
                break;
            case HOMING_STOP:
                if (digitalRead(_homing.pin) == 1) {
                    _homing.timeout = ms_now+1;
                    _mode = HOMING_STOP_QUIESCE;
                }
                break;
            case HOMING_STOP_QUIESCE:
                if (ms_now >= _homing.timeout) {
                    _mode = HOMING_STOP_BACKOFF;
                    _motor->run(RELEASE);
                    _motor->setSpeed(_pwmMinimum);
                    _motor->run(_homing.dir == FORWARD ? BACKWARD : FORWARD);
                    _homing.timeout = ms_now+10;
                    _homing.pwm = _pwmMinimum;
                    _mode = HOMING_STOP_BACKOFF;
                }
                break;
            case HOMING_STOP_BACKOFF:
                if (ms_now >= _homing.timeout) {
                    if (digitalRead(_homing.pin) == 1) {
                        if (_homing.pwm < _pwmMaximum)
                            _homing.pwm++;
                        _motor->setSpeed(_homing.pwm);
                        _homing.timeout = ms_now+10;
                    } else {
                        _encoder->write(_homing.home);
                        _mode = MOVING;
                        Axis::home(_homing.target);
                    }
                }
                break;
            case MOVING:
                if (_pinStopMin >= 0 && digitalRead(_pinStopMin) == 1) {
                    if (tar <= pos) {
                        _mode = IDLE;
                        _encoder->write(_minPos);
                        _motor->run(BRAKE);
                        break;
                    }
                }

                if (_pinStopMax >= 0 && digitalRead(_pinStopMax) == 1) {
                    if (tar >= pos) {
                        _mode = IDLE;
                        _encoder->write(_maxPos);
                        _motor->run(BRAKE);
                        break;
                    }
                }

                int32_t distance = (tar < pos) ? (pos - tar) : (tar - pos);

                if (distance < _overshoot) {
                    _motor->run(BRAKE);
                    _motor->setSpeed(0);
		    _mode = IDLE;
                    break;
                }

                if (ms_now > (_stall.ms+250)) {
                    /* Stalled? */
                    if (pos == _stall.pos) {
                        _mode = IDLE;
                        break;
                    }

                    _stall.ms = ms_now;
                    _stall.pos = pos;
                }

                if (ms_now > (_last.ms+3) && _target.velocity > 0.0) {
                    float velocity = fabs((_pos2mm(pos) - _pos2mm(_last.pos)) / (ms_now - _last.ms));

if (DEBUG) {
    Serial.print(" vel:");Serial.print(velocity*100);Serial.print("::");
    Serial.print(_target.velocity*100);
}
		    if (velocity < _target.velocity) {
                        if (_pwm < _pwmMaximum)
                            _pwm++;
                    } else {
                        if (_pwm > 0)
                            _pwm--;
                    }

		    _last.pos = pos;
		    _last.ms = ms_now;
                }

                _motor->setSpeed(_pwm);
                _motor->run((pos < tar) ? FORWARD : BACKWARD);
                break;
            }

if (DEBUG) {
    Serial.print(" => mode:");Serial.print(_mode);
    Serial.print(" pwm:");Serial.print(_pwm);
    Serial.print(motor_enabled() ? "\r" : "\r\n");
}

             if (_mode == IDLE)
                 motor_enable(false);

             return (_mode == IDLE) ? false : true;
        }
};


#endif /* AXIS_DCENCODER_H */
/* vim: set shiftwidth=4 expandtab:  */
