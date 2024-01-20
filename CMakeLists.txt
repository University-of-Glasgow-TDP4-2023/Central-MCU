cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(TestPWM)

set(MOTOR_DRIVER MotorDriver)
project(${MOTOR_DRIVER})

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


add_executable(${MOTOR_DRIVER}
    mortorDriver.c
)

add_executable(TestPWM
testPWM.c
)

add_executable(TestGPIO
    testGPIO.c
)

# Pull in our pico_stdlib which pulls in commonly used features and link the hardware_pwm library
target_link_libraries(TestPWM pico_stdlib hardware_pwm hardware_adc)

target_link_libraries(TestGPIO pico_stdlib)


target_link_libraries(${MOTOR_DRIVER} pico_stdlib hardware_pwm hardware_adc)

# Enable USB output, disable UART output:
# 1 = enable, 0 = disable.
pico_enable_stdio_usb(TestPWM 1) 
pico_enable_stdio_uart(TestPWM 0)

pico_enable_stdio_usb(TestGPIO 1)
pico_enable_stdio_uart(TestGPIO 0)

pico_enable_stdio_usb(${MOTOR_DRIVER} 1)
pico_enable_stdio_uart(${MOTOR_DRIVER} 0)

# Create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(TestPWM)
pico_add_extra_outputs(TestGPIO)

pico_add_extra_outputs(${MOTOR_DRIVER})