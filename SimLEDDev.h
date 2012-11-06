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

// To make it easier to record the dataref identifier
#define DatarefIdent PROGMEM const char

class SimLED {
public:
    SimLED(const int    &ledPin,
           const char * ident,
           const int    &lowLimit     = 1,
           const int    &highLimit    = 32000,
           const bool   &invertLimits = false,
           const bool   &enableTest   = true);

    static void setup(void);
    static void update (bool updateOutput = true);

    // A SimLED isActive when the state of the datarefs would cause it
    // to light. isLit is isActive with filters applied, to override the
    // LED state for the following functions:
    //
    // - Light test
    // - Simulator state (LEDs extinguish when sim is inactive)
    bool isActive(void) { return active_; }
    bool isLit(void)    { return lit_ ; }

    // while hasPower is false, all SimLEDs are extinguished. Represents
    // simulated power from X-Plane, not physical electrical power on
    // the USB supply!
    static void isPowered(bool hasPower = true){ hasPower_ = hasPower; }

    // calling with 'true' will cause enabled SimLEDs to light up.
    static void lightTest(bool lightAll)  {testAll_ = lightAll;}

    // Enables or disables this SimLED's participation in lightTests
    void enableTest (bool allowTest)      {allowTest_ = allowTest;}

private:
    // Hardware pin of output LED
    int _pin;

    // Annc is active when dataref is between Low and High limits,
    // unless it is Inverse. Inverse anncs are active when dataref is
    // NOT between Low and High limits. Range is inclusive.
    bool inverse_;

    FlightSimInteger _drI;
    int lowLimitI_;
    int highLimitI_;

    // input data is in range to activate this LED.
    bool active_;

    // Per annc system simulation, this LED should be lit.
    bool lit_;

    void addToLinkedList(void);
    void setup_ (void) {pinMode(_pin, OUTPUT);}
    void update_(bool updateOutput = true);

    bool allowTest_;

    static bool hasPower_;
    static bool testAll_;

    // Number of SimLEDs created
    static int count_;
    // Pointer to first SimLED in linked list
    static SimLED* first_;
    // Pointer to next SimLED in linked list. ==0 if we are last element
    SimLED* next_;
};

////////////////////////////////////////////////////////////////////////

/* Constructors
// if enableTest is True, the LED will be lit by the lightTest
// function.
//
// Create SimLED
// lowLimit and highLimit define the range where the SimLED is
// ACTIVE.
//
// example: SimLED takeoffTrim(10, trimIdent, 0.10, 0.15);
// active when trim is between 0.10 and 0.15
//
// If invertLimits is True, this is reversed, and they define the
// range where the annunciator is INACTIVE.
//
// example: SimLED balanceWarn(10, cofgIdent, 0.20, -0.10);
// balanceWarn will be inactive when the dataref is less than 0.20
// and greater than -0.10 inclusive. Therefore it will be Active
// outside of that range.
//
// Limit values are inclusive.
*/
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

// Set up all SimLED objects. This simply calls setup_ on each
// instance of SimLED, setting the pinMode to OUTPUT.
//
// Call this in the Arduino setup() function.
void SimLED::setup() {
    if (count_ > 0) {
        SimLED* buf = first_;
        while (buf != 0) {
            buf->setup_();
            buf = buf->next_;
        }
    }
}

// Update all SimLED objects. This lights them if their dataref is
// in range or the test function is active, and the system has
// power.
//
// Call this in the Arduino loop() function.
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
