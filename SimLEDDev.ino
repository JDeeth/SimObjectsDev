#include "SimLEDDev.h"
#include "Bounce.h"

// begin first master warning set
SimLED masterWarning (20);

// warn if engine is on fire
FlightSimInteger engineFireDR;
SimLED engineFire (10, &engineFireDR);

// warn if oil temperature exceeds 200°C
FlightSimFloat oilTemp;
SimLED oilTempHigh (11, &oilTemp, 200, 999);

// begin second master warning set
SimLED masterCaution (19);

// caution if oil pressure drops below 2.5 PSI or exceeds 20 PSI
FlightSimFloat oilPressure;
SimLED oilPressUnsafe (12, &oilPressure, 2.5, 20, true);

// caution if fuel less than 150kg in tank 1
FlightSimFloat fuelTotal;
SimLED fuelLow (18, &fuelTotal, 0, 150);

FlightSimInteger gearHandle;

Bounce testLights   = Bounce (0, 5);
Bounce warningReset = Bounce (3, 5);
Bounce cautionReset = Bounce (7, 5);

elapsedMillis flashTimer;

void setup() {
  SimLED::setup();
  
  engineFireDR = XPlaneRef("sim/cockpit2/annunciators/engine_fires[0]");
  oilTemp      = XPlaneRef("sim/cockpit2/engine/indicators/oil_temperature_deg_C[0]");
  oilPressure  = XPlaneRef("sim/cockpit2/engine/indicators/oil_pressure_psi[0]");
  fuelTotal    = XPlaneRef("sim/cockpit2/fuel/fuel_quantity[0]");

  gearHandle   = XPlaneRef("sim/cockpit2/controls/gear_handle_down");
  
  pinMode (0, INPUT_PULLUP);
  pinMode (3, INPUT_PULLUP);
  pinMode (7, INPUT_PULLUP);
}

void loop() {
  FlightSim.update();
  SimLED::update();
  testLights.update();
  warningReset.update();
  cautionReset.update();

  SimLED::isPowered(gearHandle);
  
  // flash lights every 0.5 seconds
  SimLED::flash(flashTimer < 250);
  if (flashTimer > 500)
    flashTimer = 0;
  
  // activate Light Test Mode when Test button is pressed
  SimLED::lightTest(testLights.read() == LOW); 
  
  // reset Master Warning when warningReset pressed
  if (warningReset.fallingEdge() == true)
    masterWarning.masterReset();

  // reset Master Caution when cautionReset pressed
  if (cautionReset.fallingEdge() == true)
    masterCaution.masterReset();
}

