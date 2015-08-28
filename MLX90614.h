/*
 * Copyright (C) 2015, Netronome, Inc.
 * All right reserved.
 * Author: Jason McMullan <jason.mcmullan@netronome.com>
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

#ifndef MLX90614_H
#define MLX90614_H

#include <Wire.h>

#include "timecmp.h"
#include "Thermometer.h"

class MLX90614 : public Thermometer {
private:
	static const int timeout_ms = 100;
	int _dev;
	unsigned long _next_ms;

public:
	MLX90614() {
		_dev = 0x5a;

		Wire.begin();
		_next_ms = millis() + timeout_ms;
	}

	virtual void update(unsigned long ms_now)
	{
		uint8_t data[3];
		int len = 0;

		if (time_before(ms_now, _next_ms))
			return;

		Wire.beginTransmission(_dev);
		Wire.write(7);
		Wire.endTransmission(false);
		Wire.requestFrom(_dev, 3);
		while (len < 3 && Wire.available()) {
			data[len++] = Wire.read();
		}

		_next_ms = ms_now + timeout_ms;
		_kelvin = (float)(((uint16_t)data[1] << 8) | data[0]) * 0.02;
	}
};

#endif /* MLX90614_H */
/* vim: set shiftwidth=8 noexpandtab:  */
