#ifndef OUTPUT_CONTROLLER_H
#define OUTPUT_CONTROLLER_H

#include <Arduino.h>
#include "ProgramState.h"

// These are a binary mask for RGB
enum class JingleColors {
  OFF,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE
};

class OutputController {
  private:
    unsigned int _red_output_pwm_pin;
    unsigned int _green_output_pwm_pin;
    unsigned int _blue_output_pwm_pin;
    unsigned int _red_pwm_channel;
    unsigned int _green_pwm_channel;
    unsigned int _blue_pwm_channel;
    unsigned long _last_jingle_time;
    unsigned long _jingle_duration;
    JingleColors _jingle_color_1;
    JingleColors _jingle_color_2;
    JingleColors _jingle_color_3;
  public:
    OutputController(unsigned int red_output_pwm_pin=D3, 
                     unsigned int green_output_pwm_pin=D4, 
                     unsigned int blue_output_pwm_pin=D2);
    
    void enter_mode(ProgramState &state);
    void process_mode(ProgramState &state);
    void run_color_jingle(JingleColors color1, JingleColors color2, 
      JingleColors color3, int duration_ms = 1500);
};

#endif