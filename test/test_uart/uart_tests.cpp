#include "uart_tests.h"

#include <unity.h>

void setUp() {
}
void tearDown() {
  uart1.flush();
  HAL_Delay(500);
  while (uart1.available()) {
    uart1.get_one();
  }
  uart1.reset_buffers();
}


const char msg[] = "Hello world!";
const int msg_len = sizeof(msg) - 1;


void test_send_buffer() {
  uart1.send("Hello world!");
  HAL_Delay(100);

  TEST_ASSERT_EQUAL(msg_len, uart1.available());

  char rec[30]{};
  for (int i = 0; i < 30 && uart1.available(); ++i) {
    rec[i] = uart1.get_one();
  }
  TEST_ASSERT_EQUAL_STRING(msg, rec);
}

void test_transmit_immediate() {
  uart1.transmit(msg);
  HAL_Delay(100);
  TEST_ASSERT_EQUAL(msg_len, uart1.available());
  char rec[30]{};
  for (int i = 0; i < 30 && uart1.available(); ++i) {
    rec[i] = uart1.get_one();
  }
  TEST_ASSERT_EQUAL_STRING(msg, rec);
}


struct S {
  int a;
  bool b;
  float c;
};

void test_transmit_data() {
  S s1{ 1, false, .1 };

  uart1.transmit(&s1, sizeof(s1));
  HAL_Delay(500);

  TEST_ASSERT_EQUAL(sizeof(s1), uart1.available());
  uint8_t rec[30];
  for (int i = 0; i < 30 && uart1.available(); ++i) {
    rec[i] = uart1.get_one();
  }

  S s2 = *reinterpret_cast<S*>(rec);

  TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, sizeof(s1));
}


void test_send_data() {
  S s1{ 568, true, -3.14 };

  uart1.send(&s1, sizeof(s1));
  uart1.flush();
  HAL_Delay(500);

  TEST_ASSERT_EQUAL(sizeof(s1), uart1.available());
  uint8_t rec[30];
  for (int i = 0; i < 30 && uart1.available(); ++i) {
    rec[i] = uart1.get_one();
  }

  S s2 = *reinterpret_cast<S*>(rec);

  TEST_ASSERT_EQUAL_MEMORY(&s1, &s2, sizeof(s1));
}

void test_printf() {
  const char fmt[] = "Hi %s %d %e";
  const char str[] = "Joe";
  const int i = -434;
  const float f = 3.1415;
  char expected[50];
  int len = snprintf(expected, 50, fmt, str, i, f);

  int len2 = uart1.printf(fmt, str, i, f);
  uart1.flush();
  HAL_Delay(1000);
  char result[50]{};
  for (int i = 0; i < 50 && uart1.available(); ++i) {
    result[i] = uart1.get_one();
  }

  TEST_ASSERT_NOT_EQUAL(0, len2);
  TEST_ASSERT_EQUAL(len, len2);
  TEST_ASSERT_EQUAL_STRING(expected, result);
}


UART_DMA uart1(UART_DMA::uart1_hw_init, UART_DMA::uart1_enable_isrs);
