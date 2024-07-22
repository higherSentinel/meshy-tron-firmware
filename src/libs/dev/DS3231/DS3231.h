#ifndef  BAYMAX_DS3231_H
#define BAYMAX_DS3231_H

// port from https://github.com/Erriez/ErriezDS3231/blob/master/src

// local includes
#include "Arduino.h"
#include "DS3231_DEV_PROP.h"
#include <Wire.h>
#include <time.h>

// local defines
#define DS3231_ADDR             0x68
#define SECONDS_FROM_1970_TO_2000 946684800

typedef enum {
    Alarm1 = 1,                 //!< Alarm ID 1
    Alarm2 = 2                  //!< Alarm ID 2
} AlarmId;

typedef enum
{
    Alarm1EverySecond = 0x0F,   //!< Alarm once per second
    Alarm1MatchSeconds = 0x0E,  //!< Alarm when seconds match
    Alarm1MatchMinutes = 0x0C,  //!< Alarm when minutes and seconds match
    Alarm1MatchHours = 0x08,    //!< Alarm when hours, minutes, and seconds match
    Alarm1MatchDay = 0x10,      //!< Alarm when date, hours, minutes, and seconds match
    Alarm1MatchDate = 0x00,     //!< Alarm when day, hours, minutes, and seconds match
} Alarm1Type;

typedef enum
{
    Alarm2EveryMinute = 0x0E,   //!< Alarm once per minute (00 seconds of every minute)
    Alarm2MatchMinutes = 0x0C,  //!< Alarm when minutes match
    Alarm2MatchHours = 0x08,    //!< Alarm when hours and minutes match
    Alarm2MatchDay = 0x10,      //!< Alarm when date, hours, and minutes match
    Alarm2MatchDate = 0x00,     //!< Alarm when day, hours, and minutes match
} Alarm2Type;

typedef enum {
    SquareWaveDisable = (1 << DS3231_CTRL_INTCN),                           //!< SQW disable
    SquareWave1Hz = ((0 << DS3231_CTRL_RS2) | (0 << DS3231_CTRL_RS1)),      //!< SQW 1Hz
    SquareWave1024Hz = ((0 << DS3231_CTRL_RS2) | (1 << DS3231_CTRL_RS1)),   //!< SQW 1024Hz
    SquareWave4096Hz = ((1 << DS3231_CTRL_RS2) | (0 << DS3231_CTRL_RS1)),   //!< SQW 4096Hz
    SquareWave8192Hz = ((1 << DS3231_CTRL_RS2) | (1 << DS3231_CTRL_RS1)),   //!< SQW 8192Hz
} SquareWave;

class DS3231
{
    public:
        static DS3231& getInstance();
        ~DS3231();
        bool init(TwoWire * i2c_instance);
        bool isActive();
        bool startClock(bool start = true);

        // Set-get date-time
        time_t getEpoch();
        bool setEpoch(time_t t);
        bool read(struct tm *dt);
        bool write(const struct tm *dt);
        bool setTime(uint8_t hour, uint8_t min, uint8_t sec);
        bool getTime(uint8_t *hour, uint8_t *min, uint8_t *sec);
        bool setDateTime(uint8_t hour, uint8_t min, uint8_t sec, uint8_t mday, uint8_t mon, uint16_t year, uint8_t wday);
        bool getDateTime(uint8_t *hour, uint8_t *min, uint8_t *sec, uint8_t *mday, uint8_t *mon, uint16_t *year, uint8_t *wday);

        // Alarm functions
        bool setAlarm1(Alarm1Type alarmType,
                            uint8_t dayDate, uint8_t hours, uint8_t minutes, uint8_t seconds);
        bool setAlarm2(Alarm2Type alarmType, uint8_t dayDate, uint8_t hours, uint8_t minutes);
        bool alarmInterruptEnable(AlarmId alarmId, bool enable);
        bool getAlarmFlag(AlarmId alarmId);
        bool clearAlarmFlag(AlarmId alarmId);

        // Output signal control
        bool setSquareWave(SquareWave squareWave);
        bool outputClockPinEnable(bool enable);

        // Aging offset compensation
        bool setAgingOffset(int8_t val);
        int8_t getAgingOffset();

        // Temperature functions
        bool startTemperatureConversion();
        bool getTemperature(int8_t *temperature, uint8_t *fraction);

        // BCD conversions
        uint8_t bcdToDec(uint8_t bcd);
        uint8_t decToBcd(uint8_t dec);

        // RW register
        uint8_t readRegister(uint8_t reg);
        bool writeRegister(uint8_t reg, uint8_t value);

        // RW buffer
        bool readBuffer(uint8_t reg, void *buffer, uint8_t len);
        bool writeBuffer(uint8_t reg, void *buffer, uint8_t len);


    private:
        DS3231();
        TwoWire * _i2c_instance = nullptr;
};

#endif