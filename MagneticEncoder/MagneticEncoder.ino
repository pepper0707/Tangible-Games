//
//    FILE: AS5600_demo.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: demo
//     URL: https://github.com/RobTillaart/AS5600
//
//  Examples may use AS5600 or AS5600L devices.
//  Check if your sensor matches the one used in the example.
//  Optionally adjust the code.


#include "AS5600.h"
#include <Adafruit_NeoPixel.h>
//Magnetic Encoder Stuff:
//  Uncomment the line according to your sensor type
//AS5600L as5600;   //  use default Wire
AS5600 as5600;   //  use default Wire
int magnetStatus = 0; //value of the status register (MD, ML, MH)
int magnetStrength = 0;

//Led Stuff:
#define MAX_FLASHING_LEDS 3
#define LED_LIFESPAN 4000
#define LED_SPAWN_MIN 1000  //min spawn delay 
#define LED_SPAWN_MAX 3000  //max spawn delay 

unsigned long randomSpawnRate = 0;  //stores next random spawn time

const int neoPixelPin = 5;  //control pin for the LED strip
const int numPixels = 60;   //number of pixels in the strip


struct FlashingLED {
  bool active = false;  
  int pixel;            //pixel number for the LED
  unsigned long startTime;   //start time of LED flash
  unsigned long lastToggleTime;   //last time the LED was toggled
  int delayTime;             //time between toggles
  bool ledOn;                
  uint32_t color;            
};

FlashingLED flashingLeds[MAX_FLASHING_LEDS];  //array for active LEDs
//activeLedVector<FlashingLED> vector(flashingLeds);

// initialize the LED strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neoPixelPin, NEO_GRB + NEO_KHZ800);

int usedCount = 0;
int used[MAX_FLASHING_LEDS];

bool isClockwise;
float lastAngle;

unsigned long previousMillis = 0;
unsigned long clockwiseCheckInterval = 500;

void setup()
{
  //led:
  randomSeed(analogRead(0));  //gets random seed using random data from empty analog pin
  
  randomSpawnRate = 1000;  //first LED spawns after 1 second
  
  strip.begin();    //initialize the LED strip
  strip.clear();    //turn off all LEDs
  strip.show();     //update the strip

  //magnetic
  while(!Serial);
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("AS5600_LIB_VERSION: ");
  Serial.println(AS5600_LIB_VERSION);
  Serial.println();

  Wire.begin();

  as5600.begin(4);  //  set direction pin.
  as5600.setDirection(AS5600_CLOCK_WISE);  //  default, just be explicit.
  int b = as5600.isConnected();
  Serial.print("Connect: ");
  Serial.println(b);
  delay(1000);

  //checkMagnetPresence();
}


void loop()
{
  //led:

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > clockwiseCheckInterval) {
    previousMillis = currentMillis;
    lastAngle = as5600.rawAngle() * AS5600_RAW_TO_DEGREES;
  }


  if ((as5600.rawAngle() * AS5600_RAW_TO_DEGREES) < lastAngle){
    isClockwise = true;
  }else if ((as5600.rawAngle() * AS5600_RAW_TO_DEGREES) > lastAngle){
    isClockwise = false;
  }
  // Serial.print("last Angle: ");
  // Serial.println(lastAngle);
  // Serial.print("current angle: ");
  // Serial.println(as5600.rawAngle() * AS5600_RAW_TO_DEGREES);
  Serial.print("Clockwise: ");
  Serial.println(isClockwise);
  updateFlashingLEDs();  //update all flashing LEDs

  

  static unsigned long lastTrigger = 0;
  if (millis() - lastTrigger > randomSpawnRate) {  //check if its time to spawn a new LED
    lastTrigger = millis();
    startFlashingLED(GetRandomPixel());  //start the LED flashing
  }
  
  
  randomSpawnRate = random(LED_SPAWN_MIN, LED_SPAWN_MAX); //randomize the next spawn rate 

  //magnetic:
  //  Serial.print(millis());
  //  Serial.print("\t");
  ///Serial.print(as5600.readAngle());
  ///Serial.print("\t");
  //Serial.println(as5600.rawAngle());
  ////Serial.println(as5600.rawAngle() * AS5600_RAW_TO_DEGREES);

  int aimedAtLed = (as5600.rawAngle() * AS5600_RAW_TO_DEGREES) / 6;
  
  for( int i = 0; i < usedCount; i++){
    if(flashingLeds[i].pixel >= aimedAtLed - 1 && flashingLeds[i].pixel <= aimedAtLed +1){


      Serial.println("HIT!!!!!!!!!!!!!!!!");
      flashingLeds[i].active = false;
      strip.setPixelColor(flashingLeds[i].pixel, 0);  //turn off LED
      flashingLeds[i].ledOn = false;

      strip.show();
      usedCount--;
      used[i] = 0;
      
    }
  }
  
  delay(100);
}

int GetRandomPixel(){
  //get a random pixel and make sure its not been used recently:
  int randomPixel;
  bool isDuplicate;

  do {
    randomPixel = random(0, numPixels);  //generate a random pixel
    isDuplicate = false;

    //check if the pixel has been used
    for (int i = 0; i < usedCount; i++) {
      if (used[i] == randomPixel) {
        isDuplicate = true;
        break;
      }
    }
  } while (isDuplicate);  //loop until randomPixel is a pixel not in the used array


  //add randomPixel to used array
  if (usedCount < MAX_FLASHING_LEDS) {
    used[usedCount++] = randomPixel;
  } else {
    //reset the array when full
    memset(used, 0, sizeof(used));
    usedCount = 0;
    used[usedCount++] = randomPixel;
  }
  return randomPixel;
}

void startFlashingLED(int pixel) {
  for (int i = 0; i < MAX_FLASHING_LEDS; i++) { //find an inactive LED slot
    if (!flashingLeds[i].active) {
      flashingLeds[i].active = true;  
      flashingLeds[i].pixel = pixel;  
      flashingLeds[i].startTime = millis();  //record the start time
      flashingLeds[i].lastToggleTime = millis();  //record the toggle time
      flashingLeds[i].delayTime = 500;  
      flashingLeds[i].ledOn = false;  
      flashingLeds[i].color = (random(2) == 0) ? strip.Color(255, 0, 0) : strip.Color(0, 0, 255);  //randomly pick blue or red
      return;  
    }
  }
  //no available slots
}

void updateFlashingLEDs() {
  unsigned long currentTime = millis();  //get current time

  for (int i = 0; i < MAX_FLASHING_LEDS; i++) {
    if (!flashingLeds[i].active) continue;  //skip inactive LEDs

    FlashingLED &led = flashingLeds[i];  //reference to the current LED

    if (currentTime - led.startTime >= LED_LIFESPAN) {  //check if LED lifespan is up
      strip.setPixelColor(led.pixel, 0);  //turn off LED
      led.ledOn = false;
      strip.show();  // update the strip
      tone(8, 100, 100);  //make sound when LED is missed
      led.active = false; 
      for (int j = 0; j < usedCount; j++){
        if (used[j] == led.pixel)
        {
          used[j] = 0;
          usedCount--;
        }

      }

      continue;
    }

    if (currentTime - led.lastToggleTime >= led.delayTime) {  //check if its time to toggle
      led.lastToggleTime = currentTime;  //update the last toggle time
      led.ledOn = !led.ledOn;  //toggle the LED state

      if (led.ledOn) {
        strip.setPixelColor(led.pixel, led.color);  //LED on
      } else {
        strip.setPixelColor(led.pixel, 0); //LED off
        led.delayTime = max(30, led.delayTime * 0.7);  //decrease the delay to flash faster and faster
      }
      strip.show(); 
    }
  }
}

void checkMagnetPresence()
{  
  //This function runs in the setup() and it locks the MCU until the magnet is not positioned properly

  while((magnetStatus & 32) != 32) //while the magnet is not adjusted to the proper distance - 32: MD = 1
  {
    magnetStatus = 0; //reset reading

    Wire.beginTransmission(0x36); //connect to the sensor
    Wire.write(0x0B); //figure 21 - register map: Status: MD ML MH
    Wire.endTransmission(); //end transmission
    Wire.requestFrom(0x36, 1); //request from the sensor

    while(Wire.available() == 0); //wait until it becomes available 
    magnetStatus = Wire.read(); //Reading the data after the request

    Serial.print("Magnet status: ");
    Serial.println(magnetStatus, BIN); //print it in binary so you can compare it to the table (fig 21)      
  }
  //Status register output: 0 0 MD ML MH 0 0 0  
  //MH: Too strong magnet - 100111 - DEC: 39 
  //ML: Too weak magnet - 10111 - DEC: 23     
  //MD: OK magnet - 110111 - DEC: 55

  Serial.println("Magnet found!");
  delay(1000); 
}      