cmake_minimum_required(VERSION 3.13)

set(NAME pico_clock)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)
include(pimoroni_pico_import.cmake)

project(${NAME} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(${NAME}
        src/main.cpp
        src/constants.h
        src/rtc.cpp
        src/rtc.hpp
        src/battery.cpp
        )

# Include pimoroni libraries
include(libraries/pico_display_2/pico_display_2)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${NAME}
        pico_stdlib
        hardware_i2c
        hardware_spi
        hardware_timer
        pico_display_2
        )

pico_enable_stdio_usb(pico_clock 1)
pico_enable_stdio_uart(pico_clock 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${NAME})
