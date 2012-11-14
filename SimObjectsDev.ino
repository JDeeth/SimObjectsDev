#include "usb_api.h"

// Arduino IDE prevents headers being included from within other headers
#include "Servo.h"
#include "LiquidCrystalFast.h"
#include "Bounce.h"

#include "SimLEDDev.h"
#include "SimServoDev.h"

#define IdentRef (const _XpRefStr_ *)

//// Gear control elements
const int gearSwitchPin = 45;
Bounce gearSwitch = Bounce (gearSwitchPin, 5);

FlightSimCommand gearUp;
FlightSimCommand gearDown;

//// Simulated power supply elements
FlightSimFloat supplyVolts;
const float voltsNeeded = 10.0;

//// Gear position lights elements
// dataref position idents
DataRefIdent gearPosID[3][64] = {
  "sim/flightmodel2/gear/deploy_ratio[1]",
  "sim/flightmodel2/gear/deploy_ratio[0]",
  "sim/flightmodel2/gear/deploy_ratio[2]" };

// gear is down when deploy-ratios are equal to 1
// these are the green lights
SimLEDFloatDR gearDownLeft  (15, gearPosID[0], 1.0, 999.0);
SimLEDFloatDR gearDownNose  (16, gearPosID[1], 1.0, 999.0);
SimLEDFloatDR gearDownRight (17, gearPosID[2], 1.0, 999.0);

// gear is wrong when gear position doesn't match the gear handle
// these are the red lights
SimLEDLocal gearWrongLeft  (12);
SimLEDLocal gearWrongNose  (13);
SimLEDLocal gearWrongRight (14);

FlightSimFloat gearPos[3];
FlightSimInteger gearHandleDown;


void setup() {
  SimObject::setup();

  pinMode (gearSwitchPin, INPUT_PULLUP);

  gearUp = XPlaneRef("sim/flight_controls/landing_gear_up");
  gearDown = XPlaneRef("sim/flight_controls/landing_gear_down");

  for (int i = 0; i < 3; ++i) {
    gearPos[i].assign(IdentRef gearPosID[i]);
  }

  gearHandleDown =  XPlaneRef("sim/cockpit2/controls/gear_handle_down");

  supplyVolts =     XPlaneRef("sim/cockpit2/electrical/bus_volts[0]");

  //setupLCD(); // You can ignore this.
}



void loop() {
  FlightSim.update();
  gearSwitch.update();
  SimObject::update();

  SimObject::hasPower = (supplyVolts > voltsNeeded);

  // blocking input on gear handle position
  if(gearSwitch.read() == LOW) { // if the switch is closed
    if (gearHandleDown == 0) { // if gear handle is up
      gearDown.once(); // move it down
    }
  } else {
    if (gearHandleDown == 1) { // if gear handle is down
      gearUp.once(); // move it up
    }
  } // gearSwitch

  gearWrongLeft.setActive(gearHandleDown != gearPos[0]);
  gearWrongNose.setActive(gearHandleDown != gearPos[1]);
  gearWrongRight.setActive(gearHandleDown != gearPos[2]);

  //updateLCD(); // You can ignore this too.
}






//////////////////////////

// unused temporary

//// SimServo flaps gauge

//// These are input/output pairings. The left column is the dataref value,
//// the right column is the servo angle needed to indicate that value.
//// Or, the angle you want to the servo to move to when the dataref has
//// that value.
//ScaleMap flapGaugeMap = {
//  {0.0, 120},
//  {0.34, 90},
//  {1.0, 45}
//};

//// Due to something complicated about Teensy architecture, we have to
//// state the dataref identifier separately to the call to SimServo's
//// constructor.
//DataRefIdent flapGaugeIdent[] = "sim/flightmodel2/controls/flap1_deploy_ratio";

//// Here we create a SimServo object. The servo's on pin 11.
//// It is important that the 3rd and 4th arguments are " myScaleMap,
//// sizeof(myScaleMap) " as shown here, or stuff will break!
//SimServo flapGauge(11,
//                   flapGaugeIdent,
//                   flapGaugeMap,
//                   sizeof(flapGaugeMap));

////// SimServo VSI gauge

//ScaleMap vsiMap = {
//  {-6000, 153},
//  {-5000, 148},
//  {-4000, 143},
//  {-3000, 136},
//  {-2000, 128},
//  {-1500, 122},
//  {-1000, 113},
//  {-500,  102},
//  {0,     90},
//  {500,   79},
//  {1000,  68},
//  {1500,  61},
//  {2000,  55},
//  {3000,  47},
//  {4000,  40},
//  {5000,  35},
//  {6000,  30}
//};

//DataRefIdent vsiIdent[] = "sim/cockpit2/gauges/indicators/vvi_fpm_pilot";

//SimServo vsi (10,
///*ident*/     vsiIdent,
///*map*/       vsiMap,
///*map size*/  sizeof(vsiMap),
///*rest pos.*/ -1,
///*sim power*/ 0);

// By setting the rest position to -1 (default), the servo will stop
// moving when simulated power is unavailable.

// But by setting the sim power source reference to 0, we state this
// gauge does not need simulated power to run, making resting position
// irrelevant.



//ScaleMap fooMap = {
//  {-2.4, 24},
//  {20,  103},
//  {67,   75},   // This is OK. Output values can increase, decrease, and
//                // change direction like this.

//  {50,   64}    // NOT OK - input values MUST be increasing order.
//                // This ScaleMap will be considered invalid and the
//                // SimServo using it will be inactive.
//};

//DataRefIdent fooIdent[] = "sim/cockpit/ipso/lorem";
//// non-existant dataref identifier will result in input always being 0.

//// We'll use fooPower as a sim-power-available flag for the next servo.
//bool fooPower = true;

//SimServo foo(9,
//             fooIdent,
//             fooMap,
//             sizeof(vsiMap), // ERROR! vsiMap is not fooMap!

//             15,             // Our resting position is 15 degrees, so
//                             // the servo will move to 15 degrees when
//                             // fooPower is false. Except this SimServo
//                             // has been improperly set up, so in fact
//                             // it will never move at all.

//             &fooPower);     // We have set fooPower as the sim-power-
//                             // available flag for this SimServo. If
//                             // fooPower is true, we have power,
//                             // otherwise we don't.


//ScaleMap bazMap = {
//  {0, 90}
//};
//// Error - we need at least 2 pairs of data in ScaleMap. This map would
//// keep the SimServo inactive.


// Various SimLEDs

// LED goes on if transponder light is on.
//DataRefIdent ident2[] = "sim/cockpit/radios/transponder_brightness";
//SimLEDFloatDR xpBr(13, ident2, 0.1, 999.0);

//DataRefIdent ident1[] = "sim/cockpit/radios/transponder_light";
//SimLEDIntDR transponderLight(12, ident1);

//// LED goes on if transponder code is 7700
////DataRefIdent ident2[] = "sim/cockpit/radios/transponder_code";
////SimLEDInt xpdrEmergency(13, ident2, 7700, 7700, true);

//// LED goes on if elevator trim is approaching its limits, when it's
//// NOT between -0.45 and 0.45. The 'true' inverts the active range.
//DataRefIdent ident3[] = "sim/cockpit2/controls/elevator_trim";
//SimLEDFloatDR elTr(14, ident3, -0.45, 0.45, true);

//// These three LEDs go on when each gear leg is down and locked.
//DataRefIdent ident4[] = "sim/flightmodel2/gear/deploy_ratio[1]";
//SimLEDFloatDR gearPosLeft(15, ident4, 1.0, 999.0);

//DataRefIdent ident5[] = "sim/flightmodel2/gear/deploy_ratio[0]";
//SimLEDFloatDR gearPosNose(16, ident5, 1.0, 999.0);

//DataRefIdent ident6[] = "sim/flightmodel2/gear/deploy_ratio[2]";
//SimLEDFloatDR gearPosRight(17, ident6, 1.0, 999.0);


//// Ordinary Teensyduino code to give us a bulb-test input
//const int testButtonPin = 45;
//Bounce testLights = Bounce (testButtonPin, 5);



