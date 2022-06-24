#pragma once


#include <cstdint>
#include "rtos_i2c.h"

class DS3231 {
public:
  using I2C = RTOS_I2C;


  enum am_pm_t { AM_PM_UNUSED, AM, PM };
  struct time {
    uint8_t sec{},              ///< seconds 0-59
        min{},                  ///< minutes 0-59
        hour{},                 ///< hours 1-12AM_PM / 0-23
        dow{},                  ///< day of the week 1-7
        date{},                 ///< day 1-31
        month{};                ///< month  1-12
    uint16_t year{};            ///< year 0-99
    const char* dow_str{ "" };  ///< day as string
    am_pm_t am_pm{ AM_PM_UNUSED };

    constexpr bool verify() const {
      using utils::within;
      if (not within(sec, 0, 59)) return false;
      if (not within(min, 0, 59)) return false;
      if (not within(hour, 0, 23)) return false;
      if (not within(dow, 1, 7)) return false;
      if (not within(date, 1, 31)) return false;
      if (not within(month, 1, 12)) return false;
      if (not within(am_pm, AM_PM_UNUSED, PM)) return false;
      if (not within(year, 2000, 2099)) return false;

      return true;
    }
  };

  DS3231(I2C& i2c_dev) : i2c_dev_(i2c_dev) {
  }

  [[nodiscard]] uint8_t get_time(time&);
  [[nodiscard]] uint8_t set_time(const time&);

  struct alarm_t {
    uint8_t hour{}, min{}, dow{};
    am_pm_t am_pm{ AM_PM_UNUSED };
    enum alarm_type_t {
      DAILY,
      ON_DOW,
    };
    bool en = false;
    alarm_type_t alarm_type{ DAILY };
  };

  [[nodiscard]] uint8_t set_alarm(int, const alarm_t&);
  [[nodiscard]] uint8_t get_alarm(int, alarm_t&);


private:
  /*[[nodiscard]] bool set_alarm_1(const alarm_t&);
  [[nodiscard]] bool set_alarm_2(const alarm_t&);
  [[nodiscard]] bool get_alarm_1(alarm_t&);
  [[nodiscard]] bool get_alarm_2(alarm_t&);*/

  static inline constexpr uint8_t i2c_address_{ 0b1101000 << 1 };
  const char* dow_to_str(uint8_t dow) const;
  I2C& i2c_dev_;
};
