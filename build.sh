#!/usr/bin/env bash
cmake --build cmake-build-debug/ --target pico_clock -- -j 6 && cp cmake-build-debug/pico_clock.uf2 /media/bassam/RPI-RP2/
