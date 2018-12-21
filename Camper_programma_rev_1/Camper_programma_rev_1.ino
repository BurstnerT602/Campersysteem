
/*
  
  Programma voor Camperdisplay
  created 2018
  by Geert Huizing
  modified 18 Dec 2018
 
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 16, 2);

// inlezen, aansturen drinkwatertank.

// ingangen van de drinkwatertank
    const int drinkw25Pin = 2;      // drinkwater 25%
    const int drinkw50Pin = 3;      // drinkwater 50%
    const int drinkw75Pin = 4;      // drinkwater 75%
    const int drinkw100Pin = 5;     // drinkwater 100%
  
// aansturing leds drinkwatertank 
//  const int led25Pin =  22;      // the number of the LED25 pin
//  const int led50Pin =  23;      // the number of the LED50 pin
//  const int led75Pin =  24;      // the number of the LED75 pin
//  const int led100Pin =  25;     // the number of the LED100 pin

// variables will change:
    int drinkw25State = 0;         // variable for reading the drinkwater 25% status
    int drinkw50State = 0;         // variable for reading the drinkwater 50% status
    int drinkw75State = 0;         // variable for reading the drinkwater 75% status
    int drinkw100State = 0;        // variable for reading the drinkwater 100% status

// inlezen, aansturen grijswatertank.

// ingangen van de grijswatertank
    const int grijsw25Pin = 6;      // grijswater 25%
    const int grijsw50Pin = 7;      // grijswater 50%
    const int grijsw75Pin = 8;      // grijswater 75%
    const int grijsw100Pin = 9;     // grijswater 100%

// variables will change:
    int grijsw25State = 0;         // variable for reading the drinkwater 25% status
    int grijsw50State = 0;         // variable for reading the drinkwater 50% status
    int grijsw75State = 0;         // variable for reading the drinkwater 75% status
    int grijsw100State = 0;        // variable for reading the drinkwater 100% status

void WaterStatus(int state, int regel1, int regel2, String tekst){
  if (state == HIGH) {
    lcd.clear();
    lcd.setCursor(regel1, regel2);    
    lcd.print (tekst);
  }
}

    
void setup() {
Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    lcd.clear();
 
  
  // initialize the pushbutton pins as an input:
    pinMode(drinkw25Pin, INPUT);
    pinMode(drinkw50Pin, INPUT);
    pinMode(drinkw75Pin, INPUT);
    pinMode(drinkw100Pin, INPUT);
}

void loop() {

  // read the state of the drinkwater value:
  drinkw25State = digitalRead(drinkw25Pin);
  drinkw50State = digitalRead(drinkw50Pin);
  drinkw75State = digitalRead(drinkw75Pin);
  drinkw100State = digitalRead(drinkw100Pin);
  
  // check niveau drinkwatertank:
  while (drinkw25State == LOW)  {
    lcd.clear();
    lcd.setCursor(0,0);
    delay(500);
    lcd.print ("Drinkwater LEEG");
    delay(1000);
    drinkw25State = digitalRead(drinkw25Pin);
  }

  WaterStatus(drinkw25State, 0, 0, "Drinkwater 25%");

  WaterStatus(drinkw50State, 0, 0, "Drinkwater 50%");

  WaterStatus(drinkw75State, 0, 0, "Drinkwater 75%");
  
  WaterStatus(drinkw100State, 0, 0, "Drinkwater 100%");
  
// check niveau grijswatertank
  while (grijsw25State == LOW)  {
    lcd.clear();
    lcd.setCursor(0,1);
    delay(500);
    lcd.print ("Grijswater LEEG");
    delay(1000);
    drinkw25State = digitalRead(drinkw25Pin);
  }
  
  WaterStatus(grijsw25State, 0, 1, "Grijswater 25%");

  WaterStatus(grijsw50State, 0, 1, "Grijswater 50%"); 

  WaterStatus(grijsw75State, 0, 1, "Grijswater 75%");

  WaterStatus(grijsw100State, 0, 1, "Grijswater 100%");

  delay(5000);
}
