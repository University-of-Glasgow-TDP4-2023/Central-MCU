#include "./inc/motorDriver.h"

uint8_t current_pwm_value;
uint8_t current_direction; // 0 for forward, 1 for backward

uint8_t temp_pwm_value;

/**
 * @brief Initialize the motor driver
 *
 */
void init_motor(uint8_t pwm_pin)
{

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

    DEBUG_PRINT("PWM delay used is %d ms\n", pwm_delay_ms);

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

void speed_up(uint target_value, uint pwm_pin)
{
    while (temp_pwm_value < target_value)
    {
        temp_pwm_value++;
        // DEBUG_PRINT(" ### Speeding up to %d\n", temp_pwm_value);
        pwm_set_gpio_level(pwm_pin, temp_pwm_value * temp_pwm_value);
        // 12ms delay to have 3 seconds for the motor to go from 0 to 255 speed:
        sleep_ms(pwm_delay_ms);
    }
    // DEBUG_PRINT(" ### Speeded up to %d\n", temp_pwm_value);
}

void slow_down(uint target_value, uint pwm_pin)
{
    while (temp_pwm_value > target_value)
    {
        temp_pwm_value--;
        // DEBUG_PRINT(" ### Slowing down to %d\n", temp_pwm_value);
        pwm_set_gpio_level(pwm_pin, temp_pwm_value * temp_pwm_value);
        // 12ms delay to have 3 seconds for the motor to go from 0 to 255 speed:
        sleep_ms(pwm_delay_ms);
    }
    // DEBUG_PRINT(" ### Slowed down to %d\n", temp_pwm_value);
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

    temp_pwm_value = current_pwm_value;

    // Check if the motor needs to speed up, slow down, or change direction
    if (current_pwm_value == 0)
    {
        speed_up(value, pwm_pin);
    }
    else if (current_direction == PWM_FORWARD_DIRECTION && value < current_pwm_value)
    {
        slow_down(value, pwm_pin);
    }
    else if (current_direction == PWM_FORWARD_DIRECTION && value > current_pwm_value)
    {
        speed_up(value, pwm_pin);
    }
    else if (current_direction == PWM_BACKWARD_DIRECTION)
    {
        slow_down(0, pwm_pin);    // to stop the motor
        speed_up(value, pwm_pin); // to start the motor in the opposite direction
    }

    // store current PWM value and direction:
    current_pwm_value = value;
    current_direction = PWM_FORWARD_DIRECTION;
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

    temp_pwm_value = current_pwm_value;

    // printf("GPIOs are %d, %d\n", gpio_get(fw_pin), gpio_get(bk_pin));
    // Square the value to make the transition appear more linear (final value is between 0 and 2**16-1):
    if (current_pwm_value == 0)
    {
        speed_up(value, pwm_pin);
    }
    else if (current_direction == PWM_BACKWARD_DIRECTION && value < current_pwm_value)
    {
        slow_down(value, pwm_pin);
    }
    else if (current_direction == PWM_BACKWARD_DIRECTION && value > current_pwm_value)
    {
        speed_up(value, pwm_pin);
    }
    else if (current_direction == PWM_FORWARD_DIRECTION)
    {
        slow_down(0, pwm_pin);    // to stop the motor
        speed_up(value, pwm_pin); // to start the motor in the opposite direction
    }
    // pwm_set_gpio_level(pwm_pin, value * value);

    // store current PWM value and direction:
    current_pwm_value = value;
    current_direction = PWM_BACKWARD_DIRECTION;
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

    // store current PWM value and direction:
    current_pwm_value = 0;
    current_direction = PWM_FORWARD_DIRECTION;
}
