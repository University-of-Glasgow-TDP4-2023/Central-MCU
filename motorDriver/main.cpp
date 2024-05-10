#include "./inc/global.h"
#include "./inc/motorDriver.h"
#include "./inc/cable_transceiver.h"
#include "./inc/stabilisation_uart.h"

bool stabilisation_error_flag = false;

void uart_rx_callback()
{
    stabilisation_error_flag = process_message();
}

int main()
{
    init_motor(PWM_PIN); // init motor driver
    setupRadio();        // init transceiver

    uart_setup(UART_TX_PIN, UART_RX_PIN, (void *)uart_rx_callback); // init stabilisation uart

    // call this to turn stabilisation on or off:
    // set_stabilisation_on(true); //! how to get the value from comms?

    //! after length is measured:
    // sendCableLength(int);

    while (true)
    {
        RX_TX();

        if (stabilisation_error_flag)
        {
            //! sendMotorPacket(speed, direction, distance);
            // speed = 100 = 1m/s
            // direction = 1 = forward
            // distance = 100 = %
            sendMotorPacket(100, 1, 100 + 10, 1);
        }
        else
        {
            sendMotorPacket(100, 1, 100, 0);
        }

        // RX_TX();

        // if (stabilisation_error_flag)
        // {
        //     stabilisationError();
        // }
        RX_TX();
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