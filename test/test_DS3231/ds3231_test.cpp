#include "../main.h"
#include "DS3231.h"
#include "unity.h"



RTOS_I2C i2c1;

void setUp() {
}
void tearDown() {
}


void test_ds3231() {
  DS3231::time t;
  DS3231 rtc(i2c1);


  // get the start time
  auto start_ms = HAL_GetTick();
  HAL_Delay(100);
  uint8_t res = rtc.get_time(t);
  auto t_start = t;
  TEST_ASSERT_EQUAL_MESSAGE(0, res, "Get time failed");

  TEST_ASSERT_NOT_EQUAL_MESSAGE(59, t.min,
                                "Test can't restore time correctly at 59minutes, wait a minute and run again");


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

  auto elapsed_s = (HAL_GetTick() - start_ms) / 1000;
  // if no seconds elapsed, just set the start time
  if (elapsed_s != 0) {
    t_start.sec += elapsed_s;
    if (t_start.sec >= 60) {
      // we don't handle minutes "overflow", but it's fine, we made sure we are not at 59 minutes
      t_start.min += 1;
      t_start.sec = 0;
    }
  }
  HAL_Delay(100);
  res = rtc.set_time(t_start);
  TEST_ASSERT_EQUAL_MESSAGE(0, res, "Time restore failed");
  HAL_Delay(100);
}


int test_task(void*) {
  i2c1.init_i2c1();

  UNITY_BEGIN();

  RUN_TEST(test_ds3231);

  UNITY_END();
  while (1) {
  }
}
