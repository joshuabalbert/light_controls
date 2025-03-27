#include "SmoothAnalogInput.h"

SmoothAnalogInput::SmoothAnalogInput(uint8_t pin, 
  uint16_t long_half_life_ms, 
  uint16_t short_half_life_ms, 
  uint8_t inputMode,
  uint8_t adc_resolution) 
  : _pin(pin), 
    _base_long_ema_factor(exp(-1.0 / long_half_life_ms)),
    _long_ema(0),
    _short_ema_factor(exp(-1.0 / short_half_life_ms)),
    _short_ema(0),
    _last_read(0),
    _last_read_time(0),
    _adc_resolution(adc_resolution),
    _ordinary_change_wide_sigma(1) {

  // Get the ordinary change wide sigma properly calculated
  // Expect no faster than full-scale per second, so 1/1000 per ms
  _ordinary_change_wide_sigma = (1 << _adc_resolution)/1000.0;
  Serial.print("DEBUG: Ordinary change wide sigma is ");
  Serial.println(_ordinary_change_wide_sigma);

  // initialization
  pinMode(_pin, inputMode); // INPUT is default, the pullup is silly here
  _last_read = analogRead(_pin);
  _long_ema = _last_read;
  _short_ema = _last_read;
}

SmoothAnalogInput::SmoothAnalogInput() {
  // Empty constructor
}

bool SmoothAnalogInput::update() {
  // Read the current physical state of the pin if needed
  unsigned long curr_time = millis();
  if (curr_time - _last_read_time < 1) {
    _last_read_time = curr_time;
    return false;
  }
  uint16_t reading = analogRead(_pin);

  // Get correction factor for long-term EMA based on how far
  // we are from the recent readings.
  double spike_factor = abs((int)reading - (int)_short_ema) / _ordinary_change_wide_sigma;
  double long_ema_factor = _base_long_ema_factor * exp(-spike_factor);

  // Update the long-term EMA
  _long_ema = long_ema_factor * reading + (1 - long_ema_factor) * _long_ema;

  // Update the short-term EMA
  _short_ema = _short_ema_factor * reading + (1 - _short_ema_factor) * _short_ema;

  _last_read = reading;
  _last_read_time = curr_time;
  return true;
}

uint16_t SmoothAnalogInput::get_smoothed_value() const {
  return (uint16_t)_long_ema;
}

uint16_t SmoothAnalogInput::get_raw_value() const {
  return _last_read;
}