# Pico Clock

Simple, accurate and interactive clock built using the Raspberry Pi Pico.

This project was used as a means of getting introduced to and learning more about
embedded software development. See the [Retrospect](#retrospect) section for more.

## Hardware

### Raspberry Pi Pico

### Waveshare Pico UPS

### Pimoroni Pico Display

### DS3231 RTC

## Software

This section assumes the use of a GNU/Linux system.

### Pico SDK

### Pimoroni SDK

### CLion Setup

CLion setup is straight-forward.

Go to `File->Settings->Build,Execution,Deployment->CMake`

In both "Debug" and "Release" profiles (create "Release" if not already there) add to the
environment the path to the pico sdk and the pimoroni sdk as so:

`PICO_SDK_PATH=/home/bassam/data/pico/pico-sdk;PIMORONI_PICO_PATH=/home/bassam/data/pico/pimoroni-pico`

Change the above as needed.

CMake should now work from the IDE.

### Building, Flashing and Monitoring

The scripts [`build.sh`](./build.sh), [`autobuild.sh`](./autobuild.sh) and [`read.sh`](./read.sh)
are helpful small utility scripts to hasten this process.

[`build.sh`](./build.sh) will build the project AND flash the binary `.uf2` file to the Pico if it
is plugged in (ready to receive file).

[`autobuild.sh`](./autobuild.sh) is a neat little helper script instead of [`build.sh`](./build.sh)
that will automatically detect a newly inserted Pico and run [`build.sh`](./build.sh) automatically,
this is done by polling. Use either [`autobuild. sh`](./autobuild.sh) or [`build.sh`](./build.sh) but not both.

[`read.sh`](./read.sh) will begin reading the serial output from the plugged in Pico using `minicom` which is
found in almost all GNU/Linux distributions. To quit use `Ctrl + A` then `X` when prompted.

## Retrospect

This project was intended as an introduction to embedded systems through the Raspberry Pi Pico,
which is a relatively new microcontroller released by the Raspberry Pi Foundation. The Pi Pico is a very
inexpensive, well documented and supported microcontroller which has many accessories made for it by third party
companies (as a result of the Raspberry Pi branding). This made it perfect for small experimentation by means of
testing the limits of a well documented microcontroller platform.

I would say this project was a success in being an introduction to embedded systems development. I learned more
about C development (especially in an embedded environment), the various common communication protocols used in
embedded systems such as SPI and I2C (and to a lesser extent UART) and the way these protocols work,
and lastly the issues and (unnecessary) complexities of C++. I feel significantly more confident now as a result,
to work on more complex platforms and systems, namely the ESP32 which adds powerful features such as
built in Wi-Fi and Bluetooth but at the cost of apparent complexity. However, with this new experience from
developing on the Pico, I feel more ready to face the next challenge and work on more advanced exciting new projects.

This project introduced me to working with hardware, namely soldering and wires and breadboards, but I feel I still
have more to learn and to address as I have not had a very fruitful experience in this regard. Hardware is still
something that seems intimidating and failure-prone to me, especially regarding soldering wires, or making small
modifications. I have already broken a Pimoroni Pico Display as a result of trying to cut some excess wires
and had to buy a new one and thus not attempt this modification again fearing the same fate. Additionally, I have
experimented with soldering some wires and pins together to mixed results and heavy frustration. This is an aspect I
have certainly gained much experience in, but will need to address and work on further to improve my skills.

Finally, this project has been *extremely* valuable in teaching me about planning, time-management and software
design, not necessarily regarding embedded systems, but certainly specifically pertaining to embedded systems.
I spent and wasted a lot of time re-writing and re-implementing code that was only a result of not adequately
planning ahead and using proper software design ideals. This can be somewhat forgiven given I was not entirely aware
how far I was going to go with this project and what I wanted to do, but in any case, I have learned a great deal
regarding planning and designing the system architecture ahead of time to save a lot of time re-writing and thinking
about solutions. This is a skill I will definitely carry over with me to future projects, and I expect I will still
learn more about this with time and experience.

## Thanks

Special thanks to the Raspberry Pi Foundation who have yet again changed my life with this inexpensive and
powerful microcontroller with excellent support and a strong community. In 2019/2020, it was the Raspberry Pi 3 and 4
that changed my life and introduced me to GNU/Linux and the power of Free As In Freedom Software. Their pursuit
for inexpensive and well-supported hardware and software means anyone can buy and hack with (and inevitably break)
their products without feeling defeated. This encourages experimentation with new ideas and was what made
me realize the power of GNU/Linux once I was no longer intimidated by it (especially if I broke something). They
have successfully blessed this same philosophy into the Pico which I hope I can look back at in many years time
as the introduction of embedded systems programming into my life.

Thanks to [this library](https://codebender.cc/library/DS1302#DS1302.cpp)
by [Henning Karlsen](http://www.rinkydinkelectronics.com/) which was a great guide in figuring out what I was doing
wrong. Read the datasheet carefully and multiple times, a few words could cost you hours if you don't
pay attention to them, I have now learned this lesson the hard way.