/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* server_ssid     = "Berry Secure";
const char* server_password = "123456789";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

int counter;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(server_ssid, server_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){

  WiFiClient client = server.available();   // Listen for incoming clients
  
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
      }
      else {
        char message[100];
        sprintf(message, "test: %d", counter);
        client.println(message);
        counter++;
        delay(100);
        // Break out of the while loop
        break;
      }
    }
    
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    delay(2000);
  }
}


///*********
//  Rui Santos
//  Complete project details at https://randomnerdtutorials.com  
//*********/
//
//// Load Wi-Fi library
//#include <SPI.h>
//#include <TinyGPS++.h>
//#include <WiFi.h>
//#include <dht.h>
//
//// Replace with your network credentials
//const char* server_ssid     = "Berry Secure";
//const char* server_password = "123456789";
//
//// Set web server port number to 80
//WiFiServer server(80);
//
//// Variable to store the HTTP request
//String header;
//
//const int espID = 2;
//
//// --------------SENSORS------------
//
//dht DHT;
//HardwareSerial gps_serial(2);
//TinyGPSPlus gps;
//
//#define DHT11_PIN 19
//
//uint32_t counter; //used for timing
//const float moisture_lower = 0.4;
//const float moisture_upper = 0.87;
//
//// https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTstable
//
//
//// --------------------------------
//
//void setup() {
//  Serial.begin(115200);
//
//
//  // GPS  
//  gps_serial.begin(9600, SERIAL_8N1, 32, 33);
//  delay(100); //wait a bit (100 ms)
//  
//  // Connect to Wi-Fi network with SSID and password
//  Serial.print("Setting AP (Access Point)…");
//  // Remove the password parameter, if you want the AP (Access Point) to be open
//  WiFi.softAP(server_ssid, server_password);
//
//  IPAddress IP = WiFi.softAPIP();
//  Serial.print("AP IP address: ");
//  Serial.println(IP);
//  
//  server.begin();
//}
//
//void loop(){
//
//  WiFiClient client = server.available();   // Listen for incoming clients
//  
//  if (client) {                             // If a new client connects,
//    Serial.println("New Client.");          // print a message out in the serial port
//    String currentLine = "";                // make a String to hold incoming data from the client
//    while (client.connected()) {            // loop while the client's connected
//      if (client.available()) {             // if there's bytes to read from the client,
//        char c = client.read();             // read a byte, then
//        Serial.write(c);                    // print it out the serial monitor
//      }
//      else {
//        if (millis() - counter > 5000){
//          char message[100];
//          float light = 1 - analogRead(A7) / 4096.0;
//          float soil_moisture = moisture_upper/(moisture_upper - moisture_lower) - (analogRead(A14) / 4096.0)/(moisture_upper - moisture_lower);
//          int chk = DHT.read11(DHT11_PIN);
//          float temp = DHT.temperature;
//          float humid = DHT.humidity;
//          sprintf(message, "%2.4f,%2.4f,%2.4f,%2.4f", light, soil_moisture, temp, humid);
//          client.println(message);
//          counter = millis();
//          // Break out of the while loop
//          break;
//        }
//      }
//    }
//    // Close the connection
//    client.stop();
//    Serial.println("Client disconnected.");
//    Serial.println("");
//    delay(5000);
//  }
//}
