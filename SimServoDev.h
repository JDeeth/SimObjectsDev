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
#include <Servo.h>

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

    _addToLinkedList();
  }

  //! Constructor where dataref is already the servo angle
  /*! This will be used when plugins will do the data-to-servo-angle
   *  mapping on the PC, allowing easier configuration for the end-user.
   */
  SimServo (const int &pin,
            const char * angleDRIdent);

  //! Static function to initialise all SimServos
  static void setup(void);

  //! Static function to update all SimLEDs
  static void update (bool updateOutput = true);

  //! Sets state of simulated power availability
  static void isPowered(bool hasPower = true){ _hasPower = hasPower; }

private:
  //! Arduino pin number of servo.
  const unsigned short _pin;

  //! Input dataref
  FlightSimFloat _dr;

  //! Pointer to dataref-to-angle map
  const double (*_map)[2];
  int _mapPair;

  bool _hasServo;
  Servo _servo;

  //! if true, will only move servo if isPowered was called with 'true'.
  static bool _hasPower;

  void _addToLinkedList(void);
  void _setup  (void) { _servo.attach(_pin); }
  void _update (bool updateOutput = true) {;}

  //! Number of this class created
  static int _count;

  //! Pointer to first instance of class in linked list
  static SimServo* _first;

  //! Pointer to next instance in linked list. ==0 if we are last element
  SimServo* _next;

};



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



void SimServo::setup() {
  if (_first != 0) {
    SimServo* buf = _first;
    while (buf != 0) {
      buf->_setup();
      buf = buf->_next;
    }
  }
}



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


//SimLED::SimLED(const int  &ledPin,
//           const char *ident,
//           const int  &lowLimit,
//           const int  &highLimit,
//           const bool &invertLimits,
//           const bool &enableTest)  :
//_pin(ledPin),
//lowLimitI_(lowLimit),
//highLimitI_(highLimit),
//inverse_(invertLimits),
//allowTest_(enableTest)
//{
//type = SLInt;
//_drI.assign((const _XpRefStr_ *) &ident[0]);
//addToLinkedList();
//}

//SimLED::SimLED(const int    &ledPin,
//           const char   *ident,
//           const double &lowLimit,
//           const double &highLimit,
//           const bool   &invertLimits,
//           const bool   &enableTest)  :
//_pin(ledPin),
//lowLimitF_(lowLimit),
//highLimitF_(highLimit),
//inverse_(invertLimits),
//allowTest_(enableTest)
//{
//type = SLFloat;
//_drF.assign((const _XpRefStr_ *) &ident[0]);
//addToLinkedList();
//}

//void SimLED::addToLinkedList() {
//next_ = 0;

//if (count_++ == 0) {
//    first_ = this;
//} else {
//    // Go through linked list and make last existing element point to us
//    SimLED *a = first_;
//    while (a->next_)
//        a = a->next_;
//    a->next_ = this;
//}
//}


//// Determine whether this SimLED should be lit
//void SimLED::update_(bool updateOutput) {

//switch(type) {
//case SLInt:
//    active_ = (lowLimitI_ <= _drI && _drI <= highLimitI_);
//    break;
//case SLFloat:
//    active_ = (lowLimitF_ <= _drF && _drF <= highLimitF_);
//    break;
//}

//if(inverse_ == true)
//    active_ = !active_;

//lit_ = active_;

//// apply lit_ filters:

//// we are lit if bulb-test is active
//if( (allowTest_ == true) && (testAll_ == true) )
//    lit_ = true;

//// we are not lit if the sim isn't running or no simulated power
//if( (FlightSim.isEnabled() == false) || (hasPower_ == false) ) {
//    lit_ = false;
//}

//// unless ordered otherwise, light or extinguish LED based on our lighting state
//if (updateOutput)
//    digitalWrite(_pin, lit_);
//}


//// Initialise static data members
//int SimLED::count_      = 0;
//SimLED* SimLED::first_  = 0;
//bool SimLED::hasPower_  = true;
//bool SimLED::testAll_   = false;

