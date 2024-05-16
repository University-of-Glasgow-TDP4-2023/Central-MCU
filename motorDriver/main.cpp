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
    // DEBUG_PRINT("\n\n STABILISATION ERROR FLAG: %d\n\n", stabilisation_error_flag);
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


    // float rope_length = measure_length(); // measure length of rope

    // // sendCableLength(10);
    // sendCableLength(rope_length);

    uart_setup(UART_TX_PIN, UART_RX_PIN, (void *)uart_rx_callback); // init stabilisation uart

    DEBUG_PRINT("Motor driver started.\n");

    // bool temp = true;
    // while (true){
    //     set_stabilisation_on(temp);
    //     DEBUG_PRINT(" ####### temp value %d ### \n", temp);
    //     sleep_ms(1000);
    //     set_stabilisation_on(!temp);
    //     DEBUG_PRINT(" ####### temp value %d ### \n", !temp);
    //     sleep_ms(1000);
    // }

    while (true)
    {
        RX_TX();

        //! sendMotorPacket(speed, direction, distance);
        // speed = 100 = 1m/s
        // direction = 1 = forward
        // distance = 100 = %
        DEBUG_PRINT("current speed %f\n", get_current_speed_ms());
        actual_speed = abs((int)(get_current_speed_ms() * 100));
        DEBUG_PRINT("Actual Speed: %d\n", actual_speed);

        actual_direction = get_current_speed_ms() >= 0 ? 1 : 0;
        DEBUG_PRINT("Actual Direction: %d\n", actual_direction);
        actual_distance = (int)get_location_percent() + 10 <= 999 ? (int)get_location_percent() + 10 : 999; // cap at 999
        DEBUG_PRINT("Actual Distance: %d\n", actual_distance - 10);
        battery_error = gpio_get(BATTERY_ERROR_PIN);
        // DEBUG_PRINT("Battery error: %d\n", battery_error);
        if (is_length_measured == 0){
            DEBUG_PRINT("Length not measured yet.\n");
            sendMotorPacket(actual_speed, actual_direction, actual_distance, 9);
        }
        else if (stabilisation_error_flag && battery_error)
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
            DEBUG_PRINT("No errors detected.\n");
            sendMotorPacket(actual_speed, actual_direction, actual_distance, 0);
        }

        RX_TX();
        DEBUG_PRINT("\n");
    }

    return 0;
}