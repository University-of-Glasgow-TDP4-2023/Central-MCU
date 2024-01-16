/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fade an LED between low and high brightness. An interrupt handler updates
// the PWM slice's output level each time the counter wraps.

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#define PWM_PIN_1 0
#define PWM_PIN_2 1

int fade = 0;
bool going_up = true;

int main()
{
    // for printf to console:
    stdio_init_all();
    adc_init();
    // Make sure the GPIO is high-impedance, no pullups etc:
    adc_gpio_init(31);
    adc_gpio_init(32);

    // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(PWM_PIN_1, GPIO_FUNC_PWM);
    gpio_set_function(PWM_PIN_2, GPIO_FUNC_PWM);

    // Figure out which slice we just connected to the LED pin
    uint slice_num_1 = pwm_gpio_to_slice_num(PWM_PIN_1);
    uint slice_num_2 = pwm_gpio_to_slice_num(PWM_PIN_2);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();

    pwm_init(slice_num_1, &config, true);
    pwm_init(slice_num_2, &config, true);

    while (1)
    {
        adc_select_input(0);
        // raw adc value between 0 and 2**12-1 (0-4095):
        uint16_t x_raw = adc_read();
        adc_select_input(1);
        uint16_t y_raw = adc_read();

        printf("x_raw is %d\n", x_raw);
        printf("y_raw is %d\n", y_raw);

        // scale to 0-255 (0-2**8-1)):
        uint8_t x_pos = x_raw >> 4;
        uint8_t y_pos = y_raw >> 4;

        printf("x_pos is %d\n", x_pos);
        printf("y_pos is %d\n", y_pos);

        // Square the value to make the transition appear more linear (final value is between 0 and 2**16-1):
        pwm_set_gpio_level(PWM_PIN_1, x_pos * x_pos);
        pwm_set_gpio_level(PWM_PIN_2, y_pos * y_pos);

        sleep_ms(100);
    }
}
