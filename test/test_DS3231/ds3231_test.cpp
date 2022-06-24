#include "../main.h"
#include "DS3231.h"
#include "unity.h"
#include <optional>


RTOS_I2C i2c1;
DS3231 rtc(i2c1);

bool can_test = false;

std::optional<DS3231::time> orig_time;
std::optional<DS3231::alarm_t> alarm0, alarm1;
uint32_t start_ms{};

void setUp() {
  DS3231::time t;
  DS3231::alarm_t a;

  orig_time = std::nullopt;
  alarm0 = std::nullopt;
  alarm1 = std::nullopt;

  start_ms = HAL_GetTick();
  if (0 == rtc.get_time(t) && t.min != 59) {
    orig_time = t;
  }
  if (0 == rtc.get_alarm(0, a)) {
    alarm0 = a;
  }
  if (0 == rtc.get_alarm(1, a)) {
    alarm1 = a;
  }

  can_test = orig_time && alarm0 && alarm1;
}

void tearDown() {
  if (orig_time) {
    DS3231::time t = *orig_time;
    t.sec += (HAL_GetTick() - start_ms) / 1000;
    while (t.sec > 59) {
      t.sec -= 60;
      t.min += 1;
    }
    uint8_t b = rtc.set_time(t);
    UNUSED(b);
  }

  if (alarm0) {
    uint8_t b = rtc.set_alarm(0, *alarm0);
    UNUSED(b);
  }
  if (alarm1) {
    uint8_t b = rtc.set_alarm(0, *alarm1);
    UNUSED(b);
  }
}


void test_ds3231() {
  DS3231::time t;

  uint8_t res = rtc.get_time(t);


  t.sec = 16;
  t.min = 12;
  t.hour = 2;
  t.dow = 1;
  t.date = 3;
  t.month = 4;
  t.year = 2010;

  HAL_Delay(100);
  res = rtc.set_time(t);
  TEST_ASSERT_EQUAL_MESSAGE(0, res, "Set time failed");
  t = DS3231::time{ 0 };

  HAL_Delay(100);
  res = rtc.get_time(t);
  TEST_ASSERT_EQUAL(0, res);
  TEST_ASSERT_LESS_OR_EQUAL_MESSAGE(2, 16 - t.sec, "Seconds wrong");
  TEST_ASSERT_EQUAL_MESSAGE(12, t.min, "Minutes wrong");
  TEST_ASSERT_EQUAL_MESSAGE(2, t.hour, "Hours wrong");
  TEST_ASSERT_EQUAL_MESSAGE(1, t.dow, "Day wrong");
  TEST_ASSERT_EQUAL_MESSAGE(3, t.date, "Date wrong");
  TEST_ASSERT_EQUAL_MESSAGE(4, t.month, "Month wrong");
  TEST_ASSERT_EQUAL_MESSAGE(2010, t.year, "Year wrong");
}


void test_alarm() {
  DS3231::alarm_t alarm;
  DS3231::time t;

  pin_mode(pins::alarm_it, pin_mode_t::INPUT_PU);

  uint8_t res = rtc.get_time(t);
  const auto t_now = t;
  TEST_ASSERT_EQUAL_MESSAGE(0, res, "Get time failed");

  res = rtc.get_alarm(0, alarm);
  TEST_ASSERT_EQUAL_MESSAGE(0, res, "Get Alarm failed");

  alarm.alarm_type = DS3231::alarm_t::alarm_type_t::DAILY;

  alarm.hour = t_now.hour;
  alarm.am_pm = t_now.am_pm;
  alarm.min = t_now.min + 1;
  alarm.en = 1;

  res = rtc.set_alarm(0, alarm);
  TEST_ASSERT_EQUAL_MESSAGE(0, res, "Set Alarm failed");

  char buff[30];
  while (t.min < alarm.min) {
    HAL_Delay(1000);
    snprintf(buff, 29, "Wait: %ds", 60 - t.sec);
    TEST_MESSAGE(buff);
    res = rtc.get_time(t);
    TEST_ASSERT_EQUAL_MESSAGE(0, res, "Get Time failed");
  }

  bool b = false;

  TEST_ASSERT_FALSE(read_pin(pins::alarm_it));
  res = rtc.get_and_clear_alarm_flag(0, b);
  TEST_ASSERT_TRUE(read_pin(pins::alarm_it));
  TEST_ASSERT_EQUAL_MESSAGE(0, res, "clear flag failed");

  TEST_ASSERT_TRUE(b);
}


int test_task(void*) {
  i2c1.init_i2c1();

  UNITY_BEGIN();

  RUN_TEST(test_ds3231);
  RUN_TEST(test_alarm);

  UNITY_END();
  while (1) {
  }
}
