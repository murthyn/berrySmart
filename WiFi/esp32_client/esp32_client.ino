#include <WiFi.h>

//----------WIFI----------//

/* change ssid and password according to yours WiFi*/
const char* client_ssid     = "Berry Secure 2";
const char* client_password = "123456789";
/*
   This is the IP address of your PC
   [Wins: use ipconfig command, Linux: use ifconfig command]
*/
const char* host = "192.168.4.1"; // need to find out what is each host id
const int port = 80;

//----------DATA----------//

char dataBuffer [30][30]; // [Number of Strings][Max Size of Strings]
int endBuffer = 0;

//----------POSTING----------//
 
const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 5000; //periodicity of getting a number fact.

const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

const char* server_ssid = "6s08";  //SSID for 6.08 Lab
const char* server_password = "iesc6s08"; //Password for 6.08 Lab


void setup()
{
  Serial.begin(115200);
//  clientSetup();
//  wifiSetup();
}

// HELPER FUNCTIONS
void post() {
  for (int i = 0; i < endBuffer; i++) {
    Serial.println("Posting!");
    char body[200]; //for body;
    sprintf(body, "text=%s", dataBuffer[i]);
    
    int body_len = strlen(body); //calculate body length (for header reporting)
    Serial.println(body);
    sprintf(request_buffer, "POST http://608dev.net/sandbox/sc/garciag/strawberryHandler.py HTTP/1.1\r\n");
    strcat(request_buffer, "Host: 608dev.net\r\n");
    strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
    sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
    strcat(request_buffer, "\r\n"); //new line from header to body
    strcat(request_buffer, body); //body
    strcat(request_buffer, "\r\n"); //header
    Serial.println(request_buffer);
    do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  }
  endBuffer = 0;
  emptyBuffer();
}

void emptyBuffer() {
  for(int i = 0; i < 30; i++){
      strcpy(dataBuffer[i], "");
  }
}

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient clientPost; //instantiate a client object
  if (clientPost.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    clientPost.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (clientPost.connected()) { //while we remain connected read out data coming back
      clientPost.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (clientPost.available()) { //read out remaining text (body of response)
      char_append(response, clientPost.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    clientPost.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    clientPost.stop();
  }
}  


void addToBuffer(){
  WiFiClient client;
  char temp_buffer[30];
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
    while(client.available()){
      ch = client.read();
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
    WiFi.begin(server_ssid, server_password);
    Serial.println("Connecting to Server");
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
    delay(2000);
    if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
      Serial.println("CONNECTED TO SERVER!");
      Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
      delay(500);
      // state = 0; only if data successfully send
      post();
      Serial.println("posted");
      state = 0;
    }
  }
}


