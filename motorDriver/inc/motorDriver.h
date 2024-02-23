#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "global.h"

// Define GPIO pin for PWM (speed and direction control):
#define PWM_PIN 0 // Pin 1
#define PWM_DIV_INT_SHIFT 4
// for 32kHz PWM clock frequency = 31250ns PWM clock period.
// WRAP = PWM clock period / system clock period = 31250ns / 8ns = 3906
#define PWM_WRAP 3906
#define PWM_CLOCK_FREQ 32000 // 32kHz

// Define joystick GPIO pin for input (analog):
#define Y_PIN 31 // Pin 31

// Define joystick raw values:
#define MIN_JOYSTICK_VALUE 20
#define MAX_JOYSTICK_VALUE 4095
#define STATIONARY_JOYSTICK_VALUE 1850 // Mid point - actual range fluctuates between 1800 and 1900

// Define motor driver constants:
#define STOP_PWM_TIME 1500 // microseconds for stop
#define STOP_RANGE 50      // range of values around the stop value
#define FW_PWM_TIME 1900   // microseconds for forward
#define BK_PWM_TIME 1100   // microseconds for backward

#if DEBUG
// Define how many bars to use for visualisation:
#define VISUALISATION_WIDTH 20
#endif

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
