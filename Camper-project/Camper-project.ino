/*
  
 * Gegevens drinkwaterwatertank
 * hoogte = 40cm
 * inhoud is 80 ltr.
 * factor: 80/40 = per centimeter 2 ltr.
 * 
 * Exacte waarden spanningen berekenen met multimeter:
 * Vin op adapter = 12.41V
 * Vout op Arduino = 2.497V
 * 5V op Arduino = 5.15V
 * Vin / vOut = factor
 * 12.41 / 2.497 = 4.97
 */
#include <Wire.h>
#include <WaterTank.h>
#include <SimpleDHT.h>
#include <DS3231.h>
#include <Nextion.h>
#include <nextionlib.h>


WaterTank vuilWaterTank = WaterTank(6, 7, 8, 9);
DS3231  rtc(SDA, SCL);
NextionLib Display;

//=============================================================
// Ultrasoon drinkwatertank
const int trigPin = 4;    // trigger pin
const int echoPin = 5;    // echo in
int   duration;           // variabele voor tijdsduur als kommagetal
int   distance;           // variabele voor afstand als kommagetal

//=============================================================
// Vuilwatertank.
int counter = 0;
int VwaterPin = 10;

//==============================================================
// Koelkast
int KkastPin = 12;

//==============================================================
// Accuspanning Woonaccu
const int woonaccuSensorPin = A1;          // sensor pin
float vwoonIn;                             // gemeten voltage (3.3V = max. 16.5V, 5V = max 25V)
float vwoonOut;
float woonaccuSensorVal;                   // waarde op pin A3 (0 - 1023)
const float factorwoonaccu = 4.092;        // reductie factor van het Voltage Sensor shield

//==============================================================
// Laden / ontladen woonaccu
const int IwooninPin = A3;
const int IwoonoutPin = A4;
float AwoonIn;
float AwoonOut;

//================================================================
// Accuspanning Startaccu
const int startaccuSensorPin = A2;          // sensor pin
float vstartIn;                             // gemeten voltage (3.3V = max. 16.5V, 5V = max 25V)
float vstartOut;
float startaccuSensorVal;                   // waarde op pin A4 (0 - 1023)
const float factorstartaccu = 4.092;        // reductie factor van het Voltage Sensor shield

//================================================================
// Constante spanning arduino
const float vCC = 5.15;                     // Arduino invoer voltage (na te meten met voltmeter)

//================================================================
// Aanwezigheid walstroom
const int WalstroomPin = 11;
int WalstroomState = 0;

//=================================================================
// Temperatuur / vochtmeting DHT11
int pinDHT11binnen = 2;
int pinDHT11buiten = 3;
SimpleDHT11 dht11;

//=================================================================
// LPG tank
const int gaslevelPin = A5;
float gaslevel;
int gasnivo;

//=================================================================

void setup() {                
    Serial.begin(9600);
    rtc.begin();
    pinMode(trigPin, OUTPUT);                  // stel de trigger pin in als uitvoer drinkwatertank
    pinMode(echoPin, INPUT);                   // stel de echo pin in als invoer
    pinMode(VwaterPin, OUTPUT);
    pinMode(KkastPin, OUTPUT);
    digitalWrite(VwaterPin, LOW);
}
  
void loop() {
 
//   Drinkwatertank.
    digitalWrite(trigPin, LOW);       
    delayMicroseconds(2);            

    digitalWrite(trigPin, HIGH);      
    delayMicroseconds(10);            
    digitalWrite(trigPin, LOW);       

    duration = pulseIn(echoPin, HIGH);                                    
    distance = (duration * 0.0343) / 2; 
    Display.Send(80-(distance * 2), "Dwater");
    
//   Vuilwatertank.
    counter++ ;
     if(counter == 3600); {
      digitalWrite(VwaterPin, HIGH);
     
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
      delay(1000);
      counter=0;
      }
   }
//      Woonaccuspanning
       woonaccuSensorVal = analogRead(woonaccuSensorPin);       // lees de waarde van de sensor (0 - 1023) 
       vwoonOut = (woonaccuSensorVal / 1024) * vCC;             // converteer de gelezen waarde naar het daadwerkelijke voltage op de analoge pin
       vwoonIn =  vwoonOut * factorwoonaccu;  
     if(vwoonIn < 13.20)  {
        Display.Send(String(vwoonIn, 1), "Vwoon", "63488"); 
       } else {
        Display.Send(String(vwoonIn, 1), "Vwoon", "2016");        
       }    


//       Laden / ontladen woonaccu
        AwoonIn = analogRead(IwooninPin);
        AwoonOut = analogRead(IwoonoutPin);
       if(AwoonOut == AwoonIn)  {
        Display.Send(String("-.-"), "Iwoon", "65504");
       } else if(AwoonIn > AwoonOut) {
        Display.Send(String((AwoonIn-AwoonOut) * 0.0048, 1), "Iwoon", "2016");
       } else {
        Display.Send(String((AwoonOut-AwoonIn) * -0.0048, 1), "Iwoon", "63844");
       }
        
//       Startaccuspanning
       startaccuSensorVal = analogRead(startaccuSensorPin);       // lees de waarde van de sensor (0 - 1023) 
       vstartOut = (startaccuSensorVal / 1024) * vCC;             // converteer de gelezen waarde naar het daadwerkelijke voltage op de analoge pin
       vstartIn =  vstartOut * factorstartaccu;  
      if(vstartIn < 13.20)  {
        Display.Send(String(vstartIn, 1), "Vstart", "63488"); 
       } else {
        Display.Send(String(vstartIn, 1), "Vstart", "2016");        
  }    
               
//    Walstroom
      WalstroomState = digitalRead(WalstroomPin);
      if (WalstroomState == HIGH) {
        Display.SendBCO("Wstroom", "2016");       
      } else {
        Display.SendBCO("Wstroom", "63488");
    }
     
//   Temeratuur & Luchtvochtigheid
     byte temperaturebinnen;
     byte humiditybinnen;
     byte databinnen[40] = {0}; 
        dht11.read(pinDHT11binnen, &temperaturebinnen, &humiditybinnen, databinnen);
        Display.Send((int)temperaturebinnen, "Tbinnen");
        Display.Send((int)humiditybinnen, "Hbinnen");
      
     byte temperaturebuiten;
     byte humiditybuiten;
     byte databuiten[40] = {0};
        dht11.read(pinDHT11buiten, &temperaturebuiten, &humiditybuiten, databuiten);  
        Display.Send((int)temperaturebuiten, "Tbuiten");
        Display.Send((int)humiditybuiten, "Hbuiten");
  
//    Clock RTC
        Display.Send(rtc.getTimeStr(FORMAT_SHORT), "Time");

//    LPG tank
          gaslevel = analogRead(gaslevelPin);
          gasnivo = (gaslevel);
          Display.Send((int)gasnivo / 46.5, "LPG");

//    Koelkastventielatie
       if ((int)temperaturebuiten >= 30) {
          digitalWrite(KkastPin, HIGH);       
      } else {
          digitalWrite(KkastPin, LOW);       
    }
  
 delay(1000);
 
}
