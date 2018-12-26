


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WaterTank.h>

WaterTank vuilWaterTank = WaterTank(22,24,26,28);
WaterTank schoonWaterTank = WaterTank(32,34,36,38);
LiquidCrystal_I2C lcd(0x27, 16,2);

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {

  delay(1000);
  WaterTankStatus vuilWaterTankStatus = vuilWaterTank.GetStatus();
  WaterTankStatus schoonWaterStatus = schoonWaterTank.GetStatus();
  
  switch(vuilWaterTankStatus){
    case WaterTankStatus::Vol:
      // De vuilwatertank is vol
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater  100%");
    break;
    case WaterTankStatus::DrieKwart:
      // De vuilwatertank is DrieKwart
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater   75%");
    break;
    case WaterTankStatus::Half:
      // De vuilwatertank is Half
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater   50%");
    break;
    case WaterTankStatus::Kwart:
      // De vuilwatertank is Kwart
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater   25%");
    break;
    case WaterTankStatus::Leeg:
      // De vuilwatertank is Leeg
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater  leeg ");
    break;
  }

  switch(schoonWaterStatus){
    case WaterTankStatus::Vol:
      // De schoonwatertank is Vol
      lcd.setCursor(0, 0);
      lcd.print("Drinkwater 100%");
    break;
    case WaterTankStatus::DrieKwart:
      // De schoonwatertank is DrieKwart
      lcd.setCursor(0, 0);
      lcd.print("Drinkwater  75%");
    break;
    case WaterTankStatus::Half:
      // De schoonwatertank is Half
      lcd.setCursor(0, 0);
      lcd.print("Drinkwater  50%");
    break;
    case WaterTankStatus::Kwart:
      // De schoonwatertank is Kwart
      lcd.setCursor(0, 0);
      lcd.print("Drinkwater  25%");
    break;
    case WaterTankStatus::Leeg:
      // De schoonwatertank is Leeg
      lcd.setCursor(0, 0);
      lcd.print("Drinkwater leeg ");
    break;
  }

  
  
  // put your main code here, to run repeatedly:

}
