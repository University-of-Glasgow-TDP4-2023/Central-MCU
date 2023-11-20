cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

# set(PROJECT_NAME my_project)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(PWM
    testPWM.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(PWM pico_stdlib hardware_pwm)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(PWM)
