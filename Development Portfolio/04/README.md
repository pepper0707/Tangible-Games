# Technical documentation for the game, including circuit diagrams and code listings.

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


## Statement of attribution
