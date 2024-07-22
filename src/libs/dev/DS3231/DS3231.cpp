#include "DS3231.h"

/**
 * Singleton
 */
DS3231& DS3231::getInstance()
{
    static DS3231 instance;
    return instance;
}

/**
 * Constructor & Destructor
 */
DS3231::DS3231()
{

}
DS3231::~DS3231()
{

}


/**
 * Call to init the RTC
 * @param i2c_instance TWI instance\
 * @warning caller is responsible to init the i2c instance
 */
bool DS3231::init(TwoWire * i2c_instance)
{
    if(i2c_instance == nullptr)
        return false;

    // Set the i2c instance
    _i2c_instance = i2c_instance;

    // Check zero bits in status register
    if (readRegister(DS3231_REG_STATUS) & 0x70) {
        return false;
    }

    // DS3231 detected
    return true;
}

bool DS3231::startClock(bool enable)
{
    uint8_t reg;

    // Read status register
    reg = readRegister(DS3231_REG_CONTROL);

    // Set or clear EOSC bit in control register
    if (enable) {
        reg &= ~(1 << DS3231_CTRL_EOSC);
    } else {
        reg |= (1 << DS3231_CTRL_EOSC);
    }

    // Write control register
    if (!writeRegister(DS3231_REG_CONTROL, reg)) {
        return false;
    }

    // Clear OSF bit in status register
    reg = readRegister(DS3231_REG_STATUS);
    reg &= ~(1 << DS3231_STAT_OSF);
    return writeRegister(DS3231_REG_STATUS, reg);
}

bool DS3231::isActive()
{
    // Check OSF bit in status register
    if (readRegister(DS3231_REG_STATUS) & (1 << DS3231_STAT_OSF)) {
        // RTC clock stopped
        return false;
    } else {
        // RTC clock is running
        return true;
    }
}

time_t DS3231::getEpoch()
{
    struct tm dt;
    time_t t;

    // Read time structure
    if (!read(&dt)) {
        // RTC read failed
        return 0;
    }

    // Convert date/time struct tm to time_t
    t = mktime(&dt);

    // An offset is needed for AVR target
#ifdef ARDUINO_ARCH_AVR
    t += UNIX_OFFSET;
#endif

    // Return Unix epoch UTC
    return t;
}

bool DS3231::setEpoch(time_t t)
{
    struct tm *dt;

    // Subtract UNIX offset for AVR targets
#ifdef ARDUINO_ARCH_AVR
    t -= UNIX_OFFSET;
#endif

    // Convert time_t to date/time struct tm
    dt = gmtime(&t);

    // Write date/time to RTC
    return write(dt);
}

bool DS3231::read(struct tm *dt)
{
    uint8_t buffer[7];

    // Read clock date and time registers
    if (!readBuffer(0x00, buffer, sizeof(buffer))) {
        memset(dt, 0, sizeof(struct tm));
        return false;
    }

    // Clear dt
    memset(dt, 0, sizeof(struct tm));

    // Convert BCD buffer to Decimal
    dt->tm_sec = bcdToDec(buffer[0] & 0x7F);
    dt->tm_min = bcdToDec(buffer[1] & 0x7F);
    dt->tm_hour = bcdToDec(buffer[2] & 0x3F);
    dt->tm_wday = bcdToDec(buffer[3] & 0x07);
    dt->tm_mday = bcdToDec(buffer[4] & 0x3F);
    dt->tm_mon = bcdToDec(buffer[5] & 0x1F);
    dt->tm_year = bcdToDec(buffer[6]) + 100; // 2000-1900

    // Month: 0..11
    if (dt->tm_mon) {
        dt->tm_mon--;
    }

    // Day of the week: 0=Sunday
    if (dt->tm_wday) {
        dt->tm_wday--;
    }

    // Check buffer for valid data
    if ((dt->tm_sec > 59) || (dt->tm_min > 59) || (dt->tm_hour > 23) ||
        (dt->tm_mday < 1) || (dt->tm_mday > 31) || (dt->tm_mon > 11) || (dt->tm_year > 199) ||
        (dt->tm_wday > 6))
    {
        return false;
    }

    return true;
}

bool DS3231::write(const struct tm *dt)
{
    uint8_t buffer[7];

    // Enable oscillator
    if (!startClock(true)) {
        return false;
    }

    // Encode date time from decimal to BCD
    buffer[0] = decToBcd(dt->tm_sec) & 0x7F;
    buffer[1] = decToBcd(dt->tm_min) & 0x7F;
    buffer[2] = decToBcd(dt->tm_hour) & 0x3F;
    buffer[3] = decToBcd(dt->tm_wday + 1) & 0x07;
    buffer[4] = decToBcd(dt->tm_mday) & 0x3F;
    buffer[5] = decToBcd(dt->tm_mon + 1) & 0x1F;
    buffer[6] = decToBcd(dt->tm_year % 100);

    // Write BCD encoded buffer to RTC registers
    return writeBuffer(0x00, buffer, sizeof(buffer));
}

bool DS3231::setTime(uint8_t hour, uint8_t min, uint8_t sec)
{
    struct tm dt;

    read(&dt);
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;
    return write(&dt);
}

bool DS3231::getTime(uint8_t *hour, uint8_t *min, uint8_t *sec)
{
    uint8_t buffer[3];

    // Read RTC time registers
    if (!readBuffer(0x00, buffer, sizeof(buffer))) {
        return false;
    }

    // Convert BCD buffer to Decimal
    *sec = bcdToDec(buffer[0] & 0x7F);
    *min = bcdToDec(buffer[1] & 0x7F);
    *hour = bcdToDec(buffer[2] & 0x3F);

    // Check buffer for valid data
    if ((*sec > 59) || (*min > 59) || (*hour > 23)) {
        // Invalid time
        *sec = 0x00;
        *min = 0x00;
        *hour = 0x00;

        return false;
    }

    return true;
}

bool DS3231::setDateTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t mday, uint8_t mon, uint16_t year, uint8_t wday)
{
    struct tm dt;

    // Prepare struct tm
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;
    dt.tm_mday = mday;
    dt.tm_mon = mon - 1;
    dt.tm_year = year - 1900;
    dt.tm_wday = wday;

    // Write date/time to RTC
    return write(&dt);
}

bool DS3231::getDateTime(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *mday, uint8_t *mon, uint16_t *year, uint8_t *wday)
{
    struct tm dt;

    // Read date/time from RTC
    if (!read(&dt)) {
        return false;
    }

    // Set return values
    *hour = dt.tm_hour;
    *min = dt.tm_min;
    *sec = dt.tm_sec;
    *mday = dt.tm_mday;
    *mon = dt.tm_mon + 1;
    *year = dt.tm_year + 1900;
    *wday = dt.tm_wday;

    return true;
}

bool DS3231::setAlarm1(Alarm1Type alarmType, uint8_t dayDate, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    uint8_t buffer[4];

    // Store alarm 1 registers in buffer
    buffer[0] = decToBcd(seconds);
    buffer[1] = decToBcd(minutes);
    buffer[2] = decToBcd(hours);
    buffer[3] = decToBcd(dayDate);

    // Set alarm 1 bits
    if (alarmType & 0x01) { buffer[0] |= (1 << DS3231_A1M1); }
    if (alarmType & 0x02) { buffer[1] |= (1 << DS3231_A1M2); }
    if (alarmType & 0x04) { buffer[2] |= (1 << DS3231_A1M3); }
    if (alarmType & 0x08) { buffer[3] |= (1 << DS3231_A1M4); }
    if (alarmType & 0x10) { buffer[3] |= (1 << DS3231_DYDT); }

    // Write alarm 1 registers
    if (!writeBuffer(DS3231_REG_ALARM1_SEC, buffer, sizeof(buffer))) {
        return false;
    }

    // Clear alarm 1 flag
    return clearAlarmFlag(Alarm1);
}

bool DS3231::setAlarm2(Alarm2Type alarmType, uint8_t dayDate, uint8_t hours, uint8_t minutes)
{
    uint8_t buffer[3];

    // Store alarm 2 registers in buffer
    buffer[0] = decToBcd(minutes);
    buffer[1] = decToBcd(hours);
    buffer[2] = decToBcd(dayDate);

    // Set alarm 2 bits
    if (alarmType & 0x02) { buffer[0] |= (1 << DS3231_A1M2); }
    if (alarmType & 0x04) { buffer[1] |= (1 << DS3231_A1M3); }
    if (alarmType & 0x08) { buffer[2] |= (1 << DS3231_A1M4); }
    if (alarmType & 0x10) { buffer[2] |= (1 << DS3231_DYDT); }

    // Write alarm 2 registers
    if (!writeBuffer(DS3231_REG_ALARM2_MIN, buffer, sizeof(buffer))) {
        return false;
    }

    // Clear alarm 2 flag
    return clearAlarmFlag(Alarm2);
}

bool DS3231::alarmInterruptEnable(AlarmId alarmId, bool enable)
{
    uint8_t controlReg;

    // Clear alarm flag
    clearAlarmFlag(alarmId);

    // Read control register
    controlReg = readRegister(DS3231_REG_CONTROL);

    // Disable square wave out and enable INT
    controlReg |= (1 << DS3231_CTRL_INTCN);

    // Set or clear alarm interrupt enable bit
    if (enable) {
        controlReg |= (1 << (alarmId - 1));
    } else {
        controlReg &= ~(1 << (alarmId - 1));
    }

    // Write control register
    return writeRegister(DS3231_REG_CONTROL, controlReg);
}

bool DS3231::getAlarmFlag(AlarmId alarmId)
{
    // Mask alarm flags
    if (readRegister(DS3231_REG_STATUS) & (1 << (alarmId - 1))) {
        return true;
    } else {
        return false;
    }
}

bool DS3231::clearAlarmFlag(AlarmId alarmId)
{
    uint8_t statusReg;

    // Read status register
    statusReg = readRegister(DS3231_REG_STATUS);

    // Clear alarm interrupt flag
    statusReg &= ~(1 << (alarmId - 1));

    // Write status register
    return writeRegister(DS3231_REG_STATUS, statusReg);
}

bool DS3231::setSquareWave(SquareWave squareWave)
{
    uint8_t controlReg;

    // Read control register
    controlReg = readRegister(DS3231_REG_CONTROL);
    controlReg &= ~((1 << DS3231_CTRL_BBSQW) |
                    (1 << DS3231_CTRL_INTCN) |
                    (1 << DS3231_CTRL_RS2) |
                    (1 << DS3231_CTRL_RS1));
    controlReg |= squareWave;

    // Write control register
    return writeRegister(DS3231_REG_CONTROL, controlReg);
}

bool DS3231::outputClockPinEnable(bool enable)
{
    uint8_t statusReg;

    // Read status register
    statusReg = readRegister(DS3231_REG_STATUS);

    // Set or clear EN32kHz flag in status register
    if (enable) {
        statusReg |= (1 << DS3231_STAT_EN32KHZ);
    } else {
        statusReg &= ~(1 << DS3231_STAT_EN32KHZ);
    }

    // Write status register
    return writeRegister(DS3231_REG_STATUS, statusReg);
}

bool DS3231::setAgingOffset(int8_t val)
{
    uint8_t regVal;

    // Convert 8-bit signed value to register value
    if (val < 0) {
        // Calculate two's complement for negative value
        regVal = ~(-val) + 1;
    } else {
        // Positive register value
        regVal = (uint8_t)val;
    }

    // Write aging offset register
    if (!writeRegister(DS3231_REG_AGING_OFFSET, regVal)) {
        return false;
    }

    // A temperature conversion is required to apply the aging offset change
    return startTemperatureConversion();
}

int8_t DS3231::getAgingOffset()
{
    uint8_t regVal;

    // Read aging register
    regVal = readRegister(DS3231_REG_AGING_OFFSET);

    // Convert to 8-bit signed value
    if (regVal & 0x80) {
        // Calculate two's complement for negative aging register value
        return regVal | ~((1 << 8) - 1);
    } else {
        // Positive aging register value
        return regVal;
    }
}

bool DS3231::startTemperatureConversion()
{
    uint8_t controlReg;

    // Check if temperature busy flag is set
    if (readRegister(DS3231_REG_STATUS) & (1 << DS3231_STAT_BSY)) {
        return false;
    }

    // Start temperature conversion
    controlReg = readRegister(DS3231_REG_CONTROL) | (1 << DS3231_CTRL_CONV);

    // Write control register
    return writeRegister(DS3231_REG_CONTROL, controlReg);
}

bool DS3231::getTemperature(int8_t *temperature, uint8_t *fraction)
{
    uint8_t temp[2];

    // Read temperature MSB and LSB registers
    if (!readBuffer(DS3231_REG_TEMP_MSB, &temp, sizeof(temp))) {
        return false;
    }

    // Set temperature argument
    *temperature = temp[0];

    // Calculate two's complement when negative
    if (*temperature & 0x80) {
        *temperature |= ~((1 << 8) - 1);
    }

    // Shift fraction bits 6 and 7 with 0.25 degree Celsius resolution
    *fraction = (temp[1] >> 6) * 25;

    return true;
}

uint8_t DS3231::bcdToDec(uint8_t bcd)
{
    return (uint8_t)(10 * ((bcd & 0xF0) >> 4) + (bcd & 0x0F));
}

uint8_t DS3231::decToBcd(uint8_t dec)
{
    return (uint8_t)(((dec / 10) << 4) | (dec % 10));
}

uint8_t DS3231::readRegister(uint8_t reg)
{
    uint8_t value = 0;

    // Read buffer with one 8-bit unsigned value
    readBuffer(reg, &value, 1);

    return value;
}

bool DS3231::writeRegister(uint8_t reg, uint8_t value)
{
    // Write buffer with one 8-bit unsigned value
    return writeBuffer(reg, &value, 1);
}

bool DS3231::writeBuffer(uint8_t reg, void *buffer, uint8_t writeLen)
{
    if(_i2c_instance == nullptr)
        return false;

    // Start I2C transfer by writing the I2C address, register number and optional buffer
    _i2c_instance->beginTransmission(DS3231_ADDR);
    _i2c_instance->write(reg);
    for (uint8_t i = 0; i < writeLen; i++) {
        _i2c_instance->write(((uint8_t *)buffer)[i]);
    }
    if (_i2c_instance->endTransmission(true) != 0) {
        return false;
    }

    return true;
}

bool DS3231::readBuffer(uint8_t reg, void *buffer, uint8_t readLen)
{
    if(_i2c_instance == nullptr)
        return false;

    // Start I2C transfer by writing the I2C address and register number
    _i2c_instance->beginTransmission(DS3231_ADDR);
    _i2c_instance->write(reg);
    // Generate a repeated start, followed by a read buffer
    if (_i2c_instance->endTransmission(false) != 0) {
        return false;
    }
    _i2c_instance->requestFrom((uint8_t)DS3231_ADDR, readLen);
    for (uint8_t i = 0; i < readLen; i++) {
        ((uint8_t *)buffer)[i] = (uint8_t)_i2c_instance->read();
    }

    return true;
}