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
#define PWM_DIV_INT_SHIFT 4
// for 32kHz PWM clock frequency = 31250ns PWM clock period.
// WRAP = PWM clock period / system clock period = 31250ns / 8ns = 3906
#define PWM_WRAP 3906
#define PWM_CLOCK_FREQ 4000 // 4KHz

// Define joystick GPIO pin for input (analog):
#define Y_PIN 31 // Pin 31

// Define joystick raw values:
#define MIN_JOYSTICK_VALUE 20
#define MAX_JOYSTICK_VALUE 4095
#define STATIONARY_JOYSTICK_VALUE 1850 // Mid point - actual range fluctuates between 1800 and 1900

// Define motor driver constants:
#define STOP_PWM_TIME 1500 // microseconds for stop
#define STOP_RANGE 50      // range of values around the stop value
#define FW_PWM_TIME 1900   // microseconds for forward
#define BK_PWM_TIME 1100   // microseconds for backward

#if DEBUG
// Define how many bars to use for visualisation:
#define VISUALISATION_WIDTH 20
#endif

//! Constants for comms:
#define CE_PIN 22
#define CSN_PIN 21

#endif // GLOBAL_H
