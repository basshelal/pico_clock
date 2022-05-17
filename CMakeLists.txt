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

set(SRC_FILES
        src/main.c
        src/constants.h
        src/utils.h src/utils.c

        src/peripherals/peripherals.h src/peripherals/peripherals.c
        src/peripherals/rtc.h src/peripherals/rtc.c
        src/peripherals/battery.h src/peripherals/battery.c
        src/peripherals/display.h src/peripherals/display.cpp

        src/middleware/button_handler.h src/middleware/button_handler.c

        src/ui/ui.h src/ui/ui.c
        src/ui/ui_model.h src/types.h src/ui/ui_model.c
        src/ui/ui_view.h src/ui/ui_view.c
        src/ui/ui_controller.h src/ui/ui_controller.c)

add_executable(${NAME} ${SRC_FILES})

# Include pimoroni libraries
include(libraries/pico_display_2/pico_display_2)
include(libraries/bitmap_fonts/bitmap_fonts)

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

set_source_files_properties(
        ${SRC_FILES}
        PROPERTIES COMPILE_OPTIONS "-Wno-error=discarded-qualifiers"
)