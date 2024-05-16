#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h> // For uint8_t, uint16_t, etc.
#include <stdio.h>

#define DEBUG 1

#if DEBUG
// DEBUG_PRINT only works if DEBUG is enabled.
#define DEBUG_PRINT(...)     \
    do                       \
    {                        \
        printf(__VA_ARGS__); \
    } while (0)
#else
#define DEBUG_PRINT(fmt, args...) // Don't print anything
#endif

// Define GPIO pin for PWM (speed and direction control):
#define PWM_PIN 0 // Pin 1
#define PWM_MAX_VALUE 255
#define MID_SPEED 100
#define PWM_MIN_VALUE 0
// #define PWM_DIV_INT_SHIFT 4
// for 32kHz PWM clock frequency = 31250ns PWM clock period.
// WRAP = PWM clock period / system clock period = 31250ns / 8ns = 3906
// #define PWM_WRAP 3906
// #define PWM_CLOCK_FREQ 4000 // 4KHz

// Define joystick GPIO pin for input (analog):
// #define Y_PIN 31 // Pin 31

// GPIO pins for motor direction control:
#define FW_PIN 14 // pin 19
#define BK_PIN 15 // pin 20

// Define joystick raw values:
// #define MIN_JOYSTICK_VALUE 20
// #define MAX_JOYSTICK_VALUE 4095
// #define STATIONARY_JOYSTICK_VALUE 1850 // Mid point - actual range fluctuates between 1800 and 1900

// Define motor driver constants:
// #define STOP_PWM_TIME 1500 // microseconds for stop
// #define STOP_RANGE 50      // range of values around the stop value
// #define FW_PWM_TIME 1900   // microseconds for forward
// #define BK_PWM_TIME 1100   // microseconds for backward

#if DEBUG
// Define how many bars to use for visualisation:
#define VISUALISATION_WIDTH 20
#endif

//' Constants for comms:
#define CE_PIN 22
#define CSN_PIN 21
#define SPEED_MAX_VALUE 1024
#define SPEED_MIN_VALUE 0

//' UART:
#define UART_TX_PIN 4 // pin 6
#define UART_RX_PIN 5 // pin 7

//' Encoder:
// definitions, constants & variables for encoder
#define ENCODER_A_PIN 10 // pin to which encoder A disk pin is connected to
#define ENCODER_B_PIN 11 // pin to which encoder B disk pin is connected to
// definitions, constants & variables for end_switches
#define END_SWITCH_A_PIN 12      // pin to which end switch A is connected to
#define END_SWITCH_B_PIN 13      // pin to which end switch B is connected to
#define DEBOUNCE_DELAY_US 200000 // TODO: FIND BETTER SYSTEM debounce time for switches in ms
#define LED_PIN 25

//' Battery error pin:
#define BATTERY_ERROR_PIN 6 // pin 9

extern int encoder_init_mode_active;

#endif // GLOBAL_H
