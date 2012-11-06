#include "SimLEDDev.h"
#include "SimAnncDev.h"
#include "Bounce.h"


DatarefIdent gearHandleIdent[] = "sim/cockpit2/controls/gear_handle_down";
SimLED gearHandle1(12, gearHandleIdent, 1, 1, true);
SimLED gearHandle2(13, gearHandleIdent);
SimLED gearHandle3(14, gearHandleIdent, 1, 1, true);
SimLED gearHandle4(15, gearHandleIdent);
SimLED gearHandle5(16, gearHandleIdent, 1, 1, true);
SimLED gearHandle6(17, gearHandleIdent);

Bounce testLights   = Bounce (45, 5);
//Bounce warningReset = Bounce (42, 5);
//Bounce cautionReset = Bounce (41, 5);

void setup() {
  SimLED::setup();
  
  pinMode (45, INPUT_PULLUP);
//  pinMode (41, INPUT_PULLUP);
//  pinMode (42, INPUT_PULLUP);
}

void loop() {
  FlightSim.update();
  SimLED::update();
  testLights.update();
//  warningReset.update();
//  cautionReset.update();
  
  SimLED::lightTest(testLights.read());
}

