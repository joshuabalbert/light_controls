#ifndef DEBOUNCE_INPUT_H
#define DEBOUNCE_INPUT_H

#include <Arduino.h>

class DebounceInput {
/*
We have a debounced input class because, when pushing a button, sometimes things aren't perfectly smooth.
We want to make sure the digital input is real, and not just noise, and we don't want to flash on and off
when the button bounces a bit when pressed.
*/
private:
// underscores start the private variable names
  uint8_t _pin;  // GPIO pin number
  uint16_t _debounceDelay;  // in milliseconds
  bool _lastStableState;
  bool _currentState;
  unsigned long _lastDebounceTime;
  uint8_t _mode;
  bool _activeLow;  // some inputs are "activated" when they input is low

public:
  /**
   * Constructor for debounced input
   * 
   * @param pin The GPIO pin number to use for input
   * @param debounceDelay Debounce time in milliseconds (default 50ms)
   * @param inputMode INPUT or INPUT_PULLUP (default INPUT_PULLUP)
   * @param activeLow Whether a LOW reading means active/pressed (default true)
   */
  // The @param stuff is for Doxygen, which I'm not using, but it doesn't hurt

  // This is the constructor, it also initializes the pin
  // We're using the internal pullup resistors to make things easier
  DebounceInput(
    uint8_t pin, 
    uint16_t debounceDelay = 50, 
    uint8_t inputMode = INPUT_PULLUP, 
    bool activeLow = true);
  
  // Empty default constructor
  DebounceInput();
  
  /**
   * Update the debounced state
   * Should be called in each loop iteration
   * 
   * @return true if state changed since last update
   */
  bool update();
  
  /**
   * Get the current debounced state
   * 
   * @return true if button is pressed/active (with debouncing considered)
   */
  bool isActive() const;
  
};

#endif