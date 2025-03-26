#ifndef OUTPUT_CONTROLLER_H
#define OUTPUT_CONTROLLER_H

#include <Arduino.h>
#include "ProgramState.h"

class OutputController {
  private:
    unsigned int _red_output_pwm_pin;
    unsigned int _green_output_pwm_pin;
    unsigned int _blue_output_pwm_pin;
    unsigned int _red_pwm_channel;
    unsigned int _green_pwm_channel;
    unsigned int _blue_pwm_channel;
  public:
    OutputController(unsigned int red_output_pwm_pin=D3, 
                     unsigned int green_output_pwm_pin=D4, 
                     unsigned int blue_output_pwm_pin=D2);
    
    void enter_mode(ProgramState &state);
    void process_mode(ProgramState &state);
};

#endif