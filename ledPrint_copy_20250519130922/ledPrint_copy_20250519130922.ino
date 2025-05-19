/*!
 * @file ledPrint.ino
 * @brief Display experiment of the digital tube
 * @n Experiment phenomenon: The digital tube displays "HALO"，and in one second, displays "H.A.L.O.", then show value of the variable val after one second
 * @copyright   Copyright (c) 2010 DFRobot Co.Ltd (http://www.dfrobot.com)
 * @license     The MIT License (MIT)
 * @author [Actor](wenzheng.wang@dfrobot.com),[TangJie](jie.tang@dfrobot.com)
 * @version  V1.0.1
 * @data  2022-03-21
 * @url https://github.com/DFRobot/DFRobot_LedDisplayModule
 */
#include "DFRobot_LedDisplayModule.h"
/**
 * DFRobot_LedDisplayModule Constructor
 * Parameter &wire  Wire
 * In default, the IIC address of 8 bits digital tube is 0xE0
 * The IIC address of 8 bits digital tube can be changed by combining A1 and A0
 * 1  1  1  0  | 0  A1 A0 0
 * 1  1  1  0  | 0  0  0  0    0xE0
 * 1  1  1  0  | 0  0  1  0    0xE2
 * 1  1  1  0  | 0  1  0  0    0xE4
 * 0  0  1  0  | 0  1  1  0    0xE6
 */ 
//DFRobot_LedDisplayModule LED(&Wire, 0xE0);

/**
 * DFRobot_LedDisplayModule Constructor
 * In default, the IIC address of 4 bits digital tube is 0x48 
 */
DFRobot_LedDisplayModule LED(&Wire, 0x48);

unsigned long startTime;
const unsigned long countdownDuration = 60000; // 60 seconds in milliseconds
unsigned long previousPrint = 0;
const unsigned long printInterval = 100; // print every 100 ms
bool timerRunning = true;

void setup() 
{
  Serial.begin(115200);
  /*
   * Wait for the chip to be initialized completely, and then exit.
   * Select several bits for initialization, e8Bit for 8 bits and e4Bit for 4 bits.
   */
  while(LED.begin(LED.e4Bit) != 0)
  {
    Serial.println("Failed to initialize the chip , please confirm the chip connection!");
    delay(1000);
  }
  Serial.println("nitialized the chip");
  /**
   * Set the display area to 1, 2, 3, 4
   * It can show 4 bits, the region of each parameter is 1~4 
   * Please resend the display value if the display area is changed
   */
  LED.setDisplayArea(1,2,3,4);
  startTime = millis(); // Start the timer
}

void loop() 
{
  if (timerRunning) {
    unsigned long currentMillis = millis();
    unsigned long elapsed = currentMillis - startTime;

    // Only print every 100 ms
    if (currentMillis - previousPrint >= printInterval) {
      previousPrint = currentMillis;

      float remaining = (countdownDuration - elapsed) / 1000.0;
      if (remaining < 0) remaining = 0;

      Serial.print("Remaining: ");
      Serial.print(remaining, 2); // 1 decimal place
      Serial.println(" s");
      LED.print(remaining);

      // If time's up
      if (elapsed >= countdownDuration) {
        timerRunning = false;
        Serial.println("Timer done!");
      }
    }
  }
  //double val = 13.25;
  //LED.print(val);
  //delay(1000);
}
