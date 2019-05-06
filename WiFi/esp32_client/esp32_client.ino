#include <WiFi.h>

//----------WIFI----------//

/* change ssid and password according to yours WiFi*/
const char* ssid     = "Berry Secure";
const char* password = "123456789";
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

//char network[] = "MIT GUEST";  //SSID for 6.08 Lab
//char pswd[] = ""; //Password for 6.08 Lab
char network[] = "iPhone (2)";  //SSID for 6.08 Lab
char pswd[] = "hello123"; //Password for 6.08 Lab

void setup()
{
  Serial.begin(115200);
  clientSetup();
}

void loop()
{
  delay(1000);
  Serial.print("connecting to ");
  Serial.println(host);
  /* Use WiFiClient class to create TCP connections */
  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    return;
  }
  uint8_t data[30];
  if (client) {
    Serial.println("new client");
    /* check client is connected */
    while (client.connected()) {
//      Serial.println("connected");
      if (client.available()) {
        Serial.println("available");
        int len = client.read(data, 30);
        if (len < 5) {
          continue; //get rid of near empty messages
        }
        else if (len < 30) {
          data[len] = '\0';
        } else {
          data[30] = '\0';
        }
        Serial.print("client sent: ");
        Serial.println((char *)data);

        strcpy(dataBuffer[endBuffer], (char *)data);
        endBuffer++;
      }

      if (endBuffer > 10) {
        client.stop();
        wifiSetup();
        post();
        client.stop();
        clientSetup();
      }
    }
  }
}






// HELPER FUNCTIONS

void clientSetup() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* connect to your WiFi */
  WiFi.begin(ssid, password);
  /* wait until ESP32 connect to WiFi*/
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Connecting to WiFi..");
  }
  Serial.println("");
  Serial.println("WiFi connected with IP address: ");
  Serial.println(WiFi.localIP());
}

void wifiSetup() {
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
}

void post() {
  for (int i = 0; i < endBuffer; i++) {
    Serial.println("Posting!");
    char body[200]; //for body;
    sprintf(body, "text=%s", dataBuffer[i]);
    
    int body_len = strlen(body); //calculate body length (for header reporting)
    Serial.println(body);
    sprintf(request_buffer, "POST http://608dev.net/sandbox/sc/garciag/StrawberryHandler.py HTTP/1.1\r\n");
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
