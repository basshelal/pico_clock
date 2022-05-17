// All code taken from https://www.waveshare.com/wiki/Pico-UPS-B
// From the "resources" section of the wiki
// Only minor non-functional changes made for better refactoring and
// added battery_getPercentage() function

#include "battery.h"
#include "../utils.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

/** default I2C address **/
#define INA219_ADDRESS (0x43)

/** config register address **/
#define INA219_REG_CONFIG (0x00)

/** shunt voltage register **/
#define INA219_REG_SHUNTVOLTAGE (0x01)

/** bus voltage register **/
#define INA219_REG_BUSVOLTAGE (0x02)

/** power register **/
#define INA219_REG_POWER (0x03)

/** current register **/
#define INA219_REG_CURRENT (0x04)

/** calibration register **/
#define INA219_REG_CALIBRATION (0x05)

/** reset bit **/
#define INA219_CONFIG_RESET (0x8000) // Reset Bit

/** mask for bus voltage range **/
#define INA219_CONFIG_BVOLTAGERANGE_MASK (0x2000) // Bus Voltage Range Mask

/** bus voltage range values **/
enum {
    INA219_CONFIG_BVOLTAGERANGE_16V = (0x0000), // 0-16V Range
    INA219_CONFIG_BVOLTAGERANGE_32V = (0x2000), // 0-32V Range
};

/** mask for gain bits **/
#define INA219_CONFIG_GAIN_MASK (0x1800) // Gain Mask

/** values for gain bits **/
enum {
    INA219_CONFIG_GAIN_1_40MV = (0x0000),  // Gain 1, 40mV Range
    INA219_CONFIG_GAIN_2_80MV = (0x0800),  // Gain 2, 80mV Range
    INA219_CONFIG_GAIN_4_160MV = (0x1000), // Gain 4, 160mV Range
    INA219_CONFIG_GAIN_8_320MV = (0x1800), // Gain 8, 320mV Range
};

/** mask for bus ADC resolution bits **/
#define INA219_CONFIG_BADCRES_MASK (0x0780)

/** values for bus ADC resolution **/
enum {
    INA219_CONFIG_BADCRES_9BIT = (0x0000),  // 9-bit bus res = 0..511
    INA219_CONFIG_BADCRES_10BIT = (0x0080), // 10-bit bus res = 0..1023
    INA219_CONFIG_BADCRES_11BIT = (0x0100), // 11-bit bus res = 0..2047
    INA219_CONFIG_BADCRES_12BIT = (0x0180), // 12-bit bus res = 0..4097
};

/** mask for shunt ADC resolution bits **/
#define INA219_CONFIG_SADCRES_MASK                                             \
  (0x0078) // Shunt ADC Resolution and Averaging Mask

/** values for shunt ADC resolution **/
enum {
    INA219_CONFIG_SADCRES_9BIT_1S_84US = (0x0000),   // 1 x 9-bit shunt sample
    INA219_CONFIG_SADCRES_10BIT_1S_148US = (0x0008), // 1 x 10-bit shunt sample
    INA219_CONFIG_SADCRES_11BIT_1S_276US = (0x0010), // 1 x 11-bit shunt sample
    INA219_CONFIG_SADCRES_12BIT_1S_532US = (0x0018), // 1 x 12-bit shunt sample
    INA219_CONFIG_SADCRES_12BIT_2S_1060US = (0x0048), // 2 x 12-bit shunt samples averaged together
    INA219_CONFIG_SADCRES_12BIT_4S_2130US = (0x0050), // 4 x 12-bit shunt samples averaged together
    INA219_CONFIG_SADCRES_12BIT_8S_4260US = (0x0058), // 8 x 12-bit shunt samples averaged together
    INA219_CONFIG_SADCRES_12BIT_16S_8510US = (0x0060), // 16 x 12-bit shunt samples averaged together
    INA219_CONFIG_SADCRES_12BIT_32S_17MS = (0x0068), // 32 x 12-bit shunt samples averaged together
    INA219_CONFIG_SADCRES_12BIT_64S_34MS = (0x0070), // 64 x 12-bit shunt samples averaged together
    INA219_CONFIG_SADCRES_12BIT_128S_69MS = (0x0078), // 128 x 12-bit shunt samples averaged together
};

/** mask for operating mode bits **/
#define INA219_CONFIG_MODE_MASK (0x0007) // Operating Mode Mask

/** values for operating mode **/
enum {
    INA219_CONFIG_MODE_POWERDOWN,
    INA219_CONFIG_MODE_SVOLT_TRIGGERED,
    INA219_CONFIG_MODE_BVOLT_TRIGGERED,
    INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED,
    INA219_CONFIG_MODE_ADCOFF,
    INA219_CONFIG_MODE_SVOLT_CONTINUOUS,
    INA219_CONFIG_MODE_BVOLT_CONTINUOUS,
    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS
};

private float current_percentage;
private bool current_is_charging;
private uint8_t ina219_i2caddr;
private uint32_t ina219_calValue;

// The following multipliers are used to convert raw current and power
// values to mA and mW, taking into account the current config settings
private uint32_t ina219_currentDivider_mA;
private float ina219_powerMultiplier_mW;

/*!
 *  @brief  Sends a single command byte over I2C
 *  @param  reg
 *          register address
 *  @param  value
 *          value to write
 */
private void wireWriteRegister(const uint8_t reg, const uint16_t value) {

    uint8_t tmpi[3];
    tmpi[0] = reg;
    tmpi[1] = (value >> 8) & 0xFF;
    tmpi[2] = value & 0xFF;

    i2c_write_blocking(BATTERY_I2C, INA219_ADDRESS, tmpi, 3, false); // true to keep master control of bus
}

/*!
 *  @brief  Reads a 16 bit values over I2C
 *  @param  reg
 *          register address
 *  @param  *value
 *          read value
 */
private void wireReadRegister(const uint8_t reg, uint16_t *value) {

    uint8_t tmpi[2];

    i2c_write_blocking(BATTERY_I2C, INA219_ADDRESS, &reg, 1, true); // true to keep master control of bus
    i2c_read_blocking(BATTERY_I2C, INA219_ADDRESS, tmpi, 2, false);
    *value = (((uint16_t) tmpi[0] << 8) | (uint16_t) tmpi[1]);
}

/*!
 *  @brief  Configures to INA219 to be able to measure up to 32V and 2A
 *          of current.  Each unit of current corresponds to 100uA, and
 *          each unit of power corresponds to 2mW. Counter overflow
 *          occurs at 3.2A.
 *  @note   These calculations assume a 0.1 ohm resistor is present
 */
private void setCalibration32V2A() {
    // By default we use a pretty huge range for the input voltage,
    // which probably isn't the most appropriate choice for system
    // that don't use a lot of power.  But all of the calculations
    // are shown below if you want to change the settings.  You will
    // also need to change any relevant register settings, such as
    // setting the VBUS_MAX to 16V instead of 32V, etc.

    // VBUS_MAX = 32V             (Assumes 32V, can also be set to 16V)
    // VSHUNT_MAX = 0.32          (Assumes Gain 8, 320mV, can also be 0.16, 0.08, 0.04)
    // RSHUNT = 0.1               (Resistor value in ohms)

    // 1. Determine max possible current
    // MaxPossible_I = VSHUNT_MAX / RSHUNT
    // MaxPossible_I = 3.2A

    // 2. Determine max expected current
    // MaxExpected_I = 2.0A

    // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
    // MinimumLSB = MaxExpected_I/32767
    // MinimumLSB = 0.000061              (61uA per bit)
    // MaximumLSB = MaxExpected_I/4096
    // MaximumLSB = 0,000488              (488uA per bit)

    // 4. Choose an LSB between the min and max values
    //    (Preferrably a roundish number close to MinLSB)
    // CurrentLSB = 0.0001 (100uA per bit)

    // 5. Compute the calibration register
    // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
    // Cal = 4096 (0x1000)

    ina219_calValue = 4096;

    // 6. Calculate the power LSB
    // PowerLSB = 20 * CurrentLSB
    // PowerLSB = 0.002 (2mW per bit)

    // 7. Compute the maximum current and shunt voltage values before overflow
    //
    // Max_Current = Current_LSB * 32767
    // Max_Current = 3.2767A before overflow
    //
    // If Max_Current > Max_Possible_I then
    //    Max_Current_Before_Overflow = MaxPossible_I
    // Else
    //    Max_Current_Before_Overflow = Max_Current
    // End If
    //
    // Max_ShuntVoltage = Max_Current_Before_Overflow * RSHUNT
    // Max_ShuntVoltage = 0.32V
    //
    // If Max_ShuntVoltage >= VSHUNT_MAX
    //    Max_ShuntVoltage_Before_Overflow = VSHUNT_MAX
    // Else
    //    Max_ShuntVoltage_Before_Overflow = Max_ShuntVoltage
    // End If

    // 8. Compute the Maximum Power
    // MaximumPower = Max_Current_Before_Overflow * VBUS_MAX
    // MaximumPower = 3.2 * 32V
    // MaximumPower = 102.4W

    // Set multipliers to convert raw current/power values
    ina219_currentDivider_mA = 1.0; // Current LSB = 100uA per bit (1000/100 = 10)
    ina219_powerMultiplier_mW = 20; // Power LSB = 1mW per bit (2/1)

    // Set Calibration register to 'Cal' calculated above
    wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Set Config register to take into account the settings above
    uint16_t config = INA219_CONFIG_BVOLTAGERANGE_32V |
                      INA219_CONFIG_GAIN_8_320MV | INA219_CONFIG_BADCRES_12BIT |
                      INA219_CONFIG_SADCRES_12BIT_32S_17MS |
                      INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
    wireWriteRegister(INA219_REG_CONFIG, config);
}

/*!
 *  @brief  Set power save mode according to parameters
 *  @param  on
 *          boolean value
 */
private void powerSave(const bool on) {
    uint16_t current;
    wireReadRegister(INA219_REG_CONFIG, &current);
    uint8_t next;
    if (on) {
        next = current | INA219_CONFIG_MODE_POWERDOWN;
    } else {
        next = current & ~INA219_CONFIG_MODE_POWERDOWN;
    }
    wireWriteRegister(INA219_REG_CONFIG, next);
}

private bool isCharging() {
    return battery_getShuntVoltageMilliVolts() >= 0.0F;
}

private bool isDischarging() {
    return battery_getShuntVoltageMilliVolts() > 0.0F;
}

private bool isFullyCharged() {
    return battery_getShuntVoltageMilliVolts() == 0.0F;
}

/*!
 *  @brief  Setups the HW (defaults to 32V and 2A for calibration values)
 *  @param theWire the TwoWire object to use
 */
public void battery_init() {
    ina219_i2caddr = INA219_ADDRESS;
    ina219_currentDivider_mA = 0;
    ina219_powerMultiplier_mW = 0.0f;
    i2c_init(BATTERY_I2C, BATTERY_BAUD_RATE);
    gpio_set_function(BATTERY_SCLK_PIN, GPIO_FUNC_I2C);
    gpio_set_function(BATTERY_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(BATTERY_SCLK_PIN);
    gpio_pull_up(BATTERY_SDA_PIN);
    setCalibration32V2A();
    powerSave(true);
}

/*!
 *  @brief  Gets the shunt voltage in mV (so +-327mV)
 *  @return the shunt voltage converted to millivolts
 */
public float battery_getShuntVoltageMilliVolts() {
    uint16_t value;
    wireReadRegister(INA219_REG_SHUNTVOLTAGE, &value);
    return (int16_t) value * 0.01;
}

/*!
 *  @brief  Gets the shunt voltage in volts
 *  @return the bus voltage converted to volts
 */
public float battery_getBusVoltageVolts() {

    uint16_t value;
    wireReadRegister(INA219_REG_BUSVOLTAGE, &value);
    // Shift to the right 3 to drop CNVR and OVF and multiply by LSB
    return (int16_t) ((value >> 3) * 4) * 0.001;
}

/*!
 *  @brief  Gets the current value in mA, taking into account the
 *          config settings and current LSB
 *  @return the current reading convereted to milliamps
 */
public float battery_getCurrentMilliAmps() {
    uint16_t value;

    // Sometimes a sharp load will reset the INA219, which will
    // reset the cal register, meaning CURRENT and POWER will
    // not be available ... avoid this by always setting a cal
    // value even if it's an unfortunate extra step
    wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Now we can safely read the CURRENT register!
    wireReadRegister(INA219_REG_CURRENT, &value);
    float valueDec = (int16_t) value;
    valueDec /= ina219_currentDivider_mA;
    return valueDec;
}

/*!
 *  @brief  Gets the power value in mW, taking into account the
 *          config settings and current LSB
 *  @return power reading converted to milliwatts
 */
public float battery_getPowerMilliWatts() {

    uint16_t value;

    // Sometimes a sharp load will reset the INA219, which will
    // reset the cal register, meaning CURRENT and POWER will
    // not be available ... avoid this by always setting a cal
    // value even if it's an unfortunate extra step
    wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);

    // Now we can safely read the POWER register!
    wireReadRegister(INA219_REG_POWER, &value);

    float valueDec = (int16_t) value;
    valueDec *= ina219_powerMultiplier_mW;
    return valueDec;
}

public float battery_getPercentage() {
    if (isFullyCharged()) {
        current_is_charging = true;
        current_percentage = 100.0F;
        return current_percentage;
    }
    const float voltage = battery_getBusVoltageVolts();
    const float percentage = ((voltage - 3.0F) / 1.2F) * 100.0F;

    float result = percentage > 100 ? 100 : // clamp to between 100 and 0
                   percentage < 0 ? 0 : percentage;

    const bool currentlyCharging = isCharging();
    if (current_is_charging != currentlyCharging) { // state has changed since last check
        current_is_charging = currentlyCharging;
    } else {
        if (current_is_charging && result < current_percentage) result = current_percentage;
        if (!current_is_charging && result > current_percentage) result = current_percentage;
    }
    current_percentage = result;
    return result;
}
