#ifndef ENCODER_H
#define ENCODER_H

#include <math.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "./global.h"
#include "./motorDriver.h"

// extern struct repeating_timer display_timer;

// extern int speed = 100; // motor speed

// Function prototypes
float get_angular_distance_deg(int n_pulses);
float get_linear_distance_cm(int n_pulses);
float get_linear_speed_m_per_s(int n_pulses);
float get_linear_speed_mph(int n_pulses);
float get_location_m();
float get_location_percent();
float get_rope_length_m();

void disk_a_isr();
bool display_timer_isr(struct repeating_timer *t);
void init_button_found_isr();
void end_switch_A_isr();
void end_switch_B_isr();
void gpio_isr(uint gpio_pin, uint32_t events);

void encoder_init();
float measure_length();

float get_current_speed_ms();


#endif // ENCODER_H