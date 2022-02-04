#include "pico/stdlib.h"
#include <stdio.h>

#include "rtc.hpp"
#include "constants.h"
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

static uint8_t read_register(uint8_t reg) {
    // The DS1302 is least-significant-bit first which the pico doesn't support, so we have to reverse the bits
    // between spi calls to both read and write

    uint8_t cmdByte = 0x81; // 10000001 first bit is mandatory for communication, last bit signifies read
    // TODO: 04-Feb-2022 @basshelal: Proper register bits location accommodating first and last bits
    cmdByte |= (reg << 1);

    cmdByte = reverse_bits(cmdByte);

    uint8_t readValue;

    enable();
    spi_write_blocking(RTC_SPI, &cmdByte, 1);
    spi_read_blocking(RTC_SPI, 0, &readValue, 1);
    disable();

    uint8_t result = reverse_bits(readValue);
    log("rtc read register %u ->\t0x%x 0b%s\n",
        reg, result, byte_to_binary(result));

    return result;
}

static void write_register(uint8_t reg, uint8_t value) {
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

bool DS1302RTC::is_halted() {
    const uint8_t rawSeconds = read_register(REG_SECONDS);
    // first bit is reserved for Clock Halt, 0 if running, 1 if halted
    bool isHalted = (rawSeconds >> 7) & 0x01;
    // get the top 1 bit by shifting by 7 places and then ANDing with 0000_0001 (in hex 0x01)
    // which means keep the last 1 bit (the ones with 1) and ignore (ie clear) the ones with 0s
    // example:        11011000
    // >> 7 :          10110001
    // AND 00000001 :  00000001
    // isHalted :        1 ie true

    return isHalted;
}

bool DS1302RTC::is_writable() {
    const uint8_t rawWriteProtect = read_register(REG_WRITE_PROTECT);
    // first bit is reserved for Write Protect, 0 if writable, 1 if write protected
    bool isProtected = (rawWriteProtect >> 7) & 0x01;
    return !isProtected;
}

uint DS1302RTC::get_baud_rate() {
    return spi_get_baudrate(RTC_SPI);
}

void DS1302RTC::read_data() {
    printf("clock on? %s\n", !is_halted() ? "true" : "false");
    printf("is writable? %s\n", is_writable() ? "true" : "false");
    printf("seconds: %i\n", get_seconds());
    printf("minutes: %i\n", get_minutes());
    printf("hours: %i\n", get_hours());
    printf("date: %i\n", get_date());
    printf("month: %i\n", get_month());
    printf("weekday: %i\n", get_weekday());
    printf("year: %i\n", get_year());
}

uint8_t DS1302RTC::get_seconds() {
    const uint8_t rawSeconds = read_register(REG_SECONDS);

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

uint8_t DS1302RTC::get_minutes() {
    const uint8_t rawMinutes = read_register(REG_MINUTES);

    // first bit is blank/ignored

    // next 3 bits are the minutes tens part (like if 26 then 2, or if 5 then 0)
    const uint8_t minutesTens = (rawMinutes >> 4) & 0x07;
    // get the top 4 bits by shifting by 4 places and then ANDing with 0000_0111 (in hex 0x07)
    // which means keep the last 3 bits (the ones with 1) and ignore (ie clear) the ones with 0s
    // example:        01011000
    // >> 4 :          10000101
    // AND 00000111 :  00000101
    // result :        decimal 5

    // last 4 bits are the minutes units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t minutesUnits = rawMinutes & 0x0F;
    // get the bottom 4 bits by ANDing with 0000_1111 (in hex 0x0F)
    // which means keep the last 4 bits (the ones with 1) and ignore (ie clear) the ones with 0s
    // example:        01011001
    // AND 00001111 :  00001001
    // result :        decimal 9

    const uint8_t minutes = (minutesTens * 10) + minutesUnits;
    return minutes;
}

uint8_t DS1302RTC::get_hours() {
    const uint8_t rawHours = read_register(REG_HOURS);

    // first bit is 12/24 mode, if 1 then in 12hr mode, if 0 then in 24hr mode
    const bool hoursMode = (rawHours >> 7) & 0x01;

    uint8_t hoursTens;
    if (!hoursMode) { // 24hr mode
        // bit 3 and 4 are hours tens part (like if 26 then 2, or if 5 then 0)
        hoursTens = (rawHours >> 4) & 0x04;
        // get the top 4 bits by shifting by 4 places and then ANDing with 0000_0011 (in hex 0x04)
        // which means keep the last 2 bits (the ones with 1) and ignore (ie clear) the ones with 0s
        // example:        01011000
        // >> 4 :          10000101
        // AND 00000011 :  00000001
        // result :        decimal 1
    } else { // 12hr mode
        // bit 4 is the hours tens part (like if 26 then 2, or if 5 then 0)
        const uint8_t hoursTens12hr = (rawHours >> 4) & 0x01;

        // bit 5 is am or pm, 1 is pm, 0 is am
        const uint8_t hoursPM = (rawHours >> 5) & 0x01;

        // tens in 24hr mode is just tens in 12hr mode plus 1 if it is pm, ie if 10pm then 1+1 ie 2
        // (tens of 22 in 24hr mode)
        hoursTens = hoursTens12hr + hoursPM;
    }

    // last 4 bits are the hours units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t hoursUnits = rawHours & 0x0F;

    const uint8_t hours = (hoursTens * 10) + hoursUnits;
    return hours;
}

uint8_t DS1302RTC::get_date() {
    const uint8_t rawDate = read_register(REG_DATE);

    // first 2 bits are blank/ignored

    // next 2 bits are the date tens part (like if 26 then 2, or if 5 then 0)
    const uint8_t dateTens = (rawDate >> 4) & 0x04;

    // last 4 bits are the date units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t dateUnits = rawDate & 0x0F;

    const uint8_t date = (dateTens * 10) + dateUnits;
    return date;
}

uint8_t DS1302RTC::get_month() {
    const uint8_t rawMonth = read_register(REG_MONTH);

    // first 3 bits are blank/ignored

    // next 1 bit is the month tens part (like if 12 then 1, or if 5 then 0)
    const uint8_t monthTens = (rawMonth >> 4) & 0x01;

    // last 4 bits are the month units (like if 12 then the 2 part, or if 5 then 5)
    const uint8_t monthUnits = rawMonth & 0x0F;

    const uint8_t date = (monthTens * 10) + monthUnits;
    return date;
}

uint8_t DS1302RTC::get_weekday() {
    const uint8_t rawMonth = read_register(REG_WEEKDAY);

    // first 5 bits are blank/ignored

    // last 3 bits are the weekday units
    const uint8_t weekday = rawMonth & 0x07;
    return weekday;
}

uint8_t DS1302RTC::get_year() {
    const uint8_t rawYear = read_register(REG_YEAR);

    // first 4 bits are the year tens part (like if 22 then 2, or if 5 then 0)
    const uint8_t yearTens = (rawYear >> 4) & 0x0F;

    // last 4 bits are the year units (like if 12 then the 2 part, or if 5 then 5)
    const uint8_t yearUnits = rawYear & 0x0F;

    const uint8_t year = (yearTens * 10) + yearUnits;
    return year;
}