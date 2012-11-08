#include "SimLEDDev.h"
#include "Bounce.h"
#include "LiquidCrystalFast.h"

// input hardware
const int testButtonPin = 45;
Bounce testLights = Bounce (testButtonPin, 5);

// SimLED
DataRefIdent ident1[] = "sim/cockpit2/controls/gear_handle_down";
SimLED gearHandle1(12, ident1, 1, 1, true);

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

// LCD
enum LCD_PINS {
  RS = 27,
  RW = 0,
  EN,
  D4,
  D5,
  D6,
  D7,
  BACKLIGHT = 24
};

LiquidCrystalFast lcd(RS, RW, EN, D4, D5, D6, D7);

void setupLCD() {
  pinMode (RS, OUTPUT);
  pinMode (RW, OUTPUT);
  pinMode (EN, OUTPUT);
  pinMode (D4, OUTPUT);
  pinMode (D5, OUTPUT);
  pinMode (D6, OUTPUT);
  pinMode (D7, OUTPUT);
  pinMode (BACKLIGHT, OUTPUT);

  analogWrite (BACKLIGHT, 128);
  lcd.begin (16, 2);
  lcd.write ("Hello World");
}

// Power available
FlightSimFloat supplyVolts;
const float voltsNeeded = 10.0;

void setup() {
  SimLED::setup();

  setupLCD();
  
  supplyVolts = XPlaneRef("sim/cockpit2/electrical/bus_volts[0]");

  pinMode (testButtonPin, INPUT_PULLUP);
}

void loop() {
  FlightSim.update();
  SimLED::update();
  testLights.update();
  
  // Optional. Power defaults to 'available'.
  SimLED::isPowered(supplyVolts > voltsNeeded);

  SimLED::lightTest(testLights.read());
}

