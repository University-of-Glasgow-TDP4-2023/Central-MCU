#include "./inc/global.h"
#include "./inc/motorDriver.h"
#include "./inc/cable_transceiver.h"
#include "./inc/stabilisation_uart.h"
#include "./inc/encoder.h"

bool stabilisation_error_flag = false;
int actual_speed;
int actual_direction;
int actual_distance;
int battery_error = 0;

void uart_rx_callback()
{
    stabilisation_error_flag = process_message();
}

int main()
{
    DEBUG_PRINT("Starting motor driver...\n");

    // STDIO initialization:
    stdio_init_all();

    // GPIO initialization:
    gpio_init(FW_PIN);
    gpio_set_dir(FW_PIN, GPIO_OUT);
    gpio_init(BK_PIN);
    gpio_set_dir(BK_PIN, GPIO_OUT);
    // GPIO for battery error:
    gpio_init(BATTERY_ERROR_PIN);
    gpio_set_dir(BATTERY_ERROR_PIN, GPIO_IN);


    init_motor(PWM_PIN); // init motor driver
    encoder_init();      // init encoder
    setupRadio();        // init transceiver

    float rope_length = measure_length(); // measure length of rope

    sendCableLength(rope_length);

    uart_setup(UART_TX_PIN, UART_RX_PIN, (void *)uart_rx_callback); // init stabilisation uart

    DEBUG_PRINT("Motor driver started.\n");



    while (true)
    {
        RX_TX();

        //! sendMotorPacket(speed, direction, distance);
        // speed = 100 = 1m/s
        // direction = 1 = forward
        // distance = 100 = %
        actual_speed = abs((int) get_current_speed_ms()*100);
        actual_direction = get_current_speed_ms() >= 0 ? 1 : 0;
        actual_distance = (int) get_location_percent()+10;
        battery_error = gpio_get(BATTERY_ERROR_PIN);

        DEBUG_PRINT("Speed: %d\n", actual_speed);
        DEBUG_PRINT("Direction: %d\n", actual_direction);
        DEBUG_PRINT("Distance: %d\n", actual_distance-10);
        DEBUG_PRINT("Battery error: %d\n", battery_error);

        if (stabilisation_error_flag && battery_error)
        {
            DEBUG_PRINT("Stabilisation error and battery error detected.\n");
            sendMotorPacket(actual_speed, actual_direction, actual_distance, 3);
        }
        else if (battery_error)
        {
            DEBUG_PRINT("Battery error detected.\n");
            sendMotorPacket(actual_speed, actual_direction, actual_distance, 2);
        }

        else if (stabilisation_error_flag)
        {
            DEBUG_PRINT("Stabilisation error detected.\n");
            sendMotorPacket(actual_speed, actual_direction, actual_distance, 1);
        }
        else
        {
            DEBUG_PRINT("No stabilisation error detected.\n");
            sendMotorPacket(actual_speed, actual_direction, actual_distance, 0);
        }

        // RX_TX();

        // if (stabilisation_error_flag)
        // {
        //     stabilisationError();
        // }
        RX_TX();
        DEBUG_PRINT("\n");
    }

    // uint16_t y_pos_raw = 0;

    // pwm high time in microseconds:
    // uint32_t high_time = 0;

    // uint32_t system_clock_hz = clock_get_hz(clk_sys);
    // uint32_t system_clock_period_ns = 1000000000 / system_clock_hz;

    // uint32_t pwm_counter = 0;

    //     while (true)
    //     {

    // #if DEBUG
    //         DEBUG_PRINT("system clock: %d Hz\n", system_clock_hz);
    //         DEBUG_PRINT("system clock period: %d ns\n", system_clock_period_ns);
    // #endif

    // raw adc value between 0 and 2**12-1 (0-4095):
    // y_pos_raw = get_adc_value(0);

    // DEBUG_PRINT("Raw joystick value: %d\n", y_pos_raw);

    // high_time = map_range(y_pos_raw, MIN_JOYSTICK_VALUE, MAX_JOYSTICK_VALUE, FW_PWM_TIME, BK_PWM_TIME);
    // if (high_time > STOP_PWM_TIME - STOP_RANGE && high_time < STOP_PWM_TIME + STOP_RANGE)
    //     high_time = STOP_PWM_TIME;

    // #if DEBUG
    //         printf("PWM high time %d ms: ", high_time);
    //         for (uint8_t i = 0; i < map_range(high_time, BK_PWM_TIME, FW_PWM_TIME, 0, VISUALISATION_WIDTH); i++)
    //             printf("#");
    //         printf("\n");
    // #endif

    // pwm_set_chan_level(pwm_gpio_to_slice_num(PWM_PIN), PWM_CHAN_A, high_time);

    // pwm_counter = pwm_get_counter(pwm_gpio_to_slice_num(PWM_PIN));
    // DEBUG_PRINT("PWM counter: %d\n", pwm_counter);

    // DEBUG_PRINT("\n");

    //     sleep_ms(100);
    // }

    return 0;
}