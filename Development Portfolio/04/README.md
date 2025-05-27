# Technical documentation for the game, including circuit diagrams and code listings.

## Key software sections and challanges

### Audio Playback
For audio playback, we took an unconventional approach that allowed us to achieve relatively detailed sound without needing an SD card module, which we couldn’t use due to limited available pins. Instead, we used the PCM library, which enables audio files to be converted into a char array and stored in the Arduino’s flash memory. This method let us play back short sound effects directly from memory without the need for additional hardware.

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


## Statement of attribution
