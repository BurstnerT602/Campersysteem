/*
 * Gegevens drinkwaterwatertank
 * hoogte = 40cm
 * inhoud is 80 ltr.
 * factor: 80/40 = per centimeter 2 ltr.
 */
#include <Wire.h>
#include <WaterTank.h>
#include <SimpleDHT.h>
#include <DS3231.h>
#include <Nextion.h>
#include <nextionlib.h>
#include <PID_v1.h>

WaterTank vuilWaterTank = WaterTank(6, 7, 8, 9);
DS3231  rtc(SDA, SCL);
NextionLib Display;

//=============================================================
// Ultrasoon drinkwatertank.
const int trigPin = 4;    // trigger pin
const int echoPin = 5;    // echo in
int   duration;           // variabele voor tijdsduur
int   distance;           // variabele voor afstand

//=============================================================
// Vuilwatertank.
int Vwcounter = 0;
const int VwaterPin = 10;

//==============================================================
// Koelkastventilatie.
const int pinTempRuimte = A8;
const int pinTempCondens = A9;
const int fanControl = 12; // pwm sturing ventilator.
int avgLoop = 60;
double kp=5;
double ki=3;
double kd=0;
double minTdiff = 20;
double minTstart = 45;
double commandMin = 4;
double commandMax = 255;
double tempRuimte, tempCondens, tempDiff, command;
PID myPID(&tempDiff, &command, &minTdiff, kp, ki, kd, DIRECT);    // instellen PID.

//==============================================================
// Accuspanning Woonaccu.
const int woonaccuSensorPin = A1;          
float vwoonIn;                             
float vwoonOut;
float woonaccuSensorVal;                   
const float factorwoonaccu = 3.55;        // reductie factor spanningsdeler.

//==============================================================
// Laden / ontladen woonaccu.
const int IwooninPin = A3;
const int IwoonoutPin = A4;
float AwoonIn;
float AwoonOut;

//================================================================
// Accuspanning Startaccu.
const int startaccuSensorPin = A2;          
float vstartIn;                             
float vstartOut;
float startaccuSensorVal;                   
const float factorstartaccu = 3.55;        // reductie factor spanningsdeler

//================================================================
// Constante spanning arduino.
const float vCC = 5.00;                     // werkspanning arduino

//================================================================
// Aanwezigheid walstroom.
const int WalstroomPin = 13;
int WalstroomState = 0;

//=================================================================
// Temperatuur / vochtmeting DHT11.
const int DHT11binnenPin = 2;
const int DHT11buitenPin = 3;
SimpleDHT11 dht11;

//=================================================================
// LPG tank.
const int gaslevelPin = A7;
float gaslevel;
int gasnivo;

//=================================================================

void setup() {  
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);             
    Serial.begin(9600);
    Serial1.begin(9600);
    rtc.begin();
    pinMode(trigPin, OUTPUT);                  // stel de trigger pin in als uitvoer drinkwatertank
    pinMode(echoPin, INPUT);                   // stel de echo pin in als invoer
    pinMode(VwaterPin, OUTPUT);                // sturing relais
    pinMode(fanControl, OUTPUT);               // pwm sturing ventilatie

    tempRuimte = getTempRuimte();
    tempCondens = getTempCondens();
    tempDiff = tempCondens - tempRuimte;
    if(tempDiff < 0) {
      tempDiff = 0; 
      }   

// start PID
    myPID.SetMode(AUTOMATIC);
    myPID.SetOutputLimits(commandMin, commandMax);
}

//ruimte temp
  double getTempRuimte() {
    return getTemp(pinTempRuimte);
}

//condensor temp
  double getTempCondens() {
    return getTemp(pinTempCondens);
}

//get temp of pin sensor
  double getTemp(int pin) {
    double rawTemp;
    rawTemp = analogRead(pin);
    double temp = rawTemp * (5.0 / 1023.0 * 100.0);
    return temp;
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
      Vwcounter ++ ;
     if (Vwcounter == 3600) {
       digitalWrite(VwaterPin, HIGH); 
       delay(1000);
       Vwcounter = 0;
   }  else {
        digitalWrite(VwaterPin, LOW);
   }
             
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
   
//        Woonaccuspanning
      woonaccuSensorVal = analogRead(woonaccuSensorPin);       
        vwoonOut = (woonaccuSensorVal / 1024) * vCC;             // converteer de gelezen waarde naar het daadwerkelijke voltage op de analoge pin
        vwoonIn =  vwoonOut * factorwoonaccu;  
     if(vwoonIn < 12.80)  {
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
        Display.Send(String((AwoonIn-AwoonOut) * 0.0488, 1), "Iwoon", "2016");
       } else {
        Display.Send(String((AwoonOut-AwoonIn) * 0.0488, 1), "Iwoon", "63844");
       }
        
//       Startaccuspanning
       startaccuSensorVal = analogRead(startaccuSensorPin);       
         vstartOut = (startaccuSensorVal / 1024) * vCC;             // converteer de gelezen waarde naar het daadwerkelijke voltage op de analoge pin
         vstartIn =  vstartOut * factorstartaccu;  
      if(vstartIn < 12.80)  {
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
        dht11.read(DHT11binnenPin, &temperaturebinnen, &humiditybinnen, databinnen);
        Display.Send((int)temperaturebinnen, "Tbinnen");
        Display.Send((int)humiditybinnen, "Hbinnen");
      
     byte temperaturebuiten;
     byte humiditybuiten;
     byte databuiten[40] = {0};
        dht11.read(DHT11buitenPin, &temperaturebuiten, &humiditybuiten, databuiten);  
        Display.Send((int)temperaturebuiten, "Tbuiten");
        Display.Send((int)humiditybuiten, "Hbuiten");
  
//    Clock RTC  
        Display.Send(rtc.getTimeStr(FORMAT_SHORT), "Time");
        
//    LPG tank
          gaslevel = analogRead(gaslevelPin);
          gasnivo = (gaslevel);
          Display.Send((int)gasnivo / 46.5, "LPG");

//    Koelkastventielatie ================================================
  //this loop is to get a more stable temp.
    double avgRuimte = 0;
     double avgCondens = 0;
      for(int i=0; i < avgLoop; i++) {
      int tRuimte = getTempRuimte();
      int tCondens = getTempCondens();
      avgRuimte += tRuimte;
      avgCondens += tCondens;
  }

  //calculate setpoint (tempDiff)
    tempRuimte = avgRuimte / avgLoop;
    tempCondens = (avgCondens / avgLoop);
    Display.Send(tempRuimte, "Truimte");
    Display.Send(tempCondens, "Tcondensor");
    tempDiff = tempCondens - tempRuimte;
      if(tempDiff < 0) {
      tempDiff = 0;
  }

  //process PID 
      myPID.Compute();
  //PID processed command uitvoeren
      if(tempCondens > minTstart){
        analogWrite(fanControl, command); 
        Display.Send(command, "Fanspeed"); 
        } else {
        digitalWrite(fanControl, LOW);
        Display.Send(0, "Fanspeed");
        command = 4;
    }
 
}
