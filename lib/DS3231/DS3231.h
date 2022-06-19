#pragma once


#include <cstdint>
#include "rtos_i2c.h"

class DS3231 {
public:
  using I2C = RTOS_I2C;
  struct time {
    uint8_t sec{},    ///< seconds 0-59
        min{},        ///< minutes 0-59
        hour{},       ///< hours 1-12AM_PM / 0-23
        dow{},        ///< day of the week 1-7
        date{},       ///< day 1-31
        month{};      /// <month  1-12
    uint16_t year{};  /// < year 0-99
    enum { AM_PM_UNUSED, AM, PM } am_pm{ AM_PM_UNUSED };
  };

  DS3231(I2C& i2c_dev) : i2c_dev_(i2c_dev) {
  }

  [[nodiscard]] bool get_time(time&);
  [[nodiscard]] bool set_time(const time&);

  [[nodiscard]] bool config();

  void report_all_registers();

  static void report_time(const time& t);

private:
  static inline constexpr uint8_t i2c_address_{ 0b1101000 << 1 };
  I2C& i2c_dev_;
};