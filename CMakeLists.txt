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
        src/main.c
        src/constants.h
        src/custom_font.hpp
        src/rtc.h src/rtc.c
        src/battery.h src/battery.c
        src/utils.h src/utils.c
        src/display.h src/display.cpp
        src/ui_view.h src/ui_view.c
        src/button_handler.h src/button_handler.c
        src/ui_controller.h src/ui_controller.c)

# Include pimoroni libraries
include(libraries/pico_display_2/pico_display_2)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${NAME}
        pico_stdlib
        hardware_i2c
        hardware_spi
        hardware_timer
        hardware_irq
        pico_display_2
        pico_multicore
        )

pico_enable_stdio_usb(pico_clock 1)
pico_enable_stdio_uart(pico_clock 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${NAME})
