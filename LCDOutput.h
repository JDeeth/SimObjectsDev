// I don't have any servos, so I'm using an LCD to display the results
// for testing purposes. I'm putting the LCD declarations/setup/updates
// over here to stop it distracting from the normal use of the
// SimObjects
// - Jack

#ifndef LCDOutput_h
#define LCDOutput_h

#include <LiquidCrystalFast.h>

// LCD declarations
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


// timer to prevent LCD updating too fast
elapsedMillis lcdTimer = 0;
const int lcdPeriod = 200;


void setupLCD(void) {
  pinMode (BACKLIGHT, OUTPUT);
  analogWrite (BACKLIGHT, 128);

  lcd.begin (16, 2);
}


void updateLCD(void) {
  if (FlightSim.isEnabled()) {
    analogWrite(BACKLIGHT, 128);
    if (lcdTimer > lcdPeriod) {
      lcdTimer = 0;

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print (xpBr.getLow());
      lcd.setCursor(4, 0);
      lcd.print (xpBr.getHigh());
      lcd.setCursor(0, 1);
      lcd.print (xpBr.getDR());

      lcd.setCursor(8, 0);
      lcd.print (elTr.getLow());
      lcd.setCursor(12, 0);
      lcd.print (elTr.getHigh());
      lcd.setCursor(8, 1);
      lcd.print (elTr.getDR());

//      lcd.print (flapGauge.getInput());
//      lcd.setCursor(0, 1);
//      lcd.print (flapGauge.getServoAngle());

//      lcd.setCursor(6, 0);
//      lcd.print (vsi.getInput());
//      lcd.setCursor (6, 1);
//      lcd.print (vsi.getServoAngle());
    }
  } else { //flightsim
    analogWrite(BACKLIGHT, 0);
    lcd.clear();
  }

}


#endif
