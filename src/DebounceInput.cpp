#include "DebounceInput.h"

DebounceInput::DebounceInput(uint8_t pin, const char* buttonName, 
  uint16_t debounceDelay, uint8_t inputMode, bool activeLow) 
  : _pin(pin), 
    _debounceDelay(debounceDelay),
    _lastStableState(false),
    _currentState(false),
    _lastDebounceTime(0),
    _mode(inputMode),
    _activeLow(activeLow),
    _buttonName(buttonName){

  // initialization
  pinMode(_pin, _mode); // INPUT_PULLUP is default, keep that in mind!
  _lastStableState = digitalRead(_pin);
  if (_activeLow) {
    _lastStableState = !_lastStableState;
  }
  _currentState = _lastStableState;

  // 
}

DebounceInput::DebounceInput() {
  // Empty constructor
}

bool DebounceInput::update() {
  // Read the current physical state of the pin
  bool reading = digitalRead(_pin);
  if (_activeLow) {
    reading = !reading; // Invert if active low
  }
  
  // If the switch changed due to noise or pressing
  if (reading != _currentState) {
    // Reset the debouncing timer
    // We will not consider the change stable until enough time has passed
    _lastDebounceTime = millis();
  }
  
  bool stateChanged = false;
  
  // If enough time has passed, consider the change stable
  if ((millis() - _lastDebounceTime) > _debounceDelay) {
    // If the reading has changed since the last stable state
    if (reading != _lastStableState) {
      _lastStableState = reading;
      stateChanged = true;
    }
  }
  
  _currentState = reading;
  return stateChanged;
}

bool DebounceInput::isActive() const {
  return _lastStableState;
}