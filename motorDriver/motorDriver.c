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
    gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
    uint8_t pwm_pin_slice_num = pwm_gpio_to_slice_num(pwm_pin);
    // Get some sensible defaults for the slice configuration:
    pwm_config config = pwm_get_default_config();
    // wrap so that the PWM frequency is 32kHz:
    pwm_config_set_wrap(&config, PWM_WRAP);

    pwm_init(pwm_pin_slice_num, &config, true);

    DEBUG_PRINT("Motor driver initialized\n");
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

        DEBUG_PRINT("Stop\n");
    }
    else if (high_time_us > STOP_PWM_TIME)
    {

        DEBUG_PRINT("Forward\n");
    }
    else if (high_time_us < STOP_PWM_TIME)
    {

        DEBUG_PRINT("Backward\n");
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

    DEBUG_PRINT("PWM duty cycle set to %d us.\n", high_time_us);
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
    return adc_read();
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

    // pwm high time in microseconds:
    uint32_t high_time = 0;

    uint32_t system_clock_hz = clock_get_hz(clk_sys);
    uint32_t system_clock_period_ns = 1000000000 / system_clock_hz;

    uint32_t pwm_counter = 0;

    while (true)
    {

        DEBUG_PRINT("system clock: %d Hz\n", system_clock_hz);
        DEBUG_PRINT("system clock period: %d ns\n", system_clock_period_ns);

        // raw adc value between 0 and 2**12-1 (0-4095):
        y_pos_raw = get_adc_value(0);

        DEBUG_PRINT("Raw joystick value: %d\n", y_pos_raw);

        high_time = map_range(y_pos_raw, MIN_JOYSTICK_VALUE, MAX_JOYSTICK_VALUE, FW_PWM_TIME, BK_PWM_TIME);
        if (high_time > STOP_PWM_TIME - STOP_RANGE && high_time < STOP_PWM_TIME + STOP_RANGE)
            high_time = STOP_PWM_TIME;

#if DEBUG
        printf("PWM high time %d ms: ", high_time);
        for (uint8_t i = 0; i < map_range(high_time, BK_PWM_TIME, FW_PWM_TIME, 0, VISUALISATION_WIDTH); i++)
            printf("#");
        printf("\n");
#endif

        pwm_set_chan_level(pwm_gpio_to_slice_num(PWM_PIN), PWM_CHAN_A, high_time);

        pwm_counter = pwm_get_counter(pwm_gpio_to_slice_num(PWM_PIN));
        DEBUG_PRINT("PWM counter: %d\n", pwm_counter);

        DEBUG_PRINT("\n");

        sleep_ms(100);
    }

    return 0;
}
