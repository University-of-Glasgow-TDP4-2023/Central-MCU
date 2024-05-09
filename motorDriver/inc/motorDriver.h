#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "./global.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"

/**
 * @brief Initialize the motor driver.
 *
 * @param pwm_pin The GPIO pin number used for PWM signal output.
 */
void init_motor(uint8_t pwm_pin);

/**
 * @brief Set the PWM duty cycle.
 *
 * @param pwm_pin The GPIO pin number used for PWM signal output.
 * @param high_time_us The PWM high time in microseconds.
 */
void set_pwm_duty(uint8_t pwm_pin, uint16_t high_time_us);

/**
 * @brief Get the ADC value from a specified pin.
 *
 * @param adc_pin The GPIO pin number used for ADC input.
 * @return uint8_t The ADC value scaled from 0 to 255.
 */
uint16_t get_adc_value(uint8_t adc_pin);

/**
 * @brief Map a value from one range to another.
 *
 * @param current_value The current value to be mapped.
 * @param min_value The minimum value of the current range.
 * @param max_value The maximum value of the current range.
 * @param new_min The minimum value of the new range.
 * @param new_max The maximum value of the new range.
 * @return uint8_t The mapped value.
 */
uint16_t map_range(uint16_t current_value, uint16_t min_value, uint16_t max_value, uint16_t new_min, uint16_t new_max);

#endif // MOTOR_CONTROL_H
