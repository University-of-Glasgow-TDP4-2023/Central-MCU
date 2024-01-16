cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(TestPWM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project


# Rest of your project

add_executable(TestPWM
    testPWM.c
)

# Pull in our pico_stdlib which pulls in commonly used features
# and link the hardware_pwm library
target_link_libraries(TestPWM pico_stdlib hardware_pwm hardware_adc)

# Enable USB output, disable UART output
pico_enable_stdio_usb(TestPWM 1) # 1 = enable, 0 = disable
pico_enable_stdio_uart(TestPWM 0) # 1 = enable, 0 = disable
# Uncomment the next line if you want to disable UART output
# pico_enable_stdio_uart(TestPWM 0)

# Create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(TestPWM)
