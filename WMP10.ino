#include "Includes.h"
#include "WiFiManager.h"
WiFiServer server1(80);


//void val()
//{
//  server.on("/", handleRoot);
//  server.begin();
//  Serial.println("HTTP server started");
//
//  while (s.length() <= 0 && p.length() <= 0) {
//    server.handleClient();
//    delay(100);
//  }
//}

String response = "";
HardwareSerial Controller(1);
bool flag=false;
void setup() {
  
  pinMode(2, OUTPUT);
//  pinMode(3, OUTPUT);
//  digitalWrite(3,LOW);
  EEPROM.begin(400);
  Serial.begin(115200);
  String ssid1 = EEPROM.readString(100);
  String pswd1 = EEPROM.readString(200);
  Serial.print(ssid1);
  Serial.print(pswd1);

  WiFi.begin(ssid1.c_str(), pswd1.c_str());
  //loadWIFICredsForm();
  //server.on("/", handleRoot);
  //server.begin();
  //server.handleClient();



  if (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    Controller.println(":Connecting to WiFi..");
    //loadWIFICredsForm();
    //val();  
  }
  Controller.begin(115200, SERIAL_8N1, 16, 17);
  delay(10);
  if (!CheckWIFICreds()) {
    Serial.println("No WIFI credentials stored in memory. Loading form...");
    while (loadWIFICredsForm());
  }
   if (CheckWIFICreds()) {
    Serial.println("check wifi ok");
    server.stop();
    server1.begin();
    Serial.println("server1. begin");
    flag==true;
   }
}


void wipeEEPROM() {
  for (int i = 0; i < 400; i++) {
    EEPROM.writeByte(i, 0);
  }
  EEPROM.commit();
}


void loop() {
//  if (digitalRead(3)  == HIGH) {
//    Serial.println("Wiping WiFi credentials from memory...");
//    wipeEEPROM();
//    while (loadWIFICredsForm());
//  }
  //server.on("/LED=ON", HTTP_GET, handlebutton);
 if(flag==true){
  Serial.println("client if");
    while (WiFi.status() == WL_CONNECTED) {
      WiFiClient client = server1.available();
      if (client.available()) {
       handleRoot(); 
      }
      Serial.println("..........");
      Serial.println("HTTP server started after connection...");
      Serial.println("My local IP..");
      Serial.println(WiFi.localIP());

      

      HTTPClient http;

      Serial.print("[HTTP] begin...\n");

      http.begin(e); //HTTP

      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0)
      {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {

          String payload = http.getString();

          response = strstr(payload.c_str(), "-_-");
          payload = "";
          if (response)
          {
            response = response;
          }
          else
          {
            response = ": Waiting for response ";
          }
          Serial.println(response);
          Controller.println(response);
        }
        else {
          Controller.println(": Target File Not Found ");
        }
      }
      else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        Controller.println(": Network Unreachable ");
      }

      http.end();
    }

    delay(10000);
  }
}
