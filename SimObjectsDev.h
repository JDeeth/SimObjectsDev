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
public:
  SimObject(const bool *powerSource) {
    setPowerSource(powerSource);
  }


  //! \param powerSource Pointer to bool which defines whether simulated
  //!        power is available to this SimObject
  void setPowerSource (const bool * powerSource) {
    _powerSource = powerSource;

    if (powerSource == 0)
      _needsPower = false;
    else
      _needsPower = true;
  }


  static void setup(void);
  static void update (bool updateOutput = true);

  //! Default simulated power source
  static bool hasPower;

protected:
  virtual void _addToLinkedList(void);
  virtual void _setup (void) =0;
  virtual void _update(bool updateOutput = true) =0;

  //! Specifies if this object needs simulated power available to operate.
  bool _needsPower;

  //! Pointer to power source.
  /*! If _needsPower is set, this will be checked during update.*/
  const bool* _powerSource;

private:
  static SimObject* _first;
  SimObject* _next;

};


bool SimObject::hasPower = true;
SimObject* SimObject::_first  = 0;


void SimObject::setup() {
  if (_first != 0) {      // if at least one SimObject is instantiated
    SimObject* buf = _first;
    while (buf != 0) {
      buf->_setup();
      buf = buf->_next;
    }
  }
}



void SimObject::update( bool updateOutput) {
  if (_first != 0) {      // if at least one SimObject is instantiated
    SimObject* buf = _first;
    while (buf != 0) {
      buf->_update(updateOutput);
      buf = buf->_next;
    }
  }
}



void SimObject::_addToLinkedList(void) {
  _next = 0;

  if (_first == 0) {  // then this must be the first object
    _first = this;
  } else {
    // Go through linked list and make last existing element point to us
    SimObject *a = _first;
    while (a->_next)
      a = a->_next;
    a->_next = this;
  }
}


#endif
