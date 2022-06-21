#include "command_parser.h"
#include "unity.h"

StringParser parser;

void setUp() {
  parser = StringParser();
}
void tearDown() {
}


template <size_t N>
static void feed_parser(StringParser& p, const char (&arr)[N]) {
  for (char c : arr) {
    if (parser.tick(c)) {
      break;
    }
  }
}


void test_tick() {
  const char command[] = "C4\n";
  for (char c : command) {
    bool ret = parser.tick(c);
    if (c == '\n') {
      TEST_ASSERT_TRUE(ret);
    } else {
      TEST_ASSERT_FALSE(ret);
    }
  }
}


void test_prefix() {
  feed_parser(parser, "K9\r");

  TEST_ASSERT_TRUE(parser.is_valid());
  TEST_ASSERT_EQUAL_CHAR('K', parser.get_prefix());
}

void test_code() {
  feed_parser(parser, "Q198\r");

  TEST_ASSERT_TRUE(parser.is_valid());
  TEST_ASSERT_EQUAL(198, parser.get_code());
}


void test_default_param() {
  feed_parser(parser, "B78 A99");

  int16_t dst{ 0 };
  auto ret = parser.get_parameter('B', dst, 10);

  TEST_ASSERT_FALSE(ret);
  TEST_ASSERT_EQUAL(10, dst);
}

void test_get_param() {
  feed_parser(parser, "B78 A99");

  int16_t dst{ 0 };
  auto ret = parser.get_parameter('A', dst, 10);

  TEST_ASSERT_TRUE(ret);
  TEST_ASSERT_EQUAL(99, dst);
}


void test_task(void*) {
  UNITY_BEGIN();


  RUN_TEST(test_tick);
  RUN_TEST(test_prefix);
  RUN_TEST(test_code);
  RUN_TEST(test_default_param);
  RUN_TEST(test_get_param);

  UNITY_END();
  while (1) {
  }
}
