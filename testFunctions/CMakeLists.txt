set(TESTTWOCORES TestTwoCores)
set(TESTPWM TestPWM)
set(TESTGPIO TestGPIO)
set(TESTADC TestADC)
set(TESTENCODERWITHINIT TestEncoderWithInit)
set(TESTENCODERWITHPOS TestEncoderWithPos)

add_executable(${TESTPWM}
    testPWM.c
)

add_executable(${TESTGPIO}
    testGPIO.c
)

add_executable(${TESTADC}
    testADC.c
)

add_executable(${TESTTWOCORES}
    myTwoCores.c
)

add_executable(${TESTENCODERWITHINIT}
    Incremental_with_initialization.c
)

add_executable(${TESTENCODERWITHPOS}
    Incremental_with_position.c
)

# Pull in our pico_stdlib which pulls in commonly used features and link the hardware_pwm library
target_link_libraries(${TESTPWM} pico_stdlib hardware_pwm hardware_adc)
target_link_libraries(${TESTGPIO} pico_stdlib)
target_link_libraries(${TESTADC} pico_stdlib hardware_adc)
target_link_libraries(${TESTTWOCORES} pico_stdlib pico_multicore)
target_link_libraries(${TESTENCODERWITHINIT}     
    pico_stdlib
    hardware_gpio
    hardware_irq
    hardware_timer
)
target_link_libraries(${TESTENCODERWITHPOS}     
    pico_stdlib
    hardware_gpio
    hardware_irq
    hardware_timer
)


# Enable USB output, disable UART output:
# 1 = enable, 0 = disable.
pico_enable_stdio_usb(${TESTPWM} 1) 
pico_enable_stdio_uart(${TESTPWM} 0)

pico_enable_stdio_usb(${TESTGPIO} 1)
pico_enable_stdio_uart(${TESTGPIO} 0)

pico_enable_stdio_usb(${TESTADC} 1)
pico_enable_stdio_uart(${TESTADC} 0)

pico_enable_stdio_usb(${TESTTWOCORES} 1)
pico_enable_stdio_uart(${TESTTWOCORES} 0)

pico_enable_stdio_usb(${TESTENCODERWITHINIT} 1)
pico_enable_stdio_uart(${TESTENCODERWITHINIT} 0)

pico_enable_stdio_usb(${TESTENCODERWITHPOS} 1)
pico_enable_stdio_uart(${TESTENCODERWITHPOS} 0)

# Create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(${TESTPWM})
pico_add_extra_outputs(${TESTGPIO})
pico_add_extra_outputs(${TESTADC})
pico_add_extra_outputs(${TESTTWOCORES})
pico_add_extra_outputs(${TESTENCODERWITHINIT})
pico_add_extra_outputs(${TESTENCODERWITHPOS})

