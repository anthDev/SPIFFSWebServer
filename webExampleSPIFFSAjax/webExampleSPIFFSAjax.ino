#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>



const char *ssid = "iPhone";
const char *password = "aaaaaaaa";


uint32_t delayMS;
#define LED_ANT D4
#define LED_COLOR D8
#define LED_ANT2 D0
#define LIGHT D6


void handleRoot() ;
void handleNotFound();
void handleUptime();
void handleLED();
void handleLED2();
void handleLedColor();
void handleHum();
void handleTemp();
void handleLight();




#define DHTPIN D3     // Digital pin connected to the DHT sensor
#define DHTTYPE           DHT11     // DHT 11 
#define DHTTYPE           DHT22 
DHT_Unified dht(DHTPIN, DHTTYPE);
ESP8266WebServer server(80);

void setup() {

  dht.begin();
  sensor_t sensor;
  sensor_t sensor2;  
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor2);
  pinMode(LIGHT,OUTPUT);
  digitalWrite(LIGHT,0);
  pinMode(LED_ANT, OUTPUT);
  digitalWrite(LED_ANT, 0);
  pinMode(LED_ANT2, OUTPUT);
  digitalWrite(LED_ANT2, 0);
  pinMode(LED_COLOR, OUTPUT);
  digitalWrite(LED_COLOR, 0);
  SPIFFS.begin(); // Start the SPI Flash Files System
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_ANT, !digitalRead(LED_ANT));
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("webExample")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);
  server.on("/getUptime", handleUptime);
  server.on("/getTemp", handleTemp);
  server.on("/getHum", handleHum);
  server.on("/setLED", handleLed);
  server.on("/setLED2", handleLed2);
  server.on("/setLEDColor", handleLedColor);
  server.on("/setLight", handleLight);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(LED_ANT, 1);
}

void loop() {

  server.handleClient();
  MDNS.update();


}


void handleNotFound() {

  // If the client requests any URI not listed in server.on()
  bool found = handleFileRead(server.uri()) ; // send it if it exists in SPIFFS
  if ( found ) {
    return ; // file found then end function
  }
  // if the URI is not found in SPIFFS send 404
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

const char INDEX_HTML[] PROGMEM = R"=====(
<html>
 <b> Uptime is = %ld milliseconds </b>
</html>
)=====";



void handleRoot() {
 char temp[400]; // >= total size of the webpage to be served.
 snprintf(temp, 400, INDEX_HTML,millis());
 server.send(200, "text/html", temp);

}


String getContentType(String filename) { // convert the file extension to the MIME type
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if ( filename.endsWith(".jpg")  or filename.endsWith(".JPG") \
         or filename.endsWith(".JPEG") or filename.endsWith(".jpeg") ) return "image/jpeg";
  return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";         // If a folder is requested, send the index file
  String contentType = getContentType(path);            // Get the MIME type
  if (SPIFFS.exists(path)) {                            // If the file exists
    File file = SPIFFS.open(path, "r");                 // Open it
    size_t sent = server.streamFile(file, contentType); // And send it to the client
    file.close();                                       // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;                                         // If the file doesn't exist, return false
}


void handleUptime() {
    String uptime = String(millis()/1000) ;
    server.send(200, "text/plain", uptime);
  }

void handleTemp() {
  
  String temp;
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  temp=String(event.temperature);
   server.send(200, "text/plain", temp);

}


void handleHum() {
  
  String hum;
  sensors_event_t event;  
  dht.humidity().getEvent(&event);
  hum=String(event.relative_humidity);
   server.send(200, "text/plain", hum);

}

void handleLed(){
for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i)=="ledState"){
     // Serial.println("Test"+server.arg(i));
     String response;
          
      if( server.arg(i) =="1") {
      digitalWrite(LED_ANT,0);
      response="ON 💡";
     }
     
     else{ 
      digitalWrite(LED_ANT,1);
      response="OFF🚫";
     }
     
         
     server.send(200, "text/plain", response);
    }
  }
}


void handleLed2(){
for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i)=="ledState2"){
     // Serial.println("Test"+server.arg(i));
     String response2;
     
     if( server.arg(i) =="1") {
      digitalWrite(LED_ANT2,0);
      response2="ON✅";
     }
     else{
      digitalWrite(LED_ANT2,1);
      response2="OFF🚫";
     }

     
         
     server.send(200, "text/plain", response2);
    }
  }
}

void handleLedColor(){
for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i)=="ledStateColor"){
     // Serial.println("Test"+server.arg(i));
     String response3;
     
     if( server.arg(i) =="1") {
      digitalWrite(LED_COLOR,0);
      response3="ON✅";
     }
     else{
      digitalWrite(LED_COLOR,1);
      response3="OFF🚫";
     }

     
         
     server.send(200, "text/plain", response3);
    }
  }
}

void handleLight(){
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i)=="lightState"){
     // Serial.println("Test"+server.arg(i));
     String response4;
     
     if( server.arg(i) =="1") {
      digitalWrite(LIGHT,1);
      response4="ON✅";
     }
     else{
      digitalWrite(LIGHT,0);

      response4="OFF🚫";
     }

     
         
     server.send(200, "text/plain", response4);
    }
  }
}

//for (uint8_t i = 2; i < server.args(); i++) {
//    if (server.argName(i)=="ledState"){
//     // Serial.println("Test"+server.arg(i));
//     String response;
//     if( server.arg(i) =="2") {
//      digitalWrite(LED_COLOR,0);
//      response="ON";
//     }
//     else if( server.arg(i) =="3")  {
//      digitalWrite(LED_COLOR,1);
//      response="OFF";
//     }
//         
//     server.send(200, "text/plain", response);
//    }
//  }
