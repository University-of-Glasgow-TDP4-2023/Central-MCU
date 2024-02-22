set(TESTPWM TestPWM)
set(TESTGPIO TestGPIO)
set(TESTADC TestADC)

add_executable(${TESTPWM}
    testPWM.c
)

add_executable(${TESTGPIO}
    testGPIO.c
)

add_executable(${TESTADC}
    testADC.c
)

# Pull in our pico_stdlib which pulls in commonly used features and link the hardware_pwm library
target_link_libraries(${TESTPWM} pico_stdlib hardware_pwm hardware_adc)

target_link_libraries(${TESTGPIO} pico_stdlib)

target_link_libraries(${TESTADC} pico_stdlib hardware_adc)

# Enable USB output, disable UART output:
# 1 = enable, 0 = disable.
pico_enable_stdio_usb(${TESTPWM} 1) 
pico_enable_stdio_uart(${TESTPWM} 0)

pico_enable_stdio_usb(${TESTGPIO} 1)
pico_enable_stdio_uart(${TESTGPIO} 0)

pico_enable_stdio_usb(${TESTADC} 1)
pico_enable_stdio_uart(${TESTADC} 0)

# Create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(${TESTPWM})
pico_add_extra_outputs(${TESTGPIO})
pico_add_extra_outputs(${TESTADC})
