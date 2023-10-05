// Fill in the blanks for the helper functions used in this project
// You only need to fill in the blanks for:
//    configureCommon()
//    configureDistinct()
//    buttonPressResponse()
// Other helper functions including the ISR is prewritten for you

/////////////////////////////////////////////////////////
// ISR FUNCTIONS FOR SETUP & GETTING USER BUTTON PRESS //
/////////////////////////////////////////////////////////
// ISR
void pressInterrupt() {
  if(!disabled) {
    if (millis() - buttonStartTime < 200) { // Debounce
      return;
    }
    buttonStartTime = millis();
    configureDistinct();                   // Setup pins for testing individual buttons
    for (int i = 0; i < numButtons; i++) { // Test each button for press
      if (!digitalRead(buttonPins[i])) {
        buttonPress = buttonID[i];
        buttonPressNum++;
        buttonPressed = true;
      }
    }
    configureCommon();                     // Return to original state for detection
  }
}

// Configures button pins for button-press detection
void configureCommon() {
  pinMode(commonPin, INPUT_PULLUP);

  for (int i = 0; i < sizeof(buttonPins) / sizeof(int); i++) {
    pinMode(buttonPins[i], OUTPUT);
    digitalWrite(buttonPins[i], LOW);
  }
}

// Configures button pins for identifying which button was pressed
void configureDistinct() {
  pinMode(commonPin, OUTPUT);
  digitalWrite(commonPin, LOW);

  for (int i = 0; i < sizeof(buttonPins) / sizeof(int); i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
}

///////////////////////////////////////////////
// KEYPAD FUNCTIONS TO RESPOND TO USER INPUT //
///////////////////////////////////////////////
// Function for progress bar and buzzerfeedback on button press
void buttonPressResponse() {
  // update progress bar display
  progress = map(buttonPressNum, 0, keyLength, 0, NUM_LEDS); // identify number of LEDs to turn on for progress bar.
  for(int i = 0; i < progress; i++){                                 // Turn on LEDs according to number of button presses out of total password length
    leds[i] = CRGB::Blue;                                                // set color to blue
  }
  FastLED.show();                                                        // Show array change

  // Check for correct key
  if(correctKey && buttonPress != key[buttonPressNum-1]) {
    correctKey = false;
  }
  
  // Play key entry tone on buzzer
  tone(buzzerPin, btnFrequency);
  delay(response);
  noTone(buzzerPin);
  delay(response);
}

// Function for keypad timeout
void timeOut() {
  Serial.println();
  Serial.println("Session timed out.");
  Serial.println();
  for(int i = 0; i < NUM_LEDS; i++){ // Turns all LEDs Orange
    leds[i] = CRGB::Orange;
  }
  FastLED.show();                               // Show array change
  tone(buzzerPin,timedOutFrequency);
  delay(response);
  for(int i = 0; i < NUM_LEDS; i++){ // Turn off LEDs
    leds[i] = CRGB::Black;
  }
  FastLED.show();                               // Show array change
  noTone(buzzerPin);

  // reset keypad entries
  correctKey = true;
  buttonPressNum = 0;
}

// Function to clear current input and/or lock the keyPad
void lockClearInput() {
  Serial.println("Clearing input and locking keypad if keypad is unlocked...");
  Serial.println();
  for(int i = 0; i < NUM_LEDS; i++){ // Turns all LEDs Purple
    leds[i] = CRGB::Purple;
  }
  FastLED.show();                               // Show array change
  tone(buzzerPin,lockClearFrequency);
  delay(response);
  for(int i = 0; i < NUM_LEDS; i++){ // Turn off LEDs
    leds[i] = CRGB::Black;
  }
  FastLED.show();                               // Show array change
  noTone(buzzerPin);
  
  // Reset keypad entries
  correctKey = true;
  buttonPressNum = 0;

  // Locks keypad if its unlocked
  if(lockState == unlocked) {
    lockState = locked;
    servoPos = servoLock;
    myServo.write(servoPos);  
  }
}

// Function to unlock the keypad
void unlock() {
  Serial.println("Unlocking...");
  Serial.println();
  for(int i = 0; i < NUM_LEDS; i++){ // Turns all LEDs Green
    leds[i] = CRGB::Green;
  }
  FastLED.show();                               // Show array change
  tone(buzzerPin,rightKeyFrequency);
  
  lockState = unlocked;                         // Unlock mechanism
  servoPos = servoUnlock;
  myServo.write(servoPos);
  
  delay(response);
  for(int i = 0; i < NUM_LEDS; i++){ // Turn off LEDs
    leds[i] = CRGB::Black;
  }
  FastLED.show();                               // Show array change
  noTone(buzzerPin);

  // Reset both the keypad entries and the number of failed attempts
  numFailedAttempts = 0;
  correctKey = true;
  buttonPressNum = 0;
}

// Function for incorrect input response
void wrongKey() {
  for(int i = 0; i < NUM_LEDS; i++){ // Turn on LEDs, set to Red
    leds[i] = CRGB::Red;
  }
  FastLED.show();                               // Show array change
  tone(buzzerPin,wrongKeyFrequency);
  delay(response);
  for(int i = 0; i < NUM_LEDS; i++){ // Turn off LEDs
    leds[i] = CRGB::Black;
  }
  FastLED.show();                               // Show array change
  noTone(buzzerPin);

  // Reset keypad entries and increment the number of failed attempts
  numFailedAttempts++;
  correctKey = true;
  buttonPressNum = 0;

  // If too many failed attempts, disables keypad and prints message
  if(numFailedAttempts == maxNumFailedAttempts) {
    Serial.println("Wrong code was entered too many times. Keypad will be temporarily disabled.");
    Serial.println();
    Serial.print("Number of failed attempts: ");
    Serial.println(numFailedAttempts);
    Serial.println();
    disabled = true;
    disableStartTime = millis();
    delay(1000);
  }
  // Otherwise prints message to try again
  else {
    Serial.println("Wrong code. Please try again.");
    Serial.println();
    Serial.print("Number of failed attempts: ");
    Serial.println(numFailedAttempts);
    Serial.println();
  }
}
