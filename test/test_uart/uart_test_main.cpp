
#include "../main.h"
#include "cmsis_os.h"
#include "uart_tests.h"
#include <unity.h>



void test_task(void*) {
  UNITY_BEGIN();

  RUN_TEST(test_send_buffer);
  RUN_TEST(test_transmit_immediate);
  RUN_TEST(test_transmit_data);
  RUN_TEST(test_send_data);
  RUN_TEST(test_printf);


  UNITY_END();

  while (1) {
  }
}


void pre_test() {
  uart1.begin(115200);
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
