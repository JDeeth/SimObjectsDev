#ifndef SIMOBJECTSDEV_H
#define SIMOBJECTSDEV_H

#include "SimObjectsDev.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#elif defined(WIRING)
#include "Wiring.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

//! Aid for recording the dataref identifier
#define DataRefIdent PROGMEM const char

// Comments for parsing by Doxygen:

/*! \page intro Introduction
 *  \title    SimObjects
 *  \brief    A family of classes to connect hardware to X-Plane.
 *  \details  Currently only SimLED for integers is implemented but this
              will eventually incorporate most common types of
              electrical components.
 */

/*! \page licence Licence
 * Copyright 2012 Jack Deeth
 * Contact: simulationelectronics@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * I would appreciate, but not insist, on attribution if this code is
 * incorporated into other projects.
 */



class SimObject {

};

#endif
