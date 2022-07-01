/**
 * @file DS3231.h
 * @brief DS3231 driver
 *
 */
#pragma once


#include <cstdint>
#include "rtos_i2c.h"

/**
 * @brief DS3231 driver class
 *
 */
class DS3231 {
public:
  using I2C = RTOS_I2C;  ///< Type of I2C driver used


  enum am_pm_t { AM_PM_UNUSED, AM, PM };  ///< Type of clock config

  /// @brief Representation of time point in the RTC
  struct time {
    uint8_t sec{},                  ///< seconds 0-59
        min{},                      ///< minutes 0-59
        hour{},                     ///< hours 1-12AM_PM / 0-23
        dow{},                      ///< day of the week 1-7
        date{},                     ///< day 1-31
        month{};                    ///< month  1-12
    uint16_t year{};                ///< year 0-99
    const char* dow_str{ "" };      ///< day as string
    am_pm_t am_pm{ AM_PM_UNUSED };  ///< AM/PM representation used

    /// @return true if all members are within limits
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
  /// @brief Stores current time in @p t , returns 0 on success
  [[nodiscard]] uint8_t get_time(time& t);

  /// @brief Sets current time from @p t , returns 0 on success
  [[nodiscard]] uint8_t set_time(const time& t);

  /// @brief Alarm representation in DS3231. Alarm on seconds is not used
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

  /// Set alarm @p n to @p a. Return 0 on success
  [[nodiscard]] uint8_t set_alarm(int n, const alarm_t& a);
  /// Read alarm @p n to @p a. Return 0 on success
  [[nodiscard]] uint8_t get_alarm(int n, alarm_t& a);

  /// Check and clear alarm flag in DS3231 for alarm @p n. Result stored in @p b
  [[nodiscard]] uint8_t get_and_clear_alarm_flag(int n, bool& b);

  /// Read the temperature from the DS3231 into @p f
  [[nodiscard]] uint8_t read_temperature(float& f);

private:
  static inline constexpr uint8_t i2c_address_{ 0b1101000 << 1 };  ///< The I2C address of the RTC, already shifted
  const char* dow_to_str(uint8_t dow) const;                       ///< day of the week as string
  I2C& i2c_dev_;                                                   ///< I2C device
};
