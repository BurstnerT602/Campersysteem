/*
  PIN AANSLUITINGEN Ultrasoon:
 *
 * Vcc  --> 5V
 * Trig --> 12
 * Echo --> 8
 * GND  --> GND
 * 
 * * Walstroom --> pin30
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
 * Vin op adapter = 12.41V
 * Vout op Arduino = 2.497V
 * 5V op Arduino = 5.15V
 * Vin / vOut = factor
 * 12.41 / 2.497 = 4.97
 */
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WaterTank.h>
#include <SimpleDHT.h>
#include <DS3231.h>
#include <Nextion.h>
#include <nextionlib.h>



WaterTank vuilWaterTank = WaterTank(22,24,26,28);
DS3231  rtc(SDA, SCL);
NextionLib Display;

//=============================================================
// Ultrasoon drinkwatertank
const int trigPin = 12;   // trigger pin
const int echoPin = 8;    // echo in

int   duration;           // variabele voor tijdsduur als kommagetal (float)
int   distance;           // variabele voor afstand als kommagetal (float)

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

//================================================================
// Aanwezigheid walstroom
const int WalstroomPin = 30;
int WalstroomState = 0;

//=================================================================
// Temperatuur / vochtmeting DHT11
int pinDHT11binnen = 2;
int pinDHT11buiten = 3;
SimpleDHT11 dht11;

void setup() {                
  Serial.begin(9600);
  rtc.begin();
  pinMode(trigPin, OUTPUT);                  // stel de trigger pin in als uitvoer drinkwatertank
  pinMode(echoPin, INPUT);                   // stel de echo pin in als invoer
 
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
  Display.Send(80-(distance * 2), "Drinkw");

    
   //Uitlezen Vuilwatertank.
  WaterTankStatus vuilWaterTankStatus = vuilWaterTank.GetStatus(); 
  switch(vuilWaterTankStatus){
    case WaterTankStatus::Vol:
      Display.Send("100", "Gwater", "63844");
    break;
    case WaterTankStatus::DrieKwart:
      Display.Send("75-100", "Gwater", "64800");
    break;
    case WaterTankStatus::Half:
      Display.Send("50-75", "Gwater", "2016");
    break;
    case WaterTankStatus::Kwart:
      Display.Send("25-50", "Gwater", "2016");
    break;
    case WaterTankStatus::Leeg:
      Display.Send("0-25", "Gwater", "2016");  
    break;
  }

//  // Uitlezen woonaccuspanning
//   woonaccuSensorVal = analogRead(woonaccuSensorPin);       // lees de waarde van de sensor (0 - 1023) 
//   vwoonOut = (woonaccuSensorVal / 1024) * vCC;             // converteer de gelezen waarde naar het daadwerkelijke voltage op de analoge pin
//   vwoonIn =  vwoonOut * factorwoonaccu;  
//     Serial.print("Woonaccu  ");Serial.print(vwoonIn);
//     Serial.println("V");                                   // converteer het voltage naar het voltage aan de bron door te vermenigvuldigen met de factor
//
//  // Uitlezen startaccuspanning
//   startaccuSensorVal = analogRead(startaccuSensorPin);       // lees de waarde van de sensor (0 - 1023) 
//   vstartOut = (startaccuSensorVal / 1024) * vCC;             // converteer de gelezen waarde naar het daadwerkelijke voltage op de analoge pin
//   vstartIn =  vstartOut * factorstartaccu;                   // converteer het voltage naar het voltage aan de bron door te vermenigvuldigen met de factor
//     Serial.print("Startaccu ");Serial.print(vstartIn);
//     Serial.println("V");
//
//  Uitlezen walstroom
      WalstroomState = digitalRead(WalstroomPin);
      if (WalstroomState == HIGH) {
        Serial.print("Wstroom.bco=2016");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      } else {
        Serial.print("Wstroom.bco=63488");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }

      
//   Uitlezen temeratuur & Luchtvochtigheid
  byte temperaturebinnen;
  byte humiditybinnen;
  byte databinnen[40] = {0};
  dht11.read(pinDHT11binnen, &temperaturebinnen, &humiditybinnen, databinnen);
  Display.Send((int)temperaturebinnen-2, "Tbinnen");
  Display.Send((int)humiditybinnen, "Hbinnen");
      
  byte temperaturebuiten;
  byte humiditybuiten;
  byte databuiten[40] = {0};
  dht11.read(pinDHT11buiten, &temperaturebuiten, &humiditybuiten, databuiten);

  Display.Send((int)temperaturebuiten-2, "Tbuiten");
  Display.Send((int)humiditybuiten, "Hbuiten");
  
  //  //Uitlezen Clock RTC
  Display.Send(rtc.getTimeStr(FORMAT_SHORT), "Time");
  
 delay(1000);

}
