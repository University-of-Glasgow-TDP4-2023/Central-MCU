#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

// Define GPIO pin speed control (PWM):
#define SPEED_PIN 0

// Define GPIO pins for direction control (digital) :
#define FW_PIN 1
#define BK_PIN 2

// define GPIO pins for controller input (analog):
#define X_PIN 31

// Define joystick thresholds:
#define STOP_UPPER_THRESHOLD 120
#define STOP_LOWER_THRESHOLD 110

/**
 * @brief Initialize the motor driver
 *
 */
void init_motor()
{
    // STDIO initialization:
    stdio_init_all();

    // GPIO initialization:
    gpio_init(FW_PIN);
    gpio_set_dir(FW_PIN, GPIO_OUT);
    gpio_init(BK_PIN);
    gpio_set_dir(BK_PIN, GPIO_OUT);

    // ADC initialization:
    adc_init();
    // Make sure the GPIO is high-impedance, no pullups etc:
    adc_gpio_init(X_PIN);

    // PWM initialization:
    // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(SPEED_PIN, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the LED pin
    uint speed_pin_slice_num = pwm_gpio_to_slice_num(SPEED_PIN);
    // Get some sensible defaults for the slice configuration:
    pwm_config config = pwm_get_default_config();
    pwm_init(speed_pin_slice_num, &config, true);
}

/**
 * @brief Control the motor to move forward
 *
 * @param fw_pin The GPIO pin for forward direction control.
 * @param bk_pin The GPIO pin for backward direction control.
 * @param speed_pin The GPIO pin for speed control.
 * @param value The speed value between 0 and 2**8-1 (0-255) for the PWM.
 */
void motor_forward(uint fw_pin, uint bk_pin, uint speed_pin, uint8_t value)
{
    gpio_put(fw_pin, true);
    gpio_put(bk_pin, false);
    // printf("GPIOs are %d, %d\n", gpio_get(fw_pin), gpio_get(bk_pin));
    // Square the value to make the transition appear more linear (final value is between 0 and 2**16-1):
    pwm_set_gpio_level(speed_pin, value * value);
}

/**
 * @brief Control the motor to move backward
 *
 * @param fw_pin The GPIO pin for forward direction control.
 * @param bk_pin The GPIO pin for backward direction control.
 * @param speed_pin The GPIO pin for speed control.
 * @param value The speed value between 0 and 2**8-1 (0-255) for the PWM.
 */
void motor_backward(uint fw_pin, uint bk_pin, uint speed_pin, uint8_t value)
{
    gpio_put(fw_pin, false);
    gpio_put(bk_pin, true);
    // printf("GPIOs are %d, %d\n", gpio_get(fw_pin), gpio_get(bk_pin));
    // Square the value to make the transition appear more linear (final value is between 0 and 2**16-1):
    pwm_set_gpio_level(speed_pin, value * value);
}

/**
 * @brief Stop the motor
 *
 * @param fw_pin The GPIO pin for forward direction control.
 * @param bk_pin The GPIO pin for backward direction control.
 * @param speed_pin The GPIO pin for speed control.
 */
void motor_stop(uint fw_pin, uint bk_pin, uint speed_pin)
{
    gpio_put(fw_pin, false);
    gpio_put(bk_pin, false);
    pwm_set_gpio_level(speed_pin, 0);
}

/**
 * @brief Get the adc value object
 *
 * @param adc_pin
 * @return uint8_t value between 0 and 2**8-1 (0-255)
 */
uint8_t get_adc_value(uint adc_pin)
{
    adc_select_input(adc_pin);
    // raw adc value between 0 and 2**12-1 (0-4095):
    uint16_t raw = adc_read();
    // scale to 0-2**8-1 (0-255):
    uint8_t pos = raw >> 4;
    // printf("raw is %d\n", raw);
    return pos;
}

int main()
{
    init_motor();

    while (true)
    {
        uint8_t y_pos = get_adc_value(0);
        // printf("Old y_pos is %d\n", y_pos);

        // Stop the motor if the joystick is in the middle:
        if (y_pos > STOP_LOWER_THRESHOLD && y_pos < STOP_UPPER_THRESHOLD)
        {
            // printf("STOPPED: y_pos is %d\n", y_pos);
            printf("\n");
            motor_stop(FW_PIN, BK_PIN, SPEED_PIN);
            // wait for more if we aren't moving (save some power):
            sleep_ms(500);
            continue;
        }
        else if (y_pos > STOP_UPPER_THRESHOLD)
        {
            // map 125 to 255 to 0 to 255
            if (y_pos < 250)
            {
                y_pos = (y_pos - STOP_UPPER_THRESHOLD) * 2;
            }
            if (y_pos >= 255)
            {
                y_pos = 255;
            }
            // printf("FW: y_pos is %d\n", y_pos);
            motor_forward(FW_PIN, BK_PIN, SPEED_PIN, y_pos);
        }
        else if (y_pos < STOP_LOWER_THRESHOLD)
        {
            // map 115 to 0 to 0 to 255
            y_pos = (STOP_LOWER_THRESHOLD - y_pos) * 2;

            // max value is (STOP_LOWER_THRESHOLD-0)*2 = 220 so just set it to max range value (255):
            // -4 to compensate for joystick error not reaching absolute 0.
            if (y_pos > STOP_LOWER_THRESHOLD * 2 - 4)
            {
                y_pos = 255;
            }
            // printf("BK: y_pos is %d\n", y_pos);
            motor_backward(FW_PIN, BK_PIN, SPEED_PIN, y_pos);
        }

        printf("\n");
        sleep_ms(100);
    }

    return 0;
}
