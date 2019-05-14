#include <WiFi.h>

const char* client_ssid     = "Wifi Range Checker";
const char* client_password = "123456789";

const char* host = "192.168.4.1"; // need to find out what is each host id
const int port = 80;
int counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFi.begin(client_ssid, client_password);
  Serial.println("Connecting to Client.");
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  delay(2000);
  if (WiFi.isConnected()){
    Serial.println("Connected to Client.");
    while (WiFi.isConnected()){
      Serial.println(counter);
      counter += 1;
    }
  }
}
