#include "command_parser.h"
#include "unity.h"

StringParser parser;

void setUp() {
  parser = StringParser();
}
void tearDown() {
}



void test_set_string() {
  TEST_ASSERT_FALSE(parser.has_string());
  parser.set_string("Hello world");
  TEST_ASSERT_TRUE(parser.has_string());
}

void test_get_prefix() {
  parser.set_string("C4");
  TEST_ASSERT_EQUAL_CHAR('C', parser.get_prefix());
  parser.set_string("  QQ78");
  TEST_ASSERT_EQUAL_CHAR('Q', parser.get_prefix());
}

void test_get_number() {
  TEST_ASSERT_EQUAL(UINT16_MAX, parser.get_number());
  parser.set_string("  \nB69");
  TEST_ASSERT_EQUAL(69, parser.get_number());
  parser.set_string("  \nA874");
  TEST_ASSERT_EQUAL(874, parser.get_number());
}

void test_get_parameter() {
  int16_t d = 0;
  TEST_ASSERT_FALSE(parser.get_parameter('B', d, 10));
  TEST_ASSERT_EQUAL(10, d);

  parser.set_string("B0 X10 Y-20 Z0");
  TEST_ASSERT_TRUE(parser.get_parameter('X', d, 80));
  TEST_ASSERT_EQUAL(10, d);
  TEST_ASSERT_TRUE(parser.get_parameter('Y', d, 80));
  TEST_ASSERT_EQUAL(-20, d);
  TEST_ASSERT_TRUE(parser.get_parameter('Z', d, 80));
  TEST_ASSERT_EQUAL(0, d);
  TEST_ASSERT_FALSE(parser.get_parameter('E', d, 80));
  TEST_ASSERT_EQUAL(80, d);
}


void test_task(void*) {
  UNITY_BEGIN();


  RUN_TEST(test_set_string);
  RUN_TEST(test_get_prefix);
  RUN_TEST(test_get_number);
  RUN_TEST(test_get_parameter);

  UNITY_END();
  while (1) {
  }
}
