#include "./inc/motorDriver.h"

/**
 * @brief Initialize the motor driver
 *
 */
void init_motor(uint8_t pwm_pin)
{
    // STDIO initialization:
    stdio_init_all();

    // GPIO initialization:
    gpio_init(FW_PIN);
    gpio_set_dir(FW_PIN, GPIO_OUT);
    gpio_init(BK_PIN);
    gpio_set_dir(BK_PIN, GPIO_OUT);

    // ADC initialization:
    // adc_init();
    // Make sure the GPIO is high-impedance, no pullups etc:
    // adc_gpio_init(Y_PIN);

    // PWM initialization:
    gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
    uint8_t pwm_pin_slice_num = pwm_gpio_to_slice_num(pwm_pin);
    // Get some sensible defaults for the slice configuration:
    pwm_config config = pwm_get_default_config();
    // wrap to set the PWM frequency:
    // pwm_config_set_wrap(&config, PWM_WRAP);

    pwm_init(pwm_pin_slice_num, &config, true);

    DEBUG_PRINT("Motor driver initialized\n");
}

/**
 * @brief Set the PWM duty cycle
 *
 * @param slice_num The PWM slice number.
 * @param high_time_ms The pwm high time in ms.
 */
// void set_pwm_duty(uint8_t pwm_pin, uint16_t high_time_us)
// {
//     // check +-25 from stop_pwm_time:
//     if (high_time_us > STOP_PWM_TIME - STOP_RANGE && high_time_us < STOP_PWM_TIME + STOP_RANGE)
//     {
//         high_time_us = STOP_PWM_TIME;

//         DEBUG_PRINT("Stop\n");
//     }
//     else if (high_time_us > STOP_PWM_TIME)
//     {

//         DEBUG_PRINT("Forward\n");
//     }
//     else if (high_time_us < STOP_PWM_TIME)
//     {

//         DEBUG_PRINT("Backward\n");
//     }

//     // pwm_set_enabled(slice_num, false);
//     uint8_t pwm_pin_slice_num = pwm_gpio_to_slice_num(pwm_pin);
//     pwm_config config = pwm_get_default_config();

//     // get current system clock frequency:
//     uint32_t system_clock_hz = clock_get_hz(clk_sys);

//     // get INT value for clkdiv:
//     uint32_t clkdiv_int = (config.div >> PWM_DIV_INT_SHIFT) & 0xFF;
//     uint16_t wrap_value = config.top;

//     uint32_t pwm_clock_hz = system_clock_hz / clkdiv_int;
//     uint32_t pwm_period_s = (wrap_value + 1) / pwm_clock_hz; // s
//     uint32_t cycle_time_ms = pwm_period_s * 1000.0f;         // ms
//     uint32_t high_time_ms = high_time_us / 1000.0f;          // ms

//     uint16_t level = (uint16_t)((high_time_ms / cycle_time_ms) * (wrap_value + 1));

//     pwm_set_chan_level(pwm_pin_slice_num, PWM_CHAN_A, level);

//     DEBUG_PRINT("PWM duty cycle set to %d us.\n", high_time_us);
// }

/**
 * @brief Get the adc value object
 *
 * @param adc_pin The GPIO pin for the adc input.
 * @return uint8_t value between 0 and 4095
 */
// uint16_t get_adc_value(uint8_t adc_pin)
// {
//     adc_select_input(adc_pin);
//     return adc_read();
// }

/**
 * @brief Map a value from one range to another
 *
 * @param current_value The current value to be mapped.
 * @param min_value The minimum value of the current range.
 * @param max_value The maximum value of the current range.
 * @param new_min The minimum value of the new range.
 * @param new_max The maximum value of the new range.
 * @return uint8_t The mapped value.
 */
uint16_t map_range(uint16_t current_value, uint16_t min_value, uint16_t max_value, uint16_t new_min, uint16_t new_max)
{
    return (uint16_t)(((current_value - min_value) * (new_max - new_min)) / (max_value - min_value) + new_min);
}

/**
 * @brief Control the motor to move forward
 *
 * @param fw_pin The GPIO pin for forward direction control.
 * @param bk_pin The GPIO pin for backward direction control.
 * @param pwm_pin The GPIO pin for speed control.
 * @param value The speed value between 0 and 2**8-1 (0-255) for the PWM.
 */
void motor_forward(uint fw_pin, uint bk_pin, uint pwm_pin, uint8_t value)
{
    gpio_put(fw_pin, true);
    gpio_put(bk_pin, false);
    // printf("GPIOs are %d, %d\n", gpio_get(fw_pin), gpio_get(bk_pin));
    // Square the value to make the transition appear more linear (final value is between 0 and 2**16-1):
    pwm_set_gpio_level(pwm_pin, value * value);
}

/**
 * @brief Control the motor to move backward
 *
 * @param fw_pin The GPIO pin for forward direction control.
 * @param bk_pin The GPIO pin for backward direction control.
 * @param pwm_pin The GPIO pin for speed control.
 * @param value The speed value between 0 and 2**8-1 (0-255) for the PWM.
 */
void motor_backward(uint fw_pin, uint bk_pin, uint pwm_pin, uint8_t value)
{
    gpio_put(fw_pin, false);
    gpio_put(bk_pin, true);
    // printf("GPIOs are %d, %d\n", gpio_get(fw_pin), gpio_get(bk_pin));
    // Square the value to make the transition appear more linear (final value is between 0 and 2**16-1):
    pwm_set_gpio_level(pwm_pin, value * value);
}

/**
 * @brief Stop the motor
 *
 * @param fw_pin The GPIO pin for forward direction control.
 * @param bk_pin The GPIO pin for backward direction control.
 * @param pwm_pin The GPIO pin for speed control.
 */
void motor_stop(uint fw_pin, uint bk_pin, uint pwm_pin)
{
    gpio_put(fw_pin, false);
    gpio_put(bk_pin, false);
    pwm_set_gpio_level(pwm_pin, 0);
}
