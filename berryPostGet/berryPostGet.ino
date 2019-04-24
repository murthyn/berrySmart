#include <TinyGPS++.h>
#include <WiFi.h> //Connect to WiFi Network
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h> //Used in support of TFT Display
#include <string.h>  //used for some string handling and processing.

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

const int RESPONSE_TIMEOUT = 6000; //ms to wait for response from host
const int GETTING_PERIOD = 5000; //periodicity of getting a number fact.

const uint16_t IN_BUFFER_SIZE = 1000; //size of buffer to hold HTTP request
const uint16_t OUT_BUFFER_SIZE = 1000; //size of buffer to hold HTTP response
char request_buffer[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

char request_buffer2[IN_BUFFER_SIZE]; //char array buffer to hold HTTP request
char response_buffer2[OUT_BUFFER_SIZE]; //char array buffer to hold HTTP response

char query_string[50] = {0};
char message[500] = {0};
int state;


uint32_t dot_state = 0; // state of dot for selecting request
bool button_pressed_last = false; // for button1
bool button_pressed_last2 = false; // for button2

char network[] = "6s08";  //SSID for 6.08 Lab
char password[] = "iesc6s08"; //Password for 6.08 Lab

const uint8_t LOOP_PERIOD = 10; //milliseconds
uint32_t primary_timer = 0;
uint32_t posting_timer = 0;

const uint8_t PIN_1 = 27; //button 1

HardwareSerial gps_serial(2);
TinyGPSPlus gps;

class Button {
  public:
    uint32_t t_of_state_2;
    uint32_t t_of_button_change;
    uint32_t debounce_time;
    uint32_t long_press_time;
    uint8_t pin;
    uint8_t flag;
    bool button_pressed;
    uint8_t state; // This is public for the sake of convenience
    Button(int p) {
      flag = 0;
      state = 0;
      pin = p;
      t_of_state_2 = millis(); //init
      t_of_button_change = millis(); //init
      debounce_time = 10;
      long_press_time = 1000;
      button_pressed = 0;
    }
    void read() {
      uint8_t button_state = digitalRead(pin);
      button_pressed = !button_state;
    }
    int update() {
      flag = 0;
      read();
      if (state == 0) {
        if (button_pressed) {
          state = 1;
          t_of_button_change = millis();
        }
      } else if (state == 1) {
        if (button_pressed && (millis() - t_of_button_change) >= debounce_time) {
          state = 2;
          t_of_state_2 = millis();
        }
        else if (!button_pressed) {
          state = 0;
          t_of_button_change = millis();
        }

      } else if (state == 2) {
        if (!button_pressed) {
          state = 4;
          t_of_button_change = millis();
        }
        else if (button_pressed && (millis() - t_of_state_2) >= long_press_time) {
          state = 3;
        }

      } else if (state == 3) {
        if (!button_pressed) {
          state = 4;
          t_of_button_change = millis();
        }
      } else if (state == 4) {
        if (!button_pressed && (millis() - t_of_button_change) >= debounce_time) {
          state = 0;
          if ((millis() - t_of_state_2) < long_press_time) {
            flag = 1;
          }
          else if ((millis() - t_of_state_2) >= long_press_time) {
            flag = 2;
          }
        }
        else if (button_pressed && (millis() - t_of_state_2) <= long_press_time) {
          state = 2;
          t_of_button_change = millis();
        }
        else if (button_pressed && (millis() - t_of_state_2) >= long_press_time) {
          state = 3;
          t_of_button_change = millis();
        }
      }
      return flag;
    }
};

Button button1(PIN_1);

void setup() {
  tft.init();
  tft.setRotation(2);
  tft.setTextSize(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  Serial.begin(115200); //begin serial
  gps_serial.begin(9600, SERIAL_8N1, 32, 33);
  delay(100); //wait a bit (100 ms)
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
  pinMode(PIN_1, INPUT_PULLUP);

}

void loop() {
  uint8_t flag1 = button1.update();

  if (gps_serial.available()) {
    while (gps_serial.available())
      gps.encode(gps_serial.read());      // Check GPS
  }

  if (flag1 == 1) { //post
    float temp = (rand() % 100);
    float humid = (rand() % 100);
    float light = (rand()% 1000)/1000.0;
    

    Serial.println("PUSHED!");
    if (gps.location.isValid()) {
      Serial.println("GOOD DATA");
      char body[200]; //for body;
      sprintf(body, "lat=%f&lon=%f&temp=%f&humid=%f&light=%f", gps.location.lat(), gps.location.lng(), temp, humid, light); //generate body, posting to User, 1 step
      //sprintf(body, "lat=%f&lon=%f&temp=%f&humid=%f&light=%f", 100.0, 180.0, temp, humid, light); //generate body, posting to User, 1 step
      int body_len = strlen(body); //calculate body length (for header reporting)
      Serial.println(body);
      sprintf(request_buffer, "POST http://608dev.net/sandbox/sc/irin/berryhandler.py HTTP/1.1\r\n");
      strcat(request_buffer, "Host: 608dev.net\r\n");
      strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
      sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); //append string formatted to end of request buffer
      strcat(request_buffer, "\r\n"); //new line from header to body
      strcat(request_buffer, body); //body
      strcat(request_buffer, "\r\n"); //header
      Serial.println(request_buffer);
      do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
      tft.fillScreen(TFT_BLACK); //fill background
      tft.setCursor(0, 0, 1); // set the cursor
      char body1[200]; //for body;
      sprintf(body1, "lat=%f        lon=%f       temp=%f       humid=%f      light=%f", gps.location.lat(), gps.location.lng(), temp, humid, light); //generate body, posting to User, 1 step
      tft.println(body1);
    }
  }


  while (millis() - primary_timer < LOOP_PERIOD); //wait for primary timer to increment
  primary_timer = millis();
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
