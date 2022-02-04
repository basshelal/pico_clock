# Pico Clock

## Development Setup

I use JetBrains CLion

Go to File->Settings->Build,Execution,Deployment->CMake

Add a new profile for "Release"

In both "Debug" and "Release" profiles add to the environment the path to the pico sdk and the pimoroni sdk as so:

PICO_SDK_PATH=/home/bassam/data/pico/pico-sdk;PIMORONI_PICO_PATH=/home/bassam/data/pico/pimoroni-pico

## Thanks

Thanks to [this library](https://codebender.cc/library/DS1302#DS1302.cpp)
by [Henning Karlsen](http://www.rinkydinkelectronics.com/) which was a great guide in figuring out what I was doing
wrong.

Read the datasheet carefully and multiple times, a few words could cost you hours if you don't pay attention to them.