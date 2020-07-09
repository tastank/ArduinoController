// Program used to test the USB Joystick object on the 
// Arduino Leonardo or Arduino Micro.
//
// Matthew Heironimus
// 2015-03-28 - Original Version
// 2015-11-18 - Updated to use the new Joystick library 
//              written for Arduino IDE Version 1.6.6 and
//              above.
// 2016-05-13   Updated to use new dynamic Joystick library
//              that can be customized.
//------------------------------------------------------------

#include "Joystick.h"

#define RANGE_MAXIMUM 1023
#define RAW_INPUT_MAXIMUM 1023
#define LED_PIN 13
#define ENABLE_PIN 12
#define ENABLE_SWITCH false
#define CALIBRATE_PIN 11
#define THROTTLE_PIN A5
#define PROPELLER_PIN A4
#define MIXTURE_PIN A3
#define DEBUG false

// Create Joystick
Joystick_ Joystick(
  0x03, // HID ID
  JOYSTICK_TYPE_JOYSTICK, // HID input device type
  0, // button count
  0, // hat switch count
  true, // include X axis TODO can I hack the library to rename these axes?
  true, // include Y axis
  true, // include Z axis
  false, // include RX axis
  false, // include RY axis
  false, // include RZ axis
  false, // include rudder
  false, // include throttle
  false, // include accelerator
  false, // include brake
  false // include steering
);

// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
const bool auto_send_mode = true;

// Calibration values
int throttle_range_maximum, throttle_range_minimum, propeller_range_maximum, propeller_range_minimum, mixture_range_maximum, mixture_range_minimum;
bool calibration_enable;

char debug_text[256];

void debug_write(char *msg) {
  if (DEBUG and Serial) {
    Serial.println(msg);
  }
}

int get_raw_throttle() {
  int raw_throttle = analogRead(THROTTLE_PIN);
  sprintf(debug_text, "Read raw throttle position: %d", raw_throttle);
  debug_write(debug_text);
  return raw_throttle;
}

int get_raw_propeller() {
  int raw_propeller = analogRead(PROPELLER_PIN);
  sprintf(debug_text, "Read raw propeller position: %d", raw_propeller);
  debug_write(debug_text);
  return raw_propeller;
}

int get_raw_mixture() {
  int raw_mixture = analogRead(MIXTURE_PIN);
  sprintf(debug_text, "Read raw mixture position: %d", raw_mixture);
  debug_write(debug_text);
  return raw_mixture;
}
/* TODO I mixed this up with a different sketch that didn't require calibration. redo.
int get_calibrated_throttle() {
  int raw_val = get_raw_throttle();
  int throttle_pos = throttle_calibrate(raw_val);
  return throttle_pos;
}

int get_calibrated_propeller() {
  int raw_val = get_raw_propeller();
  int propeller_pos = propeller_calibrate(raw_val);
  return propeller_pos;
}

int get_calibrated_mixture() {
  int raw_val = get_raw_mixture();
  int mixture_pos = mixture_calibrate(raw_val);
  return mixture_pos;
}
*/
void setup() {

  Serial.begin(9600);
  debug_write("Beginning");

  // Set up calibration
  calibration_enable = false;
  throttle_range_minimum = propeller_range_minimum = mixture_range_minimum = RANGE_MAXIMUM;
  throttle_range_maximum = propeller_range_maximum = mixture_range_maximum = 0;

  // Set Range Values
  // TODO can these be used for calibration?
  Joystick.setXAxisRange(0, RANGE_MAXIMUM);
  Joystick.setYAxisRange(0, RANGE_MAXIMUM);
  Joystick.setZAxisRange(0, RANGE_MAXIMUM);
  
  Joystick.begin(auto_send_mode);
  
  pinMode(ENABLE_PIN, INPUT_PULLUP);
  pinMode(CALIBRATE_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {

  // System Disabled
  if (digitalRead(ENABLE_PIN) == HIGH && ENABLE_SWITCH) {
    // Turn indicator light off.
    digitalWrite(LED_PIN, 0);
    return;
  }

  if (digitalRead(CALIBRATE_PIN) == LOW) {
    debug_write("Calibration mode");
    calibration_enable = true;
    int observed_raw_throttle = get_raw_throttle();
    int observed_raw_propeller = get_raw_propeller();
    int observed_raw_mixture = get_raw_mixture();

    if (observed_raw_throttle < throttle_range_minimum) {
      throttle_range_minimum = observed_raw_throttle;
      Joystick.setXAxisRange(throttle_range_minimum, throttle_range_maximum);
    }
    if (observed_raw_throttle > throttle_range_maximum) {
      throttle_range_maximum = observed_raw_throttle;
      Joystick.setXAxisRange(throttle_range_minimum, throttle_range_maximum);
    }
    if (observed_raw_propeller < propeller_range_minimum) {
      propeller_range_minimum = observed_raw_propeller;
      Joystick.setYAxisRange(propeller_range_minimum, propeller_range_maximum);
    }
    if (observed_raw_propeller > propeller_range_maximum) {
      propeller_range_maximum = observed_raw_propeller;
      Joystick.setYAxisRange(propeller_range_minimum, propeller_range_maximum);
    }
    if (observed_raw_mixture < mixture_range_minimum) {
      mixture_range_minimum = observed_raw_mixture;
      Joystick.setZAxisRange(mixture_range_minimum, mixture_range_maximum);
    }
    if (observed_raw_mixture > mixture_range_maximum) {
      mixture_range_maximum = observed_raw_mixture;
      Joystick.setZAxisRange(mixture_range_minimum, mixture_range_maximum);
    }

    delay(1);
    if (DEBUG) {
      delay(1000);
    }
    return;
  }

  // Turn indicator light on.
  digitalWrite(LED_PIN, 1);
  debug_write("Normal mode");

  // TODO use get_calibrated_fn instead to use calibration
  int throttle = get_raw_throttle();
  int propeller = get_raw_propeller();
  int mixture = get_raw_mixture();

  Joystick.setXAxis(throttle);
  sprintf(debug_text, "Sending throttle position  (X axis): %d", throttle);
  debug_write(debug_text);
  Joystick.setYAxis(propeller);
  sprintf(debug_text, "Sending propeller position (Y axis): %d", propeller);
  debug_write(debug_text);
  Joystick.setZAxis(mixture);
  sprintf(debug_text, "Sending mixture position   (Z axis): %d", mixture);
  debug_write(debug_text);

  delay(1);
  if (DEBUG) {
    delay(1000);
  }
}
