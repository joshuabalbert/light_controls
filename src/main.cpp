#include <Arduino.h>
#include "DebounceInput.h"
#include "ProgramState.h"

// PWM stuff
const int PWM_CHANNEL = 0;
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 12;
const int MAX_PWM = (1 << PWM_RESOLUTION) - 1; // Clever way to get 2^PWM_RESOLUTION - 1
// const int LED_PWM_OUTPUT_PIN = A4;
const int full_cycle_time = 40000;
const int scale_factor = full_cycle_time/MAX_PWM;

// set up program state in global scope
ProgramState program_state;

// Declare a function to set the LED
void set_led(ProgramState &state);

// set up digital input pins
DebounceInput button_a(D6);
DebounceInput motion_detector_a(D7, 50, INPUT, false);

// For debugging inputs
long int last_report_millis = 0;
long int last_analog_read_millis = 0;


// cycle_counter for testing purposes
int cycle_counter = 0;
int last_changed_milli = 0;
int press_counter = 0;

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

  // // set up our PWM LED
  // // This sets up the internal LED PWM controller to connect to pin A0
  // ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  // ledcAttachPin(LED_PWM_OUTPUT_PIN, PWM_CHANNEL);

}

void loop() {
  // put your main code here, to run repeatedly:
  cycle_counter++;
  // Update the input states
  if (button_a.update()) {
    // If the state changed, print it
    if (button_a.isActive()) {
      Serial.println("Button pressed!");
      press_counter++;
    } else {
      Serial.println("Button released!");
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
  if (motion_detector_a.update()) {
    if (motion_detector_a.isActive()) {
      Serial.println("Motion detected!");
      program_state.last_motion_detected = millis();
    }
  }
  if (motion_detector_a.isActive()) {
    program_state.last_motion_detected = millis();
  }
  // Do the background task
  set_led(program_state);


  // Read the analog inputs
  // Only run this once every 10 ms
  long int curr_time = millis();
  if (curr_time % 10 == 0) {
    if (curr_time - last_analog_read_millis > 1) {
      last_analog_read_millis = curr_time;
      program_state.read_pot_values();
    }
  }

  // Write the analog inputs once per second
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
    }
  }

}

// Define various things

void green_set() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, HIGH);
}

void red_set() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

void blue_set() {
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, LOW);
}

void white_set() {
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
}


void set_led(ProgramState &state) {
  // Set the LED based on the state
  // This is a placeholder function
  // It will be replaced with the actual LED control code
  // For now, we just print the state
  if (millis() - state.last_cycle_start < 0) {
    // This might happen in the case of rollover,
    // reset the last_cycle_start to zero, then
    state.last_cycle_start = 0;
  }
  // PWM stuff
  int brightness = (millis() - state.last_cycle_start) / scale_factor;
  if (brightness > MAX_PWM) {
    brightness = MAX_PWM;
  }
  if (millis() - state.last_motion_detected < 2000) {
    ledcWrite(PWM_CHANNEL, brightness);
  }
  else {
    ledcWrite(PWM_CHANNEL, 0);
  }
  if (millis() - state.last_cycle_start < 10000) {
    blue_set();
  }
  else if (millis() - state.last_cycle_start < 20000) {
    red_set();
  }
  else if (millis() - state.last_cycle_start < 30000) {
    green_set();
  }
  else if (millis() - state.last_cycle_start < 40000) {
    white_set();
  }
  else {
    state.last_cycle_start = millis();
    Serial.print("Cycle complete at millis: ");
    Serial.println(millis());
    Serial.println(state.last_cycle_start);
  }
}