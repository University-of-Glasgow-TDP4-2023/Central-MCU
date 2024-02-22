#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "motorDriver.h"

/**
 * @brief Initialize the motor driver
 *
 */
void init_motor(uint8_t pwm_pin)
{
    // STDIO initialization:
    stdio_init_all();

    // ADC initialization:
    adc_init();
    // Make sure the GPIO is high-impedance, no pullups etc:
    adc_gpio_init(Y_PIN);

    // PWM initialization:
    // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the LED pin
    uint8_t pwm_pin_slice_num = pwm_gpio_to_slice_num(pwm_pin);
    // Get some sensible defaults for the slice configuration:
    pwm_config config = pwm_get_default_config();
    // The default PWM clock is 125MHz. Adjust the divider for your frequency
    // uint32_t clock_divider = pwm_hw->clkdiv; // Get current clock divider
    // float divider = clock_divider / (125000000.0 / frequency);
    // Set the PWM frequency
    // pwm_set_clkdiv(slice_num, divider);
    // Set PWM duty cycle (0.0 to 1.0 scale)
    pwm_init(pwm_pin_slice_num, &config, true);
    set_pwm_duty(pwm_pin, STOP_PWM_TIME);

    printf("Motor driver initialized\n");
}

/**
 * @brief Set the PWM duty cycle
 *
 * @param slice_num The PWM slice number.
 * @param high_time_ms The pwm high time in ms.
 */
void set_pwm_duty(uint8_t pwm_pin, uint16_t high_time_us)
{
    // check +-25 from stop_pwm_time:
    if (high_time_us > STOP_PWM_TIME - STOP_RANGE && high_time_us < STOP_PWM_TIME + STOP_RANGE)
    {
        high_time_us = STOP_PWM_TIME;
        printf("Stop\n");
    }
    else if (high_time_us > STOP_PWM_TIME)
    {
        printf("Forward\n");
    }
    else if (high_time_us < STOP_PWM_TIME)
    {
        printf("Backward\n");
    }

    // pwm_set_enabled(slice_num, false);
    uint8_t pwm_pin_slice_num = pwm_gpio_to_slice_num(pwm_pin);
    pwm_config config = pwm_get_default_config();

    // get current system clock frequency:
    uint32_t system_clock_hz = clock_get_hz(clk_sys);

    // get INT value for clkdiv:
    uint32_t clkdiv_int = (config.div >> PWM_DIV_INT_SHIFT) & 0xFF;
    uint16_t wrap_value = config.top;

    uint32_t pwm_clock_hz = system_clock_hz / clkdiv_int;
    uint32_t pwm_period_s = (wrap_value + 1) / pwm_clock_hz; // s
    uint32_t cycle_time_ms = pwm_period_s * 1000.0f;         // ms
    uint32_t high_time_ms = high_time_us / 1000.0f;          // ms

    uint16_t level = (uint16_t)((high_time_ms / cycle_time_ms) * (wrap_value + 1));

    pwm_set_chan_level(pwm_pin_slice_num, PWM_CHAN_A, level);

    printf("PWM duty cycle set to %d us.\n", high_time_us);
}

/**
 * @brief Get the adc value object
 *
 * @param adc_pin The GPIO pin for the adc input.
 * @return uint8_t value between 0 and 4095
 */
uint16_t get_adc_value(uint8_t adc_pin)
{
    adc_select_input(adc_pin);
    uint16_t raw = adc_read();

    return raw;
}

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

int main()
{
    init_motor(PWM_PIN);

    uint16_t y_pos_raw = 0;
    uint16_t period = 0;

    while (true)
    {
        // raw adc value between 0 and 2**12-1 (0-4095):
        y_pos_raw = get_adc_value(0);
        printf("Raw joystick value: %d\n", y_pos_raw);

        period = map_range(y_pos_raw, MIN_JOYSTICK_VALUE, MAX_JOYSTICK_VALUE, FW_PWM_TIME, BK_PWM_TIME);
        printf("PWM on time: %d\n", period);

        set_pwm_duty(PWM_PIN, period);

        printf("\n");
        sleep_ms(100);
    }

    return 0;
}
