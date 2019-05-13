//#include <SPI.h>
#include <WiFi.h>
#include <dht.h>

//----------WIFI----------//

/* change ssid and password according to yours WiFi*/
const char* client_ssid     = "Berry Secure";
const char* client_password = "123456789";
/*
   This is the IP address of your PC
   [Wins: use ipconfig command, Linux: use ifconfig command]
*/
const char* host = "192.168.4.1"; // need to find out what is each host id
const int port = 80;

//----------DATA----------//

char dataBuffer [10][1000]; // [Number of Strings][Max Size of Strings]
int endBuffer = 0;
float lastTimeDataCollect; // timer for when was last data collection at this node
const float SENSOR_READ_INTERVAL = 60000; // in ms

//-------SENSORS--------//
const int espID = 2;

dht DHT;
HardwareSerial gps_serial(2);
TinyGPSPlus gps;

#define DHT11_PIN 19

const float moisture_lower = 0.4;
const float moisture_upper = 0.87;
//----------POSTING----------//
 
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 5000; //periodicity of getting a number fact.

const uint16_t IN_BUFFER_SIZE = 1200; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1200; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

const char* server_ssid = "Berry Secure 2";  //SSID for 6.08 Lab
const char* server_password = "123456789"; //Password for 6.08 Lab

WiFiServer server(80);

//-----------SLEEPING-------------//

const float SLEEP_TIME = 60; // in seconds
const float MICRO_S_TO_S = 1000000; // conversion factor (do not change)
RTC_DATA_ATTR int packetNumber = 1;
//-------------------------------//


void setup()
{
  Serial.begin(115200);
  lastTimeDataCollect = millis();
}

// HELPER FUNCTIONS

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

void sendingBuffer(){
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
        char message[500];
        char message2[500];
        sprintf(message, "%s", dataBuffer[0]);
        float light = 1 - analogRead(A7) / 4096.0;
        float soil_moisture = moisture_upper/(moisture_upper - moisture_lower) - (analogRead(A14) / 4096.0)/(moisture_upper - moisture_lower);
        int chk = DHT.read11(DHT11_PIN);
        float temp = DHT.temperature;
        float humid = DHT.humidity;
        sprintf(message2, "#{'espID': %d, 'packet_number': %d, 'soil_moisture': %2.4f, 'temp': %2.4f, 'humid': %2.4f, 'light': %2.4f}", espID, packetNumber, soil_moisture, temp, humid, light);
        strcat(message, message2);
        packetNumber += 1;
        client.println(message);
        Serial.println("message");
        Serial.println(message);
        delay(5000);
        Serial.println("posted");
        
        // Break out of the while loop
        break;
      }
    }
    
    endBuffer = 0;
    emptyBuffer();
//    if (millis() - lastTimeDataCollect > SENSOR_READ_INTERVAL){
//        char message[200];
//        float light = 1 - analogRead(A7) / 4096.0;
//        float soil_moisture = moisture_upper/(moisture_upper - moisture_lower) - (analogRead(A14) / 4096.0)/(moisture_upper - moisture_lower);
//        int chk = DHT.read11(DHT11_PIN);
//        float temp = DHT.temperature;
//        float humid = DHT.humidity;
//        sprintf(message, "{'espID': %2.4f, 'soil_moisture': %2.4f, 'temp': %2.4f, 'humid': %2.4f, 'light': %2.4f}", espID, soil_moisture, temp, humid, light);
//        client.println(message);
//        Serial.println(message);
//        delay(500);
//        lastTimeDataCollect = millis();
//        endBuffer = 0;
//        emptyBuffer();
//    }
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
//    ESP.restart();
  } else{
    delay(5000);
    Serial.println("Sending not successful. Resending...");
    sendingBuffer();
  }
}
void emptyBuffer() {
  for(int i = 0; i < 30; i++){
      strcpy(dataBuffer[i], "");
  }
}

void addToBuffer(){
  WiFiClient client;
  char temp_buffer[200];
  Serial.println("connecting to client now");
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }
  Serial.println("connected to client now, waiting for data");
  while(endBuffer == 0){
    char ch;
    uint8_t bufptr = 0;
    while (client.available()){
      ch = client.read(); // ch iterates over each character as it reads from client
      temp_buffer[bufptr++] = ch;
    }
    if (bufptr != 0){
      temp_buffer[bufptr] = '\0';
      strcpy(dataBuffer[endBuffer++], temp_buffer);
    }
  }
}


uint8_t state = 0; // 0 = connecting to client
//1 is connecting to mit guest

void loop(){
  if (state == 0){
    WiFi.begin(client_ssid, client_password);
    Serial.println("Connecting to Client");
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
    delay(2000);
    if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
      Serial.println("CONNECTED TO CLIENT!");
      Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
      delay(500);
      addToBuffer();
      // Connected to Berry Secure, now get data and store in buffer
      Serial.print("posting number of packets of data: ");
      Serial.println(endBuffer);
      Serial.println(dataBuffer[endBuffer]);
      state = 1;
    }
  } else if (state == 1) {
    sendingSetup();
    sendingBuffer();
    state = 0;
    Serial.println("Now sleeping");
    delay(1000);
    esp_sleep_enable_timer_wakeup(SLEEP_TIME * MICRO_S_TO_S);
    esp_deep_sleep_start();
  }
}
