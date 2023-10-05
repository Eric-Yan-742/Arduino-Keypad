// Fill in the code for void setup() and void loop() in this file

void setup() {
  // Keypad setup
  configureCommon();                                                          // Configures button pins for detection (function starts at line 168)
  attachInterrupt(digitalPinToInterrupt(commonPin), pressInterrupt, FALLING); // Sets up interrupt to run whenever the condition is met
                                                                              // ISR Function starts on line 149
  // Progress bar/LED setup
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_CODE>(leds, NUM_LEDS);   // Initialize the LED array for use
  FastLED.setBrightness(BRIGHTNESS);                                          // Set brightness to about half to lower power consumption
  for(int i = 0; i < NUM_LEDS; i++){                                          // Initialize LED to be off
    leds[i] = CRGB::Black;
  }
  FastLED.show();

  // Buzzer setup
  pinMode(buzzerPin, OUTPUT); // Configure buzzer pin
  noTone(buzzerPin);            // Initializes buzzer to be silent
  
  // Lock setup
  lockState = locked;             // Initializes the lock
  myServo.attach(servoPin);
  servoPos = servoLock;
  myServo.write(servoPos);

  // Establish serial communication
  Serial.begin(9600);
}

void loop() {
  // Checks if the keypad is disabled
  if(!disabled) {
    // Keypad resets entries if too much time passes before the next entry
    if(buttonPressNum > 0 && millis() - buttonStartTime >= millisPerButton) { 
      timeOut();                              // Function starts on line 214
      return;
    }
    // Runs a function accordng to the button pressed
    if(buttonPressed == true) {
      // If lockClear button is pressed (ID is -1), resets keypad entries
      if(buttonPress == lockClearButtonID) {
        lockClearInput();                // Function starts on line 237
      }        
      // If any other button is pressed (ID's are 0, 1, and 2), compares current entry to corresponding element in the key
      // Only the lockClear button will work if the keypad is unlocked!
      else if(lockState == locked) {
        buttonPressResponse();                // Function starts on line 191
        
        // Checks if the correct key was entered and unlocks the keypad if it was, otherwise increments numFailedAttempts
        if(buttonPressNum == keyLength) {
          if(correctKey == true) {
            unlock();                         // Function starts on line 265
          }
          else {
            wrongKey();                       // Function starts on line 292
          }
        }
      }
      buttonPressed = false;
    }
  }
  // Checks if the set disableTime has passed since the keypad was disabled
  else if(millis() - disableStartTime >= disableTime) {
    Serial.println("Key Pad is Enabled Now");
    disabled = false;
    numFailedAttempts = 0;
  }
}
