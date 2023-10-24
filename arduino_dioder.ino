// Inspired by IKEA DIODER LED Strip Light
// Added brightness control and all controlled by a rotary.
// No smooth and fast color change effects. Maybe in the next versions.
// https://github.com/ricardomiguelcardoso/arduino_dioder.git

#include <FastLED.h>
#include <Button2.h>
#include <ESPRotary.h>

#define NUM_LEDS 16
#define DATA_PIN 5
#define ROTARY_PIN1 8
#define ROTARY_PIN2 7
#define BUTTON_PIN  6
#define NCLICKS 4


/////////////////////////////////////////////////////////////////

CRGBArray<NUM_LEDS> leds;
ESPRotary rotary = ESPRotary(ROTARY_PIN1, ROTARY_PIN2,NCLICKS);
Button2 b_rotary = Button2(BUTTON_PIN);

/////////////////////////////////////////////////////////////////

uint8_t brightness = 0;
bool stateLeds = 0;
uint8_t red = 255;
uint8_t green = 0;
uint8_t blue = 0;
bool buttonMode = 0;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);

  rotary.setLeftRotationHandler(showDirectionL);
  rotary.setRightRotationHandler(showDirectionR);

  b_rotary.setClickHandler(changeButtonMode);
  b_rotary.setLongClickHandler( toggleLeds);
  updateLedState();
}


void showDirectionL(ESPRotary& r) {
  if (buttonMode) {
    setLed(0);
  } else {
    setLedBrightness(0);
  }
}

void showDirectionR(ESPRotary& r) {
  if (buttonMode) {
    setLed(1);
  } else {
    setLedBrightness(1);
  }
}

void changeButtonMode(Button2& btn) {

  buttonMode = !buttonMode;

  Serial.print("Mode: ");
  if(buttonMode==0){
    Serial.println("Brightness.");
  }else{
    Serial.println("Color select.");
  }

}

void loop() {
  rotary.loop();
  b_rotary.loop();
}

void toggleLeds(Button2& btn) {
  stateLeds = !stateLeds;

  if (!stateLeds) {
     fill_solid( leds, NUM_LEDS, CRGB(0, 0, 0));
     FastLED.show();
     Serial.println("Leds OFF.");
  } else {
    Serial.println("Leds ON.");
    updateLedState();
  }
}

void updateLedState() {
  fill_solid( leds, NUM_LEDS, CRGB(red, green, blue));
  FastLED.setBrightness(brightness);
  FastLED.show();
}

uint8_t brightPot = 0;
void setLedBrightness(bool dir) {
  stateLeds = 1;
  float divisions = 255.0 / 80.0;
  if (dir) {
    if (brightPot == 80) {
      Serial.println("Max BRT");
      return;
    }
    brightPot+=(NCLICKS);
  } else {
    if (brightPot == 0) {
      Serial.println("Min BRT");
      return;
    }
    brightPot-=(NCLICKS);
  }

  brightness = uint8_t(brightPot * divisions);
  
  Serial.print("BRT: ");
  Serial.println(brightness);
  
  updateLedState();
}


uint8_t ledPot = 0;
void setLed(bool dir) {

  stateLeds = 1;
  if (brightness == 0) {
    brightness = 12;
  }

  if (dir) {
    if (ledPot == 160) {
      return;
    }
    
    ledPot+=(NCLICKS);
  } else {
    if (ledPot == 0) {
      return;
    }
    ledPot-=(NCLICKS);
  }

  red = 255;

  int step1 = 0;
  int step2 = 27;
  int step3 = 54;
  int step4 = 81;
  int step5 = 108;
  int step6 = 135;
  int step7 = 159;

  if (ledPot < step1) {
    red = 255;
    green = 0;
    blue = 0;
  } else if (ledPot >= step1 && ledPot <= step2 )
  {
    //R 255, G 0, B 0-255
    red = 255;
    green = 0;
    blue = calcValue(ledPot, step1, step2);

  } else if (ledPot > step2 && ledPot <= step3 )
  {
    //R 255-0, G 0, B 255
    red = 255 - calcValue(ledPot, step2, step3);
    green = 0;
    blue = 255;

  } else if (ledPot > step3 && ledPot <= step4 )
  {
    //R 0, G 0-255, B 255
    red = 0;
    green = calcValue(ledPot, step3, step4);
    blue = 255;

  } else if (ledPot > step4 && ledPot <= step5 )
  {
    //R 0, G 255, B 255-0
    red = 0;
    green = 255;
    blue = 255 - calcValue(ledPot, step4, step5);
  } else if (ledPot > step5 && ledPot <= step6 )
  {
    //R 0-255, G 255, B 0
    red = calcValue(ledPot, step5, step6);
    green = 255;
    blue = 0;
  } else if (ledPot > step6 && ledPot <= step7 )
  {
    //R 255, G 255, B 0-255
    red = 255;
    green = 255;
    blue = calcValue(ledPot, step6, step7);

  } else if (ledPot > step7) {
    red = 255;
    green = 255;
    blue = 255;
  }

  Serial.print("RGB: ");
  Serial.print(red);  
  Serial.print(" ");
  Serial.print(green);  
  Serial.print(" ");
  Serial.println(blue);

  updateLedState();
}

int calcValue(float pot, int step1, int step2) {
  int diff = step2 - step1;
  float divisions = 255.0 / float(diff);
  return   int((pot - float(step1)) * divisions);
}
