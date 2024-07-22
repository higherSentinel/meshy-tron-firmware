// registers
#define DS3231_REG_SECONDS      0x00    // Seconds register
#define DS3231_REG_MINUTES      0x01    // Minutes register
#define DS3231_REG_HOURS        0x02    // Hours register
#define DS3231_REG_DAY_WEEK     0x03    // Day of the week register
#define DS3231_REG_DAY_MONTH    0x04    // Day of the month register
#define DS3231_REG_MONTH        0x05    // Month register
#define DS3231_REG_YEAR         0x06    // Year register
#define DS3231_REG_ALARM1_SEC   0x07    // Alarm 1 seconds register
#define DS3231_REG_ALARM1_MIN   0x08    // Alarm 1 minutes register
#define DS3231_REG_ALARM1_HOUR  0x09    // Alarm 1 hour register
#define DS3231_REG_ALARM1_DD    0x0A    // Alarm 1 day/date register
#define DS3231_REG_ALARM2_MIN   0x0B    // Alarm 2 seconds register
#define DS3231_REG_ALARM2_HOUR  0x0C    // Alarm 2 hour register
#define DS3231_REG_ALARM2_DD    0x0D    // Alarm 2 day/date register
#define DS3231_REG_CONTROL      0x0E    // Control register
#define DS3231_REG_STATUS       0x0F    // Status register
#define DS3231_REG_AGING_OFFSET 0x10    // Aging offset register
#define DS3231_REG_TEMP_MSB     0x11    // Temperature MSB register
#define DS3231_REG_TEMP_LSB     0x12    // Temperature LSB register

//! DS3231 number of registers
#define DS3231_NUM_REGS         19      //!< 19 RTC register: 0x00..0x12

//! DS3231 register bit defines
#define DS3231_HOUR_12H_24H     6       //!< 12 or 24 hour mode
#define DS3231_HOUR_AM_PM       5       //!< AM/PM

#define DS3231_MONTH_CENTURY    7       //!< Century

#define DS3231_CTRL_EOSC        7       //!< Enable oscillator
#define DS3231_CTRL_BBSQW       6       //!< Battery-Backed Square-Wave Enable
#define DS3231_CTRL_CONV        5       //!< Start temperature conversion
#define DS3231_CTRL_RS2         4       //!< Square wave rate-select 2
#define DS3231_CTRL_RS1         3       //!< Square wave rate-select 1
#define DS3231_CTRL_INTCN       2       //!< Interrupt control
#define DS3231_CTRL_A2IE        1       //!< Alarm 2 interrupt enable
#define DS3231_CTRL_A1IE        0       //!< Alarm 1 interrupt enable

#define DS3231_STAT_OSF         7       //!< Oscillator Stop Flag
#define DS3231_STAT_EN32KHZ     3       //!< Enable 32kHz clock output
#define DS3231_STAT_BSY         2       //!< Temperature conversion busy flag
#define DS3231_STAT_A2F         1       //!< Alarm 2 status flag
#define DS3231_STAT_A1F         0       //!< Alarm 1 status flag

#define DS3231_A1M1             7       //!< Alarm 1 bit 7 seconds register
#define DS3231_A1M2             7       //!< Alarm 1 bit 7 minutes register
#define DS3231_A1M3             7       //!< Alarm 1 bit 7 hours register
#define DS3231_A1M4             7       //!< Alarm 1 bit 7 day/date register
#define DS3231_A2M2             7       //!< Alarm 2 bit 7 minutes register
#define DS3231_A2M3             7       //!< Alarm 2 bit 7 hours register
#define DS3231_A2M4             7       //!< Alarm 2 bit 7 day/date register
#define DS3231_DYDT             6       //!< Alarm 2 bit 6