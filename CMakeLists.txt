cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
# ---------------------------------------------------

# set(PROJECT_NAME my_project)

# project(my_project)


# rest of your project..

add_executable(TestPWM
    testPWM.c
)

# Pull in our pico_stdlib which pulls in commonly used features
target_link_libraries(TestPWM pico_stdlib)

# enable usb output, disable uart output
# pico_enable_stdio_usb(TestPWM 1) # 1 = enable, 0 = disable

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(TestPWM)


