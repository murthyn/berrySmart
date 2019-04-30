#include <SPI.h>
#include <TFT_eSPI.h>
#include <dht.h>
#include <TinyGPS++.h>
#include <WiFi.h> //Connect to WiFi Network

dht DHT;
TFT_eSPI tft = TFT_eSPI();

// -------------POSTING-------------
 
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 5000; //periodicity of getting a number fact.

const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

int state;
bool shouldPrint = true;

char network[] = "6s08";  //SSID for 6.08 Lab
char password[] = "iesc6s08"; //Password for 6.08 Lab
//char network[] = "iPhone (2)";  //SSID for 6.08 Lab
//char password[] = "hello123"; //Password for 6.08 Lab

const uint8_t LOOP_PERIOD = 15000; //milliseconds
uint32_t primary_timer = 0;

HardwareSerial gps_serial(2);
TinyGPSPlus gps;

// --------------SENSORS------------

#define DHT11_PIN 19

uint32_t counter; //used for timing
const float moisture_lower = 0.4;
const float moisture_upper = 0.87;

// https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTstable


// --------------------------------

void setup() {
  Serial.begin(115200); // Set up serial port

  // TFT
  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_WHITE); //fill background
  tft.setTextColor(TFT_BLACK, TFT_WHITE); //set color for font

  // GPS  
  gps_serial.begin(9600, SERIAL_8N1, 32, 33);
  delay(100); //wait a bit (100 ms)

  // WIFI
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 6) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }

  // TIMERS
  counter = millis();
}

void loop() {

  // Always collect GPS
  if (gps_serial.available()) {
    while (gps_serial.available())
      gps.encode(gps_serial.read());      // Check GPS
  }

  if (millis() - counter > 3000) {
    Serial.println("State: collecting sensor data");
    // BRIGHTNESS
    tft.setCursor(0, 0, 1); //set cursor, font size 1
    char message[100]; //char buffer allocate
    float light = 1 - analogRead(A7) / 4096.0;
    sprintf(message, "Brightness: %2.4f     ", light); //message about brightness
    tft.println(message); //print
    Serial.println(message);
    

    // MOISTURE
    tft.setCursor(0, 30, 1);
    char message2[100]; //char buffer allocate
    float soil_moisture = moisture_upper/(moisture_upper - moisture_lower) - (analogRead(A14) / 4096.0)/(moisture_upper - moisture_lower);
    sprintf(message2, "Soil Moisture: %2.4f     ", soil_moisture); //message about soil moisture
    tft.println(message2); //print
    Serial.println(message2);

    tft.setCursor(0, 60, 1); //set cursor, font size 1

    // TEMPERATURE AND HUMIDITY
    int chk = DHT.read11(DHT11_PIN);
    Serial.print("Temperature = ");
    Serial.println(DHT.temperature);
    float temp = DHT.temperature;
    Serial.print("Humidity = ");
    Serial.println(DHT.humidity);
    float humid = DHT.humidity;

    char message3[100]; //char buffer allocate
    sprintf(message3, "Temp/humidity: %2.4f %2.4f", DHT.temperature, DHT.humidity); //message about brightness
    tft.println(message3); //print

    // PUSHING
    
    Serial.println("State: pushing");
    if (gps.location.isValid()) {
      Serial.println("GPS is valid!");
      char body[200]; //for body;
      sprintf(body, "lat=%f&lon=%f&temp=%f&humid=%f&light=%f&moist=%f", gps.location.lat(), gps.location.lng(), temp, humid, light, soil_moisture); //generate body, posting to User, 1 step

      int body_len = strlen(body); //calculate body length (for header reporting)
      Serial.println(body);
      sprintf(request_buffer, "POST http://608dev.net/sandbox/sc/garciag/berryHandler_v2.py HTTP/1.1\r\n");
      strcat(request_buffer, "Host: 608dev.net\r\n");
      strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
      strcat(request_buffer, "\r\n"); //new line from header to body
      strcat(request_buffer, body); //body
      strcat(request_buffer, "\r\n"); //header
      Serial.println(request_buffer);
      do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
      
      // DISPLAY ON SCREEN
      tft.fillScreen(TFT_BLACK); //fill background
      tft.setCursor(0, 0, 1); // set the cursor
      char body1[200]; //for body;
      sprintf(body1, "lat=%f        lon=%f       temp=%f       humid=%f      light=%f      moisture=%f", gps.location.lat(), gps.location.lng(), temp, humid, light, soil_moisture); //generate body, posting to User, 1 step
      tft.println(body1);
    }
    
    counter = millis();
    Serial.println("");
    shouldPrint = true;
  }

  else {
    if (shouldPrint) {
      Serial.println("State: waiting");
      shouldPrint = false;
    }
  }
}


// POSTING HELPER FUNCTIONS

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
