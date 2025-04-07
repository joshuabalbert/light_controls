#include <Arduino.h>
#include "OutputController.h"

/*
Control the lights and the program logic!
*/

OutputController::OutputController(unsigned int red_output_pwm_pin, 
                                   unsigned int green_output_pwm_pin, 
                                   unsigned int blue_output_pwm_pin,
                                   unsigned long rainbow_duration) {
  _red_output_pwm_pin = red_output_pwm_pin;
  _green_output_pwm_pin = green_output_pwm_pin;
  _blue_output_pwm_pin = blue_output_pwm_pin;
  _rainbow_duration = rainbow_duration;
  _last_jingle_time = 0;
  _jingle_duration = 0;
  _jingle_color_1 = JingleColors::OFF;
  _jingle_color_2 = JingleColors::OFF;
  _jingle_color_3 = JingleColors::OFF;
  _rainbow_start_time = 0;

  // initialize PWM pins and parameters
  // Set the PWM frequency above human hearing range
  const int pwm_freq = 20000;
  const int pwm_resolution = 10;
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

void OutputController::run_color_jingle(JingleColors color1, JingleColors color2, 
                                         JingleColors color3, int duration_ms) {
  /*
  Run a jingle of colors for a given duration.
  This is a simple function to run a jingle of colors.
  */

  // Set the colors
  _jingle_color_1 = color1;
  _jingle_color_2 = color2;
  _jingle_color_3 = color3;

  // Set the duration
  _jingle_duration = duration_ms;

  // Set the last jingle time
  _last_jingle_time = millis();

  Serial.print("Jingle stop time: ");
  Serial.println(_last_jingle_time + _jingle_duration);
}

void OutputController::enter_mode(ProgramState &state) {
  /*
  Take actions appropriate for when we enter a new mode.
  This should be called as soon as a new mode is set.
  */

  // Reset the mode start time
  state.last_mode_start = millis();
  
  unsigned int red_out = 0;
  unsigned int green_out = 0;
  unsigned int blue_out = 0;
  unsigned int white_out = 0;

  switch(state.curr_mode) {
    case Mode::OFF:
      // Turn off all the lights
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 0);
      if (state.last_mode != Mode::OFF) {
        // Only run the jingle if we're not already off
        run_color_jingle(JingleColors::WHITE, JingleColors::WHITE, JingleColors::OFF);
      }
      break;
    case Mode::SLEEP_PREP:
      // Turn off all the lights
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 0);
      run_color_jingle(JingleColors::WHITE, JingleColors::OFF, JingleColors::OFF);
      break;
    case Mode::RGB:
      // Set the lights to the RGB values
      // Convert 12-bit to 10-bit
      red_out = state.red_pot_val >> 2;
      green_out = state.green_pot_val >> 2;
      blue_out = state.blue_pot_val >> 2;
      // Set the lights to the RGB values
      ledcWrite(_red_pwm_channel, red_out);
      ledcWrite(_green_pwm_channel, green_out);
      ledcWrite(_blue_pwm_channel, blue_out);
      run_color_jingle(JingleColors::RED, JingleColors::GREEN, JingleColors::BLUE);
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
      // Convert 12-bit to 10-bit
      white_out = state.white_pot_val >> 2;
      // Set the lights to white
      ledcWrite(_red_pwm_channel, white_out);
      ledcWrite(_green_pwm_channel, white_out);
      ledcWrite(_blue_pwm_channel, white_out);

      run_color_jingle(JingleColors::WHITE, JingleColors::OFF, JingleColors::WHITE);
      break;
    case Mode::CUSTOM_1:
      // Set the lights to custom 1
      Serial.println("Setting custom 1");
      Serial.println("Checking channels");
      Serial.println(_red_pwm_channel);
      Serial.println(_green_pwm_channel);
      Serial.println(_blue_pwm_channel);
      ledcWrite(_red_pwm_channel, 250);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 0);
      run_color_jingle(JingleColors::RED, JingleColors::OFF, JingleColors::RED);
      break;
    case Mode::CUSTOM_2:
      // Set the lights to custom 2
      Serial.println("Setting custom 2");
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 250);
      ledcWrite(_blue_pwm_channel, 0);
      run_color_jingle(JingleColors::GREEN, JingleColors::OFF, JingleColors::GREEN);
      break;
    case Mode::CUSTOM_3:
      // Set the lights to custom 3
      Serial.println("Setting custom 3");
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 250);
      run_color_jingle(JingleColors::BLUE, JingleColors::OFF, JingleColors::BLUE);
      break;
    case Mode::CUSTOM_4:
      // Set the lights to custom 4
      Serial.println("Setting custom 4");
      ledcWrite(_red_pwm_channel, 250);
      ledcWrite(_green_pwm_channel, 250);
      ledcWrite(_blue_pwm_channel, 0);
      run_color_jingle(JingleColors::YELLOW, JingleColors::OFF, JingleColors::YELLOW);
      break;
    case Mode::CUSTOM_5:
      // Set the lights to custom 5
      Serial.println("Setting custom 5");
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 250);
      ledcWrite(_blue_pwm_channel, 250);
      run_color_jingle(JingleColors::CYAN, JingleColors::OFF, JingleColors::CYAN);
      break;
    case Mode::CUSTOM_6:
      // Set the lights to custom 6
      Serial.println("Setting custom 6");
      ledcWrite(_red_pwm_channel, 250);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 250);
      run_color_jingle(JingleColors::MAGENTA, JingleColors::OFF, JingleColors::MAGENTA);
      break;
    case Mode::CUSTOM_7:
      // Set the lights to custom 7
      // BEGIN THE SECRET RAINBOW!!!
      Serial.println("Setting custom 7");
      ledcWrite(_red_pwm_channel, 800);
      ledcWrite(_green_pwm_channel, 0);
      ledcWrite(_blue_pwm_channel, 0);
      // don't reset the rainbow if we just briefly started to doze
      if (state.last_mode != Mode::SLEEP_PREP) {
        _rainbow_start_time = millis();
      }

      run_color_jingle(JingleColors::MAGENTA, JingleColors::CYAN, JingleColors::YELLOW);
      break;
    case Mode::CUSTOM_8:
      // Set the lights to custom 8
      Serial.println("Setting custom 8");
      ledcWrite(_red_pwm_channel, 0);
      ledcWrite(_green_pwm_channel, 100);
      ledcWrite(_blue_pwm_channel, 100);
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

  // Regardless of mode, check if we're in the jingle!
  // Also, check if we're just after it, to turn it off
  unsigned long curr_time = millis();
  unsigned int white_out = 0;
  unsigned int red_out = 0;
  unsigned int green_out = 0;
  unsigned int blue_out = 0;
  if (curr_time - _last_jingle_time < _jingle_duration + 100) {
    // We're in the jingle, set the lights to the jingle colors
    JingleColors curr_color = JingleColors::OFF;
    if (curr_time - _last_jingle_time < _jingle_duration / 3) {
      curr_color = _jingle_color_1;
    } else if (curr_time - _last_jingle_time < 2 * _jingle_duration / 3) {
      curr_color = _jingle_color_2;
    } else if (curr_time - _last_jingle_time < _jingle_duration) {
      curr_color = _jingle_color_3;
    }
    if (static_cast<unsigned int>(curr_color) & static_cast<unsigned int>(JingleColors::RED)) {
      digitalWrite(LED_RED, LOW);
    } else {
      digitalWrite(LED_RED, HIGH);
    }
    if (static_cast<unsigned int>(curr_color) & static_cast<unsigned int>(JingleColors::GREEN)) {
      digitalWrite(LED_GREEN, LOW);
    } else {
      digitalWrite(LED_GREEN, HIGH);
    }
    if (static_cast<unsigned int>(curr_color) & static_cast<unsigned int>(JingleColors::BLUE)) {
      digitalWrite(LED_BLUE, LOW);
    } else {
      digitalWrite(LED_BLUE, HIGH);
    }
  }

  switch(state.curr_mode) {
    case Mode::OFF:
      // Turn off all the lights
      break;
    case Mode::SLEEP_PREP:
      // Turn off all the lights
      break;
    case Mode::RGB:
      // Set the lights to the RGB values
      // Convert 12-bit to 10-bit
      red_out = state.red_pot_val >> 2;
      green_out = state.green_pot_val >> 2;
      blue_out = state.blue_pot_val >> 2;
      // Set the lights to the RGB values
      ledcWrite(_red_pwm_channel, red_out);
      ledcWrite(_green_pwm_channel, green_out);
      ledcWrite(_blue_pwm_channel, blue_out);
      break;
    case Mode::WHITE:
      // Set the lights to white
      // Convert 12-bit to 10-bit
      white_out = state.white_pot_val >> 2;
      // Set the lights to white
      ledcWrite(_red_pwm_channel, white_out);
      ledcWrite(_green_pwm_channel, white_out);
      ledcWrite(_blue_pwm_channel, white_out);
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
      // Secret rainbow functionality!!
      set_rainbow(curr_time - _rainbow_start_time);

      break;
    case Mode::CUSTOM_8:
      // Set the lights to custom 8
      break;
    case Mode::INVALID:
      // Set the lights to invalid
      break;
  }
}

void OutputController::set_rainbow(unsigned long time_diff, uint16_t max_brightness) {
  /*
  Set the lights to a rainbow pattern.
  This is a simple function to set the lights to a rainbow pattern.
  */
  time_diff = time_diff % _rainbow_duration;
  // Set the colors
  int red = 0;
  int green = 0;
  int blue = 0;
  // Start at full red, moving towards green
  if (time_diff < _rainbow_duration/3){
    // Set the colors
    red = max_brightness - (max_brightness * time_diff / (_rainbow_duration/3));
    green = max_brightness * time_diff / (_rainbow_duration/3);
    blue = 0;
    }
  // Then move towards blue
  else if (time_diff < 2 * _rainbow_duration/3){
    // Set the colors
    red = 0;
    green = max_brightness - (max_brightness * (time_diff - _rainbow_duration/3) / (_rainbow_duration/3));
    blue = max_brightness * (time_diff - _rainbow_duration/3) / (_rainbow_duration/3);
  }
  // Finally move back to red
  else {
    // Set the colors
    red = max_brightness * (time_diff - 2 * _rainbow_duration/3) / (_rainbow_duration/3);
    green = 0;
    blue = max_brightness - (max_brightness * (time_diff - 2 * _rainbow_duration/3) / (_rainbow_duration/3));
  }


  // Set the colors
  ledcWrite(_red_pwm_channel, red);
  ledcWrite(_green_pwm_channel, green);
  ledcWrite(_blue_pwm_channel, blue);
}
