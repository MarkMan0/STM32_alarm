#include "uart_task.h"
#include "globals.h"
#include "command_parser.h"

CommandDispatcher disp;


static std::array<char, 40> buff{};

void uart_task(void*) {
  disp.inject_uart_dependency(&uart2);
  while (1) {
    xTaskNotifyWait(0, UINT32_MAX, nullptr, portMAX_DELAY);

    while (uart2.available()) {
      disp.input_char(uart2.get_one());
    }
  }
}
