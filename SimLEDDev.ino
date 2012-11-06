#include "SimLEDDev.h"
#include "Bounce.h"

// Example code for SimLED

// it is probably not necessary to give DataRefIdents a long or
// especially descriptive name.

DataRefIdent ident1[] = "sim/cockpit2/controls/gear_handle_down";
SimLED gearHandle1(12, ident1, 1, 1, true);

// SimLED discriminates between float and int datarefs by the limits
// you pass to the SimLED constructor. No limits or int limits imply
// an int dataref. Float limits imply a float dataref. Mixed limits
// will give you a compile error at best!

DataRefIdent ident2[] = "sim/cockpit/radios/transponder_code";
SimLED xpdrEmergency(13, ident2, 7700, 7700);

DataRefIdent ident3[] = "sim/cockpit2/controls/parking_brake_ratio";
SimLED gearHandle3(14, ident3, 0.2, 999.0);

DataRefIdent ident4[] = "sim/flightmodel2/gear/deploy_ratio[1]";
SimLED gearPosLeft(15, ident4, 1.0, 999.0);

DataRefIdent ident5[] = "sim/flightmodel2/gear/deploy_ratio[0]";
SimLED gearPosNose(16, ident5, 1.0, 999.0);

DataRefIdent ident6[] = "sim/flightmodel2/gear/deploy_ratio[2]";
SimLED gearPosRight(17, ident6, 1.0, 999.0);

// Power available
// this is a vanilla TeensyDuino FlightSimFloat (for now)
FlightSimFloat supplyVolts;
const float voltsNeeded = 10.0;

Bounce testLights   = Bounce (45, 5);

void setup() {
  SimLED::setup();
  
  supplyVolts = XPlaneRef("sim/cockpit2/electrical/bus_volts[0]");

  pinMode (45, INPUT_PULLUP);
}

void loop() {
  FlightSim.update();
  SimLED::update();
  testLights.update();
  
  SimLED::isPowered(supplyVolts > voltsNeeded);

  SimLED::lightTest(testLights.read());
}

