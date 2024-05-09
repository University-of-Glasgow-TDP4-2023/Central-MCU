# use "make MotorDriver" to build this project:
set(MOTOR_DRIVER MotorDriver)

include_directories(inc)

include(./inc/RF24/CMakeLists.txt)

add_executable(${MOTOR_DRIVER}
    main.cpp
    motorDriver.cpp
    cable_transceiver.cpp

)

# Pull in our pico_stdlib which pulls in commonly used features and link the hardware_pwm library
target_link_libraries(${MOTOR_DRIVER} pico_stdlib hardware_pwm hardware_adc hardware_spi hardware_i2c RF24)

# Enable USB output, disable UART output:
# 1 = enable, 0 = disable.
pico_enable_stdio_usb(${MOTOR_DRIVER} 1)
pico_enable_stdio_uart(${MOTOR_DRIVER} 0)

# Create map/bin/hex/uf2 file in addition to ELF
pico_add_extra_outputs(${MOTOR_DRIVER})
