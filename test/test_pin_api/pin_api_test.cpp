/**
 * @file main.cpp
 * @brief tests for pin_api
 */

#include "../main.h"
#include "pin_api.h"
#include <unity.h>

void setUp(void) {
}
void tearDown(void) {
}


void test_port_lookup(void) {
  using pin_api::pin_name_to_port;
  TEST_ASSERT_EQUAL(GPIOA, pin_name_to_port(PA7));
  TEST_ASSERT_EQUAL(GPIOA, pin_name_to_port(PA4));
  TEST_ASSERT_EQUAL(GPIOC, pin_name_to_port(PC0));
  TEST_ASSERT_EQUAL(GPIOD, pin_name_to_port(PD15));

#ifndef GPIOE
  TEST_ASSERT_EQUAL(pin_name_to_port(PE3), nullptr);
#endif
}

void test_num_lookup(void) {
  using pin_api::pin_name_to_num;

  TEST_ASSERT_EQUAL(GPIO_PIN_1, pin_name_to_num(PA1));
  TEST_ASSERT_EQUAL(GPIO_PIN_4, pin_name_to_num(PB4));
  TEST_ASSERT_EQUAL(GPIO_PIN_7, pin_name_to_num(PC7));
  TEST_ASSERT_EQUAL(GPIO_PIN_15, pin_name_to_num(PE15));
}

void test_pin_is_available() {
  using pin_api::pin_is_available;

  TEST_ASSERT(pin_is_available(PA1));

  // in release should be true, but not in debug
#ifdef NDEBUG
  TEST_ASSERT(pin_is_available(PG0));
#else
  TEST_ASSERT(not pin_is_available(PG0));
#endif
}


void test_pin_io() {
  constexpr auto led_pin = PB3;
  pin_mode(led_pin, pin_mode_t::OUT_PP);

  TEST_ASSERT_EQUAL(read_pin(led_pin), 0);

  write_pin(led_pin, 1);
  TEST_ASSERT_EQUAL(read_pin(led_pin), 1);

  write_pin(led_pin, 0);
  TEST_ASSERT_EQUAL(read_pin(led_pin), 0);

  deinit_pin(led_pin);
}


void test_pin_toggle() {
  constexpr auto led_pin = PB3;
  pin_mode(led_pin, pin_mode_t::OUT_PP);

  TEST_ASSERT_EQUAL(read_pin(led_pin), 0);
  toggle_pin(led_pin);
  TEST_ASSERT_EQUAL(read_pin(led_pin), 1);
  toggle_pin(led_pin);
  TEST_ASSERT_EQUAL(read_pin(led_pin), 0);

  deinit_pin(led_pin);
}

void set_and_test(pin_name p, uint8_t exp) {
  write_pin(p, exp);
  TEST_ASSERT_EQUAL(read_pin(p), exp);
};


void test_multiple_pins() {
  auto p1 = PB3, p2 = PA4;

  pin_mode(p1, pin_mode_t::OUT_PP);
  pin_mode(p2, pin_mode_t::OUT_PP);

  set_and_test(p1, 1);
  set_and_test(p2, 0);
  set_and_test(p1, 0);
  set_and_test(p2, 1);

  deinit_pin(p1);
  deinit_pin(p2);
}



int main() {
  HAL_Init();  // initialize the HAL library
  HAL_Delay(1000);

  UNITY_BEGIN();
  RUN_TEST(test_port_lookup);
  RUN_TEST(test_num_lookup);
  RUN_TEST(test_pin_is_available);
  RUN_TEST(test_pin_io);
  RUN_TEST(test_pin_toggle);
  RUN_TEST(test_multiple_pins);

  UNITY_END();  // stop unit testing

  while (1) {
  }
}
