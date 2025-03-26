#ifndef MOTION_SENSOR_STATE_H
#define MOTION_SENSOR_STATE_H

#include <Arduino.h>
#include "DebounceInput.h"

class MotionSensorState {
  private:
    unsigned int _motion_pin;
    DebounceInput _motion_input;
    unsigned long _last_motion_detected;
    unsigned long _cooldown_time;
    bool _enabled;
  public:
    MotionSensorState(unsigned int motion_pin, bool enabled = true);
    MotionSensorState(); // Default will have it off
    bool update();
    bool occupied();
    
};

#endif