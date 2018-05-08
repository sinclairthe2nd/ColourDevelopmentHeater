#include <SevSeg.h>
#include <Adafruit_MAX31865.h>
#include<PID_v1.h>

Adafruit_MAX31865 tempSensor = Adafruit_MAX31865(10, 11, 12, 13);
#define RREF      439.0 //adjust here
#define RNOMINAL  100.0

double temp;
double pwmOut;
double setpoint, kP = 2, kI = 5, kD = 1;
PID control(&temp, &pwmOut, &setpoint, kP, kI, kD, DIRECT);
//Seven Segment Magic
SevSeg sevseg; //Instantiate a seven segment controller object

  /* TODO
   *  7Segment output
   *  setpoint input
   *  
   * 
   */

void setup() {

//sevenSegement
byte numDigits = 2;   
byte digitPins[] = {2, A1}; //Digits: 1,2,3,4 <--put one resistor (ex: 220 Ohms, or 330 Ohms, etc, on each digit pin)
byte segmentPins[] = {8, A0, 4, 6, 5, 9, 7, A2}; //Segments: A,B,C,D,E,F,G,Period
sevseg.begin(COMMON_ANODE, numDigits, digitPins, segmentPins);
sevseg.setBrightness(256);
  
  Serial.begin(115200);
  tempSensor.begin(MAX31865_2WIRE);  
  control.SetMode(AUTOMATIC);
  setpoint = 30;
}

// Fault logic for the Temp Sensor
int tempSensorFault(uint8_t fault) {
   if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    tempSensor.clearFault();
    return 1;
  }

  return 0;
}



void loop() {
  sevseg.setNumber((int)setpoint);
  for(uint16_t i=0; i < 30000; i++) {
    sevseg.refreshDisplay();
  }
  uint16_t rtd = tempSensor.readRTD();
  float ratio = rtd;
  ratio /= 32768;
  temp = (tempSensor.temperature(RNOMINAL, RREF));

   uint8_t fault = tempSensor.readFault();

  if(!tempSensorFault(fault)) {
    Serial.print("Temperature = "); Serial.println(temp);
    sevseg.setNumber((int)temp);
      control.Compute();
      analogWrite(3, pwmOut);
  }

  //Output to Seven Segment
  for(uint16_t i = 0; i < 65000; i++) {
  sevseg.refreshDisplay();
  }


  

}



