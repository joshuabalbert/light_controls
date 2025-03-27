#include <Arduino.h>
#include "ProgramState.h"

/*
Program State class and functions!
*/

ProgramState::ProgramState(unsigned int red_pot_pin, 
                           unsigned int green_pot_pin, 
                           unsigned int blue_pot_pin, 
                           unsigned int white_pot_pin,
                           Mode max_usable_mode) {
  last_cycle_start = millis();
  last_motion_detected = millis();
  last_mode_start = millis();
  _red_pot_pin = red_pot_pin;
  _green_pot_pin = green_pot_pin;
  _blue_pot_pin = blue_pot_pin;
  _white_pot_pin = white_pot_pin;
  _max_usable_mode = max_usable_mode;


  // initialize pins
  red_pot = SmoothAnalogInput(_red_pot_pin);
  green_pot = SmoothAnalogInput(_green_pot_pin);
  blue_pot = SmoothAnalogInput(_blue_pot_pin);
  white_pot = SmoothAnalogInput(_white_pot_pin);

  // initialize logic mode
  curr_mode = Mode::OFF;
  last_mode = Mode::OFF;

  // initialize motion sensors
  motion_detector_a = MotionSensorState(A5);
  motion_detector_b = MotionSensorState(A6);
  motion_detector_c = MotionSensorState(A7, false);

}

bool ProgramState::read_pot_values() {
  red_pot_val = red_pot.update_and_get_smoothed();
  green_pot_val = green_pot.update_and_get_smoothed();
  blue_pot_val = blue_pot.update_and_get_smoothed();
  white_pot_val = white_pot.update_and_get_smoothed();
  return true;
}

bool ProgramState::update_motion_sensors() {
  motion_detector_a.update();
  motion_detector_b.update();
  motion_detector_c.update();
  return true;
}

Mode ProgramState::update_mode(Mode new_mode) {
  last_mode = curr_mode;
  curr_mode = new_mode;
  last_mode_start = millis();
  return curr_mode;
}

Mode ProgramState::cycle_mode() {
  Mode new_mode = static_cast<Mode>(static_cast<int>(curr_mode) + 1);
  Serial.println("Trying to cycle mode");
  Serial.println(static_cast<int>(new_mode));
  // Cycle around if we hit the end
  Serial.println(static_cast<int>(_max_usable_mode));
  if (static_cast<int>(new_mode) > static_cast<int>(_max_usable_mode)) {
    Serial.println("Cycling back to OFF");
    new_mode = Mode::OFF;
  }
  // Skip over SLEEP_PREP, which can only be triggered by lack of motion
  if (new_mode == Mode::SLEEP_PREP) {
    Serial.println("Trying to add more?");
    new_mode = static_cast<Mode>(static_cast<int>(new_mode) + 1);
    Serial.println(static_cast<int>(new_mode));
  }
  Serial.print("New mode: ");
  Serial.println(static_cast<int>(new_mode));

  return update_mode(new_mode);
}