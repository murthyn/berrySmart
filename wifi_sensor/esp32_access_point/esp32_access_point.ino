/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <SPI.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <dht.h>

// Replace with your network credentials
const char* server_ssid     = "Berry Secure";
const char* server_password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
const int espID = 1;
RTC_DATA_ATTR int packetNumber = 1;

// --------------SENSORS------------

dht DHT;
HardwareSerial gps_serial(2);
TinyGPSPlus gps;

#define DHT11_PIN 19

uint32_t counter; //used for timing
const float moisture_lower = 0.4;
const float moisture_upper = 0.87;

// https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTstable

const float SLEEP_TIME = 60; // in seconds
const float MICRO_S_TO_S = 1000000; // conversion factor (do not change)

const float SENSOR_READ_INTERVAL = 6000; // in ms
// --------------------------------

void setup() {
  Serial.begin(115200);


  // GPS  
  gps_serial.begin(9600, SERIAL_8N1, 32, 33);
  delay(100); //wait a bit (100 ms)
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(server_ssid, server_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  counter = millis();
  server.begin();
}

void loop(){ // this loop has sleeping and it does not work

  WiFiClient client = server.available();   // Listen for incoming clients
  
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
        char message[200];
        float light = 1 - analogRead(A7) / 4096.0;
        float soil_moisture = moisture_upper/(moisture_upper - moisture_lower) - (analogRead(A14) / 4096.0)/(moisture_upper - moisture_lower);
        int chk = DHT.read11(DHT11_PIN);
        float temp = DHT.temperature;
        float humid = DHT.humidity;
        sprintf(message, "{'espID': %d, 'packet_number': %d, 'soil_moisture': %2.4f, 'temp': %2.4f, 'humid': %2.4f, 'light': %2.4f}", espID, packetNumber, soil_moisture, temp, humid, light);
        packetNumber += 1;
        client.println(message);
        Serial.println(message);
        counter = millis();
        delay(500);
        // Break out of the while loop
        break;
    }
    // Close the connection
    client.stop();
    delay(500);
    Serial.println("Client disconnected.");
    Serial.println("Sleeping now.");
    esp_sleep_enable_timer_wakeup(SLEEP_TIME * MICRO_S_TO_S);
    esp_deep_sleep_start();
  }
}

void loop2(){ // this loop has no sleep and it works

  WiFiClient client = server.available();   // Listen for incoming clients
  
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    Serial.println("Waiting before taking sensor reading.");
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      
      if (millis() - counter > SENSOR_READ_INTERVAL){
        char message[200];
        float light = 1 - analogRead(A7) / 4096.0;
        float soil_moisture = moisture_upper/(moisture_upper - moisture_lower) - (analogRead(A14) / 4096.0)/(moisture_upper - moisture_lower);
        int chk = DHT.read11(DHT11_PIN);
        float temp = DHT.temperature;
        float humid = DHT.humidity;
        sprintf(message, "{'espID': %d, 'soil_moisture': %2.4f, 'temp': %2.4f, 'humid': %2.4f, 'light': %2.4f}", espID, soil_moisture, temp, humid, light);
        client.println(message);
        Serial.println(message);
        counter = millis();
        // Break out of the while loop
        break;
      }
    }
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
