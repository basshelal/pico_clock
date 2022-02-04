#include "pico/stdlib.h"
#include <stdio.h>

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

// From https://stackoverflow.com/a/34236981/7175336
uint8_t msb_to_lsb(const uint8_t n) {
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
    uint8_t cmdByte = 0x81;
    cmdByte |= (reg << 1);

    printf("cmd: %x ", cmdByte);
    cmdByte = msb_to_lsb(cmdByte);
    printf("(%x) -> ", cmdByte);

    uint8_t readValue;

    enable();
    spi_write_blocking(RTC_SPI, &cmdByte, 1);
    spi_read_blocking(RTC_SPI, 0, &readValue, 1);
    disable();

    printf("res: %x\n", readValue);

    return readValue;
}

static void writeRegister(uint8_t reg, uint8_t value) {
    uint8_t cmdByte = 0x80;
    cmdByte |= (reg << 1);



    cmdByte = msb_to_lsb(cmdByte);
    value = msb_to_lsb(value);

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

uint DS1302RTC::get_baud_rate() {
    return spi_get_baudrate(RTC_SPI);
}

void DS1302RTC::read_data() {
    // The DS1302 is least significant bit first which the pico doesn't support so we will have to do some shifting
    // The DS1302 accepts its command byte as follows:
    // 0  | 1  | 2  | 3  | 4  | 5  |  6  | 7 |
    // RD | A0 | A1 | A2 | A3 | A4 | RAM | 1 |
    // ^LSB|                             | ^MSB

    readRegister(REG_SEC);
    readRegister(REG_MIN);
    readRegister(REG_HOUR);
    readRegister(REG_DATE);
    readRegister(REG_MON);
    readRegister(REG_DOW);
    readRegister(REG_YEAR);
    readRegister(REG_WP);
    readRegister(REG_TCR);

}