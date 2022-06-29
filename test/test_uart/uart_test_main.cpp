/**
 * @file uart_test_main.cpp
 * @brief UART1 test one-time setup and ISRs
 */
#include "../main.h"
#include "uart_tests.h"
#include <unity.h>



void test_task(void*) {
  UNITY_BEGIN();

  RUN_TEST(test_send_buffer);
  RUN_TEST(test_transmit_immediate);
  RUN_TEST(test_transmit_data);
  RUN_TEST(test_send_data);
  RUN_TEST(test_printf);
  RUN_TEST(test_printf_overflow);

  UNITY_END();

  while (1) {
  }
}


void pre_test() {
  uart1.hw_init(115200);
  TaskHandle_t handle;
  uart1.begin(&handle);
}



#ifdef __cplusplus
extern "C" {
#endif

void DMA1_Channel4_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart1.hdmatx_);
}

void DMA1_Channel5_IRQHandler(void) {
  HAL_DMA_IRQHandler(&uart1.hdmarx_);
}
void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&uart1.huart_);
}


#ifdef __cplusplus
}
#endif
