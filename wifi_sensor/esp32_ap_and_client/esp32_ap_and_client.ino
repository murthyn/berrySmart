#include <WiFi.h>

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

char dataBuffer [30][200]; // [Number of Strings][Max Size of Strings]
int endBuffer = 0;

//----------POSTING----------//
 
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 5000; //periodicity of getting a number fact.

const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

const char* server_ssid = "Berry Secure 2";  //SSID for 6.08 Lab
const char* server_password = "123456789"; //Password for 6.08 Lab

WiFiServer server(80);

void setup()
{
  Serial.begin(115200);
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

  Serial.println("CLIENT");
  Serial.println(client);
  
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
      }
      else {
        char message[200];
        sprintf(message, "%s", dataBuffer[0]);
        client.println(message);
        delay(100);
        // Break out of the while loop
        break;
      }
    }
    
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    endBuffer = 0;
    emptyBuffer();
    ESP.restart();
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
    Serial.println("while endBuffer == 0");
    char ch;
    uint8_t bufptr = 0;
    if (client.available()){
      ch = client.read();
      Serial.println("here");
      Serial.println(ch);
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
      state = 1;
    }
  } else if (state == 1) {
    sendingSetup();
    sendingBuffer();
    state = 0;

    Serial.println("posted");
//    WiFi.begin(server_ssid, server_password);
//    Serial.println("Connecting to Server");
//    while (WiFi.status() != WL_CONNECTED){
//      delay(500);
//      Serial.print(".");
//    }
//    delay(2000);
//    if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
//      Serial.println("CONNECTED TO SERVER!");
//      Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
//      delay(500);
//      // state = 0; only if data successfully send
//      sendingSetup();
//      sendingBuffer();
//      Serial.println("posted");
//      state = 0;
//    }
  }
}
