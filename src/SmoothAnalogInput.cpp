#include "SmoothAnalogInput.h"

SmoothAnalogInput::SmoothAnalogInput(uint8_t pin, 
  uint16_t long_half_life_ms, 
  uint16_t short_half_life_ms, 
  uint8_t inputMode,
  uint8_t adc_resolution) 
  : _pin(pin), 
    _base_long_ema_factor(1 - exp(-1.0 * M_LN2 / long_half_life_ms)), // HL->tau
    _long_ema(0),
    _short_ema_factor(1 - exp(-1.0 * M_LN2 / short_half_life_ms)), // HL->tau
    _short_ema(0),
    _last_read(0),
    _last_read_time(0),
    _adc_resolution(adc_resolution),
    _ordinary_change_wide_sigma(1),
    _deadband_zero(30),
    _max_brightness(4000){

  // Get the ordinary change wide sigma properly calculated
  // Expect no faster than full-scale per second, so 1/1000 per ms
  _ordinary_change_wide_sigma = (1 << _adc_resolution)/1000.0;

  // Calculate max brightness to 90% of full scale
  _max_brightness = static_cast<uint16_t>((1 << _adc_resolution) * 0.9);

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

  unsigned long time_since_last_read = curr_time - _last_read_time;

  uint16_t reading = analogRead(_pin);
  _last_read_time = curr_time;

  // Get correction factor for long-term EMA based on how far
  // we are from the recent readings.
  // We worry more about smoothing when the light is dim, while we want
  // faster responsiveness when the light is bright.
  double reading_diff = static_cast<double>(abs((int)reading - (int)_last_read));
  double spike_factor = reading_diff / _ordinary_change_wide_sigma;
  double brightness_relaxing = 1.0;
  if (reading > 50 * _ordinary_change_wide_sigma) {
    brightness_relaxing = 1 + 0.5 * sqrt(reading - 50 * _ordinary_change_wide_sigma)/_ordinary_change_wide_sigma;
  }
  spike_factor = spike_factor / brightness_relaxing;
  
  double long_ema_factor = _base_long_ema_factor * exp(-spike_factor);

  // if we're having problems with not making it every millisecond, we can increase the
  // influence of this reading so we can try to keep the 50ms half-life
  if (time_since_last_read > 1) {
    long_ema_factor *= constrain(0.95 * time_since_last_read, 1, 10);
  }

  // Update the long-term EMA
  _long_ema = long_ema_factor * reading + (1 - long_ema_factor) * _long_ema;

  // Update the short-term EMA
  _short_ema = _short_ema_factor * reading + (1 - _short_ema_factor) * _short_ema;

  _last_read = reading;
  _last_read_time = curr_time;
  return true;
}

uint16_t SmoothAnalogInput::get_smoothed_value() const {
  // Give deadband at bottom, max brightness at top
  if (_long_ema < _deadband_zero) {
    return static_cast<uint16_t>(0);
  }
  if (_long_ema > _max_brightness) {
    return _max_brightness;
  }
  return static_cast<uint16_t>(_long_ema);
}

uint16_t SmoothAnalogInput::get_raw_value() const {
  return _last_read;
}