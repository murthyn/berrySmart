//#include <SPI.h>
#include <WiFi.h>
#include <dht.h>

//----------WIFI----------//

/* change ssid and password according to yours WiFi*/

// intermediate node 3

const int espID = 3;
// collect from
const char* client_ssid     = "Berry Secure 23";
const char* client_password = "123456789";

// send to
const char* server_ssid = "Berry Secure 34";
const char* server_password = "123456789"; 
const char* host = "192.168.4.1"; // need to find out what is each host id

WiFiServer server(80);

const int port = 80;

//----------DATA----------//

const int sizeOfDataBuffer = 5;
const int sizeOfDataInDataBuffer = 10000;

char dataBuffer [sizeOfDataBuffer][sizeOfDataInDataBuffer]; // [Number of Strings][Max Size of Strings]
int endBuffer = 0;
// float lastTimeDataCollect; // timer for when was last data collection at this node
const float SENSOR_READ_INTERVAL = 60000; // in ms

//-------SENSORS--------//

dht DHT;

#define DHT11_PIN 19

const float moisture_lower = 0.4;
const float moisture_upper = 0.87;
//----------POSTING----------//
 
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 5000; //periodicity of getting a number fact.

const uint16_t IN_BUFFER_SIZE = 10000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 10000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

//-----------SLEEPING-------------//

const float SLEEP_TIME = 60; // in seconds
const float MICRO_S_TO_S = 1000000; // conversion factor (do not change)
RTC_DATA_ATTR int packetNumber = 1;
//-------------------------------//


void setup()
{
  Serial.begin(115200);
}

// HELPER FUNCTIONS

int state = 0; // 0 = connecting to client
int prev_state = 0;
//1 is connecting to mit guest

void sendingSetup(){

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(server_ssid, server_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

bool sendingBuffer(){
  WiFiClient client = server.available();   // Listen for incoming clients
  
  if (client) {                             // If a new client connects,
    Serial.println("Client found.");        // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
      }
      else {
        char message[500];
        char message2[500];
        sprintf(message, "%s", dataBuffer[0]);
        float light = 1 - analogRead(A7) / 4096.0;
        float soil_moisture = analogRead(A14) / 4096.0; //moisture_upper/(moisture_upper - moisture_lower) - (analogRead(A14) / 4096.0)/(moisture_upper - moisture_lower);
        DHT.read11(DHT11_PIN);
        float temp = DHT.temperature;
        float humid = DHT.humidity;
        sprintf(message2, "#{'espID': %d, 'packet_number': %d, 'soil_moisture': %2.4f, 'temp': %2.4f, 'humid': %2.4f, 'light': %2.4f}", espID, packetNumber, soil_moisture, temp, humid, light);
        strcat(message, message2);
        packetNumber += 1;
        client.println(message);
        Serial.println("Message received:");
        Serial.println(message);
        delay(5000);
        Serial.println("Packet sent.");
        
        // Break out of the while loop
        break;
      }
    }
    
    endBuffer = 0;
    emptyBuffer();
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    return true;
  } else{
    delay(5000);
    Serial.println("Sending not successful. Resending...");
    return false;
  }
}
void emptyBuffer() {
  for(int i = 0; i < sizeOfDataBuffer; i++){
      strcpy(dataBuffer[i], "");
  }
}

void addToBuffer(){
  WiFiClient client;
  char temp_buffer[5000];
  Serial.println("connecting to client now");
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }
  Serial.println("connected to client now, waiting for data");
  while(endBuffer == state){
    char ch;
    int bufptr = 0;
    while (client.available()){
      ch = client.read(); // ch iterates over each character as it reads from client
      temp_buffer[bufptr++] = ch;
//      if (sizeof(ch) != 0){
//        bufptr += 1;
//      }
    }
    temp_buffer[bufptr] = '\0';
//    Serial.println(temp_buffer);
    if (bufptr != 0){
      strcpy(dataBuffer[endBuffer++], temp_buffer);
    }
  }
}





void loop(){
  if (state == 0){
    WiFi.begin(client_ssid, client_password);
    Serial.println("Connecting to Client");
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
    delay(1000);
    if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
      Serial.println("CONNECTED TO CLIENT!");
      Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
      delay(500);
      addToBuffer();
      // Connected to Berry Secure, now get data and store in buffer
      Serial.print("Posting number of packets of data: ");
      Serial.println(endBuffer);
      Serial.println(dataBuffer[endBuffer]);
      prev_state = state;
      state = 1;
    }
  } else if (state == 1) {
    if (prev_state == 0){
      sendingSetup();
    }
    bool success = sendingBuffer();
    prev_state = state;
    if (success){
      state = 0;
      Serial.println("Now sleeping");
      delay(1000);
      esp_sleep_enable_timer_wakeup(SLEEP_TIME * MICRO_S_TO_S);
      esp_deep_sleep_start();
    }
  }
}

//#include <WiFi.h>
//#include <dht.h>
//const int espID = 2;
//
//// client and server ssids and passwords
//const char* client_ssid     = "Berry Secure";
//const char* client_password = "123456789";
//const char* server_ssid = "Berry Secure 2";
//const char* server_password = "123456789";
//const char* host = "192.168.4.1";
//const int port = 80;
//WiFiServer server(port);
//
//char dataBuffer [10][1000]; // [Number of Strings][Max Size of Strings]
//int endBuffer = 0;
//
//#define DHT11_PIN 19
//dht DHT;
// 
//const uint16_t IN_BUFFER_SIZE = 1200; //size of buffer to hold HTTP request
//const uint16_t OUT_BUFFER_SIZE = 1200; //size of buffer to hold HTTP response
//char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
//char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response
//
//const float SLEEP_TIME = 30; // in seconds
//const float MICRO_S_TO_S = 1000000; // conversion factor
//
//RTC_DATA_ATTR int packetNumber = 1;
//
//void setup()
//{
//  Serial.begin(115200);
//}
//
//void sendingSetup(){
//  // Connect to Wi-Fi network with SSID and password
//  Serial.print("Setting AP (Access Point)..."); 
//  WiFi.softAP(server_ssid, server_password); // remove password parameter to make AP open
//  IPAddress IP = WiFi.softAPIP();
//  Serial.print("AP IP address: ");
//  Serial.println(IP);
//  
//  server.begin();
//}
//
//void sendingBuffer(){
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
//        char message[500]; // holds incoming request
//        char message2[500]; // holds this sensor data
//        sprintf(message, "%s", dataBuffer[0]);
//        float light = 1 - analogRead(A7) / 4096.0;
//        float soil_moisture = analogRead(A14) / 4096.0;
//        DHT.read11(DHT11_PIN);
//        float temp = DHT.temperature;
//        float humid = DHT.humidity;
//        sprintf(message2, "#{'espID': %d, 'packet_number': %d, 'soil_moisture': %2.4f, 'temp': %2.4f, 'humid': %2.4f, 'light': %2.4f}", espID, packetNumber, soil_moisture, temp, humid, light);
//        strcat(message, message2);
//        packetNumber += 1;
//        client.println(message);
//        Serial.println("message");
//        Serial.println(message);
//        delay(5000);
//        Serial.println("posted");
//        
//        // Break out of the while loop
//        break;
//      }
//    }
//    
//    endBuffer = 0;
//    emptyBuffer();
//    // Close the connection
//    client.stop();
//    Serial.println("Client disconnected.");
//    Serial.println("");
//  } else{
//    delay(5000);
//    Serial.println("Sending not successful. Resending...");
//    sendingBuffer();
//  }
//}
//
//void emptyBuffer() {
//  for(int i = 0; i < 30; i++){
//      strcpy(dataBuffer[i], "");
//  }
//}
//
//void addToBuffer(){
//  WiFiClient client;
//  char temp_buffer[200];
//  Serial.println("connecting to client now");
//  if (!client.connect(host, port)) {
//    Serial.println("connection failed");
//    delay(5000);
//    return;
//  }
//  Serial.println("connected to client now, waiting for data");
//  while(endBuffer == 0){
//    char ch;
//    uint8_t bufptr = 0;
//    while (client.available()){
//      ch = client.read(); // ch iterates over each character as it reads from client
//      temp_buffer[bufptr++] = ch;
//    }
//    if (bufptr != 0){
//      temp_buffer[bufptr] = '\0';
//      strcpy(dataBuffer[endBuffer++], temp_buffer);
//    }
//  }
//}
///* state variable:
//0 = connecting to client
//1 = connecting to server
//*/
//uint8_t state = 0; 
//
//void loop(){
//  if (state == 0){
//    WiFi.begin(client_ssid, client_password);
//    Serial.println("Connecting to Client");
//    while (WiFi.status() != WL_CONNECTED){
//      delay(500);
//      Serial.print(".");
//    }
//    delay(2000);
//    if (WiFi.isConnected()) { // if we're connected then print our IP, MAC, and SSID
//      Serial.println("CONNECTED TO CLIENT!");
//      Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
//      delay(500);
//      addToBuffer();
//      // connected to Berry Secure, now get data and store in buffer
//      Serial.print("posting number of packets of data: ");
//      Serial.println(endBuffer);
//      Serial.println(dataBuffer[endBuffer]);
//      state = 1;
//    }
//  } else if (state == 1) {
//    sendingSetup();
//    sendingBuffer();
//    state = 0;
//    Serial.println("Now sleeping");
//    delay(1000);
//    esp_sleep_enable_timer_wakeup(SLEEP_TIME * MICRO_S_TO_S);
//    esp_deep_sleep_start();
//  }
//}
