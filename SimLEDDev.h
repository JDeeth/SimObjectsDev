////////////////////////////////////////////////////////////////////////
//
// SimLED Development Version
//
// Copyright 2012 Jack Deeth
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//
// I would appreciate, but not insist, on attribution if this code is
// incorporated into other projects.
//
////////////////////////////////////////////////////////////////////////

#ifndef SIMLEDDEV_H
#define SIMLEDDEV_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#elif defined(WIRING)
#include "Wiring.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

/// Aid for recording the dataref identifier
#define DatarefIdent PROGMEM const char

//! High-level dataref-to-LED linking class
/*! High-level dataref-to-LED linking class incorporating bulb-test and
    power-available features.*/
class SimLED {
public:
    //! Integer constructor
    /*! Integer constructor incorporating upper and lower limits.
      \param ledPin the Arduino pin number of the LED.
      \param ident the dataref ident. MUST point to DatarefIdent string!
      \param lowLimit the lower limit (inclusive) of the range of values
      which cause this LED to light.
      \param highLimit the upper limit (inclusive) of the range of
      values which causes this LED to light.
      \param invertLimits inverts the active state of the LED. If true,
      lowLimit and highLimit specify the range where the LED is off.
      \param enableTest allows this LED to participate in bulb tests.
      */
    SimLED(const int    &ledPin,
           const char * ident,
           const int    &lowLimit     = 1,
           const int    &highLimit    = 32000,
           const bool   &invertLimits = false,
           const bool   &enableTest   = true);

    //! Static function to initialise all SimLEDs
    static void setup(void);

    //! Static function to update all SimLEDs
    /*! Static function to update all SimLEDs.
      \param updateOutput if false, will update the state of each SimLED
      but not change the lit state.
      */
    static void update (bool updateOutput = true);

    // A SimLED isActive when the state of the datarefs would cause it
    // to light. isLit is isActive with filters applied, to override the
    // LED state for the following functions:
    //
    // - Light test
    // - Simulator state (LEDs extinguish when sim is inactive)
    /// True if input conditions would cause this LED to light
    bool isActive(void) { return active_; }

    /// True if this LED should light, applying filters (power, test etc)
    bool isLit(void)    { return lit_ ; }

    /// Sets state of simulated power availability
    static void isPowered(bool hasPower = true){ hasPower_ = hasPower; }

    /// Enable/disable bulb test mode
    static void lightTest(bool lightAll)  {testAll_ = lightAll;}

    /// Enable/disable this SimLED's participation in lightTests
    void enableTest (bool allowTest)      {allowTest_ = allowTest;}

private:
    /// Arduino pin number of LED
    int _pin;

    bool inverse_;

    FlightSimInteger _drI;
    int lowLimitI_;
    int highLimitI_;

    bool active_;
    bool lit_;

    void addToLinkedList(void);
    void setup_ (void) {pinMode(_pin, OUTPUT);}
    void update_(bool updateOutput = true);

    bool allowTest_;

    static bool hasPower_;
    static bool testAll_;

    /// Number of SimLEDs created
    static int count_;

    /// Pointer to first SimLED in linked list
    static SimLED* first_;

    /// Pointer to next SimLED in linked list. ==0 if we are last element
    SimLED* next_;
};

////////////////////////////////////////////////////////////////////////

SimLED::SimLED(const int  &ledPin,
               const char *ident,
               const int  &lowLimit,
               const int  &highLimit,
               const bool &invertLimits,
               const bool &enableTest)  :
    _pin(ledPin),
    lowLimitI_(lowLimit),
    highLimitI_(highLimit),
    inverse_(invertLimits),
    allowTest_(enableTest)
{
    _drI.assign((const _XpRefStr_ *) &ident[0]);
    addToLinkedList();
}

void SimLED::addToLinkedList() {
    next_ = 0;

    if (count_++ == 0) {
        first_ = this;
    } else {
        // Go through linked list and make last existing element point to us
        SimLED *a = first_;
        while (a->next_)
            a = a->next_;
        a->next_ = this;
    }
}

void SimLED::setup() {
    if (count_ > 0) {
        SimLED* buf = first_;
        while (buf != 0) {
            buf->setup_();
            buf = buf->next_;
        }
    }
}

void SimLED::update( bool updateOutput) {

    if (count_ > 0) {
        SimLED* buf = first_;
        while (buf != 0) {
            buf->update_(updateOutput);
            buf = buf->next_;
        }
    }
}

// Determine whether this SimLED should be lit
void SimLED::update_(bool updateOutput) {

    active_ = (lowLimitI_ <= _drI && _drI <= highLimitI_);

    if(inverse_ == true)
        active_ = !active_;

    lit_ = active_;

    // apply lit_ filters:

    // we are lit if bulb-test is active
    if( (allowTest_ == true) && (testAll_ == true) )
        lit_ = true;

    // we are not lit if the sim isn't running or no simulated power
    if( (FlightSim.isEnabled() == false) || (hasPower_ == false) ) {
        lit_ = false;
    }

    // unless ordered otherwise, light or extinguish LED based on our lighting state
    if (updateOutput)
        digitalWrite(_pin, lit_);
}


// Initialise static data members
int SimLED::count_      = 0;
SimLED* SimLED::first_  = 0;
bool SimLED::hasPower_  = true;
bool SimLED::testAll_   = false;

#endif // SIMLEDDEV_H
