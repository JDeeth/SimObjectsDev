#include "SimLEDDev.h"
#include "SimServoDev.h"
#include "Bounce.h"
#include "LiquidCrystalFast.h"

// SimServo
ScaleMap flapGaugeMap = {
  {0.0, 120},
  {0.34, 90},
  {1.0, 45}
};

DataRefIdent identFlapRatio[] = "sim/flightmodel2/controls/flap1_deploy_ratio";

SimServo flapGauge(11, identFlapRatio, flapGaugeMap, sizeof(flapGaugeMap));

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

// input hardware
const int testButtonPin = 45;
Bounce testLights = Bounce (testButtonPin, 5);

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
  if (flapGauge.inputValid())
    lcd.print("Input valid!");
  else
    lcd.print("Input invalid!");
  lcd.setCursor(0, 1);
  lcd.print(flapGauge.getMapPair());
  lcd.print(" pairs input");


}

// Power available
FlightSimFloat supplyVolts;
const float voltsNeeded = 10.0;

void setup() {
  SimLED::setup();
  SimServo::setup();

  setupLCD();
  
  supplyVolts = XPlaneRef("sim/cockpit2/electrical/bus_volts[0]");

  pinMode (testButtonPin, INPUT_PULLUP);
}

void loop() {
  FlightSim.update();
  SimLED::update();
  SimServo::update();
  testLights.update();
  
  // Optional. Power defaults to 'available'.
  SimLED::isPowered(supplyVolts > voltsNeeded);

  SimLED::lightTest(testLights.read());
}

