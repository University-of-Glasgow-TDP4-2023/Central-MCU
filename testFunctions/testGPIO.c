#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"

#define FW_PIN 2 // pin 4
#define BK_PIN 3 // pin 5

int main()
{
    stdio_init_all();

    gpio_init(FW_PIN);
    gpio_set_dir(FW_PIN, GPIO_OUT);

    gpio_init(BK_PIN);
    gpio_set_dir(BK_PIN, GPIO_OUT);

    uint8_t value1 = 0;

    while (true)
    {
        gpio_put(FW_PIN, value1);
        if (value1 == 0)
        {
            value1 = 1;
        }
        else
        {
            value1 = 0;
        }
        gpio_put(BK_PIN, value1);
        printf("GPIOs are %d, %d\n", gpio_get(FW_PIN), gpio_get(BK_PIN));
        sleep_ms(200);
    }

    return 0;
}
