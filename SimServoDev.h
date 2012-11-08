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

class SimServo {
public:
  //! Constructor where we convert dataref to angle in this class
  SimServo (const unsigned short &pin,
            const char * ident,
            ScaleMap map,
            const size_t &sizeof_map) :
    _pin(pin)
  {
    _dr.assign((const _XpRefStr_ *) &ident[0]);
    _map = map;
    _mapPair =  sizeof_map / (2*sizeof(double));

    _mapValid = _validateMap();

    if(_mapValid)
      _addToLinkedList();

  }

  //! Constructor where dataref is already the servo angle
  /*! This will be used when plugins will do the data-to-servo-angle
   *  mapping on the PC, allowing easier configuration for the end-user.
   */
  SimServo (const int &pin,
            const char * angleDRIdent);

  //! debugging function to validate ScaleMap input
  bool inputValid() { return _mapValid; }

  // debug
  int getMapPair() { return _mapPair; }

  double getAngle() { return _out; }
  double getInput() { return _in; }

  //! Static function to initialise all SimServos
  static void setup(void);

  //! Static function to update all SimLEDs
  static void update (bool updateOutput = true);

  //! Sets state of simulated power availability
  static void isPowered(bool hasPower = true){ _hasPower = hasPower; }

private:

  enum ScaleMapIndex {
    In,
    Out
  };

  //! Input value
  double _in;

  //! Computed angle to send to servo
  double _out;

  //! Check ScaleMap input to see that input values are in increasing order
  bool _validateMap();

  //! If false, no _setup or _update occurs. Stores result of _validateMap().
  bool _mapValid;

  //! Arduino pin number of servo.
  const unsigned short _pin;

  //! Input dataref
  FlightSimFloat _dr;

  //! Pointer to dataref-to-angle map
  const double (*_map)[2];
  unsigned int _mapPair;

  bool _hasServo;
  Servo _servo;

  //! if true, will only move servo if isPowered was called with 'true'.
  static bool _hasPower;

  void _addToLinkedList(void);
  void _setup  (void) { _servo.attach(_pin); }
  void _update (bool updateOutput = true);

  //! Number of this class created
  static int _count;

  //! Pointer to first instance of class in linked list
  static SimServo* _first;

  //! Pointer to next instance in linked list. ==0 if we are last element
  SimServo* _next;

};



bool SimServo::_validateMap() {
  bool ok;

  switch (_mapPair) {
    case 0:
      ok = false;
      break;
    case 1:
      ok = true;
      break;
    default:
      ok = true;
      for(unsigned int i = 1; (i < _mapPair) && ok; ++i) {
        if (_map[i][In] < _map[i-1][In])
          ok = false;
      } // for each pair
      break;
  } //switch
  return ok;
}


void SimServo::_update(bool updateOutput) {

  _in = _dr;

  if (_in <= _map[0][In]) {
    _out = _map[0][Out];
  }

  if (_in >= _map[_mapPair-1][In]) {
    _out = _map[_mapPair-1][Out];
  }

  for (int i = 1; i < _mapPair; ++i) {
    if (_in < _map[i][In]) {

      double zzbuf = _in;
      zzbuf -= _map[i-1][In];
      zzbuf /= _map[i][In] - _map[i-1][In];
      zzbuf *= _map[i][Out] - _map[i-1][Out];
      zzbuf += _map[i-1][Out];
      _out = zzbuf;

      // exit for loop
      i = _mapPair;
    }
  }

  if (updateOutput) {
    _servo.write(_out);
  }

  return;
}



//! add this instance to a linked list of all SimServos
void SimServo::_addToLinkedList() {

  _next = 0;

  if (_first == 0) {
    _first = this;
  } else {
    // Go through linked list and make last existing element point to us
    SimServo *a = _first;
    while (a->_next)
      a = a->_next;
    a->_next = this;
  }
}



//! set up all instances of SimServo
void SimServo::setup() {
  if (_first != 0) {
    SimServo* buf = _first;
    while (buf != 0) {
      buf->_setup();
      buf = buf->_next;
    }
  }
}



//! update all instances of SimServo.
/*! \param updateOutput If false, only updates SimServo state internally
  *        and does not push new value to output.
  */
void SimServo::update( bool updateOutput) {
  if (_first != 0) {
    SimServo* buf = _first;
    while (buf != 0) {
      buf->_update(updateOutput);
      buf = buf->_next;
    }
  }
}



// Initialise static data members
int SimServo::_count        = 0;
SimServo* SimServo::_first  = 0;



#endif // SIMSERVODEV_H
