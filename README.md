# Pico Clock

## Development Setup

I use JetBrains CLion

Go to File->Settings->Build,Execution,Deployment->CMake

Add a new profile for "Release"

In both "Debug" and "Release" profiles add to the environment the path to the pico sdk and the pimoroni sdk as so:

PICO_SDK_PATH=/home/bassam/data/pico/pico-sdk;PIMORONI_PICO_PATH=/home/bassam/data/pico/pimoroni-pico
