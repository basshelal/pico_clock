#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

#include "rtc.hpp"
#include "constants.h"

#define REG_SEC 0
#define REG_MIN 1
#define REG_HOUR 2
#define REG_DATE 3
#define REG_MON 4
#define REG_DOW 5
#define REG_YEAR 6
#define REG_WP 7
#define REG_TCR 8

// From https://stackoverflow.com/a/112956/7175336
const char *byte_to_binary(int x) {
    static char b[9];
    b[0] = '\0';
    int z;
    for (z = 128; z > 0; z >>= 1) {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }
    return b;
}

// From https://stackoverflow.com/a/34236981/7175336
uint8_t reverse_bits(const uint8_t n) {
    uint8_t r, i;
    for (r = 0, i = 0; i < 8; ++i)
        r |= ((n >> i) & 1) << (8 - i - 1);
    return r;
}

static void enable() {
    gpio_put(RTC_CS_PIN, 1);
}

static void disable() {
    gpio_put(RTC_CS_PIN, 0);
}

static uint8_t decode(uint8_t value) {
    uint8_t decoded = value & 127;
    decoded = (decoded & 15) + 10 * ((decoded & (15 << 4)) >> 4);
    return decoded;
}

static uint8_t readRegister(uint8_t reg) {
    // The DS1302 is least significant bit first which the pico doesn't support so we will have to do some shifting
    // The DS1302 accepts its command byte as follows:
    // 0  | 1  | 2  | 3  | 4  | 5  |  6  | 7 |
    // RD | A0 | A1 | A2 | A3 | A4 | RAM | 1 |
    // ^LSB|                             | ^MSB

    uint8_t cmdByte = 0x81;
    cmdByte |= (reg << 1);

    printf("cmd: %x %s\n", cmdByte, byte_to_binary(cmdByte));
    cmdByte = reverse_bits(cmdByte);

    uint8_t readValue;

    enable();
    spi_write_blocking(RTC_SPI, &cmdByte, 1);
    spi_read_blocking(RTC_SPI, 0, &readValue, 1);
    disable();

    uint8_t result = reverse_bits(readValue);

    printf("res: %x %s\n", result, byte_to_binary(result));

    return result;
}

static void writeRegister(uint8_t reg, uint8_t value) {
    uint8_t cmdByte = 0x80;
    cmdByte |= (reg << 1);

    cmdByte = reverse_bits(cmdByte);
    value = reverse_bits(value);

    enable();
    spi_write_blocking(RTC_SPI, &cmdByte, 1);
    spi_write_blocking(RTC_SPI, &value, 1);
    disable();
}

void DS1302RTC::init() {
    gpio_set_function(RTC_MISO_PIN, GPIO_FUNC_SPI);
    gpio_set_function(RTC_MOSI_PIN, GPIO_FUNC_SPI);
    gpio_set_function(RTC_SCK_PIN, GPIO_FUNC_SPI);

    gpio_set_function(RTC_CS_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(RTC_CS_PIN, GPIO_OUT);
    disable();

    spi_init(RTC_SPI, 500 KHz);
    spi_set_slave(RTC_SPI, false);

    spi_set_format(RTC_SPI, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

bool DS1302RTC::is_halted() {
    const uint8_t rawSeconds = readRegister(REG_SEC);
    // first bit is reserved for Clock Halt, 0 if running, 1 if halted
    bool  result = (rawSeconds >> 7) & 0x01;
    // get the top 1 bit by shifting by 7 places and then ANDing with 0000_0001 (in hex 0x01)
    // which means keep the last 1 bit (the ones with 1) and ignore (ie clear) the ones with 0s
    // example:        11011000
    // >> 7 :          10110001
    // AND 00000001 :  00000001
    // result :        1 ie true

    return result;
}

uint DS1302RTC::get_baud_rate() {
    return spi_get_baudrate(RTC_SPI);
}

void DS1302RTC::read_data() {
    printf("clock on? %s\n", is_halted() ? "false": "true");
    printf("seconds: %i\n", get_seconds());
    readRegister(REG_MIN);
    readRegister(REG_HOUR);
    readRegister(REG_DATE);
    readRegister(REG_MON);
    readRegister(REG_DOW);
    readRegister(REG_YEAR);
    readRegister(REG_WP);
    readRegister(REG_TCR);
}

uint8_t DS1302RTC::get_seconds() {
    const uint8_t rawSeconds = readRegister(REG_SEC);

    // first bit is reserved for Clock Halt, 0 if running, 1 if halted

    // next 3 bits are the seconds tens part (like if 26 then 2, or if 5 then 0)
    const uint8_t secondsTens = (rawSeconds >> 4) & 0x07;
    // get the top 4 bits by shifting by 4 places and then ANDing with 0000_0111 (in hex 0x07)
    // which means keep the last 3 bits (the ones with 1) and ignore (ie clear) the ones with 0s
    // example:        01011000
    // >> 4 :          10000101
    // AND 00000111 :  00000101
    // result :        decimal 5

    // last 4 bits are the seconds units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t secondsUnits = rawSeconds & 0x0F;
    // get the bottom 4 bits by ANDing with 0000_1111 (in hex 0x0F)
    // which means keep the last 4 bits (the ones with 1) and ignore (ie clear) the ones with 0s
    // example:        01011001
    // AND 00001111 :  00001001
    // result :        decimal 9

    const uint8_t seconds = (secondsTens * 10) + secondsUnits;
    return seconds;
}


