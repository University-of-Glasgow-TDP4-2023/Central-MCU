#include "pico/stdlib.h"
#include "pico/multicore.h"

// Function to run on core 0
void core0_task()
{
    const uint LED_PIN_CORE0 = 25; // Built-in LED on Pico
    gpio_init(LED_PIN_CORE0);
    gpio_set_dir(LED_PIN_CORE0, GPIO_OUT);
    while (true)
    {
        gpio_put(LED_PIN_CORE0, 1);
        sleep_ms(1000);
        gpio_put(LED_PIN_CORE0, 0);
        sleep_ms(1000);
    }
}

// Function to run on core 1
void core1_task()
{
    const uint LED_PIN_CORE1 = 15; // Example pin for an external LED
    gpio_init(LED_PIN_CORE1);
    gpio_set_dir(LED_PIN_CORE1, GPIO_OUT);
    int delay = 500;
    while (true)
    {
        gpio_put(LED_PIN_CORE1, 1);
        sleep_ms(delay);
        gpio_put(LED_PIN_CORE1, 0);
        sleep_ms(delay);
        delay += 100; // Increase delay
        if (delay > 2000)
            delay = 500; // Reset delay
    }
}

int main()
{
    stdio_init_all();

    // Launch core 1 task
    multicore_launch_core1(core1_task);

    // Execute core 0 task
    core0_task();

    return 0;
}
