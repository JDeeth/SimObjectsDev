///////////////////////////////////////////////////////////////////////////////
//
// SimLED v1.00
//
// Copyright 2012 Jack Deeth
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// I would appreciate, but not insist, on attribution if this code is
// incorporated into other projects.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef SIMLED_H
#define SIMLED_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#elif defined(WIRING)
#include "Wiring.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif

enum SimLedType {
  SLUndefined,
  SLMaster,
  SLIntAnnc,
  SLFloatAnnc
};

class SimLED {
public:
  // Constructors
  // if enableTest is True, the LED will be lit by the lightTest function.
  // if enableFlash is True, the flash filter will apply to this SimLED.
  
  // Create Master SimLED
  // It will be lit by annunciator LEDs created AFTER it, until another
  // Master SimLED is defined or the last SimLED is declared.
  //
  // The compiler will regard SimLED(pinNo, false) as SimLED(int, int)
  // and complain of ambiguity between Master and Int Annc constructors. 
  // In this case, cast to boolean: 
  // SimLED(pinNo, (bool) false);
  //
  // Annunciators created before the first Master SimLED
  SimLED(const int    &masterLedPin,
         const bool   &enableTest = true,
         const bool   &enableFlash = true);
  
  // Create Annunciator SimLEDs
  // lowLimit and highLimit define the range where the SimLED is ACTIVE.
  //
  // example: SimLED takeoffTrim(10, &elevTrimDR, 0.10, 0.15);
  // active when trim is between 0.10 and 0.15
  //
  // If invertLimits is True, this is reversed, and they define the range where
  // the annunciator is INACTIVE.
  //
  // example: SimLED balanceWarn(10, &cOfGPosition, 0.20, -0.10);
  // balanceWarn will be inactive when the dataref is less than 0.20 and
  // greater than -0.10 inclusive. Therefore it will be Active outside of that
  // range.
  //
  // Integer annunciator limit values are inclusive.
  //
  SimLED(const int    &ledPin,
         FlightSimInteger *dr,
         const int    &lowLimit     = 1,
         const int    &highLimit    = 32000,
         const bool   &invertLimits = false,
         const bool   &enableTest   = true,
         const bool   &enableFlash  = false);
  
  SimLED(const int    &ledPin,
         FlightSimFloat *dr,
         const float  &lowLimit,
         const float  &highLimit,
         const bool   &invertLimits = false,
         const bool   &enableTest   = true,
         const bool   &enableFlash  = false);
  
  // Set up all SimLED objects. This simply calls setup_ on each instance of
  // SimLED, setting the pinMode to OUTPUT.
  //
  // Call this in the Arduino setup() function.
  static void setup(void);
  
  // Update all SimLED objects. This takes input from the datarefs, applies
  // annunciator system logic to determine which are Active, and then applies
  // other filters to determine which are Lit. If updateOutput is True, then
  // each LED pin is set High or Low as well.
  //
  // Call this in the Arduino loop() function.
  static void update (bool updateOutput = true);
  
  // A SimLED isActive when the state of the datarefs and annunciators would
  // cause it to light. isLit is isActive with filters applied, to override
  // the LED state for the following functions:
  // - Light test
  // - Light flasher
  // - Simulator state (LEDs extinguish when sim is inactive)
  //
  // For example, a flashing master warning SimLED is continually Active,
  // but alternately Lit and not Lit. A SimLED illuminated by a test is
  // Lit but not Active.
  //
  bool isActive(void) { return active_; }
  bool isLit(void)    { return lit_ ; }
  
  // Acknowledges currently lit Annunciator SimLEDs belonging to this Master
  // SimLED and extinguishes the Master SimLED.
  //
  // masterReset has no effect when called on SimLEDs that aren't Master SimLEDs.
  //
  void masterReset(void);
  
  // Operate the flashing sequence for flash-enabled SimLEDs.
  // When called with True, all flash-enabled SimLEDs will light when Active.
  // When called with False, they will be extinguished.
  static void flash (bool litNow)       { flashNow_ = litNow; }
  
  // Sets whether this SimLED will react to the flash filter
  void enableFlash (bool allowFlash)    { allowFlash_ = allowFlash; }
  
  // While called with 'true', will override enabled SimLEDs to light up.
  static void lightTest(bool lightAll)  {testAll_ = lightAll;}
  
  // Enables or disables this SimLED's participation in lightTests
  void enableTest (bool allowTest)      {allowTest_ = allowTest;}
  
private:
  // Hardware pin of output LED
  int pin_;
  
  // Defines this as Master, Int Annc., or Float Annc.
  SimLedType type_;
  
  // Annc is active when dataref is between Low and High limits, unless
  // it is Inverse. Inverse anncs are active when dataref is NOT between
  // Low and High limits. Range is inclusive.
  bool inverse_; 
  
  FlightSimInteger * drInt_;
  int lowLimitI_;     
  int highLimitI_;    
  
  FlightSimFloat * drFloat_;
  float lowLimitF_;
  float highLimitF_;
  
  // Annc input data is in range to activate this annc.
  bool active_; 
  
  // Per annc system simulation, this annc LED should be lit.
  bool lit_;    
  
  // This Active_ annc has been Ack_nowledged by Master Reset key
  bool ack_;    
  
  void addToLinkedList(void);
  void setup_ (void) {pinMode(pin_, OUTPUT);}
  void update_(bool updateOutput = true);
  
  bool allowTest_;
  bool allowFlash_;
  
  static bool flashNow_;
  static bool testAll_;
  
  // Number of SimLEDs created
  static int count_;
  // Pointer to first SimLED in linked list
  static SimLED* first_;
  // Pointer to next SimLED in linked list. == 0 if this is last element
  SimLED* next_;
};

//////////
// Constructors
//
SimLED::SimLED(const int  &masterLedPin,
               const bool &enableTest,
               const bool &enableFlash) {
  pin_        = masterLedPin;
  type_       = SLMaster;
  allowTest_  = enableTest;
  allowFlash_ = enableFlash;
  addToLinkedList();
}

SimLED::SimLED(const int  &ledPin,
               FlightSimInteger *dr,
               const int  &lowLimit,
               const int  &highLimit,
               const bool &invertLimits,
               const bool &enableTest,
               const bool &enableFlash) {
  pin_          = ledPin;
  type_         = SLIntAnnc;
  drInt_        = dr;
  lowLimitI_    = lowLimit;
  highLimitI_   = highLimit;
  inverse_      = invertLimits;
  allowTest_    = enableTest;
  allowFlash_   = enableFlash;
  addToLinkedList();
}

SimLED::SimLED(const int    &ledPin,
               FlightSimFloat *dr,
               const float  &lowLimit,
               const float  &highLimit,
               const bool   &invertLimits,
               const bool   &enableTest,
               const bool   &enableFlash) {
  pin_          = ledPin;
  type_         = SLFloatAnnc;
  drFloat_      = dr;
  lowLimitF_    = lowLimit;
  highLimitF_   = highLimit;
  inverse_      = invertLimits;
  allowTest_    = enableTest;
  allowFlash_   = enableFlash;
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

// Run setup_() function on each instance of SimLED
void SimLED::setup() {
  if (count_ > 0) {
    SimLED* buf = first_;
    while (buf != 0) {
      buf->setup_();
      buf = buf->next_;
    }
  }
}

// Run update()_ on each instance of SimLED
// if updateOutput, set LED pins according to lit_ status
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
  switch (type_) {
    case SLMaster: {
        // If we set active_ to false here, Master SimLEDs will extinguish
        // when all their annunciators are out. Without this line, they stay
        // lit until the Reset button is pressed.
        
        // active_ = false;
        
        SimLED *buf = next_;
        while ( (buf->type_ != SLMaster) && (buf != 0) ) {
          // Step through anncs until we reach the end, or the next Master
          // Activate this Master if any unAck'd Active anncs are found
          active_ += (buf->active_ && !buf->ack_);
          // reset acknowledgements for inactive anncs
          if (!buf->active_)
            buf->ack_ = false;
          buf = buf->next_;
        }
      }
      break;
      // for regular anncs, we are active if our dataref is within the limits.
    case SLIntAnnc:
      active_ = (lowLimitI_ <= *drInt_ && *drInt_ <= highLimitI_);
      if(inverse_ == true)
        active_ = !active_;
      break;
    case SLFloatAnnc:
      active_ = (lowLimitF_ <= *drFloat_&& *drFloat_ <= highLimitF_);
      if(inverse_ == true)
        active_ = !active_;
      break;
    default:
      return;
  }
  
  lit_ = active_;
  
  // apply lit_ filters:
  
  // we are lit if bulb-test is active
  if( (allowTest_ == true) && (testAll_ == true) )
    lit_ = true;
  
  // we are not lit if we're a flashing light and the flash timers says not to
  if ( (allowFlash_ == true) && (flashNow_ == false) )
    lit_ = false;
  
  // we are not lit if the sim isn't running
  if(FlightSim.isEnabled() == false) {
    lit_ = false;
  }
  
  // unless ordered otherwise, light or extinguish LED based on our lighting state
  if (updateOutput)
    digitalWrite(pin_, lit_);
}

void SimLED::masterReset() {
  // do nothing if this isn't a Master
  if(type_ != SLMaster)
    return;
  
  // Acknowledge all our Active anncs, which are the subsequent anncs in
  // the list, until we reach another Master or the end.
  SimLED *buf = next_;
  while((buf->type_ != SLMaster) && (buf != 0)) {
    buf->ack_ = buf->isActive();
    buf = buf->next_;
  }
  // Resetting the Master extinguishes it.
  active_ = false;
}

// Initialise static data members
int SimLED::count_      = 0;
SimLED* SimLED::first_  = 0;
bool SimLED::testAll_   = false;
bool SimLED::flashNow_  = true;
// flashNow defaults True. If user does not implement a flash timer, SimLEDs
// specified as flashing will instead stay steadily lit.

#endif // SIMLED_H

