#ifndef PROGRAM_STATE_H
#define PROGRAM_STATE_H

#include <Arduino.h>
#include "MotionSensorState.h"

enum class Mode {
  OFF,
  SLEEP_PREP,
  RGB,
  WHITE,
  CUSTOM_1,
  CUSTOM_2,
  CUSTOM_3,
  CUSTOM_4,
  CUSTOM_5,
  CUSTOM_6,
  CUSTOM_7,
  CUSTOM_8
};

class ProgramState {
  private:
    unsigned int _red_pot_pin;
    unsigned int _green_pot_pin;
    unsigned int _blue_pot_pin;
    unsigned int _white_pot_pin;
  public:
    ProgramState(unsigned int red_pot_pin=A1, 
                 unsigned int green_pot_pin=A0, 
                 unsigned int blue_pot_pin=A2, 
                 unsigned int white_pot_pin=A3);
    unsigned long last_cycle_start;
    unsigned long last_motion_detected;
    unsigned int red_pot_val;
    unsigned int green_pot_val;
    unsigned int blue_pot_val;
    unsigned int white_pot_val;

    // Mode data
    Mode curr_mode;
    Mode last_mode;

    MotionSensorState motion_detector_a;
    MotionSensorState motion_detector_b;
    MotionSensorState motion_detector_c;
    
    Mode update_mode(Mode new_mode);
    bool read_pot_values();
};

#endif