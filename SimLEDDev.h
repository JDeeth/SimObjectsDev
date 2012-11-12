
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



//! High-level dataref-to-LED linking class
/*! Incorporating bulb-test and power-available features.*/
class SimLED : public SimObject {
public:
  //! Integer constructor
  /*! Incorporating upper and lower limits.
      \param ledPin The Arduino pin number of the LED.
      \param ident The dataref ident. MUST point to DatarefIdent string!
      \param lowLimit The lower limit (inclusive) of the range of values
      which cause this LED to light.
      \param highLimit The upper limit (inclusive) of the range of
      values which causes this LED to light.
      \param invertLimits Inverts the active state of the LED. If true,
      lowLimit and highLimit specify the range where the LED is off.
      \param enableTest Allows this LED to participate in bulb tests.
      */
  SimLED(const int    &ledPin,
         const char * ident,
         const int    &lowLimit     = 1,
         const int    &highLimit    = 32000,
         const bool   &invertLimits = false,
         const bool   &enableTest   = true);

  //! Floating-point constructor
  /*! Incorporating upper and lower limits.
      \param ledPin The Arduino pin number of the LED.
      \param ident The dataref ident. MUST point to DatarefIdent string!
      \param lowLimit The lower limit (inclusive) of the range of values
      which cause this LED to light.
      \param highLimit The upper limit (inclusive) of the range of
      values which causes this LED to light.
      \param invertLimits Inverts the active state of the LED. If true,
      lowLimit and highLimit specify the range where the LED is off.
      \param enableTest Allows this LED to participate in bulb tests.
      */
  SimLED(const int    &ledPin,
         const char * ident,
         const double &lowLimit,
         const double &highLimit,
         const bool   &invertLimits = false,
         const bool   &enableTest   = true);

//  //! Static function to initialise all SimLEDs
//  static void setup(void);

  //! Static function to update all SimLEDs
  /*!
   *  \param updateOutput If false, will update the state of each SimLED
   * but not change the lit state.
   */
  //static void update (bool updateOutput = true);

  /// True if input conditions would cause this LED to light
  bool isActive(void) { return _active; }

  /// True if this LED should light, applying filters (power, test etc)
  bool isLit(void)    { return _lit ; }

  /// Sets state of simulated power availability
  static void isPowered(bool hasPower = true){ _hasPower = hasPower; }

  /// Enable/disable bulb test mode
  static void lightTest(bool lightAll)  {_testAll = lightAll;}

  /// Enable/disable this SimLED's participation in lightTests
  void enableTest (bool allowTest)      {_allowTest = allowTest;}

private:
  /// Possible types of SimLED
  enum SimLEDType {
    SLUndefined,
    SLInt,
    SLFloat
  };

  /// Type of this SimLED instance.
  SimLEDType type;

  /// Arduino pin number of LED.
  int _pin;

  /// Inverts active state if true.
  bool _inverse;

  FlightSimInteger _drI;
  int _lowLimitI;
  int _highLimitI;

  FlightSimFloat _drF;
  double _lowLimitF;
  double _highLimitF;

  bool _active;
  bool _lit;

  //void _addToLinkedList(void);
  void _setup (void) {pinMode(_pin, OUTPUT);}
  void _update(bool updateOutput = true);

  bool _allowTest;

  static bool _hasPower;
  static bool _testAll;

  ///// Pointer to first SimLED in linked list
  //static SimLED* _first;

  ///// Pointer to next SimLED in linked list. ==0 if we are last element
  //SimLED* _next;
};

////////////////////////////////////////////////////////////////////////

SimLED::SimLED(const int  &ledPin,
               const char *ident,
               const int  &lowLimit,
               const int  &highLimit,
               const bool &invertLimits,
               const bool &enableTest)  :
  _pin(ledPin),
  _lowLimitI(lowLimit),
  _highLimitI(highLimit),
  _inverse(invertLimits),
  _allowTest(enableTest)
{
  type = SLInt;
  _drI.assign((const _XpRefStr_ *) &ident[0]);
  _addToLinkedList();
}

SimLED::SimLED(const int    &ledPin,
               const char   *ident,
               const double &lowLimit,
               const double &highLimit,
               const bool   &invertLimits,
               const bool   &enableTest)  :
  _pin(ledPin),
  _lowLimitF(lowLimit),
  _highLimitF(highLimit),
  _inverse(invertLimits),
  _allowTest(enableTest)
{
  type = SLFloat;
  _drF.assign((const _XpRefStr_ *) &ident[0]);
  _addToLinkedList();
}



// Determine whether this SimLED should be lit
void SimLED::_update(bool updateOutput) {

  switch(type) {
    case SLInt:
      _active = (_lowLimitI <= _drI && _drI <= _highLimitI);
      break;
    case SLFloat:
      _active = (_lowLimitF <= _drF && _drF <= _highLimitF);
      break;
  }

  if(_inverse == true)
    _active = !_active;

  _lit = _active;

  // apply lit_ filters:

  // we are lit if bulb-test is active
  if( (_allowTest == true) && (_testAll == true) )
    _lit = true;

  // we are not lit if the sim isn't running or no simulated power
  if( (FlightSim.isEnabled() == false) || (_hasPower == false) ) {
    _lit = false;
  }

  // unless ordered otherwise, light or extinguish LED based on our lighting state
  if (updateOutput)
    digitalWrite(_pin, _lit);
}


// Initialise static data members
bool SimLED::_hasPower  = true;
bool SimLED::_testAll   = false;

#endif // SIMLEDDEV_H
