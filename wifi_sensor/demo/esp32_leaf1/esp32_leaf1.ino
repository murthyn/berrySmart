// Load Wi-Fi library
#include <WiFi.h>
#include <dht.h> //https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTstable

// Replace with your network credentials
const char* server_ssid     = "Berry Secure 12";
const char* server_password = "123456789";

WiFiServer server(80); // Set web server port number to 80

// Variable to store the HTTP request
const int espID = 1;
RTC_DATA_ATTR int packetNumber = 1;

dht DHT;
#define DHT11_PIN 19

const float SLEEP_TIME = 20; // in seconds
const float MICRO_S_TO_S = 1000000; // conversion factor

const float moisture_upper = 0.84;
const float moisture_lower = 0.44;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP #1");
  
  // Connect to Wi-Fi network with SSID and password
  Serial.println("Setting AP (Access Point)...");
  WiFi.softAP(server_ssid, server_password); // Remove password parameter to make AP open
  IPAddress IP = WiFi.softAPIP();
  Serial.println("AP IP address: ");
  Serial.print(IP);
  Serial.println('\n');

  server.begin(); 
  Serial.println("Looking for client.");
}

void loop(){

  WiFiClient client = server.available();   // Listen for incoming clients
  
  if (client) {                             // If a new client connects,
    Serial.println("Client found.");        // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    if (client.connected()) {               // loop while the client's connected
        Serial.println("Client connected; reading sensors.");
        char message[500];

        // Get sensor readings
        float light = 1 - analogRead(A7) / 4096.0;
//        float soil_moisture = analogRead(A14) / 4096.0;
        float soil_moisture = moisture_upper/(moisture_upper - moisture_lower) - (analogRead(A14) / 4096.0)/(moisture_upper - moisture_lower);
        if (soil_moisture > 1.0){
          soil_moisture = 1.0;
        }
        if (soil_moisture < 0.0){
          soil_moisture = 0.0;
        }
        DHT.read11(DHT11_PIN);
        delay(2000);
        float temp = DHT.temperature;
        delay(2000);
        float humid = DHT.humidity;
        sprintf(message, "{'espID': %d, 'packet_number': %d, 'soil_moisture': %2.4f, 'temp': %2.4f, 'humid': %2.4f, 'light': %2.4f}", espID, packetNumber, soil_moisture, temp, humid, light);
        packetNumber += 1;
        client.println(message);
        Serial.println(message);

        delay(500);
        Serial.println("Packet sent.");
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
