# Technical documentation

## Game Overview

### Game Flow
1.	Idle State: Displays LED strip animation and high score. Waits for button input.
2.	Start Game: Player selects difficulty, then a countdown begins.
3.	Gameplay:  
•	LEDs flash on the strip  
•	Collect LEDs by aiming and rotating in the correct direction.  
•	Special color LEDs require two rotations.  
•	Score and combo are updated; missed LEDs reset combo.  
4.	End Game: If the score beats the high score, the player enters their name.
5.	High Score: Stored in EEPROM for persistence.

### List Of Used Components

- Arduino Uno: Microcontroller
-	AS5600 Magnetic Encoder: Detects the angle of rotation.
-	Adafruit NeoPixel Strip: with 60 individually addressable RGB LEDs.
-	TM1637 Segment Displays: Three 4-digit displays for showing score, combo, and timer.
- Arcade Pushbutton: Used for input (starting game, confirming name selection and difficulty, holding to reset high score).
- 3" Speaker - 4 Ohm 3 Watt
- Adafruit PAM8302 Audio Amplifier: Used in conjuction with speaker
- Potentiometer - Used in conjuction with speaker

### Dependencies
•	[AS5600 library](https://github.com/RobTillaart/AS5600)  
•	[Adafruit NeoPixel library](https://github.com/adafruit/Adafruit_NeoPixel)  
•	[TM1637 library](https://github.com/avishorp/TM1637)  
•	[PCM library](https://github.com/damellis/PCM)  

## Key software sections and challanges

### Converting from angle to pixel index

To determine which LED the marker on the record was pointing toward, we needed to convert the angle provided by our magnetic encoder into a corresponding LED index. This was achieved using the following lines of code:
``` c++
  aimedAtLed = round((angle / 360.0) * 61) % 61;
  aimedAtLed = (numPixels)-aimedAtLed;  // Reverse direction
  aimedAtLed -= 1;

  aimedAtLed = (aimedAtLed + numPixels) % numPixels; // Account for wrap around
```
Our strip has 60 pixels but at the point where the beginning of the strip meets the end of the strip we made sure the gap was consistent with the spacing between the LEDs. We treated this gap as if it were a 61st “blank” LED, which is why you'll see the value 61 used in our calculations. This step was crucial in maintaining accurate synchronization between the angle data from the magnetic encoder and the corresponding LED index.

We had to reverse the direction because the LED strip was accidentally mounted so that its indices increased in an anti-clockwise direction. Additionally, we needed to ensure that the LED index wrapped around correctly when passing the point where the strip’s ends meet. This was handled using the modulo operation.

### Determining direction

The other critical input we needed from the player was the direction in which they were rotating the record(clockwise or anti-clockwise). To determine this, we compared the current angle from the magnetic encoder with a previous angle value.

Initially, we attempted to track direction by simply storing the previous angle in the main loop and comparing it to the current angle on each iteration. However, during testing we found this method too inconsistent due to noise and small fluctuations in the data.

To address this, we implemented our own timed delay between comparisons to smooth out the readings and ensure more reliable direction detection. This approach involves checking the angle at defined intervals rather than every frame, which greatly improved stability. The implementation looks like this:
``` c++
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > clockwiseCheckInterval) {
    previousMillis = currentMillis;
    lastAngle = angle;
  }

  float angleDelta = angle - lastAngle;

  if (angleDelta > 180.0) {
    angleDelta -= 360.0;
  } else if (angleDelta < -180.0) {
    angleDelta += 360.0;
  }

  isClockwise = angleDelta > 0;
  isClockwise = !isClockwise;
```

### Managing LEDs

One of the first software hurdles to overcome was the spawning and management of multiple different LEDs at the same time, each counting down at their own rates and with other different variables associated with them. The clear approach was to use a struct, which is called `FlashingLED`. This struct encapsulates all the state and timing information needed for each individual LED, such as whether it is active, its color, position on the strip, and timing variables for both its lifespan and blink rate and more. By maintaining an array of FlashingLED structs, the we can efficiently manage several independent LEDs, updating their states together in the `updateFlashingLEDs()` function which is called in the main game loop. This design also makes it straightforward for us to extend the game with new LED behaviors or interaction rules in the future and something we did continually through development. Here is what that struct looks like:
``` c++
// Structure for a flashing LED
struct FlashingLED {
  // State flags
  bool active = false;            // Whether this LED is currently active
  bool ledOn;                     // Current LED illumination state
  bool beenPassed;                // Whether LED has been passed but not collected
  bool passedClockwise;           // Direction LED was passed in
  bool hasLeftPixel;              // Whether we've moved away after passing
  
  // Position information
  uint8_t pixel;                  // LED position on the strip
  
  // Visual properties
  uint32_t color;                 // LED color
  
  // Timing variables
  unsigned long startTime;        // When LED was spawned
  unsigned long lastToggleTime;   // Last time LED was toggled
  unsigned long passedTime;       // When LED was passed
  int delayTime;                  // Current blinking interval
};
```
### Spawning Logic

An issue that was quickly fixed early on was our random spawning having the same pattern each run, this was fixed by getting a seed from random data output from an empty analog pin using: 
``` c++
 randomSeed(analogRead(0));          // Gets random seed using random data from empty analog pin
```
LED spawning was originally handled just randomly, with only a basic check to prevent LEDs from appearing in the exact same position. However, through playtesting, we identified several improvements that could make the game feel more intuitive and engaging.

A simple but effective change was to prevent LEDs from spawning within a small range of existing LEDs, rather than just checking for exact position overlaps. We applied a similar rule to the marker, ensuring that no new LEDs would appear too close to its current position. This greatly increases the minimum amount of movement the player has to do.

The more complex change was making it so LEDs between the marker and the nearest LED in the direction the marker is moving would only spawn as the opposite colour of that direction. This prevented accidental collections of LEDs that while effective in completing the games goal, was not rewarding for the player. 

Below, you can see some pseudocode and a rough diagram that outlines this implementation:
![Spawning Logic Notes](Notes1.jpg)

The final code ended up split into these two functions:
``` c++
bool CheckForInbetweenSpawn(int randomPixel){
  int nearestLed = -1;
  int minDistance = numPixels;

  // Find the nearest active LED in the specified direction
  for (int i = 0; i < MAX_FLASHING_LEDS; i++) {
      if (!flashingLeds[i].active) continue;
      int ledPos = flashingLeds[i].pixel;
      int distance = 0;

      if (isClockwise) {
        distance = (aimedAtLed - ledPos + numPixels) % numPixels;
        if (distance > 0 && distance < minDistance) {
          minDistance = distance;
          nearestLed = ledPos;
          //Serial.print("Nearest led: ");
          //Serial.println(nearestLed);
        }
      } else {
        distance = (ledPos - aimedAtLed + numPixels) % numPixels;
        if (distance > 0 && distance < minDistance) {
          minDistance = distance;
          nearestLed = ledPos;
          //Serial.print("Nearest led: ");
          //Serial.println(nearestLed);
        }
      }
  }

  // Get all LEDs between marker and nearest LED 
  bool found = false;
  if (nearestLed != -1) {
    int pos = aimedAtLed;
    while (true) {
      // Move in the correct direction
      pos = isClockwise ? (pos + 1) % numPixels : (pos - 1 + numPixels) % numPixels;
      if (pos == nearestLed) break;
      if (pos == randomPixel) {
        found = true;
        break;
      }
    }
  }
  return found;
}

uint32_t DetermineColor(bool foundInbetween) {
  uint8_t randVal = random(100);  // 0–99

  if (randVal < colorCRarity) {
    return colorC;
  }

  if(foundInbetween){
    return isClockwise ? colorB : colorA;
  }

  return (random(2) == 0) ? colorA : colorB;
}
```
Which are used in this line from inside `startFlashingLED`:
``` c++
flashingLeds[i].color = DetermineColor(CheckForInbetweenSpawn(pixel));
```
### Preventing unintended play

During playtesting, we realized that players could exploit the system by moving toward an LED in any direction and then quickly reversing course if their initial direction didn’t match the LED’s requirement. To prevent this, we introduced a `beenPassed` boolean in the `FlashingLED` struct. This flag prevents an LED from being collected if the player passes it in the wrong direction, setting beenPassed to true upon the first incorrect pass.

However, since the LED still needs to be collected eventually, we needed a way to reset `beenPassed` to false. Initially, we did this upon the successful collection of any other LED. The reasoning was that if a player was playing honestly, they would only pass an LED in the wrong direction on their way to another, so the reset would happen naturally and go unnoticed.

Despite this, we encountered rare edge cases where unlucky LED spawns made it difficult for players to trigger that reset. To address this, we implemented a time-based fallback: `beenPassed` would automatically reset to false after a certain period, ensuring that the LED could be collected again without soft-locking or player frustration.

Below you can see some notes made for this idea:
![unintended play note](Notes2.jpg)


### Special/Third Colour handling 
The special third-color LED requires two spins to be successfully collected, with the second spin needing to match the direction of the first. While this seemed straightforward to implement at first, we encountered an issue where the logic for detecting the second spin was being triggered immediately after the first.

To fix this, we needed to ensure that the marker had moved a sufficient distance away from the LED before it could trigger the second spin. This required adding a new boolean variable, `hasLeftPixel`, to the `FlashingLed struct`. This variable is set to true once the difference between the marker's current position and the LED's index exceeds a value of 1, as shown here:

``` c++
// This is the case when delta > 1 (not aiming at the LED)
      if (flashingLeds[i].color == colorC && flashingLeds[i].beenPassed && !flashingLeds[i].hasLeftPixel) {
        // Need to be at least 3 pixels away to count as "left"
        flashingLeds[i].hasLeftPixel = true;
```
Then the rest of the specfic code for the special LED/colour looks like this:

``` c++
if (flashingLeds[i].color == colorC) {
        if (!flashingLeds[i].beenPassed) {
          // First pass logic
          flashingLeds[i].beenPassed = true;
          flashingLeds[i].passedClockwise = isClockwise;
          flashingLeds[i].hasLeftPixel = false; // Reset this flag
          startPlayback(secondCount, sizeof(secondCount));
        } 

        if (flashingLeds[i].hasLeftPixel) {
          // Second pass logic, only if we have left the pixel since the first pass
          if (flashingLeds[i].passedClockwise == isClockwise) {
            CollectLed(i, specialScoring);
            startPlayback(goCount, sizeof(goCount));
          } else {
            TurnOffLed(i);
            startPlayback(miss, sizeof(miss));
          }
        }
        continue;
      }
```

### Storing High Score Data

Keeping track of the high score was important to us, as we believed it would encourage competition between players and motivate them to replay and improve. However, a high score without an associated name wouldn't achieve this goal. To address this, we leveraged the rotational input method we had already developed.

When a new high score is achieved, the `NameInput()` function is called. In this function, the player can spin the record to select a letter from the alphabet, then press a button to confirm the selection and move to the next letter. They can also hold down the button to go back to the previous later if they want to make a change. This process repeats until all four letters of the name are entered.
Once the name is entered, both the high score and the corresponding name are stored in EEPROM, a type of non-volatile memory. This ensures the data persists even after the system is powered off.

In addition to storing the high score and name, we also store an initialization flag  which can be any unique value (in our case, 123). On startup, we check whether this flag is present. If it's missing or incorrect, we reinitialize the high score data to default values. This also allows for resetting the stored data, which can be done during the idle phase of the game by holding down the button for a few seconds. Below is a snippet of the initialization code.
``` c++
  uint8_t flag;
  EEPROM.get(addrFlag, flag);

  if (flag != initFlag) {                           // First run,  initialize high score
    char emptyName[5];
    for (int i = 0; i < 4; i++) {
      emptyName[i] = '-';
    }
    emptyName[4] = '\0';

    EEPROM.put(addrName, emptyName);
    EEPROM.put(addrHighScore, (uint16_t)0);
    EEPROM.put(addrFlag, initFlag);
    Serial.println("EEPROM initialized.");
  } else {
    Serial.println("EEPROM already initialized.");
  }

  EEPROM.get(addrHighScore, highScore);
  EEPROM.get(addrName, highScoreName);
  if (highScore > 9999) highScore = 9999;
```

### Audio Playback
For audio playback, we took an unconventional approach that allowed us to achieve relatively detailed sound without needing an SD card module, which we couldn’t use due to limited available pins. Instead, we used the PCM library, which enables audio files to be converted into a char array and stored in the Arduino’s flash memory. This method let us play back short sound effects directly from memory without the need for additional hardware.

## Images showing game construction progress

![01](07.jpg)
![02](08.jpg)
![03](09.jpg)
![04](10.jpg)
![05](11.jpg)

## Statement of attribution
- Chloe: Core coding and physical construction
- Ethan: Coding and bugfixing assistance
- Alex: Physical construction and Audio assistance
