#include "../main.h"
#include "unity.h"
#include "encoder.h"




/**
 * 0 0
 * 1 0
 * 1 1
 * 0 1
 * 0 0
 * 1 0
 * 1 1
 * 0 1
 * 0 0
 */




Encoder enc;

void setUp() {
  enc.reset();
  enc.init(0);
}


void test_inc_from_0() {
  enc.init(0);
  TEST_ASSERT_EQUAL(0, enc.get());
  enc.update(1, 0);
  enc.update(1, 1);
  TEST_ASSERT_EQUAL(1, enc.get());
  enc.update(0, 1);
  enc.update(0, 0);
  TEST_ASSERT_EQUAL(2, enc.get());
}

void test_inc_from_1() {
  enc.init(1);
  TEST_ASSERT_EQUAL(0, enc.get());
  enc.update(0, 1);
  enc.update(0, 0);
  TEST_ASSERT_EQUAL(1, enc.get());
  enc.update(1, 0);
  enc.update(1, 1);
  TEST_ASSERT_EQUAL(2, enc.get());
}

void test_dec_from_0() {
  enc.init(0);
  TEST_ASSERT_EQUAL(0, enc.get());
  enc.update(0, 1);
  enc.update(1, 1);
  TEST_ASSERT_EQUAL(-1, enc.get());
  enc.update(1, 0);
  enc.update(0, 0);
  TEST_ASSERT_EQUAL(-2, enc.get());
}

void test_dec_from_1() {
  enc.init(1);
  TEST_ASSERT_EQUAL(0, enc.get());
  enc.update(1, 0);
  enc.update(0, 0);
  TEST_ASSERT_EQUAL(-1, enc.get());
  enc.update(0, 1);
  enc.update(1, 1);
  TEST_ASSERT_EQUAL(-2, enc.get());
}



void test_task(void*) {
  UNITY_BEGIN();

  RUN_TEST(test_inc_from_0);
  RUN_TEST(test_inc_from_1);
  RUN_TEST(test_dec_from_0);
  RUN_TEST(test_dec_from_1);

  UNITY_END();

  while (1) {
  }
}
