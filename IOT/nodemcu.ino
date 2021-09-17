//https://script.google.com/macros/s/AKfycbz_ym0TFsRsq-v4nTtY8z3-fQaLLkxOI3_iV0bhpSad0VFp9Ahr_cQK/exec
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
SoftwareSerial s(D6,D5);
float aqi,o3,pm,co,no2,nh3;
int k;
const char* ssid = "sneha";    
const char* password = "12345678";   
//const char* ssid = "ACTFIBERNET";    
//const char* password = "act12345";     
const char* host = "script.google.com";
const int httpsPort = 443;
// Use WiFiClientSecure class to create TLS connection
WiFiClientSecure client;
// SHA1 fingerprint of the certificate, don't care with your GAS service
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID = "AKfycbz_ym0TFsRsq-v4nTtY8z3-fQaLLkxOI3_iV0bhpSad0VFp9Ahr_cQK";   

void setup() 
{
  
  Serial.begin(9600); //Serial
  Serial.println();
  s.begin(9600);
  //connecting to internet
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
   WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 

}

void loop() 
{
  for(k=0;k<1;k++){
  s.write("s");
  int i,j=0;
  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root=jsonBuffer.parseObject(s);
  
  if(root == JsonObject::invalid())
     return;

  root.prettyPrintTo(Serial);
  
  pm=root["data1"];
  no2=root["data2"];
  nh3=root["data3"];
  co=root["data4"];
  o3=root["data5"];
  aqi=root["data6"];

  Serial.println(pm);
  Serial.println(no2);
  Serial.println(nh3);
  Serial.println(co);
  Serial.println(o3);
  Serial.println(aqi);
  sendData(pm,no2,nh3,co,o3,aqi);
 
 delay(2000);
  }
}

// Function for Send data into Google Spreadsheet
void sendData(float pm,float no2,float nh3,float co,float o3,float aqi)
{
  Serial.print("connecting to ");
  Serial.println(host);
  client.setInsecure();
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
  Serial.println("certificate matches");
  } else {
  Serial.println("certificate doesn't match");
  }
  String string_aqi =  String(aqi);
  String string_o3 =  String(o3); 
  String string_pm =  String(pm); 
  String string_co =  String(co);
  String string_no2 =  String(no2);
  String string_nh3=  String(nh3);
  String url = "/macros/s/" + GAS_ID + "/exec?PM2.5=" + string_pm+"&NO2=" +string_no2+"&NH3=" +string_nh3+"&CO=" +string_co+"&O3=" +string_o3+"&AQI=" +string_aqi;
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
         "Host: " + host + "\r\n" +
         "User-Agent: BuildFailureDetectorESP8266\r\n" +
         "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
  String line = client.readStringUntil('\n');
  if (line == "\r") {
    Serial.println("headers received");
    break;
  }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
  Serial.println("esp8266/Arduino CI successfull!");
  } else {
  Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
} 
