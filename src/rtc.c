#include "rtc.h"

#define RTC_I2C_ADDRESS 0x68
#define REG_SECONDS 0x00
#define REG_MINUTES 0x01
#define REG_HOURS 0x02
#define REG_WEEKDAY 0x03
#define REG_DATE 0x04
#define REG_MONTH 0x05
#define REG_YEAR 0x06
#define REG_CONTROL 0x0E
#define REG_STATUS 0x0F
#define I2C_TIMEOUT_US 5000

private uint32_t rtcBaudrate;
private DateTime currentDateTime;
private DateTime oldDateTime;
private DateTimeChangedCallback dateTimeChangedCallback;

private uint8_t readRegister(const uint8_t reg) {
    uint8_t readValue;
    i2c_write_timeout_us(RTC_I2C, RTC_I2C_ADDRESS, &reg, 1, true, I2C_TIMEOUT_US);
    i2c_read_timeout_us(RTC_I2C, RTC_I2C_ADDRESS, &readValue, 1, false, I2C_TIMEOUT_US);
    return readValue;
}

private void writeRegister(const uint8_t reg, const uint8_t value) {
    const uint8_t array[2] = {reg, value};
    i2c_write_timeout_us(RTC_I2C, RTC_I2C_ADDRESS, array, 2, false, I2C_TIMEOUT_US);
}

public inline bool rtc_dateTimeEquals(const DateTime *a, const DateTime *b) {
    if (!a && !b) return true;
    if (!a || !b) return false;
    return a->seconds == b->seconds &&
           a->minutes == b->minutes &&
           a->hours == b->hours &&
           a->weekDay == b->weekDay &&
           a->date == b->date &&
           a->month == b->month &&
           a->year == b->year;
}

public inline const char *rtc_weekdayToString(const WeekDay weekDay) {
    switch (weekDay) {
        case MONDAY:
            return "Mon";
        case TUESDAY:
            return "Tue";
        case WEDNESDAY:
            return "Wed";
        case THURSDAY:
            return "Thu";
        case FRIDAY:
            return "Fri";
        case SATURDAY:
            return "Sat";
        case SUNDAY:
            return "Sun";
        default:
            return "Mon";
    }
}

public inline const char *rtc_monthToString(const uint8_t month) {
    const uint8_t clampedMonth = (month < 1 || month > 12) ? 1 : month;
    switch (clampedMonth) {
        case 1:
            return "Jan";
        case 2:
            return "Feb";
        case 3:
            return "Mar";
        case 4:
            return "Apr";
        case 5:
            return "May";
        case 6:
            return "Jun";
        case 7:
            return "Jul";
        case 8:
            return "Aug";
        case 9:
            return "Sep";
        case 10:
            return "Oct";
        case 11:
            return "Nov";
        case 12:
            return "Dec";
        default:
            return "Jan";
    }
}

public void rtc_init() {
    gpio_set_function(RTC_SCLK_PIN, GPIO_FUNC_I2C);
    gpio_set_function(RTC_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(RTC_SCLK_PIN);
    gpio_pull_up(RTC_SDA_PIN);
    rtcBaudrate = i2c_init(RTC_I2C, RTC_BAUD_RATE);
}

public void rtc_setDateTimeChangedCallback(DateTimeChangedCallback callback) {
    dateTimeChangedCallback = callback;
}

public bool rtc_isRunning() {
    return !rtc_isErrored() && rtc_isBatteryEnabled();
}

public bool rtc_isErrored() {
    const uint8_t rawStatus = readRegister(REG_STATUS);
    // first bit is reserved for OSC stop flag, 0 if ok, 1 if stopped
    bool isHalted = get_bits(rawStatus, 0, 0);
    return isHalted;
}

public bool rtc_isBatteryEnabled() {
    const uint8_t rawControl = readRegister(REG_CONTROL);
    // first bit is reserved for OSC enable (during battery power), 0 if ok, 1 if stopped
    bool oscStoppedOnBattery = get_bits(rawControl, 0, 0);
    return !oscStoppedOnBattery;
}

public uint32_t rtc_getBaudRate() { return rtcBaudrate; }

public uint8_t rtc_getSeconds() {
    const uint8_t rawSeconds = readRegister(REG_SECONDS);

    // first bit is unused

    // next 3 bits are the seconds tens part (like if 26 then 2, or if 5 then 0)
    const uint8_t secondsTens = get_bits(rawSeconds, 1, 3);

    // last 4 bits are the seconds units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t secondsUnits = get_bits(rawSeconds, 4, 7);

    const uint8_t seconds = (secondsTens * 10) + secondsUnits;
    return seconds;
}

public uint8_t rtc_getMinutes() {
    const uint8_t rawMinutes = readRegister(REG_MINUTES);

    // first bit is unused

    // next 3 bits are the minutes tens part (like if 26 then 2, or if 5 then 0)
    const uint8_t minutesTens = get_bits(rawMinutes, 1, 3);

    // last 4 bits are the minutes units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t minutesUnits = get_bits(rawMinutes, 4, 7);

    const uint8_t minutes = (minutesTens * 10) + minutesUnits;
    return minutes;
}

public uint8_t rtc_getHours() {
    const uint8_t rawHours = readRegister(REG_HOURS);
    // bit 1 is 12/24 mode, if 1 then in 12hr mode, if 0 then in 24hr mode
    const bool hoursMode = get_bits(rawHours, 1, 1);

    uint8_t hoursTens;
    if (!hoursMode) { // 24hr mode
        // bit 2 and 3 are hours tens part (like if 26 then 2, or if 5 then 0)
        hoursTens = get_bits(rawHours, 2, 3);
    } else { // 12hr mode
        // bit 2 is am or pm, 1 is pm, 0 is am
        const uint8_t hoursPM = get_bits(rawHours, 2, 2);

        // bit 3 is the hours tens part (like if 26 then 2, or if 5 then 0)
        const uint8_t hoursTens12hr = get_bits(rawHours, 3, 3);

        // tens in 24hr mode is just tens in 12hr mode plus 1 if it is pm, ie if 10pm then 1+1 ie 2
        // (tens of 22 in 24hr mode)
        hoursTens = hoursTens12hr + hoursPM;
    }

    // last 4 bits are the hours units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t hoursUnits = get_bits(rawHours, 4, 7);

    const uint8_t hours = (hoursTens * 10) + hoursUnits;
    return hours;
}

public WeekDay rtc_getWeekday() {
    const uint8_t rawWeekday = readRegister(REG_WEEKDAY);
    // first 5 bits are unused

    // last 3 bits are the weekday units
    const uint8_t weekday = get_bits(rawWeekday, 5, 7);
    return (WeekDay) weekday;
}

public uint8_t rtc_getDate() {
    const uint8_t rawDate = readRegister(REG_DATE);
    // first 2 bits are blank/ignored

    // next 2 bits are the date tens part (like if 26 then 2, or if 5 then 0)
    const uint8_t dateTens = get_bits(rawDate, 2, 3);

    // last 4 bits are the date units (like if 26 then the 6 part, or if 5 then 5)
    const uint8_t dateUnits = get_bits(rawDate, 4, 7);

    const uint8_t date = (dateTens * 10) + dateUnits;
    return date;
}

public uint8_t rtc_getMonth() {
    const uint8_t rawMonth = readRegister(REG_MONTH);
    // first bit is for century which we won't use

    // bit 3 is the month tens part (like if 12 then 1, or if 5 then 0)
    const uint8_t monthTens = get_bits(rawMonth, 3, 3);

    // last 4 bits are the month units (like if 12 then the 2 part, or if 5 then 5)
    const uint8_t monthUnits = get_bits(rawMonth, 4, 7);

    const uint8_t month = (monthTens * 10) + monthUnits;
    return month;
}

public uint8_t rtc_getYear() {
    const uint8_t rawYear = readRegister(REG_YEAR);
    // first 4 bits are the year tens part (like if 22 then 2, or if 5 then 0)
    const uint8_t yearTens = get_bits(rawYear, 0, 3);

    // last 4 bits are the year units (like if 12 then the 2 part, or if 5 then 5)
    const uint8_t yearUnits = get_bits(rawYear, 4, 7);

    const uint8_t year = (yearTens * 10) + yearUnits;
    return year;
}

public void rtc_getDateTime(DateTime *const result) {
    if (!result) return;
    result->seconds = rtc_getSeconds();
    result->minutes = rtc_getMinutes();
    result->hours = rtc_getHours();
    result->weekDay = rtc_getWeekday();
    result->date = rtc_getDate();
    result->month = rtc_getMonth();
    result->year = rtc_getYear();
}

public void rtc_setIsRunning(const bool isRunning) {
    rtc_setIsErrored(!isRunning);
    rtc_setIsBatteryEnabled(isRunning);
}

public void rtc_setIsErrored(const bool isErrored) {
    const uint8_t rawStatus = readRegister(REG_STATUS);
    // first bit is reserved for OSC stop flag, 0 if ok, 1 if stopped
    const uint8_t value = set_bits(rawStatus, 0, 0, isErrored);
    writeRegister(REG_STATUS, value);
}

public void rtc_setIsBatteryEnabled(const bool isBatteryEnabled) {
    const uint8_t rawControl = readRegister(REG_CONTROL);
    // first bit is reserved for OSC enable (during battery power), 0 if ok, 1 if stopped
    const uint8_t value = set_bits(rawControl, 0, 0, isBatteryEnabled);
    writeRegister(REG_STATUS, value);
}

public void rtc_setBaudRate(const uint32_t baudrate) {
    rtcBaudrate = i2c_set_baudrate(RTC_I2C, baudrate);
}

public void rtc_setSeconds(const uint8_t seconds) {
    const uint8_t clampedSeconds = (seconds < 0 || seconds > 59) ? 0 : seconds;
    // if user gave seconds out of range, set it to 0

    const uint8_t secondsTens = clampedSeconds / 10;
    const uint8_t secondsUnits = clampedSeconds % 10;

    const uint8_t rawSeconds = readRegister(REG_SECONDS);
    // get original seconds, we will change all but the 0th bit which is unused

    uint8_t value = set_bits(rawSeconds, 1, 3, secondsTens);
    // bits 1-3 are the seconds tens part

    value = set_bits(value, 4, 7, secondsUnits);
    // bits 4-7 are the seconds units part

    writeRegister(REG_SECONDS, value);
}

public void rtc_setMinutes(const uint8_t minutes) {
    const uint8_t clampedMinutes = (minutes < 0 || minutes > 59) ? 0 : minutes;
    // if user gave minutes out of range, set it to 0

    const uint8_t minutesTens = clampedMinutes / 10;
    const uint8_t minutesUnits = clampedMinutes % 10;

    uint8_t value = set_bits(0, 1, 3, minutesTens);
    // bits 1-3 are the minutes tens part

    value = set_bits(value, 4, 7, minutesUnits);
    // bits 4-7 are the minutes units part

    writeRegister(REG_MINUTES, value);
}

public void rtc_setHours(const uint8_t hours) {
    const uint8_t clampedHours = (hours < 0 || hours > 23) ? 0 : hours;
    // if user gave hours out of range, set it to 0

    const uint8_t hoursTens = clampedHours / 10;
    const uint8_t hoursUnits = clampedHours % 10;

    uint8_t value = 0;
    // bit 1 is 12/24 mode, 1 if 12hr mode, 0 if 24hr mode, we always use 24hr mode

    value = set_bits(value, 2, 3, hoursTens);
    // bits 2-3 are the hours tens part

    value = set_bits(value, 4, 7, hoursUnits);
    // bits 4-7 are the hours units part

    writeRegister(REG_HOURS, value);
}

public void rtc_setWeekday(const WeekDay weekday) {
    const WeekDay clampedWeekday = (weekday < 1 || weekday > 7) ? MONDAY : weekday;
    // if user gave weekday out of range, set it to Monday (1)

    uint8_t value = set_bits(0, 5, 7, clampedWeekday);
    // bits 5-7 are the weekday units

    writeRegister(REG_WEEKDAY, value);
}

public void rtc_setDate(const uint8_t date) {
    const uint8_t clampedDate = (date < 1 || date > 31) ? 1 : date;
    // if user gave date out of range, set it to 1

    const uint8_t dateTens = clampedDate / 10;
    const uint8_t dateUnits = clampedDate % 10;

    uint8_t value = set_bits(0, 2, 3, dateTens);
    // bits 2-3 are the date tens part

    value = set_bits(value, 4, 7, dateUnits);
    // bits 4-7 are the date units part

    writeRegister(REG_DATE, value);
}

public void rtc_setMonth(const uint8_t month) {
    const uint8_t clampedMonth = (month < 1 || month > 12) ? 1 : month;
    // if user gave month out of range, set it to 1

    const uint8_t monthTens = clampedMonth / 10;
    const uint8_t monthUnits = clampedMonth % 10;

    uint8_t value = set_bits(0, 3, 3, monthTens);
    // bit 3 is the month tens part

    value = set_bits(value, 4, 7, monthUnits);
    // bits 4-7 are the month units part

    writeRegister(REG_MONTH, value);
}

public void rtc_setYear(const uint8_t year) {
    const uint8_t clampedYear = (year < 0 || year > 99) ? 0 : year;
    // if user gave year out of range, set it to 0

    const uint8_t yearTens = clampedYear / 10;
    const uint8_t yearUnits = clampedYear % 10;

    uint8_t value = set_bits(0, 0, 3, yearTens);
    // bits 0-3 are the year tens part

    value = set_bits(value, 4, 7, yearUnits);
    // bits 4-7 are the year units part

    writeRegister(REG_YEAR, value);
}

public void rtc_setDateTime(const DateTime *const dateTime) {
    if (!dateTime) return;
    rtc_setSeconds(dateTime->seconds);
    rtc_setMinutes(dateTime->minutes);
    rtc_setHours(dateTime->hours);
    rtc_setWeekday(dateTime->weekDay);
    rtc_setDate(dateTime->date);
    rtc_setMonth(dateTime->month);
    rtc_setYear(dateTime->year);
}

public void rtc_loop() {
    rtc_getDateTime(&currentDateTime);
    if (!rtc_dateTimeEquals(&oldDateTime, &currentDateTime)) {
        if (dateTimeChangedCallback)
            dateTimeChangedCallback(&oldDateTime, &currentDateTime);
        oldDateTime = currentDateTime;
    }
}