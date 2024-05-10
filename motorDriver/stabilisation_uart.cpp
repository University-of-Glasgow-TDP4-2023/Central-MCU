
#include "./inc/stabilisation_uart.h"

bool process_message()
{
    uint8_t c = uart_getc(UART_ID);

    switch (c)
    {
    case 't':
        return true;
    case 'f':
        return false;
    default:
        return false;
    }
}

void send_error(bool has_error)
{
    if (has_error)
    {
        uart_putc(UART_ID, 't');
    }
    else
    {
        uart_putc(UART_ID, 'f');
    }
}

void set_stabilisation_on(bool is_on)
{
    if (is_on)
    {
        uart_putc(UART_ID, 't');
    }
    else
    {
        uart_putc(UART_ID, 'f');
    }
}

// void uart_rx_callback()
// {
//     stabilisation_error_flag = process_message();
//     //! send stabilisation error flag to comms..
// }

void uart_setup(int tx_pin, int rx_pin, void *uart_rx_callback)
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(tx_pin, GPIO_FUNC_UART);
    gpio_set_function(rx_pin, GPIO_FUNC_UART);

    irq_set_exclusive_handler(UART0_IRQ, (irq_handler_t)uart_rx_callback);
    irq_set_enabled(UART0_IRQ, true);

    uart_set_irq_enables(UART_ID, true, false);
}
