#include <Arduino.h>
#include "MotionSensorState.h"

/*
Motion Sensor state and functions
*/

// Normal constructor for enabled motion sensor
MotionSensorState::MotionSensorState(unsigned int motion_pin, bool enabled) {
  _motion_pin = motion_pin;
  _last_motion_detected = 0;
  _cooldown_time = 4000;
  // intialize the pin
  _motion_input = DebounceInput(motion_pin, "motion", 20, INPUT, false);
  _enabled = enabled;
}

// Default constructor for a disabled motion sensor
MotionSensorState::MotionSensorState() {
  _motion_pin = 0;
  _last_motion_detected = 0;
  _cooldown_time = 4000;
  _enabled = false;
  _motion_input = DebounceInput(0, "motion", 20, INPUT, false);
}

bool MotionSensorState::update() {
  /*
  This function updates the motion sensor state.
  */
  if (!_enabled) {
    return false;
  }
  if (_motion_input.update()) {
    _last_motion_detected = millis();
    return true;
  }
  return false;
}

bool MotionSensorState::occupied() {
  /*
  This function checks if we should be in occupied mode, based on the motion sensor.
  Considers both current state and cooldown since last motion detected.
  */
  if (!_enabled) {
    return false;
  }
  if (update()) {
    return true;
  }
  if (millis() - _last_motion_detected < _cooldown_time) {
    return true;
  }
  return false;
}