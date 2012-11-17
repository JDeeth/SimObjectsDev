/*
///////////////////////////////////////////////////////////////////////////////
//
// SimObjects b737 Warning System demonstration
//
// Demonstration of development SimObjects code
//
// The System Annunciators (SA) are located prominently on the glareshield, in two
// clusters of six, referred to as a 'six-pack'. They represent multiple sub-
// annunciators, all located less visibly on the overhead panel. When one of
// the sub-annunciators lights up, the associated System Annunciator lights up
// too, along with the Master Caution light. Pressing the Master Caution light
// extinguishes (resets) the SAs - they will light up if a new sub-annunciator
// becomes active. Pressing the six-pack lights all the SAs for bulb testing.
// When the six-pack is released, any SA with an active sub-annunciator will
// remain lit.
//
// This code is written for the PJRC Teensy board, v2.0 or higher, using the
// Arduino+Teensyduino framework and driven by X-Plane.
//
// With thanks to Anthony Musaluke and b737.org.uk for reference information.
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
*/

#include <Bounce.h>
#include <SimObjectsDev.h>
#include <SimLEDDev.h>
#include <SystemAnnc.h>



/////// Overhead panel fault lights



//// Flight system fault lights

// list of dataref identifiers
DataRefIdent fltIdent[][64] =  {
  "sim/cockpit/switches/yaw_damper_on",
  "sim/cockpit2/annunciators/autopilot_trim_fail",
  "sim/cockpit2/controls/elevator_trim"
};

// list of SimLEDs using those identifiers
SimLEDBase * fltAnncs[] = {
  new SimLEDIntDR(-1, fltIdent[0], true), // fault if yaw damper off
  new SimLEDIntDR(-1, fltIdent[1]), // fault if trim fails
  new SimLEDFloatDR(-1, fltIdent[2], -0.45, 0.45, true)
};



//// IRS system fault lights

// alternative method of creating SimLEDs
// as long as we end up with an array of pointers to our SimLEDs,
// all is good.

DataRefIdent irsIdent1[] = "sim/cockpit2/electrical/dc_voltmeter_selection";
SimLEDIntDR irsAnnc1(-1, irsIdent1);

DataRefIdent irsIdent2[] = "sim/cockpit2/controls/parking_brake_ratio";
SimLEDFloatDR irsAnnc2(-1, irsIdent2, 0.6, 1.0);

SimLEDBase * irsAnncs[] = {
  &irsAnnc1,
  &irsAnnc2
};



//// Fuel system fault lights

DataRefIdent fuelIdent[][64] =  {
  "sim/cockpit2/annunciators/oil_pressure_low[0]",
  "sim/cockpit2/annunciators/oil_pressure_low[1]"
};

SimLEDBase * fuelAnncs[] = {
  new SimLEDIntDR(-1, fuelIdent[0]),
  new SimLEDIntDR(-1, fuelIdent[1])
};



//// Electrical system fault lights

DataRefIdent elecIdent[][64] =  {
  "sim/cockpit2/annunciators/low_voltage",
  "sim/cockpit2/annunciators/generator_off[0]",
  "sim/cockpit2/annunciators/generator_off[1]",
  "sim/cockpit2/annunciators/inverter_off[0]"
};

SimLEDBase * elecAnncs[] = {
  new SimLEDIntDR(-1, elecIdent[0]),
  new SimLEDIntDR(-1, elecIdent[1]),
  new SimLEDIntDR(-1, elecIdent[2]),
  new SimLEDIntDR(-1, elecIdent[3])
};



//// APU fault lights

DataRefIdent apuIdent[][64] =  {
  "sim/cockpit2/electrical/APU_generator_on",
  "sim/operation/failures/rel_APU_press"
};

SimLEDBase * apuAnncs[] = {
  new SimLEDIntDR(-1, apuIdent[0]),
  new SimLEDIntDR(-1, apuIdent[1])
};


DataRefIdent ovhtIdent[][64] =  {
  "sim/cockpit2/annunciators/hvac"
};

SimLEDBase * ovhtAnncs[] = {
  new SimLEDIntDR(-1, ovhtIdent[0])
};



/////// System annunciators

// Here I'm creating six SystemAnnunciators, and creating an array of
// pointers to them, in one go. It could alternately be done using
// SystemAnnc foo(12, fltAnncs, sizeof(fltAnncs);
// SystemAnnc baz(13, irsAnncs, sizeof(irsAnncs);
// SystemAnnc * systemAnncs[] = { &foo, &baz };
// syntax - your choice
b737::SystemAnnc * systemAnncs[] = {
  new b737::SystemAnnc(12, fltAnncs, sizeof(fltAnncs)),
  new b737::SystemAnnc(13, irsAnncs, sizeof(irsAnncs)),
  new b737::SystemAnnc(14, fuelAnncs, sizeof(fuelAnncs)),
  new b737::SystemAnnc(15, elecAnncs, sizeof(elecAnncs)),
  new b737::SystemAnnc(16, apuAnncs, sizeof(apuAnncs)),
  new b737::SystemAnnc(17, ovhtAnncs, sizeof(ovhtAnncs))
};



////// Master Caution light

b737::MasterCaution masterCaution (24, systemAnncs, sizeof(systemAnncs));



////// Ordinary Teensyduino buttons for inputs

Bounce reset = Bounce(20, 5);  //left enc button
Bounce recall = Bounce(11, 5); //right enc button




void setup() {
  SimObject::setup();
  // b737::SystemAnnc and Mastercaution are derived from SimObject
  // and are setup/updated by it

  pinMode(11, INPUT_PULLUP);
  pinMode(20, INPUT_PULLUP);
}



void loop() {
  FlightSim.update();
  recall.update();
  reset.update();

  // Reset all SystemAnncs belonging to masterCaution
  if(reset.fallingEdge())
    masterCaution.reset();

  // set/remove Recall mode on all SystemAnncs belonging to MasterCaution
  masterCaution.setRecall(!recall.read());

  SimObject::update();
}
