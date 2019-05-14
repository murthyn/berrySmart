#include <WiFi.h>

const char* server_ssid = "Wifi Range Checker";
const char* server_password = "123456789";

WiFiServer server(80);
int counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.softAP(server_ssid, server_password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();
  if (client){
    Serial.println("Client found.");
    while (client.connected()){
      Serial.println(counter);
      counter += 1;
      delay(500);
    }
    client.stop();
    delay(500);
    Serial.println("Client disconnected.");
  }
}
