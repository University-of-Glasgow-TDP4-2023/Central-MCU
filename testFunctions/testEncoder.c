#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/timer.h"
#include "hardware/gpio.h"

// definitions, constants & variables for encoder
#define ENCODER_A_PIN 0 // pin to which encoder A disk pin is connected to
#define ENCODER_B_PIN 1 // pin to which encoder B disk pin is connected to

const int CM_PER_REV = 8;       // how much movement happening linearly in cm per 1 full revolution of the encoder
const int TIME_PERIOD_MS = 100; // time over which speed is calculated in ms
const int PULSES_PER_REV = 200; // how many pulses the encoder triggers for a whole revolution
const float TIMER_PERIOD_S = TIME_PERIOD_MS / 1000.0f;
const float DEGREES_PER_PULSE = 360.0f / (PULSES_PER_REV * 2);

int n_pulses = 0;

// definitions, constants & variables for end_switches
#define END_SWITCH_A_PIN 14      // pin to which end switch A is connected to
#define END_SWITCH_B_PIN 15      // pin to which end switch B is connected to
#define DEBOUNCE_DELAY_US 200000 // TODO: FIND BETTER SYSTEM debounce time for switches in ms

bool end_switch_A_found = false;
bool end_switch_B_found = false;

uint64_t end_switch_A_last_interrupt_t = 0;
uint64_t end_switch_B_last_interrupt_t = 0;

int location_pulses = 0;    // location of camera according to pulses
int rope_length_pulses = 0; // lenght of the rope in pulses

// flags

const int SLOWER_SPEED_PULSE_BOUNDARY = 200;

bool init_mode_active = 0;
bool emergency_stop_A_flag = false;
bool emergency_stop_B_flag = false;
bool slower_speed_clockwise_flag = false;
bool slower_speed_counterclockwise_flag = false;

#define LED_PIN 25

////////////////////////////////////////////////////////////////////////////////////////////////////////////// SIMPLE CALC FUNCTIONS

// get angular distance in degrees
// direction defined by sign
float get_angular_distance_deg(int n_pulses)
{
    return n_pulses * DEGREES_PER_PULSE;
}

// get linear distance in cm
// direction defined by sign
float get_linear_distance_cm(int n_pulses)
{
    return CM_PER_REV * get_angular_distance_deg(n_pulses) / 360.0f;
}

// get linear speed in m/s
// direction defined by sign
float get_linear_speed_m_per_s(int n_pulses)
{
    return get_linear_distance_cm(n_pulses) / TIMER_PERIOD_S / 100.0f; // div 100 to convert from cm to m
}

// get linear speed in miles/h
float get_linear_speed_mph(int n_pulses)
{
    return get_linear_speed_m_per_s(n_pulses) * 2.236936f; // constant to convert
}

// get current location of the cable cam from the second end point in meters
float get_location_m()
{
    return get_linear_distance_cm(location_pulses) / 100;
}

// get current location of the cable cam in percentage of the total rope length
float get_location_percent()
{
    return ((float)location_pulses / rope_length_pulses) * 100.0f;
}

// get the total rope length measured during the initialization phase
float get_rope_length_m()
{
    return get_linear_distance_cm(rope_length_pulses) / 100.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////// ISRs

// triggered whenever disk A value of the encoder rises or falls (as the encoder turns)
//// finds the direction of turn
//// updates n_pulses and location_pulses accordingly
//// activates security flags
void disk_a_isr()
{
    const int encoder_a_value = gpio_get(ENCODER_A_PIN);
    const int encoder_b_value = gpio_get(ENCODER_B_PIN);

    int current_direction = (encoder_a_value == encoder_b_value) ? -1 : 1; // current direction = 1 if disk values differ, or -1 if disk values equal

    n_pulses += current_direction;        // update n_pulses
    location_pulses += current_direction; // update location_pulses

    if (!init_mode_active)
    {
        slower_speed_counterclockwise_flag = (location_pulses <= SLOWER_SPEED_PULSE_BOUNDARY);               // set the flag if only SLOWER_SPEED_PULSE_BOUNDARY pulses away from counterclockwise end of rope
        slower_speed_clockwise_flag = (rope_length_pulses - location_pulses <= SLOWER_SPEED_PULSE_BOUNDARY); // set the flag if only SLOWER_SPEED_PULSE_BOUNDARY pulses away from clockwise end of rope
    }
}

// triggered every TIME_PERIOD_MS
//// calculates the information needed to be sent to the controller
void display_timer_isr(struct repeating_timer *t)
{
    float angular_distance_deg = get_angular_distance_deg(n_pulses);
    float linear_distance_cm = get_linear_distance_cm(n_pulses);
    float speed_m_s = get_linear_speed_m_per_s(n_pulses);
    float speed_mph = get_linear_speed_mph(n_pulses);
    float location_m = get_location_m();
    float location_percent = get_location_percent();

    printf("\n\n");
    printf("n_pulses %d \n", n_pulses);
    printf("° distance %.2f\n", angular_distance_deg);
    printf("Distance %.2f \n", linear_distance_cm);
    printf("Speed m/s: %.2f\n", speed_m_s);
    printf("Speed mph: %.2f\n", speed_mph);
    printf("Location pulses: %d\n", location_pulses);
    printf("Location m: %.2f\n", location_m);
    printf("Location %%: %.2f\n", location_percent);
    printf("Rope l pulses: %d\n", rope_length_pulses);
    printf("Rope l m: %.2f\n", get_rope_length_m());

    printf("\nACTIVE FLAGS:\n");

    if (emergency_stop_A_flag)
    {
        printf("    ES A\n");
    }
    if (emergency_stop_B_flag)
    {
        printf("    ES B\n");
    }
    if (slower_speed_clockwise_flag)
    {
        printf("    SS CW\n");
    }
    if (slower_speed_counterclockwise_flag)
    {
        printf("    SS CCW\n");
    }
    n_pulses = 0; // reset n_pulses
}

// called if in initialization phase and one of the end buttons was pressed
//// if first button found: change direction of motor and start measuring rope length
//// if second button found: store length of rope and finish init_mode_active, reset location pulses
void init_button_found_isr()
{
    if (end_switch_A_found && end_switch_B_found)
    {
        rope_length_pulses = abs(n_pulses);
        init_mode_active = false;
        location_pulses = 0;
        printf("Second endswitch found\n");
        printf("Rope length pulses: %d\n", rope_length_pulses);
        printf("Rope length m: %0.2f\n", get_rope_length_m());
    }
    else if (end_switch_A_found || end_switch_B_found)
    {
        printf("First endswitch found\n");
        n_pulses = 0;
        // TODO: AND GO INTO OPPOSITE DIRECTION
    }
}

// called if end switch A called and NOT in initialization phase
void end_switch_A_isr()
{
    // TODO: IMPLEMENT SHOULD STOP MOTOR
    printf("xxxxxxxxxxx EMERGENCY STOP A xxxxxxxxxxx\n");
    emergency_stop_A_flag = true;
}

// called if end switch B called and NOT in initialization phase
void end_switch_B_isr()
{
    // TODO: IMPLEMENT SHOULD STOP MOTOR
    printf("xxxxxxxxxxx EMERGENCY STOP B xxxxxxxxxxx\n");
    emergency_stop_B_flag = true;
}

// called when any interrupt pin is called
//// switch to the correct isr handler based on status of code and pin
void gpio_isr(uint gpio_pin, uint32_t events)
{

    switch (gpio_pin)
    {

    case END_SWITCH_A_PIN:

        // debounce
        if (time_us_32() - end_switch_A_last_interrupt_t < DEBOUNCE_DELAY_US)
        {
            break;
        }

        end_switch_A_last_interrupt_t = time_us_32();

        // choose handler
        if (init_mode_active && !end_switch_A_found)
        {
            end_switch_A_found = true;
            init_button_found_isr();
        }
        else
        {
            end_switch_A_isr();
        }
        break;

    case END_SWITCH_B_PIN:

        // debounce
        if (time_us_32() - end_switch_B_last_interrupt_t < DEBOUNCE_DELAY_US)
        {
            break;
        }

        end_switch_B_last_interrupt_t = time_us_32();

        // choose handler
        if (init_mode_active && !end_switch_B_found)
        {
            end_switch_B_found = true;
            init_button_found_isr();
        }
        else
        {
            end_switch_B_isr();
        }
        break;

    case ENCODER_A_PIN:
        disk_a_isr();
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////// PROGRAM FLOW FUNCTIONS

// setup all pins and interrupts
//// first thing called from main
void setup()
{
    // Initialize stdio
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Initialize encoder pins
    gpio_init(ENCODER_A_PIN);
    gpio_set_dir(ENCODER_A_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_A_PIN);

    gpio_init(ENCODER_B_PIN);
    gpio_set_dir(ENCODER_B_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_B_PIN);

    // Initialize end switch pins
    gpio_init(END_SWITCH_A_PIN);
    gpio_set_dir(END_SWITCH_A_PIN, GPIO_IN);
    gpio_pull_down(END_SWITCH_A_PIN); // TODO: IS THIS PULL UP?

    gpio_init(END_SWITCH_B_PIN);
    gpio_set_dir(END_SWITCH_B_PIN, GPIO_IN);
    gpio_pull_down(END_SWITCH_B_PIN); // TODO: IS THIS PULL UP?

    gpio_set_irq_enabled_with_callback(END_SWITCH_A_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_isr);
    gpio_set_irq_enabled(END_SWITCH_B_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(ENCODER_A_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

// initialization function
void start_init()
{
    init_mode_active = true;
    printf("START INITIALISATION\n");

    // TODO: START MOVING INTO ONE DIRECTION

    while (init_mode_active)
    {
        if (end_switch_A_found || end_switch_B_found)
        {
            printf("Searching for second end switch...\n");
        }
        else
        {
            printf("Searching for first end switch...\n");
        }

        sleep_ms(500);
    }
}

// general program
void run()
{
    // Start timer for calculating information to be sent to display
    struct repeating_timer display_timer;
    add_repeating_timer_ms(TIME_PERIOD_MS, display_timer_isr, NULL, &display_timer);

    // TODO: do nothing atm
    while (1)
    {
        sleep_ms(10000);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////// MAIN
int main()
{

    setup();

    start_init();

    run();

    return 0;
}