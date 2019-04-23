#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

uint32_t counter; //used for timing

void setup() {
  Serial.begin(115200); // Set up serial port

  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_WHITE); //fill background
  tft.setTextColor(TFT_BLACK, TFT_WHITE); //set color for font

  counter = millis();

}

void loop() {
  if (millis() - counter > 1000) {
    tft.setCursor(0, 0, 1); //set cursor, font size 1
    char message[20]; //char buffer allocate
    sprintf(message, "Brightness: %2.4f     ", 1 - analogRead(A7) / 4096.0); //message about brightness
    tft.println(message); //print
    Serial.println(message);
    counter = millis();
  }
  //backlight.update(); //callupdate as fast as possible on our software PWM object
}
