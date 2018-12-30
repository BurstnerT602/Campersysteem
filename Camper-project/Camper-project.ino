
/*
  PIN AANSLUITINGEN Ultrasoon:
 *
 * Vcc  --> 5V
 * Trig --> 12
 * Echo --> 8
 * GND  --> GND
 * 
 * Gegevens drinkwaterwatertank
 * hoogte = 40cm
 * inhoud is 80 ltr.
 * factor: 80/40 = per centimeter 2 ltr.
 * 
 * PIN AANSLUITINGEN Voltmeter:
 * * S --> A3 en A4 groen
 * + --> 5V  rood
 * - --> GND blauw
 * 
 * Exacte waarden berekenen met multimeter:
 * 
 * Vin op adapter = 12.41V
 * Vout op Arduino = 2.497V
 * 5V op Arduino = 5.15V
 * 
 * Vin / vOut = factor
 * 12.41 / 2.497 = 4.97
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WaterTank.h>
#include <SimpleDHT.h>

WaterTank vuilWaterTank = WaterTank(22,24,26,28);
//WaterTank schoonWaterTank = WaterTank(32,34,36,38);
LiquidCrystal_I2C lcd(0x27, 16,2);

//=============================================================
// Ultrasoon drinkwatertank
const int trigPin = 12;   // trigger pin
const int echoPin = 8;    // echo in

float duration;           // variabele voor tijdsduur als kommagetal (float)
float distance;           // variabele voor afstand als kommagetal (float)

//==============================================================
// Accuspanning Woonaccu
const int woonaccuSensorPin = A3;          // sensor pin
float vwoonIn;                                // gemeten voltage (3.3V = max. 16.5V, 5V = max 25V)
float vwoonOut;
float woonaccuSensorVal;                   // waarde op pin A3 (0 - 1023)
const float factorwoonaccu = 4.092;               // reductie factor van het Voltage Sensor shield

//================================================================
// Accuspanning Startaccu
const int startaccuSensorPin = A4;          // sensor pin
float vstartIn;                             // gemeten voltage (3.3V = max. 16.5V, 5V = max 25V)
float vstartOut;
float startaccuSensorVal;                   // waarde op pin A4 (0 - 1023)
const float factorstartaccu = 4.092;        // reductie factor van het Voltage Sensor shield

//================================================================
// Constante spanning arduino
const float vCC = 5.15;                     // Arduino invoer voltage (na te meten met voltmeter)

//=================================================================
// Temperatuur / vochtmeting DHT11
int pinDHT11binnen = 2;
int pinDHT11buiten = 3;
SimpleDHT11 dht11;

void setup() {
// Setup ultrasoon drinkwatertank
  pinMode(trigPin, OUTPUT);                 // stel de trigger pin in als uitvoer
  pinMode(echoPin, INPUT);                   // stel de echo pin in als invoer
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
}
  
void loop() {
  // Uitlezen Drinkwatertank.
  digitalWrite(trigPin, LOW);       
  delayMicroseconds(2);            

  digitalWrite(trigPin, HIGH);      
  delayMicroseconds(10);            
  digitalWrite(trigPin, LOW);       

  duration = pulseIn(echoPin, HIGH);                                    
  distance = (duration * 0.0343) / 2; 

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Drinkwater ");
  lcd.print((80-(distance * 2)) ,0);
  lcd.print("ltr");   

  //Uitlezen Vuilwatertank.
  WaterTankStatus vuilWaterTankStatus = vuilWaterTank.GetStatus();
  // WaterTankStatus schoonWaterStatus = schoonWaterTank.GetStatus();
  
  switch(vuilWaterTankStatus){
    case WaterTankStatus::Vol:
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater  100%");
    break;
    case WaterTankStatus::DrieKwart:
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater   75%");
    break;
    case WaterTankStatus::Half:
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater   50%");
    break;
    case WaterTankStatus::Kwart:
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater   25%");
    break;
    case WaterTankStatus::Leeg:
      lcd.setCursor(0, 1);
      lcd.print("Vuilwater  leeg ");
    break;
  }

  // Uitlezen woonaccuspanning
   woonaccuSensorVal = analogRead(woonaccuSensorPin);       // lees de waarde van de sensor (0 - 1023) 
   vwoonOut = (woonaccuSensorVal / 1024) * vCC;             // converteer de gelezen waarde naar het daadwerkelijke voltage op de analoge pin
   vwoonIn =  vwoonOut * factorwoonaccu;  
     Serial.print(vwoonIn);
     Serial.println("V");// converteer het voltage naar het voltage aan de bron door te vermenigvuldigen met de factor

  // Uitlezen startaccuspanning
   startaccuSensorVal = analogRead(startaccuSensorPin);       // lees de waarde van de sensor (0 - 1023) 
   vstartOut = (startaccuSensorVal / 1024) * vCC;             // converteer de gelezen waarde naar het daadwerkelijke voltage op de analoge pin
   vstartIn =  vstartOut * factorstartaccu;                   // converteer het voltage naar het voltage aan de bron door te vermenigvuldigen met de factor
     Serial.print(vstartIn);
     Serial.println("V");

  // Uitlezen temeratuur & Luchtvochtigheid
  byte temperaturebinnen = 0;
  byte humiditybinnen = 0;
  dht11.read(pinDHT11binnen, &temperaturebinnen, &humiditybinnen);

    Serial.print((int)temperaturebinnen); Serial.print(" *C, ");
    Serial.print((int)humiditybinnen); Serial.println(" %");
    
  byte temperaturebuiten = 0;
  byte humiditybuiten = 0;
  dht11.read(pinDHT11buiten, &temperaturebuiten, &humiditybuiten);

    Serial.print((int)temperaturebuiten); Serial.print(" *C, ");
    Serial.print((int)humiditybuiten); Serial.println(" %");

     delay(1000);
     
//  switch(schoonWaterStatus){
//    case WaterTankStatus::Vol:
//      // De schoonwatertank is Vol
//      lcd.setCursor(0, 0);
//      lcd.print("Drinkwater 100%");
//    break;
//    case WaterTankStatus::DrieKwart:
//      // De schoonwatertank is DrieKwart
//      lcd.setCursor(0, 0);
//      lcd.print("Drinkwater  75%");
//    break;
//    case WaterTankStatus::Half:
//      // De schoonwatertank is Half
//      lcd.setCursor(0, 0);
//      lcd.print("Drinkwater  50%");
//    break;
//    case WaterTankStatus::Kwart:
//      // De schoonwatertank is Kwart
//      lcd.setCursor(0, 0);
//      lcd.print("Drinkwater  25%");
//    break;
//    case WaterTankStatus::Leeg:
//      // De schoonwatertank is Leeg
//      lcd.setCursor(0, 0);
//      lcd.print("Drinkwater leeg ");
//    break;
//  }

}
