#include <SPI.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

class PWM_608
{
  public:
    int pin; //digital pin class controls
    int period; //period of PWM signal (in milliseconds)
    int on_amt; //duration of "on" state of PWM (in milliseconds)
    PWM_608(int op, float frequency); //constructor op = output pin, frequency=freq of pwm signal in Hz
    void set_duty_cycle(float duty_cycle); //sets duty cycle to be value between 0 and 1.0
    void update(); //updates state of system based on millis() and duty cycle
};

//add your PWM_608 class definitions here:
PWM_608::PWM_608(int op, float frequency){
  period = (int) 1000.0/frequency;
  pin = op;
}

void PWM_608::update(){
  if (millis() % period < on_amt) {
    // ON
    digitalWrite(pin,1);
  }
  else {
    // OFF
    digitalWrite(pin,0);
  }
}
void PWM_608::set_duty_cycle(float duty_cycle){
  float clipped_duty_cycle;
  if (duty_cycle < 0) {
    clipped_duty_cycle = 0;
  }
  else if (duty_cycle > 1) {
    clipped_duty_cycle = 1;
  }
  else{
    clipped_duty_cycle = duty_cycle;
  }
  on_amt = (int) (clipped_duty_cycle * period);
}


uint32_t counter; //used for timing
const uint32_t pwm_channel = 0; //hardware pwm channel used in secon part

PWM_608 backlight(12, 50); //create instance of PWM to control backlight on pin 12, operating at 50 Hz

void setup() {
  Serial.begin(115200); // Set up serial port

  //For Second part (uncomment when get to )
  ledcSetup(pwm_channel, 70, 12);//create pwm channel, @50 Hz, with 12 bits of precision
  ledcAttachPin(14, pwm_channel); //link pwm channel to IO pin 14

  tft.init();  //init screen
  tft.setRotation(2); //adjust rotation
  tft.setTextSize(1); //default font size
  tft.fillScreen(TFT_WHITE); //fill background
  tft.setTextColor(TFT_BLACK, TFT_WHITE); //set color for font
  pinMode(12, OUTPUT); //controlling TFT with our PWM controller (first part of lab)
  pinMode(14, OUTPUT); //controlling TFT with hardware PWM (second part of lab)

  counter = millis();

}

void loop() {
  if (millis() - counter > 100) {
    tft.setCursor(0, 0, 1); //set cursor, font size 1
    char message[20]; //char buffer allocate
    sprintf(message, "%2.4f V     ", analogRead(A3) * 3.3 / 4096); //message about volts
    tft.println(message); //print
    Serial.println(backlight.on_amt);
    //backlight.set_duty_cycle(1 - analogRead(A3) / 4096.0);

    //------------
    //Add line(s) here to implement a Hardware PWM on pin 14
    ledcWrite(pwm_channel, 4095 - analogRead(A3));
    //------------
    counter = millis();
  }
  //backlight.update(); //callupdate as fast as possible on our software PWM object
}
