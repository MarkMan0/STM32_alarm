#include "DS3231.h"

#include "rtos_i2c.h"
#include <tuple>

namespace DS3231Reg {
  /**
   * @brief DS3231 register addresses
   *
   */
  enum reg {
    SECONDS,
    MINUTES,
    HOURS,
    DAY,
    DATE,
    MONTH,
    YEAR,
    ALARM_1_SECONDS,
    ALARM_1_MINUTES,
    ALARM_1_HOURS,
    ALARM_1_DAY_DATE,
    ALARM_2_MINUTES,
    ALARM_2_HOURS,
    ALARM_2_DAY_DATE,
    CONTROL,
    CONTROL_STATUS,
    AGING_OFFSET,
    TEMP_MSB,
    TEMP_LSB,
    REGISTER_END,
  };

  /**
   * @brief Masks to hel manipulate registers
   *
   */
  enum mask {
    MASK_SECONDS = 0b1111,
    MASK_10_SECONDS = 0b1110000,
    MASK_MINUTES = 0b1111,
    MASK_10_MINUTES = 0b1110000,
    MASK_HOURS = 0b1111,
    MASK_10_HOUR = 0b10000,
    MASK_AM_PM_20_HOUR = 0b100000,
    MASK_12_24 = 0b1000000,
    MASK_DAY = 0b111,
    MASK_DATE = 0b1111,
    MASK_10_DATE = 0b110000,
    MASK_MONTH = 0b1111,
    MASK_10_MONTH = 0b10000,
    MASK_CENTURY = 0b10000000,
    MASK_YEAR = 0b1111,
    MASK_10_YEAR = 0b11110000,
    MASK_N_EOSC = 1 << 7,
    MASK_BBSQW = 1 << 6,
    MASK_CONV = 1 << 5,
    MASK_RS2 = 1 << 4,
    MASK_RS1 = 1 << 3,
    MASK_INTCN = 1 << 2,
    MASK_A2IE = 1 << 1,
    MASK_A1IE = 1 << 0,
    MASK_OSF = 1 << 7,
    MASK_EN32KHZ = 1 << 3,
    MASK_BSY = 1 << 2,
    MASK_A2F = 1 << 1,
    MASK_A1F = 1 << 0,
  };
}  // namespace DS3231Reg



using namespace DS3231Reg;

/// @name register to value conversions
/// @{
static uint8_t sec_from_reg(uint8_t reg) {
  return (reg & MASK_SECONDS) + 10 * ((reg & MASK_10_SECONDS) >> 4);
}
static uint8_t min_from_reg(uint8_t reg) {
  return (reg & MASK_MINUTES) + 10 * ((reg & MASK_10_MINUTES) >> 4);
}
static std::tuple<uint8_t, DS3231::am_pm_t> hour_from_reg(uint8_t reg) {
  uint8_t hour{};
  DS3231::am_pm_t am_pm{};

  hour = (reg & MASK_HOURS) + 10 * (!!(reg & MASK_10_HOUR));

  if (reg & MASK_12_24) {
    am_pm = (reg & MASK_AM_PM_20_HOUR) ? DS3231::am_pm_t::PM : DS3231::am_pm_t::AM;
  } else {
    am_pm = DS3231::am_pm_t::AM_PM_UNUSED;
  }

  if (am_pm == DS3231::am_pm_t::AM_PM_UNUSED) {
    hour += 20 * (!!(reg & MASK_AM_PM_20_HOUR));
  }

  return { hour, am_pm };
}
static uint8_t dow_from_reg(uint8_t reg) {
  return reg & MASK_DAY;
}
static uint8_t date_from_reg(uint8_t reg) {
  return (reg & MASK_DATE) + 10 * ((reg & MASK_10_DATE) >> 4);
}
static uint8_t month_from_reg(uint8_t reg) {
  return (reg & MASK_MONTH) + 10 * ((reg & MASK_10_MONTH) >> 4);
}
static uint16_t year_from_reg(uint8_t reg) {
  return 2000 + ((reg & MASK_YEAR) + 10 * ((reg & MASK_10_YEAR) >> 4));
}
/// @}


/// @name value to register conversion
/// @{
static uint8_t sec_to_reg(uint8_t val) {
  return ((val / 10) << 4) | ((val % 10) & MASK_SECONDS);
}
static uint8_t min_to_reg(uint8_t val) {
  return ((val / 10) << 4) | ((val % 10) & MASK_MINUTES);
}
static uint8_t hour_to_reg(uint8_t hour, DS3231::am_pm_t am_pm) {
  uint8_t ret = 0;
  if (am_pm == DS3231::am_pm_t::AM_PM_UNUSED) {
    if (hour >= 20) {
      ret |= MASK_AM_PM_20_HOUR;
    }
  } else {
    ret |= MASK_12_24;
    if (am_pm == DS3231::am_pm_t::PM) {
      ret |= MASK_AM_PM_20_HOUR;
    }
  }
  if (hour >= 10 && hour < 20) {
    ret |= MASK_10_HOUR;
  }

  ret |= (hour % 10) & MASK_HOURS;

  return ret;
}
static uint8_t dow_to_reg(uint8_t val) {
  return val & MASK_DAY;
}
static uint8_t date_to_reg(uint8_t val) {
  return ((val / 10) << 4) | ((val % 10) & MASK_DATE);
}
static uint8_t month_to_reg(uint8_t val) {
  uint8_t ret{ 0 };

  ret = (val % 10) & MASK_MONTH;

  if (val >= 10) {
    ret |= MASK_10_MONTH;
  }
  return ret;
}
static uint8_t year_to_reg(uint16_t val) {
  val -= 2000;
  return ((val / 10) << 4) | ((val % 10) & MASK_YEAR);
}
/// @}

uint8_t DS3231::get_time(time& t) {
  uint8_t buff[7];

  buff[0] = SECONDS;

  if (!i2c_dev_.read_register(i2c_address_, SECONDS, buff, 7)) {
    return 1;
  }


  t.sec = sec_from_reg(buff[0]);
  t.min = min_from_reg(buff[1]);
  auto [hour, am_pm] = hour_from_reg(buff[2]);
  t.hour = hour;
  t.am_pm = am_pm;

  t.dow = dow_from_reg(buff[3]);
  t.date = date_from_reg(buff[4]);
  t.month = month_from_reg(buff[5]);
  t.year = year_from_reg(buff[6]);

  t.dow_str = dow_to_str(t.dow);
  return 0;
}


uint8_t DS3231::set_time(const time& t) {
  uint8_t buff[7];

  if (not t.verify()) {
    return 1;
  }

  buff[0] = sec_to_reg(t.sec);
  buff[1] = min_to_reg(t.min);
  buff[2] = hour_to_reg(t.hour, t.am_pm);
  buff[3] = dow_to_reg(t.dow);
  buff[4] = date_to_reg(t.date);
  buff[5] = month_to_reg(t.month);
  buff[6] = year_to_reg(t.year);

  if (not i2c_dev_.write_register(i2c_address_, SECONDS, buff, 7)) {
    return 2;
  }

  return 0;
}


const char* DS3231::dow_to_str(uint8_t dow) const {
  static constexpr const char* lookup[] = {
    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
  };
  static constexpr const char* empty = "";

  if (not utils::within(dow, 1, 7)) {
    return empty;
  }

  return lookup[dow - 1];
}


uint8_t DS3231::set_alarm(int n, const alarm_t& t) {
  using utils::within;

  if (not within(t.min, 0, 59)) return 1;
  if (not within(t.hour, 0, 23)) return 1;
  if (t.alarm_type == alarm_t::alarm_type_t::ON_DOW && not within(t.dow, 1, 7)) return 1;


  const auto [dy_dt, flag_4] = [&]() {
    switch (t.alarm_type) {
      default:
      case alarm_t::alarm_type_t::DAILY:
        return std::make_tuple(0, 1);

      case alarm_t::alarm_type_t::ON_DOW:
        return std::make_tuple(1, 0);
    }
  }();


  const auto start_reg = n == 0 ? reg::ALARM_1_MINUTES : reg::ALARM_2_MINUTES;
  const auto flag_ie = n == 0 ? 1 : 2;


  if (n == 0) {
    // set seconds to 0 for alarm 1
    uint8_t zero = 0;
    if (not i2c_dev_.write_register(i2c_address_, reg::ALARM_1_SECONDS, &zero, 1)) {
      return 2;
    }
  }

  uint8_t buff[3]{ 0 };


  buff[0] = min_to_reg(t.min);
  buff[1] = hour_to_reg(t.hour, t.am_pm);

  buff[2] = 0;
  if (t.alarm_type == alarm_t::alarm_type_t::ON_DOW) {
    buff[2] |= t.dow & MASK_DAY;
  }
  buff[2] |= flag_4 << 7;
  buff[2] |= dy_dt << 6;



  if (not i2c_dev_.write_register(i2c_address_, start_reg, buff, 3)) {
    return 3;
  }

  uint8_t cntrl = 0;
  if (not i2c_dev_.read_register(i2c_address_, reg::CONTROL, &cntrl, 1)) {
    return 4;
  }

  if (t.en) {
    cntrl |= flag_ie;
    cntrl |= MASK_INTCN;
  } else {
    cntrl &= ~flag_ie;
  }

  if (not i2c_dev_.write_register(i2c_address_, reg::CONTROL, &cntrl, 1)) {
    return 5;
  }

  return 0;
}


uint8_t DS3231::get_alarm(int n, alarm_t& t) {
  const auto start_reg = n == 0 ? reg::ALARM_1_MINUTES : reg::ALARM_2_MINUTES;
  uint8_t buff[3]{ 0 };

  if (not i2c_dev_.read_register(i2c_address_, start_reg, buff, 3)) {
    return 1;
  }


  t.min = min_from_reg(buff[0]);
  auto [hour, am_pm] = hour_from_reg(buff[1]);
  t.hour = hour;
  t.am_pm = am_pm;
  t.dow = dow_from_reg(buff[2]);

  const bool flag_4 = buff[2] & (1 << 7), dy_dt = buff[2] & (1 << 6);

  if (flag_4 == 1) {
    t.alarm_type = alarm_t::DAILY;
  } else if (dy_dt == 1 && flag_4 == 0) {
    t.alarm_type = alarm_t::ON_DOW;
  }

  uint8_t cfg{ 0 };
  if (not i2c_dev_.read_register(i2c_address_, reg::CONTROL, &cfg, 1)) {
    return 2;
  }

  if (n == 0) {
    t.en = cfg & 1;
  } else {
    t.en = cfg & 2;
  }

  return 0;
}

uint8_t DS3231::get_and_clear_alarm_flag(int n, bool& b) {
  assert_param(n == 0 || n == 1);

  uint8_t stat{};
  if (not i2c_dev_.read_register(i2c_address_, reg::CONTROL_STATUS, &stat, 1)) {
    return 1;
  }

  if (n == 0) {
    b = stat & MASK_A1F;
    stat &= ~MASK_A1F;
  } else if (n == 1) {
    b = stat & MASK_A2F;
    stat &= ~MASK_A2F;
  } else {
    b = false;
  }

  if (not i2c_dev_.write_register(i2c_address_, reg::CONTROL_STATUS, &stat, 1)) {
    return 2;
  }

  return 0;
}
