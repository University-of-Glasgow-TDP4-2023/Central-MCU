#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// Define GPIO pin for PWM
const uint PWM_PIN = 20;

int main() {

    // Steps to initialise PWM:
    // 1. Claim the GPIO pin for the PWM
    // 2. Set the GPIO function to PWM
    // 3. Find the slice number for the GPIO pin
    // 4. Set the PWM wrap value
    // 5. Set the PWM clock divider
    // 6. Enable the PWM slice

    // Initialize GPIO pins
    gpio_init(PWM_PIN);
    gpio_set_dir(PWM_PIN, GPIO_OUT);

    // Initialize PWM for speed control
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_clkdiv(slice_num, 4.f); // Set PWM frequency
    pwm_set_enabled(slice_num, true);
    
    // Test the motor control
    while (true) {
        for (int i=0; i<65535; i+=5) {
            pwm_set_chan_level(slice_num, PWM_CHAN_A, i);
            sleep_ms(100);
        }
    }

    return 0;


}
