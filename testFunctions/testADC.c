#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

int main() {
    stdio_init_all();
    adc_init();

    adc_gpio_init(31);

    while (1) {
        adc_select_input(0);
        uint adc_x_raw = adc_read();

        printf("X: %d\n", adc_x_raw);

        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 1;
        uint bar_x_pos = adc_x_raw * bar_width / adc_max;

        sleep_ms(50);

    }
}
