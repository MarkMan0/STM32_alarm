#include "DS3231.h"

#include "rtos_i2c.h"

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





bool DS3231::get_time(time& t) {
  uint8_t buff[7];

  buff[0] = SECONDS;

  if (!i2c_dev_.read_register(i2c_address_, SECONDS, buff, 7)) {
    return false;
  }


  t.sec = (buff[0] & MASK_SECONDS) + 10 * ((buff[0] & MASK_10_SECONDS) >> 4);
  t.min = (buff[1] & MASK_MINUTES) + 10 * ((buff[1] & MASK_10_MINUTES) >> 4);
  if (buff[2] & MASK_12_24) {
    t.am_pm = (buff[2] & MASK_AM_PM_20_HOUR) ? time::PM : time::AM;
  } else {
    t.am_pm = time::AM_PM_UNUSED;
  }

  t.hour = (buff[2] & MASK_HOURS) + 10 * (!!(buff[2] & MASK_10_HOUR));
  if (t.am_pm == time::AM_PM_UNUSED) {
    t.hour += 20 * (!!(buff[2] & MASK_AM_PM_20_HOUR));
  }

  t.dow = buff[3] & MASK_DAY;
  t.date = (buff[4] & MASK_DATE) + 10 * ((buff[4] & MASK_10_DATE) >> 4);
  t.month = (buff[5] & MASK_MONTH) + 10 * (!!(buff[5] & MASK_10_MONTH));
  t.year = 2000 + (buff[6] & MASK_YEAR) + 10 * ((buff[6] & MASK_10_YEAR) >> 4);


  return true;
}


bool DS3231::set_time(const time& t) {
  uint8_t buff[7];

  auto is_within = [](const auto& w, int low, int high) -> bool { return w >= low && w <= high; };

  if (!is_within(t.sec, 0, 59)) return false;
  if (!is_within(t.min, 0, 59)) return false;
  if (!is_within(t.hour, 0, 23)) return false;
  if (!is_within(t.dow, 1, 7)) return false;
  if (!is_within(t.date, 1, 31)) return false;
  if (!is_within(t.month, 1, 12)) return false;
  if (!is_within(t.am_pm, time::AM_PM_UNUSED, time::PM)) return false;
  if (!is_within(t.year, 2000, 2099)) return false;


  buff[0] = ((t.sec / 10) << 4) | ((t.sec % 10) & MASK_SECONDS);
  buff[1] = ((t.min / 10) << 4) | ((t.min % 10) & MASK_MINUTES);

  buff[2] = 0;
  if (t.am_pm == time::AM_PM_UNUSED) {
    if (t.hour >= 20) {
      buff[2] |= MASK_AM_PM_20_HOUR;
    }
  } else {
    buff[2] |= MASK_12_24;
    if (t.am_pm == time::PM) {
      buff[2] |= MASK_AM_PM_20_HOUR;
    }
  }
  if (t.hour >= 10 && t.hour < 20) {
    buff[2] |= MASK_10_HOUR;
  }
  buff[2] |= (t.hour % 10) & MASK_HOURS;

  buff[3] = t.dow & MASK_DAY;


  buff[4] = ((t.date / 10) << 4) | ((t.date % 10) & MASK_DATE);


  buff[5] = (t.month % 10) & MASK_MONTH;
  if (t.month >= 10) {
    buff[5] |= MASK_10_MONTH;
  }

  buff[6] = ((t.year - 2000) / 10) << 4;
  buff[6] |= (t.year % 10) & MASK_YEAR;

  return i2c_dev_.write_register(i2c_address_, SECONDS, buff, 7);
}
