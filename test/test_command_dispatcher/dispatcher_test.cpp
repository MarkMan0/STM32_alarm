#include "uart.h"
#include "task.h"
#include "command_parser.h"
#include "unity.h"

UART_DMA uart1(UART_DMA::uart1_hw_init, UART_DMA::uart1_enable_isrs);
static CommandDispatcher cmd(&uart1);

static int glob_val = 0;

void CommandDispatcher::T100() {
  ++glob_val;
}


TaskHandle_t uart_handle;
static void uart_task(void*) {
  while (1) {
    xTaskNotifyWait(0, UINT32_MAX, nullptr, portMAX_DELAY);

    while (uart1.available()) {
      cmd.input_char(uart1.get_one());
    }
  }
}


void pre_test() {
  uart1.hw_init(115200);
  TaskHandle_t handle;
  uart1.begin(&handle);
  xTaskCreate(uart_task, "uart1 task", 128, nullptr, 10, &uart_handle);
  uart1.register_task_to_notify_on_rx(uart_handle);
}



void test_command_called() {
  glob_val = 0;
  uart1.printf("T100\n");
  vTaskDelay(pdMS_TO_TICKS(200));
  TEST_ASSERT_EQUAL(1, glob_val);
}


void test_task(void*) {
  UNITY_BEGIN();

  RUN_TEST(test_command_called);

  UNITY_END();

  while (1) {
  }
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
