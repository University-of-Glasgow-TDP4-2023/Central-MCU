#ifndef STABILISATION_UART_H
#define STABILISATION_UART_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

#define UART_ID uart1
#define BAUD_RATE 115200

bool process_message();

void send_error(bool has_error);

void set_stabilisation_on(bool is_on);

void uart_setup(int tx_pin, int rx_pin, void *uart_rx_callback);

void obc_init_uart();

void stabMCU_init_uart();

// void uart_rx_callback();

#endif // STABILISATION_UART_H