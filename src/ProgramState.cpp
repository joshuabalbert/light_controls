#include <Arduino.h>
#include "ProgramState.h"

/*
Program State class and functions!
*/

ProgramState::ProgramState(unsigned int red_pot_pin, 
                           unsigned int green_pot_pin, 
                           unsigned int blue_pot_pin, 
                           unsigned int white_pot_pin) {
  last_cycle_start = millis();
  last_motion_detected = millis();
  _red_pot_pin = red_pot_pin;
  _green_pot_pin = green_pot_pin;
  _blue_pot_pin = blue_pot_pin;
  _white_pot_pin = white_pot_pin;

  // initialize pin modes
  pinMode(_red_pot_pin, INPUT);
  pinMode(_green_pot_pin, INPUT);
  pinMode(_blue_pot_pin, INPUT);
  pinMode(_white_pot_pin, INPUT);

  // initialize logic mode
  curr_mode = Mode::OFF;
  last_mode = Mode::OFF;

  // initialize motion sensors
  motion_detector_a = MotionSensorState(A5);
  motion_detector_b = MotionSensorState(A6);
  motion_detector_c = MotionSensorState(A7, false);

}

bool ProgramState::read_pot_values() {
  red_pot_val = analogRead(_red_pot_pin);
  green_pot_val = analogRead(_green_pot_pin);
  blue_pot_val = analogRead(_blue_pot_pin);
  white_pot_val = analogRead(_white_pot_pin);
  return true;
}

Mode ProgramState::update_mode(Mode new_mode) {
  last_mode = curr_mode;
  curr_mode = new_mode;
  return curr_mode;
}