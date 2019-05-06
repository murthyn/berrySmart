/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "Berry Secure";
const char* password = "123456789";

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
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
}

void loop() {
  float temp = (rand() % 100);
  float humid = (rand() % 100);
  float light = (rand() % 1000) / 1000.0;
  float moist = (rand() % 1000);

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
        char message[200];
        sprintf(message, "lat=%f&lon=%f&temp=%f&humid=%f&light=%f&moist=%f", 100.0, -45.89, temp, humid, light, moist); //generate body, posting to User, 1 step
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

  }
}
