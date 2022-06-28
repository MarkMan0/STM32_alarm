#include "unity.h"
#include "../main.h"
#include "encoder.h"



void test_button_tracker() {
  using e = btn_event_t;
  BtnTracker<true> tracker;
  tracker.update_btn(0);
  TEST_ASSERT_EQUAL_MESSAGE(e::NONE, tracker.get_state(), "first press wrong");
  tracker.update_btn(1);
  TEST_ASSERT_EQUAL_MESSAGE(e::NONE, tracker.get_state(), "first update wrong");
  tracker.update_btn(0);
  HAL_Delay(200);
  TEST_ASSERT_EQUAL_MESSAGE(e::NONE, tracker.get_state(), "did not ignore noise");
  // correct debounce, ignored noise

  tracker.update_btn(1);
  TEST_ASSERT_EQUAL_MESSAGE(e::NONE, tracker.get_state(), "second press wrong");
  HAL_Delay(150);
  tracker.update_btn(1);
  TEST_ASSERT_EQUAL_MESSAGE(e::PRESSED, tracker.get_state(), "second press not detected");  // detected edge
  TEST_ASSERT_EQUAL_MESSAGE(e::NONE, tracker.get_state(), "HELD delay not respected");      // waiting for HELD_DELAY
  HAL_Delay(200);
  tracker.update_btn(1);
  TEST_ASSERT_EQUAL_MESSAGE(e::HELD, tracker.get_state(), "HELD not detected");      // button is held
  TEST_ASSERT_EQUAL_MESSAGE(e::HELD, tracker.get_state(), "2nd HELD not detected");  // button is held

  tracker.update_btn(0);
  TEST_ASSERT_EQUAL_MESSAGE(e::HELD, tracker.get_state(), "Release wrong");  // still held, wait for release
  HAL_Delay(150);
  tracker.update_btn(0);
  TEST_ASSERT_EQUAL_MESSAGE(e::RELEASED, tracker.get_state(), "Release not detected");  // button is released
  TEST_ASSERT_EQUAL_MESSAGE(e::NONE, tracker.get_state(), "State is not none at end");  // button is released
}



void test_task(void*) {
  UNITY_BEGIN();

  RUN_TEST(test_button_tracker);

  UNITY_END();
  while (1) {
  }
}
