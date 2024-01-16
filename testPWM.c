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

#define PWM_PIN 2
int fade = 0;
bool going_up = true;

int main()
{
    // for printf to console:
    stdio_init_all();
    // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(PICO_DEFAULT_LED_PIN, GPIO_FUNC_PWM);
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the LED pin
    uint slice_num_2 = pwm_gpio_to_slice_num(PICO_DEFAULT_LED_PIN);
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);

    // Mask our slice's IRQ output into the PWM block's single interrupt line,
    // and register our interrupt handler
    // pwm_clear_irq(slice_num);
    // pwm_set_irq_enabled(slice_num, true);
    // irq_set_exclusive_handler(PWM_IRQ_WRAP, on_pwm_wrap);
    // irq_set_enabled(PWM_IRQ_WRAP, true);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();

    pwm_init(slice_num_2, &config, true);
    pwm_init(slice_num, &config, true);

    // Everything after this point happens in the PWM interrupt handler, so we
    // can twiddle our thumbs
    while (1)
    {
        // tight_loop_contents();

        printf("fade is %d\n", fade);
        printf("going_up is %d\n", going_up);

        if (going_up)
        {
            fade += 10;
            if (fade > 255)
            {
                fade = 255;
                going_up = false;
            }
        }
        else
        {
            fade -= 10;
            if (fade < 0)
            {
                fade = 0;
                going_up = true;
            }
        }

        // Square the fade value to make the LED's brightness appear more linear
        // Note this range matches with the wrap value
        pwm_set_gpio_level(PICO_DEFAULT_LED_PIN, fade * fade);
        pwm_set_gpio_level(PWM_PIN, fade * fade);

        sleep_ms(100);
    }
}
