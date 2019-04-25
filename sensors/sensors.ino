#include <SPI.h>
#include <TFT_eSPI.h>
#include <dht.h>

dht DHT;
TFT_eSPI tft = TFT_eSPI();

#define DHT11_PIN 19

uint32_t counter; //used for timing

// https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTstable

void setup() {
  Serial.begin(115200); // Set up serial port

  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_WHITE); //fill background
  tft.setTextColor(TFT_BLACK, TFT_WHITE); //set color for font

  //pinMode(DHT11_PIN, INPUT_PULLUP);
  
  counter = millis();

}

void loop() {
  if (millis() - counter > 3000) {
    tft.setCursor(0, 0, 1); //set cursor, font size 1
    
    char message[100]; //char buffer allocate
    sprintf(message, "Brightness: %2.4f     ", 1 - analogRead(A7) / 4096.0); //message about brightness
    tft.println(message); //print
    Serial.println(message);

    tft.setCursor(0, 50, 1); //set cursor, font size 1

    int chk = DHT.read11(DHT11_PIN);
    Serial.print("Temperature = ");
    Serial.println(DHT.temperature);
    Serial.print("Humidity = ");
    Serial.println(DHT.humidity);

    char message2[100]; //char buffer allocate
    sprintf(message2, "Temp/humidity: %2.4f %2.4f", DHT.temperature, DHT.humidity); //message about brightness
    tft.println(message2); //print
    
    counter = millis();
  }
}
