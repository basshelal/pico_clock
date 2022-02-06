#include "pico/stdlib.h"
#include <stdio.h>

#include "rtc.hpp"
#include "constants.hpp"
#include "utils.hpp"

#define REG_SECONDS 0
#define REG_MINUTES 1
#define REG_HOURS 2
#define REG_DATE 3
#define REG_MONTH 4
#define REG_WEEKDAY 5
#define REG_YEAR 6
#define REG_WRITE_PROTECT 7
#define REG_TRICKLE_CHARGE 8

static void enable() {
    gpio_put(RTC_CS_PIN, 1);
}

static void disable() {
    gpio_put(RTC_CS_PIN, 0);
}

uint8_t DS1302RTC::read_register(uint8_t reg) {
    // The DS1302 is least-significant-bit first which the pico doesn't support, so we have to reverse the bits
    // between spi calls to both read and write

    uint8_t cmdByte = 0x81; // 10000001 first bit is mandatory for communication, last bit signifies read
    cmdByte |= (reg << 1);

    cmdByte = reverse_bits(cmdByte);

    uint8_t readValue;

    enable();
    spi_write_blocking(RTC_SPI, &cmdByte, 1);
    spi_read_blocking(RTC_SPI, 0, &readValue, 1);
    disable();

    uint8_t result = reverse_bits(readValue);
    log("rtc read register %u ->\t0x%02x 0b%s\n",
        reg, result, byte_to_binary(result));

    return result;
}

void DS1302RTC::write_register(uint8_t reg, uint8_t value) {
    uint8_t cmdByte = 0x80; // 10000000 first bit is mandatory for communication, last bit signifies read 0 means write
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

bool DS1302RTC::is_running() {
    const uint8_t rawSeconds = read_register(REG_SECONDS);
    // first bit is reserved for Clock Halt, 0 if running, 1 if halted
    bool isHalted = get_bits(rawSeconds, 0, 0);
    return !isHalted;
}

bool DS1302RTC::is_writable() {
    const uint8_t rawWriteProtect = read_register(REG_WRITE_PROTECT);
    // first bit is reserved for Write Protect, 0 if writable, 1 if write protected
    bool isProtected = get_bits(rawWriteProtect, 0, 0);
    return !isProtected;
}

uint DS1302RTC::get_baud_rate() {
    return spi_get_baudrate(RTC_SPI);
}

uint8_t DS1302RTC::get_seconds() {
    const uint8_t rawSeconds = read_register(REG_SECONDS);

    // first bit is reserved for Clock Halt, 0 if running, 1 if halted

    // next 3 bits are the seconds tens part (like if 26 then 2, or if 5 then 0)
    const uint8_t secondsTens = get_bits(rawSeconds, 1, 3);

    // last 4 bits are the seconds units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t secondsUnits = get_bits(rawSeconds, 4, 7);

    const uint8_t seconds = (secondsTens * 10) + secondsUnits;
    return seconds;
}

uint8_t DS1302RTC::get_minutes() {
    const uint8_t rawMinutes = read_register(REG_MINUTES);

    // first bit is blank/ignored

    // next 3 bits are the minutes tens part (like if 26 then 2, or if 5 then 0)
    const uint8_t minutesTens = get_bits(rawMinutes, 1, 3);

    // last 4 bits are the minutes units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t minutesUnits = get_bits(rawMinutes, 4, 7);

    const uint8_t minutes = (minutesTens * 10) + minutesUnits;
    return minutes;
}

uint8_t DS1302RTC::get_hours() {
    const uint8_t rawHours = read_register(REG_HOURS);

    // first bit is 12/24 mode, if 1 then in 12hr mode, if 0 then in 24hr mode
    const bool hoursMode = get_bits(rawHours, 0, 0);

    uint8_t hoursTens;
    if (!hoursMode) { // 24hr mode
        // bit 3 and 4 are hours tens part (like if 26 then 2, or if 5 then 0)
        hoursTens = get_bits(rawHours, 2, 3);
    } else { // 12hr mode
        // bit 4 is the hours tens part (like if 26 then 2, or if 5 then 0)
        const uint8_t hoursTens12hr = get_bits(rawHours, 3, 3);

        // bit 5 is am or pm, 1 is pm, 0 is am
        const uint8_t hoursPM = get_bits(rawHours, 4, 4);

        // tens in 24hr mode is just tens in 12hr mode plus 1 if it is pm, ie if 10pm then 1+1 ie 2
        // (tens of 22 in 24hr mode)
        hoursTens = hoursTens12hr + hoursPM;
    }

    // last 4 bits are the hours units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t hoursUnits = get_bits(rawHours, 4, 7);

    const uint8_t hours = (hoursTens * 10) + hoursUnits;
    return hours;
}

uint8_t DS1302RTC::get_date() {
    const uint8_t rawDate = read_register(REG_DATE);

    // first 2 bits are blank/ignored

    // next 2 bits are the date tens part (like if 26 then 2, or if 5 then 0)
    const uint8_t dateTens = get_bits(rawDate, 2, 3);

    // last 4 bits are the date units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t dateUnits = get_bits(rawDate, 4, 7);

    const uint8_t date = (dateTens * 10) + dateUnits;
    return date;
}

uint8_t DS1302RTC::get_month() {
    const uint8_t rawMonth = read_register(REG_MONTH);

    // first 3 bits are blank/ignored

    // next 1 bit is the month tens part (like if 12 then 1, or if 5 then 0)
    const uint8_t monthTens = get_bits(rawMonth, 3, 3);

    // last 4 bits are the month units (like if 12 then the 2 part, or if 5 then 5)
    const uint8_t monthUnits = get_bits(rawMonth, 4, 7);

    const uint8_t date = (monthTens * 10) + monthUnits;
    return date;
}

uint8_t DS1302RTC::get_weekday() {
    const uint8_t rawMonth = read_register(REG_WEEKDAY);

    // first 5 bits are blank/ignored

    // last 3 bits are the weekday units
    const uint8_t weekday = get_bits(rawMonth, 5, 7);
    return weekday;
}

uint8_t DS1302RTC::get_year() {
    const uint8_t rawYear = read_register(REG_YEAR);

    // first 4 bits are the year tens part (like if 22 then 2, or if 5 then 0)
    const uint8_t yearTens = get_bits(rawYear, 0, 3);

    // last 4 bits are the year units (like if 12 then the 2 part, or if 5 then 5)
    const uint8_t yearUnits = get_bits(rawYear, 4, 7);

    const uint8_t year = (yearTens * 10) + yearUnits;
    return year;
}

void DS1302RTC::set_running(bool running) {
    const uint8_t rawSeconds = read_register(REG_SECONDS);
    // get original seconds, we will only change the first bit
    const uint8_t value = ((running ? 0 : 1) << 7) | rawSeconds;
    // 0 if running, 1 if halted, put that in the first bit
}

void DS1302RTC::set_writable(bool writable) {
    const uint8_t value = set_bits(0x00, 0, 0, !writable);
    // 0 if writable, 1 if write protected at 0th bit
    write_register(REG_WRITE_PROTECT, value);
}