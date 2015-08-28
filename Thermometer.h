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

#ifndef THERMOMETER_H
#define THERMOMETER_H

class Thermometer {
protected:
	float _kelvin;

public:
	Thermometer() { _kelvin = 0.0; }

	float kelvin() {
		return _kelvin;
	}

	float celsius() {
		return kelvin() - 273.16;
	}

	virtual void update(unsigned long ms_now) = 0;
};

#endif /* THERMOPILE_H */
/* vim: set shiftwidth=8 noexpandtab:  */
