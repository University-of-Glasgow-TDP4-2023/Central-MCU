#include "pico/stdlib.h"
#include "hardware/pwm.h"
// #include "build/_deps/pico_sdk-src/src/rp2_common/hardware_pwm/include/hardware/pwm.hmake"
// #include "build/_deps/pico_sdk-src/src/rp2_common/hardware_pwm/include/hardware/pwm.h"


// Define GPIO pins for IN1 and IN2 for direction control
const uint IN1 = 14;
const uint IN2 = 15;

// Define GPIO pin for PWM for speed control
const uint PWM_PIN = 16;

void motor_forward(uint slice_num) {
    gpio_put(IN1, true);
    gpio_put(IN2, false);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 32768); // Adjust this value for speed (0-65535)
}

void motor_backward(uint slice_num) {
    gpio_put(IN1, false);
    gpio_put(IN2, true);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 32768); // Adjust this value for speed
}

void motor_stop(uint slice_num) {
    gpio_put(IN1, false);
    gpio_put(IN2, false);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
}

int main() {
    // Initialize GPIO pins
    gpio_init(IN1);
    gpio_set_dir(IN1, GPIO_OUT);
    gpio_init(IN2);
    gpio_set_dir(IN2, GPIO_OUT);

    // Initialize PWM for speed control
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_set_wrap(slice_num, 65535);
    pwm_set_clkdiv(slice_num, 4.f); // Set PWM frequency
    pwm_set_enabled(slice_num, true);

    // Test the motor control
    while (true) {
        motor_forward(slice_num);
        sleep_ms(2000);
        motor_stop(slice_num);
        sleep_ms(1000);
        motor_backward(slice_num);
        sleep_ms(2000);
        motor_stop(slice_num);
        sleep_ms(1000);
    }

    return 0;
}
