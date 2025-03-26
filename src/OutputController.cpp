#include <Arduino.h>
#include "OutputController.h"

/*
Control the lights and the program logic!
*/

OutputController::OutputController(unsigned int red_output_pwm_pin, 
                                   unsigned int green_output_pwm_pin, 
                                   unsigned int blue_output_pwm_pin) {
  _red_output_pwm_pin = red_output_pwm_pin;
  _green_output_pwm_pin = green_output_pwm_pin;
  _blue_output_pwm_pin = blue_output_pwm_pin;

  // initialize PWM pins and parameters
  const int pwm_freq = 5000;
  const int pwm_resolution = 12;
  const int max_pwm = (1 << pwm_resolution) - 1; // Clever way to get 2^PWM_RESOLUTION - 1

  _red_pwm_channel = 0;
  _green_pwm_channel = 1;
  _blue_pwm_channel = 2;

  // Connect the PWM controllers to the pins
  ledcSetup(_red_pwm_channel, pwm_freq, pwm_resolution);
  ledcAttachPin(_red_output_pwm_pin, _red_pwm_channel);
  ledcSetup(_green_pwm_channel, pwm_freq, pwm_resolution);
  ledcAttachPin(_green_output_pwm_pin, _green_pwm_channel);
  ledcSetup(_blue_pwm_channel, pwm_freq, pwm_resolution);
  ledcAttachPin(_blue_output_pwm_pin, _blue_pwm_channel);
}

void OutputController::enter_mode(ProgramState &state) {
  /*
  Take actions appropriate for when we enter a new mode.
  This should be called as soon as a new mode is set.
  */

  // Reset the mode start time
  state.last_mode_start = millis();

  switch(state.curr_mode) {
    case Mode::OFF:
      // Turn off all the lights
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 0);
      break;
    case Mode::SLEEP_PREP:
      // Turn off all the lights
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 0);
      break;
    case Mode::RGB:
      // Set the lights to the RGB values
      // Rely on convenience that ADC is 12-bit as is PWM
      ledcWrite(_red_pwm_channel, state.red_pot_val);
      ledcWrite(_green_pwm_channel, state.green_pot_val);
      ledcWrite(_blue_pwm_channel, state.blue_pot_val);
      Serial.print("Setting RGB maybe TO ");
      Serial.print(state.red_pot_val);
      Serial.print(", ");
      Serial.print(state.green_pot_val);
      Serial.print(", ");
      Serial.print(state.blue_pot_val);
      Serial.print(" to channels ");
      Serial.print(_red_pwm_channel);
      Serial.print(", ");
      Serial.print(_green_pwm_channel);
      Serial.print(", ");
      Serial.println(_blue_pwm_channel);

      break;
    case Mode::WHITE:
      // Set the lights to white
      ledcWrite(_red_pwm_channel, state.white_pot_val);
      ledcWrite(_green_pwm_channel, state.white_pot_val);
      ledcWrite(_blue_pwm_channel, state.white_pot_val);
      break;
    case Mode::CUSTOM_1:
      // Set the lights to custom 1
      Serial.println("Setting custom 1");
      Serial.println("Checking channels");
      Serial.println(_red_pwm_channel);
      Serial.println(_green_pwm_channel);
      Serial.println(_blue_pwm_channel);
      ledcWrite(_red_pwm_channel, 40);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 0);
      break;
    case Mode::CUSTOM_2:
      // Set the lights to custom 2
      Serial.println("Setting custom 2");
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 40);
      ledcWrite(_blue_pwm_channel, 0);
      break;
    case Mode::CUSTOM_3:
      // Set the lights to custom 3
      Serial.println("Setting custom 3");
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 40);
      break;
    case Mode::CUSTOM_4:
      // Set the lights to custom 4
      Serial.println("Setting custom 4");
      ledcWrite(_red_pwm_channel, 40);
      ledcWrite(_green_pwm_channel, 40);
      ledcWrite(_blue_pwm_channel, 0);
      break;
    case Mode::CUSTOM_5:
      // Set the lights to custom 5
      Serial.println("Setting custom 5");
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 400);
      ledcWrite(_blue_pwm_channel, 400);
      break;
    case Mode::CUSTOM_6:
      // Set the lights to custom 6
      Serial.println("Setting custom 6");
      ledcWrite(_red_pwm_channel, 400);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 400);
      break;
    case Mode::CUSTOM_7:
      // Set the lights to custom 7
      break;
    case Mode::CUSTOM_8:
      // Set the lights to custom 8
      break;
    case Mode::INVALID:
      // Set the lights to invalid
      break;
  }
}

void OutputController::process_mode(ProgramState &state) {
  /*
  Take actions appropriate for the current mode.
  This should be called in the main loop.
  */

  switch(state.curr_mode) {
    case Mode::OFF:
      // Turn off all the lights
      break;
    case Mode::SLEEP_PREP:
      // Turn off all the lights
      break;
    case Mode::RGB:
      // Set the lights to the RGB values
      // Rely on convenience that ADC is 12-bit as is PWM
      ledcWrite(_red_pwm_channel, state.red_pot_val);
      ledcWrite(_green_pwm_channel, state.green_pot_val);
      ledcWrite(_blue_pwm_channel, state.blue_pot_val);
      if (state.red_pot_val > 1 || state.green_pot_val > 1 || state.blue_pot_val > 1) {
        Serial.print("Setting RGB TO ");
        Serial.print(state.red_pot_val);
        Serial.print(", ");
        Serial.print(state.green_pot_val);
        Serial.print(", ");
        Serial.print(state.blue_pot_val);
        Serial.print(" to channels ");
        Serial.print(_red_pwm_channel);
        Serial.print(", ");
        Serial.print(_green_pwm_channel);
        Serial.print(", ");
        Serial.println(_blue_pwm_channel);
      }
      break;
    case Mode::WHITE:
      // Set the lights to white
      ledcWrite(_red_pwm_channel, state.white_pot_val);
      ledcWrite(_green_pwm_channel, state.white_pot_val);
      ledcWrite(_blue_pwm_channel, state.white_pot_val);
      break;
    case Mode::CUSTOM_1:
      // Set the lights to custom 1
      break;
    case Mode::CUSTOM_2:
      // Set the lights to custom 2
      break;
    case Mode::CUSTOM_3:
      // Set the lights to custom 3
      break;
    case Mode::CUSTOM_4:
      // Set the lights to custom 4
      break;
    case Mode::CUSTOM_5:
      // Set the lights to custom 5
      break;
    case Mode::CUSTOM_6:
      // Set the lights to custom 6
      break;
    case Mode::CUSTOM_7:
      // Set the lights to custom 7
      break;
    case Mode::CUSTOM_8:
      // Set the lights to custom 8
      break;
    case Mode::INVALID:
      // Set the lights to invalid
      break;
  }
}