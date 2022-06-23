#include "uart_task.h"
#include "globals.h"


void uart_task(void*) {
  while (1) {
    xTaskNotifyWait(0, UINT32_MAX, nullptr, portMAX_DELAY);

    while (uart2.available()) {
      cmd.input_char(uart2.get_one());
    }
  }
}
