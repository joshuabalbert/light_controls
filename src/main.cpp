#include <Arduino.h>
#include "DebounceInput.h"
#include "ProgramState.h"
#include "OutputController.h"

///////////////////////////////////////////////////////////
// Set some global parameters for how this should all work!
///////////////////////////////////////////////////////////
const bool DEBUG_MODE = false; // Set to false when ready
const unsigned long WAKE_TO_DOZE_TIME = 30000; // 30 seconds
const unsigned long DOZE_TO_SLEEP_TIME = 5000; // 5 seconds past doze
const double wake_dial_deriv_threshold = 1.2; // dial speed to keep from sleep
const double mode_grab_dial_deriv_threshold = 1.85; // dial speed to grab mode

///////////////////////////////////////////////////////////

// PWM stuff
const int PWM_CHANNEL = 0;
const int PWM_FREQ = 20000;
const int PWM_RESOLUTION = 10;
const int MAX_PWM = (1 << PWM_RESOLUTION) - 1; // Clever way to get 2^PWM_RESOLUTION - 1

// set up program state in global scope
ProgramState program_state;

// set up the output controller in global scope
OutputController output_controller;

// Declare a function to set the LED
void led_debug_heartbeat(ProgramState &state);

// set up digital input pins
DebounceInput button_rgb(D12, "rgb");
DebounceInput button_white(D11, "white");
DebounceInput button_cycle(D9, "cycle");
DebounceInput button_off(D10, "off");
DebounceInput button_s1(D8, "s1");
DebounceInput button_s2(D7, "s2");
DebounceInput button_s3(D5, "s3");
DebounceInput button_s4(D6, "s4");

// For debugging inputs
long int last_report_millis = 0;
long int last_analog_read_millis = 0;

// cycle_counter for testing purposes
int cycle_counter = 0;
int last_changed_milli = 0;
int press_counter = 0;

// Setup function called once after power up or reset
void setup() {
  // set up console
  Serial.begin(115200);

  Serial.println("Beginning button test!");
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.print("Using PWM resolution of: ");
  Serial.println(PWM_RESOLUTION);

  // Get the program state set up
  program_state = ProgramState(
    A0, // Red pot pin
    A1, // Green pot pin
    A2, // Blue pot pin
    A3, // White pot pin
    Mode::CUSTOM_6, // Max usable mode (aside from secrets)
    WAKE_TO_DOZE_TIME, // Wake to doze time
    DOZE_TO_SLEEP_TIME); // Doze to sleep time

  // Initially, the arduino LEDs should be off
  // This requires writing the to high main ones
  // to high and the built-in to low
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_BUILTIN, LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  cycle_counter++;
  // Update the input states

  // Check all the inputs here!!
  // We're only going to consider the last activation of the iteration
  // If two buttons are pressed at exactly the same time, we'll only consider the last one
  // Later on, we will add in functionality for multiple buttons pressed at once
  // Okay, fine, here's one: if multiple of the special buttons are pressed at once, we'll go
  // to special mode with rainbows...
  bool mode_updated = false;
  bool special_button_pressed = false;

  // CYCLE BUTTON
  if (button_cycle.update()) {
    program_state.manual_motion_update();
    // If the state changed, print it
    if (button_cycle.isActive()) {
      Serial.println("Cycle button pressed!");
      press_counter++;
      program_state.cycle_mode();
      mode_updated = true;
    } else {
      Serial.println("Cycle button released!");
    }
    // Print the counting info
    Serial.print("Cycle count, millis: ");
    Serial.print(cycle_counter);
    Serial.print(", ");
    Serial.println((millis() - last_changed_milli));
    Serial.print("Rate:");
    Serial.println(cycle_counter / (millis() - last_changed_milli));
    Serial.print("Presses:" );
    Serial.println(press_counter);
    last_changed_milli = millis();
    cycle_counter = 0;
  }

  // OFF BUTTON
  if (button_off.update()) {
    // If the state changed, print it
    if (button_off.isActive()) {
      Serial.println("Off button pressed!");
      program_state.update_mode(Mode::OFF);
      mode_updated = true;
    } else {
      Serial.println("Off button released!");
    }
  }

  // WHITE BUTTON
  if (button_white.update()) {
    program_state.manual_motion_update();
    // If the state changed, print it
    if (button_white.isActive()) {
      Serial.println("White button pressed!");
      program_state.update_mode(Mode::WHITE);
      mode_updated = true;
    } else {
      Serial.println("White button released!");
    }
  }

  // RGB BUTTON
  if (button_rgb.update()) {
    program_state.manual_motion_update();
    // If the state changed, print it
    if (button_rgb.isActive()) {
      Serial.println("RGB button pressed!");
      program_state.update_mode(Mode::RGB);
      mode_updated = true;
    } else {
      Serial.println("RGB button released!");
    }
  }

  // S1 BUTTON
  if (button_s1.update()) {
    program_state.manual_motion_update();
    // If the state changed, print it
    if (button_s1.isActive()) {
      Serial.println("S1 button pressed!");
      special_button_pressed = true;
      program_state.update_mode(Mode::CUSTOM_1);
      mode_updated = true;
    } else {
      Serial.println("S1 button released!");
    }
  }

  // S2 BUTTON
  if (button_s2.update()) {
    program_state.manual_motion_update();
    // If the state changed, print it
    if (button_s2.isActive()) {
      Serial.println("S2 button pressed!");
      special_button_pressed = true;
      program_state.update_mode(Mode::CUSTOM_2);
      mode_updated = true;
    } else {
      Serial.println("S2 button released!");
    }
  }

  // S3 BUTTON

  if (button_s3.update()) {
    program_state.manual_motion_update();
    // If the state changed, print it
    if (button_s3.isActive()) {
      Serial.println("S3 button pressed!");
      special_button_pressed = true;
      program_state.update_mode(Mode::CUSTOM_3);
      mode_updated = true;
    } else {
      Serial.println("S3 button released!");
    }
  }

  // S4 BUTTON
  if (button_s4.update()) {
    program_state.manual_motion_update();
    // If the state changed, print it
    if (button_s4.isActive()) {
      Serial.println("S4 button pressed!");
      special_button_pressed = true;
      program_state.update_mode(Mode::CUSTOM_4);
      mode_updated = true;
    } else {
      Serial.println("S4 button released!");
    }
  }

  // Do we go to special secret mode?
  // This part should be updated when we want to 
  // add in more fun things!
  if (special_button_pressed) {
    // count how many special buttons are active
    int special_button_count = 0;
    if (button_s1.isActive()) special_button_count++;
    if (button_s2.isActive()) special_button_count++;
    if (button_s3.isActive()) special_button_count++;
    if (button_s4.isActive()) special_button_count++;

    // If we have more than one special button pressed, go to special mode
    if (special_button_count > 1) {
      Serial.println("Going to special mode!");
      program_state.update_mode(Mode::CUSTOM_7);
      mode_updated = true;
    }
  }

  // Read the analog inputs
  program_state.read_pot_values();

  // Check analog inputs for sleep and mode grab
  // If dial speed is above threshold, prevent sleep
  if (program_state.red_pot.get_smooth_deriv() > wake_dial_deriv_threshold ||
      program_state.green_pot.get_smooth_deriv() > wake_dial_deriv_threshold ||
      program_state.blue_pot.get_smooth_deriv() > wake_dial_deriv_threshold ||
      program_state.white_pot.get_smooth_deriv() > wake_dial_deriv_threshold) {
    program_state.manual_motion_update();
  }

  // Check for mode grab
  if (program_state.curr_mode != Mode::OFF) {
    if (program_state.curr_mode != Mode::RGB) {
      if (abs(program_state.red_pot.get_smooth_deriv()) > mode_grab_dial_deriv_threshold ||
          abs(program_state.green_pot.get_smooth_deriv()) > mode_grab_dial_deriv_threshold ||
          abs(program_state.blue_pot.get_smooth_deriv()) > mode_grab_dial_deriv_threshold) {
        program_state.update_mode(Mode::RGB);
        mode_updated = true;
      }
    } else if (program_state.curr_mode != Mode::WHITE) {
      if (abs(program_state.white_pot.get_smooth_deriv()) > mode_grab_dial_deriv_threshold) {
        program_state.update_mode(Mode::WHITE);
        mode_updated = true;
      }
    }
  }

  // Do the background task
  if (DEBUG_MODE) {
    led_debug_heartbeat(program_state);
  }

  // Read motion sensors, handle sleep decision
  // Only run this once per ten milliseconds
  long int curr_time = millis();
  if (curr_time % 10 == 0) {
    if (curr_time - last_analog_read_millis > 1) {
      last_analog_read_millis = curr_time;  
      // Check for sleep stuff, this also updates the motion sensors
      mode_updated = program_state.handle_sleep() || mode_updated;
    }
  }



  // Handle the logic
  if (mode_updated) {
    output_controller.enter_mode(program_state);
  }
  output_controller.process_mode(program_state);


  // Write the analog inputs and some output data once per second
  // This is for debugging
  if (DEBUG_MODE) {

    if (curr_time % 1000 == 0) {
      if (curr_time - last_report_millis > 1) {
        last_report_millis = curr_time;
        Serial.print(curr_time);
        Serial.print(" Red: ");
        Serial.print(program_state.red_pot_val);
        Serial.print(", Green: ");
        Serial.print(program_state.green_pot_val);
        Serial.print(", Blue: ");
        Serial.print(program_state.blue_pot_val);
        Serial.print(", White: ");
        Serial.print(program_state.white_pot_val);
        Serial.print(", Motion A: ");
        Serial.print(program_state.motion_detector_a.update());
        Serial.print(": ");
        Serial.print(program_state.motion_detector_a.occupied());
        Serial.print(", Motion B: ");
        Serial.print(program_state.motion_detector_b.update());
        Serial.print(": ");
        Serial.println(program_state.motion_detector_b.occupied());
        Serial.print("Current, last mode: ");
        Serial.print(static_cast<int>(program_state.curr_mode));
        Serial.print(", ");
        Serial.println(static_cast<int>(program_state.last_mode));
        Serial.print("Dial speeds: ");
        Serial.print(program_state.red_pot.get_smooth_deriv());
        Serial.print(", ");
        Serial.print(program_state.green_pot.get_smooth_deriv());
        Serial.print(", ");
        Serial.print(program_state.blue_pot.get_smooth_deriv());
        Serial.print(", ");
        Serial.println(program_state.white_pot.get_smooth_deriv());
      }
    }
  }

}

// A function to blink the built-in LED for testing to confirm it is on
void led_debug_heartbeat(ProgramState &state) {
  if (millis() - state.last_cycle_start < 0) {
    // This might happen in the case of rollover,
    // reset the last_cycle_start to zero, then
    state.last_cycle_start = 0;
  }
  if (millis() - state.last_cycle_start < 1000) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (millis() - state.last_cycle_start < 2000) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else if (millis() - state.last_cycle_start < 3000) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (millis() - state.last_cycle_start < 4000) {
    digitalWrite(LED_BUILTIN, LOW);
  }
  else {
    state.last_cycle_start = millis();
  }
}