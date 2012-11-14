
// SimLED Development Version

/*
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

#ifndef SIMLEDDEV_H
#define SIMLEDDEV_H

#include "SimObjectsDev.h"

// for code editing purposes
// remove this from final version of SimLED
#include "usb_api.h"

//! High-level dataref-to-LED linking class
/*! Incorporating bulb-test and power-available features.*/
class SimLEDBase : public SimObject {
public:
  /// True if input conditions would cause this LED to light
  bool isActive(void) { return _active; }

  /// True if this LED should light, applying filters (power, test etc)
  bool isLit(void)    { return _lit ; }

  /// Enable/disable bulb test mode
  static void lightTest(bool lightAll)  {_testAll = lightAll;}

  /// Enable/disable this SimLED's participation in lightTests
  void enableTest (bool allowTest)      {_allowTest = allowTest;}

protected:
  SimLEDBase(const int  &ledPin,
         const bool &enableTest,
         const bool *hasPowerFlag
         ) :
    SimObject(hasPowerFlag),
    _pin(ledPin),
    _allowTest(enableTest)
  {
    _addToLinkedList();
  }

  bool _active;

private:
  /// Arduino pin number of LED.
  int _pin;


  bool _lit;

  void _setup (void) { pinMode(_pin, OUTPUT); }
  void _update(bool updateOutput = true);

  virtual void _updateActive() = 0;

  bool _allowTest;

  static bool _testAll;

};


////////////////////////////////////////////////////////////////////////


// Determine whether this SimLED should be lit
void SimLEDBase::_update(bool updateOutput) {

  _updateActive();

  _lit = _active;

  // we are lit if bulb-test is active
  if( (_allowTest == true) && (_testAll == true) )
    _lit = true;

  // we are not lit if the sim isn't running or no simulated power
  if( (FlightSim.isEnabled() == false) || (hasPower == false) ) {
    _lit = false;
  }

  // unless ordered otherwise, light or extinguish LED based on our lighting state
  if (updateOutput)
    digitalWrite(_pin, _lit);
}


// Initialise static data members
bool SimLEDBase::_testAll   = false;





class SimLEDIntDR : public SimLEDBase {
public:
  SimLEDIntDR(const int    &ledPin,
            const char * ident,
            const int    &lowLimit     = 1,
            const int    &highLimit    = 32000,
            const bool   &invertLimits = false,
            const bool   &enableTest   = true,
            const bool   *hasPowerFlag = &SimObject::hasPower
      ) : SimLEDBase(ledPin, enableTest, hasPowerFlag)
  {
    _drInt.assign((const _XpRefStr_ *) ident);

    if (lowLimit < highLimit) {
      _lowLimitInt = lowLimit;
      _highLimitInt = highLimit;
    } else {
      _lowLimitInt = highLimit;
      _highLimitInt = lowLimit;
    }
    _inverse = invertLimits;

  } // constructor

private:
  FlightSimInteger _drInt;
  int _lowLimitInt;
  int _highLimitInt;
  bool _inverse;

  void _updateActive() {
    _active = (_lowLimitInt <= _drInt && _drInt <= _highLimitInt);
    if (_inverse == true) {
      _active = _active? false: true;
    }
  }
};



class SimLEDFloatDR : public SimLEDBase {
public:
  SimLEDFloatDR(const int    &ledPin,
              const char * ident,
              const float  &lowLimit,
              const float  &highLimit,
              const bool   &invertLimits = false,
              const bool   &enableTest   = true,
              const bool   *hasPowerFlag = &SimObject::hasPower
      ) : SimLEDBase(ledPin, enableTest, hasPowerFlag)
  {
    _drFloat.assign((const _XpRefStr_ *) ident);

    if (lowLimit < highLimit) {
      _lowLimitFloat = lowLimit;
      _highLimitFloat = highLimit;
    } else {
      _lowLimitFloat = highLimit;
      _highLimitFloat = lowLimit;
    }

    _inverse = invertLimits;
  } // constructor

private:
  FlightSimFloat _drFloat;
  double _lowLimitFloat;
  double _highLimitFloat;
  bool _inverse;

  void _updateActive() {
    _active = (   _lowLimitFloat <= _drFloat
                 && _drFloat <= _highLimitFloat);
    if (_inverse == true) {
      _active = _active? false: true;
    }
  }

};

class SimLEDLocal : public SimLEDBase {
public:
  SimLEDLocal(const int  &ledPin,
              const bool &enableTest = true,
              const bool *hasPowerFlag = &SimObject::hasPower)
    : SimLEDBase(ledPin, enableTest, hasPowerFlag)
  {
  _active = false;
  }

  void setActive(bool active) { _active = active; }

private:
  void _updateActive() {}

};

#endif // SIMLEDDEV_H
