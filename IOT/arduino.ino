#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "MQ135.h"
const int ANALOGPIN=0;


SoftwareSerial s(5,6); // (Rx, Tx)
int aqi;
int o3;
int i,j,k;
float o3_final,aqi_final,co_final,no2_final,nh3_final;

int measurePin = 5; //Connect dust sensor to Arduino A0 pin
int ledPower = 2;   //Connect 3 led driver pins of dust sensor to Arduino D2

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

float co,no2,nh3;
int co_pin=4;
int no2_pin=2;
int nh3_pin=3;

void setup(){ 
s.begin(9600); // sets the serial port to 9600
Serial.begin(9600); 
pinMode(ledPower,OUTPUT);
 }
void loop(){
for(i=0;i<1;i++){
//aqi = analogRead(0); 
MQ135 gasSensor = MQ135(ANALOGPIN);
aqi = gasSensor.getPPM(); 
aqi_final=aqi;   
Serial.print("AQI=");
Serial.print(aqi_final);    
Serial.println(" PPM");
delay(1000);  


o3 =analogRead(1);
o3_final=o3/10.0; //(0.0409*o3*48.00)
Serial.print("Ozone=");
Serial.print(o3_final);
Serial.println(" ug/m3");
delay(1000);



digitalWrite(ledPower,LOW); // power on the LED
 delayMicroseconds(samplingTime);

  voMeasured = analogRead(measurePin); // read the dust value

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024.0);

  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 17* calcVoltage - 0.1;

  Serial.print("PM2.5=");
  Serial.println(dustDensity); // unit: ug/m3

  delay(1000);


co =analogRead(co_pin);

no2=analogRead(no2_pin);

nh3=analogRead(nh3_pin);

co=co/100.0;
no2=no2/10.0;
nh3=nh3/10.0;

co_final=co; 
Serial.print("CO=");
Serial.print(co_final);
Serial.println(" ug/m3");
delay(1000);

no2_final=no2; 
Serial.print("NO2=");
Serial.print(no2_final);
Serial.println(" ug/m3");
delay(1000);

nh3_final=nh3; 
Serial.print("NH3=");
Serial.print(nh3_final);
Serial.println(" ug/m3");
delay(1000);


StaticJsonBuffer<1000> jsonBuffer;
JsonObject& root=jsonBuffer.createObject();
root["data1"]=dustDensity;
root["data2"]=no2_final;
root["data3"]= nh3_final;
root["data4"]= co_final;
root["data5"]= o3_final;
root["data6"]= aqi_final;

if (s.available()>0)
{
  root.printTo(s);
}
//delay (60UL * 60UL * 1000UL); 
delay(5000);
}                          
}
