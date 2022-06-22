#include <String.h>
#include <WebServer.h>
String s, p, e;
WebServer server(80);


/*
   Function to handle unknown URLs
*/
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

/*
   Function for writing WiFi creds to EEPROM
   Returns: true if save successful, false if unsuccessful
*/
bool writeToMemory(String ssid, String pass, String endpoint) {
  char ssid_str[30];
  char pswd_str[30];
  char endpoint_str[60];
  ssid.toCharArray(ssid_str, 30);
  pass.toCharArray(pswd_str, 30);
  endpoint.toCharArray(endpoint_str, 60);
  EEPROM.writeString(100, ssid_str);
  EEPROM.writeString(200, pswd_str);
  EEPROM.writeString(300, endpoint_str);
  delay(100);
  String s = EEPROM.readString(100);
  String p = EEPROM.readString(200);
  String e = EEPROM.readString(300);
  //#if DEBUG
  Serial.print("Stored SSID, password, endpooint are: ");
  Serial.print(s);
  Serial.print(" / ");
  Serial.print(p);
  Serial.print(" / ");
  Serial.print(e);
  //#endif
  if (ssid == s && pass == p) {
    return true;
  } else {
    return false;
  }
}


/*
   Function for handling form
*/
void handleSubmit() {
  String response_success = "<h1>Success</h1>";
  response_success += "<h2>Device will restart in 3 seconds</h2>";

  String response_error = "<h1>Error</h1>";
  response_error += "<h2><a href='/'>Go back</a>to try again";

  if (writeToMemory(String(server.arg("ssid")), String(server.arg("password")), String(server.arg("endpoint")))) {
    server.send(200, "text/html", response_success);
    EEPROM.commit();
    delay(3000);
    ESP.restart();
  } else {
    server.send(200, "text/html", response_error);
  }
}

/*
   Function for home page
*/
void handleRoot() {
  if (server.hasArg("ssid") && server.hasArg("password") && server.hasArg("endpoint")) {
    handleSubmit();
  }
  else {
    server.send(200, "text/html", INDEX_HTML);
  }
}


/*
   Function for loading form
   Returns: false if no WiFi creds in EEPROM
*/

bool loadWIFICredsForm() {
  s = EEPROM.readString(100);
  p = EEPROM.readString(200);
  e = EEPROM.readString(300);

  const char* ssid     = "ESP32 WiFi Manager";
  const char* password = "12345678";


  Serial.println("Setting Access Point...");

  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("HTTP server started");

  while (s.length() <= 0 && p.length() <= 0) {
    server.handleClient();
    delay(100);
  }
  
  return false;
}

/*
   Function checking WiFi creds in memory
   Returns: true if not empty, false if empty
*/
bool CheckWIFICreds() {
  Serial.println("Checking WIFI credentials");
  s = EEPROM.readString(100);
  p = EEPROM.readString(200);
  e = EEPROM.readString(300);

  //#if DEBUG
  Serial.print("Found credentials: ");
  Serial.print(s);
  Serial.print("/");
  Serial.print(p);
  Serial.print("/");
  Serial.print(e);
  delay(5000);
  //#endif
  if (s.length() > 0 && p.length() > 0 && e.length() > 0) {
    return true;
  } else {
    return false;
  }
}
