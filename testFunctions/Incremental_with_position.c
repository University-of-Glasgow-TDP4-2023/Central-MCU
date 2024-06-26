#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/timer.h"

#define PIN_A 0
#define PIN_B 1
#define PIN_BUTTON_A 14
#define PIN_BUTTON_B 15
#define TIME_PERIOD_MS 1000

volatile int pulse_counter = 0;
volatile int total_pulses = 0;
volatile int current_direction = 0;

volatile int init_button_A_found = 0;
volatile int init_button_B_found = 0;

double rope_length_m = 0;

alarm_id_t timer_id;

void ISR_A(uint gpio, uint32_t events);
void ISR_button_A_init(uint gpio, uint32_t events);
void ISR_button_B_init(uint gpio, uint32_t events);
int64_t display_callback(alarm_id_t id, void *user_data);

void ISR_A(uint gpio, uint32_t events) {
    gpio_set_irq_enabled(PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);

    int state_A = gpio_get(PIN_A);
    int state_B = gpio_get(PIN_B);

    if (state_A == state_B) {
        current_direction = -1;
    } else {
        current_direction = 1;
    }

    pulse_counter += current_direction;
    total_pulses += current_direction;

    gpio_set_irq_enabled(PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

void ISR_button_A_init(uint gpio, uint32_t events) {
    gpio_set_irq_enabled(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, false);

    printf("First end-point found.\nSearching for second end-point...\n");

    pulse_counter = 0;
    init_button_A_found = 1;

    // gpio_set_irq_enabled(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, true, ISR_button_B_init);
}

void ISR_button_B_init(uint gpio, uint32_t events) {
    gpio_set_irq_enabled(PIN_BUTTON_B, GPIO_IRQ_EDGE_FALL, false);
    init_button_B_found = 1;

    total_pulses = 0;

    printf("Second end-point found.\n");
}

double get_angular_distance() {

    const int pulses_per_round_per_disk = 100;
    const double degrees_per_pulse = 360.0 / (pulses_per_round_per_disk * 2);

    return pulse_counter * degrees_per_pulse;
}

double get_linear_distance() {
    const double wheel_diameter = 8.0;
    const double wheel_circumference = M_PI * wheel_diameter;
    return wheel_circumference * (get_angular_distance(pulse_counter) / 360.0);
}

double get_linear_speed() {
    const double time_period_s = TIME_PERIOD_MS / 1000.0;
    double speed_cm_per_s = get_linear_distance(pulse_counter) / time_period_s;
    double speed_m_per_s = speed_cm_per_s / 100.0;
    return speed_m_per_s;
}

void reset_counters() {
    current_direction = 0;
    pulse_counter = 0;
}

void deactivate_interrupts() {
    gpio_set_irq_enabled(PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false);
    cancel_alarm(timer_id);
}

void activate_interrupts() {
    // gpio_set_irq_enabled(PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled_with_callback(PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, ISR_A);

    timer_id = add_alarm_in_ms(TIME_PERIOD_MS, display_callback, NULL, true);
}

void start_initialisation() {
    deactivate_interrupts();

    // gpio_set_irq_enabled_with_callback(PIN_A, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, ISR_A);
    gpio_set_irq_enabled_with_callback(PIN_BUTTON_A, GPIO_IRQ_EDGE_FALL, true, ISR_button_A_init);

    printf("Initialisation running...\nSearching for first end-point...\n");

    while (!init_button_A_found || !init_button_B_found) {
        sleep_ms(1000);
        printf("Please wait...\n");
    }

    rope_length_m = fabs(get_linear_distance(total_pulses) / 100.0);

    printf("Total rope length: %.2f m\n", rope_length_m);
    printf("Ready for operation.\n");

    activate_interrupts();
}

double get_location_m() {
    return get_linear_distance() / 100.0;
}

double get_location_perc() {
    return (get_location_m() / rope_length_m) * 100.0;
}

int64_t display_callback(alarm_id_t id, void *user_data) {
    printf("Rope length: %.2f m\n", rope_length_m);
    printf("LOCATION m: %.2f m\n", get_location_m());
    printf("LOCATION %%: %.2f %%\n", get_location_perc());

    reset_counters();
    return TIME_PERIOD_MS * 1000;
}

int main() {
    stdio_init_all();

    gpio_init(PIN_A);
    gpio_set_dir(PIN_A, GPIO_IN);
    gpio_pull_up(PIN_A);

    gpio_init(PIN_B);
    gpio_set_dir(PIN_B, GPIO_IN);
    gpio_pull_up(PIN_B);

    gpio_init(PIN_BUTTON_A);
    gpio_set_dir(PIN_BUTTON_A, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_A);

    gpio_init(PIN_BUTTON_B);
    gpio_set_dir(PIN_BUTTON_B, GPIO_IN);
    gpio_pull_down(PIN_BUTTON_B);

    start_initialisation();

    while (1) {
        tight_loop_contents();
    }
}
