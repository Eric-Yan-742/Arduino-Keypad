// Define all constants and variables that you will use for this project in this file
// Reference used for multiple buttons using one interrupt:
// https://create.arduino.cc/projecthub/Svizel_pritula/10-buttons-using-1-interrupt-2bd1f8

////////////////////
// LOCK MECHANISM //
////////////////////

// Declare lock mechansm variables
bool locked = true;                       // initialize locked value locked
bool unlocked = false;                    // initialize unlocked value
bool lockState = locked;                  // initialize lockState to be locked

// Include servo library
#include <Servo.h>

// Declare servo object and variables
Servo myServo;
const int servoPin = 10;     // Digital pin for Servo
const int servoLock = 90;                 // 90° Servo lock position
const int servoUnlock = 0;                // 0°  Servo unlock position
int servoPos = servoLock;                 // initialize servo to be locked

//////////////////////
// BUZZER VARIABLES //
//////////////////////
// Declare alarm (piezo buzzer) variables
const int buzzerPin = 11;        // Digital pin for buzzer
const unsigned long btnFrequency = 440;       // When any button is pressed, plays A4
const unsigned long rightKeyFrequency = 880;  // Inputting key correctly plays A5
const unsigned long wrongKeyFrequency = 196;  // Inputting key incorrectly plays G3
const unsigned long timedOutFrequency = 220;  // The keypad timing out plays A3
const unsigned long lockClearFrequency = 392; // Inputting key incorrectly plays G4

////////////////////////////
// PROGRESS BAR VARIABLES //
////////////////////////////
// Include fast LED library
#include <FastLED.h>

// Declare Addressable LED Progress Bar variables
#define LED_PIN 3   // Digital Pin
#define NUM_LEDS 10  // Number of LEDs in LED strip
#define COLOR_CODE RGB            // WS2812B chips work with RGB values in the GRB format, not the RGB format
#define CHIPSET WS2812B           // Name of the LED driver chipset
#define BRIGHTNESS 50            // Brightness can be from 0 - 255
CRGB leds[NUM_LEDS];              // Create an array for our LEDs
int progress = 0;                 // Create progress bar variable (similar to mappedReading from the SoundReactiveLEDs Challenge)

//////////////////////
// KEYPAD VARIABLES //
//////////////////////
// Define lock key variables
  // Rules & recommendations for creating your key:
  // 1. Must only consist of 0s, 1s, and 2s (based on a 3-button entry system)
  // 2. Recommended length: 3-10
const int key[] = {0, 1, 0, 0, 2}; // password
const int keyLength = sizeof(key)/sizeof(key[0]);                      // length of the password

// Declare keypad button variables (based on 4-button system; 3 entry buttons, 1 clear & lock button)
const int commonPin = 2;
const int buttonPins[] = {4,7,6,5};
const int buttonID[] = {-1,0,1,2};
const int lockClearButtonID = -1;
const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

// Declare user input and evaluation variables
volatile int buttonPress = lockClearButtonID; // User input for current button press
volatile int buttonPressNum = 0;              // Initialize button press counter to 0
volatile bool buttonPressed = false;          // Initialize button press status as false (assume no buttons pressed on startup)
bool correctKey = true;                       // Used to evaluate the input password; set to false at the first incorrect entry
const int response = 100;                     // Determines how long each response should last (alarm & LED)

// Declare keypad attempt and disable control variables
const unsigned long millisPerButton = 5000; // Number of milliseconds without button press before timeout (resets user input)
const unsigned long disableTime = 10000;     // Number of milliseconds to disable keypad for too many failed attempts
const int maxNumFailedAttempts = 3;     // Number of attempts before disabling the keypad
unsigned long buttonStartTime = 0;                            // Used to check if too much time has passed between button presses. If so, the keypad times out
unsigned long disableStartTime = 0;                           // Acts as a timestamp for when the keypad is disabled. When the disableTime has passed, the keypad is re-enabled
int numFailedAttempts = 0;                                    // Tracks the number of failed attempts that have been made since the keypad was last unlocked or disabled
bool timedOut = false;                                        // set to true if difference between current clock timestamp and buttonStartTime is ≥ millisPerButton 
bool disabled = false;                                        // set to true if numFailedAttempts = maxNumFailedAttempts

