#ifndef SMOOTH_ANALOG_INPUT_H
#define SMOOTH_ANALOG_INPUT_H

#include <Arduino.h>

class SmoothAnalogInput {
/*
We have found that the ADC inputs are not nearly as smooth as they
should be.  Probably noise from the power supply.  The voltage from
the potentiometers shouldn't have abrupt spikes.  Here we have a class
to dramatically smooth out the abrupt spikes while still being highly
responsive.

Some basic assumptions: The potentiometer shouldn't be changing by more
than 50% per second, thus it shouldn't be changing by more than 0.5/1000
= 0.0005 per millisecond.  With the 12-bit ADC, that comes to about 2 units
out of 4096 per millisecond.

We're going to do a few things to make this work:
------
1. We're only going to take one reading per millisecond.  I don't think
anyone will notice.  The goal is to keep the behavior of this algorithm
independent of the program loop rate.
2. We're going to use an exponential moving average to provide a smoothed
output without needing to carry around a huge buffer.  The goal is to
make it strongly affected by values from the most recent 50ms.  That should
more or less be the half-life of the exponential.
3. But we're going to add in an extra step!  The thing I most want to avoid
is brief and abrupt spikes.  Those make very annoying flashes and flicker
is awful.  So, the amount that a particular reading will change the
exponential moving average will be inversely proportional to how far
the reading is from a separate moving average, which will have a half-life
of around 5 ms.  I might adjust these values.  Point is, if we have a huge
spike for a few milliseconds, I don't want it to affect the output much.
------
It might be nice to be able to run this more consistently than once per
millisecond (especially because I'm not sure how even millis() is), but
let's be honest, millisecond, nobody can tell.
*/
private:
// underscores start the private variable names
  uint8_t _pin;  // GPIO pin number
  double _base_long_ema_factor; // Exponential moving average factor before adjustment
  double _long_ema; // Long-term exponential moving average
  double _short_ema_factor; // Spike reduction moving average factor
  double _short_ema; // Short-term exponential moving average
  uint16_t _last_read; // Last reading from the ADC
  unsigned long _last_read_time;
  uint8_t _adc_resolution; // Resolution of the ADC, 12-bit for ESP32
  double _ordinary_change_wide_sigma; // Expect ordinary change per ms within this

public:
  /**
   * Constructor for smoothed analog input
   * 
   * @param pin The GPIO pin number to use for input
   * @param long_half_life_ms The half-life of the long-term average in milliseconds
   * @param short_half_life_ms The half-life of the short-term average in milliseconds
   * @param inputMode INPUT or INPUT_PULLUP (default INPUT)
   * @param adc_resolution The resolution of the ADC (default 12)
  */
  // The @param stuff is for Doxygen, which I'm not using, but it doesn't hurt

  // This is the constructor, it also initializes the pin
  // We.... could be using the internal pullup resistor but I can't imagine
  // why we would, the potentiometers should never be floating.
  SmoothAnalogInput(
    uint8_t pin, 
    uint16_t long_half_life_ms = 50, 
    uint16_t short_half_life_ms = 5,
    uint8_t inputMode = INPUT,
    uint8_t adc_resolution = 12);
  
  // Empty default constructor
  SmoothAnalogInput();

  /**
   * Take a reading and update the smoothed state, if needed
   * Should be called in each loop iteration
   * 
   * @return true if reading was necessary
   */
  bool update();

  /**
   * Get the current smoothed state
   * 
   * @return The smoothed value of the analog input
   */
  uint16_t get_smoothed_value() const;

  /**
   * Get the current raw state
   * 
   * @return The raw value of the analog input
   */
  uint16_t get_raw_value() const;

  /**
   * Update and get the smoothed value in one step
   * 
   * @return The smoothed value of the analog input
   */
  inline uint16_t update_and_get_smoothed() {
    update();
    return get_smoothed_value();
  };

  /**
   * Update and get the raw value in one step
   * 
   * @return The raw value of the analog input
   */
  inline uint16_t update_and_get_raw() {
    update();
    return get_raw_value();
  };

};

#endif