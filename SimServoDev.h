// SimServo Development Version

// Copyright 2012 Jack Deeth

// The following lines are for the Doxygen automatic documentation tool.

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

#ifndef SIMSERVODEV_H
#define SIMSERVODEV_H


#include "SimObjectsDev.h"



//! Input/output pairs for conversion
typedef const double ScaleMap [][2];

class SimServo : protected SimObject {
public:
  //! Constructor for when we need to convert the dataref into an angle.
  /*! The SimServo will use a ScaleMap to convert the input dataref into
   *  an angle, apply power-supply simulation, and if the connection to
   *  X-Plane is active, move the servo.
   *  \param pin Arduino pin linked to servo signal pin
   *  \param ident DataRefIdent identifier of input dataref
   *  \param map At least two pairs of doubles, defined using ScaleMap,
   *  showing relationship between input and outputs.
   *  \param sizeof_map IMPORTANT: this MUST be sizeof(foo), where foo
   *  is the name of your ScaleMap. For example, SimServo myServo(5,
   *  myIdent, myScaleMap, sizeof(myScaleMap);
   *  \param restAngle Position servo will come to rest in when it lacks
   *  simulated power. Set to -1 to have servo remain in position when
   *  power is removed.
   *  \param hasPowerFlag Pointer to boolean value that represents
   *  simulated power supply. Default is SimServo::hasPower static
   *  boolean. Set to 0 for servo to be always powered. (It will still
   *  be stopped by calling SimServo::update(false).)
   */
  SimServo (const unsigned short &pin,
            const char * ident,
            ScaleMap map,
            const size_t &sizeof_map,
            const int restAngle = -1,
            const bool *hasPowerFlag = &SimServo::hasPower
            ) :
    _pin(pin),
    _powerSource(hasPowerFlag)
  {
    _dr.assign((const _XpRefStr_ *) &ident[0]);
    _map = map;
    _mapPair =  sizeof_map / (2*sizeof(double));

    _mapValid = _validateMap();

    if(_mapValid)
      _addToLinkedList();

    if (hasPowerFlag == 0)
      _needsPower = false;
    else
      _needsPower = true;

    _restAngle = restAngle;
  }

  //! Constructor where the dataref is already a suitable angle
  /*! This will be used when plugins will do the data-to-servo-angle
   *  mapping on the PC, avoiding the need to alter the Teensy's source
   *  code to configure the servo. This is just a prototype.
   *  \todo Implementation of precomputed-dataref SimServo
   */
  SimServo (const int &pin,
            const char * angleDRIdent);

  //! Returns stored input value
  double getInput(void) { return _in; }

  //! Returns stored output value.
  /*! This value is converted from _in via _map but does not have any
   *  power-simulation effects added.
   */
  double getAngle(void) { return _out; }

  //! Returns computed servo angle
  /*! This is the integer value fed to the servo through the Arduino
   *  Servo object. It takes power-simulation effects into account.
   */
  int getServoAngle(void) {return _servoAngle; }

//  //! Static function to initialise all SimServos
//  static void setup(void);

//  //! Static function to update all SimLEDs
//  static void update (bool updateOutput = true);

  //! Default simulated power source for powered servos
  static bool hasPower;

private:

  //! Position the needle will rest in if simulated power is unavailable
  /*! Units are servo degrees (integer between 0 and 180). If set to -1,
   *  the servo will just stop moving when power is removed.
   */
  int _restAngle;

  //! Pointer to power source.
  /*! If _needsPower is set, this will be checked during update.*/
  const bool* _powerSource;

  //! Specifies if this SimServo needs simulated power available to move.
  /*! If false, servo will either not move, or will move to defined
   *  resting position, if power is not available.
   */
  bool _needsPower;

  //! To clarify accessing indexes of _map.
  enum ScaleMapIndex {
    In,
    Out
  };

  //! Input value
  double _in;

  //! Result of passing _in through _map
  double _out;

  //! _out with power simulation effects added, and converted to integer
  int _servoAngle;

  //! Check ScaleMap input to see that input values are in increasing order
  bool _validateMap(void);

  //! If false, no _setup or _update occurs. Stores result of _validateMap().
  bool _mapValid;

  //! Number of Arduino pin connected to servo.
  const unsigned short _pin;

  //! Input dataref
  FlightSimFloat _dr;

  //! Pointer to input-to-output conversion map
  const double (*_map)[2];

  //! Number of input/output pairs in _map.
  /*! Reliant on unmangled (map, sizeof(map)) arguments in constructor.*/
  unsigned int _mapPair;

  //! Ordinary Arduino Servo object, which actually moves the servo
  Servo _servo;

  //! Run setup routines on this class instance.
  /*! This only consists of attaching the pin to the integrated Servo
   *  object. */
  void _setup  (void) { _servo.attach(_pin); }

  //! Run update routines on this class instance.
  void _update (bool updateOutput = true);

  // linked list:

//  //! Add this element to linked list of SimServos
//  /*! Note, this is not called when the ScaleMap input is invalid. */
//  void _addToLinkedList(void);

//  //! Pointer to first instance of class in linked list
//  static SimServo* _first;

//  //! Pointer to next instance in linked list. ==0 if we are last element
//  SimServo* _next;

};



//! Check we have at least two pairs and inputs are in increasing order
bool SimServo::_validateMap(void) {

  if (_mapPair < 2) {
    return false;
  }

  // each input must be greater than the previous input
  for(unsigned int i = 1; i < _mapPair; ++i) {
    if (_map[i][In] < _map[i-1][In]) {
      return false;
    }
  }

  return true;
}



//! Convert input to output via map, and write new servo-angle to servo
void SimServo::_update(bool updateOutput) {

  // assign dataref to stored 'input' value
  _in = _dr;

  // if input off map, put output on edge of map
  if (_in <= _map[0][In]) {
    _out = _map[0][Out];
  }

  if (_in >= _map[_mapPair-1][In]) {
    _out = _map[_mapPair-1][Out];
  }

  // if input within map, interpolate output
  for (int i = 1; i < _mapPair; ++i) {
    if (_in < _map[i][In]) {

      double buf = _in;
      buf -= _map[i-1][In];
      buf /= _map[i][In] - _map[i-1][In];
      buf *= _map[i][Out] - _map[i-1][Out];
      buf += _map[i-1][Out];
      _out = buf;

      // exit for loop
      i = _mapPair;
    }
  }

  // if we have power, or don't need power
  if(*_powerSource || !_needsPower) {
    // convert double to int to give to RC servo
    _servoAngle = (int)(_out + 0.5);
  } else {
    // move to resting position if defined
    if (_restAngle > -1) {
      _servoAngle = _restAngle;
    }
    // otherwise servoAngle does not change
  }

  // use updateOutput as a final gate to write to servo
  if (updateOutput && FlightSim.isEnabled()) {
    _servo.write(_servoAngle);
  }

  return;
}



////! add this instance to a linked list of all SimServos
//void SimServo::_addToLinkedList(void) {

//  _next = 0;

//  if (_first == 0) {
//    _first = this;
//  } else {
//    // Go through linked list and make last existing element point to us
//    SimServo *a = _first;
//    while (a->_next)
//      a = a->_next;
//    a->_next = this;
//  }
//}



////! set up all instances of SimServo
//void SimServo::setup(void) {
//  if (_first != 0) {
//    SimServo* buf = _first;
//    while (buf != 0) {
//      buf->_setup();
//      buf = buf->_next;
//    }
//  }
//}



////! update all instances of SimServo.
///*! \param updateOutput If false, only updates SimServo state internally
//  *        and does not push new value to output.
//  */
//void SimServo::update( bool updateOutput) {
//  if (_first != 0) {
//    SimServo* buf = _first;
//    while (buf != 0) {
//      buf->_update(updateOutput);
//      buf = buf->_next;
//    }
//  }
//}



// Initialise static data members
//SimServo* SimServo::_first  = 0;
bool SimServo::hasPower     = true;



#endif // SIMSERVODEV_H
