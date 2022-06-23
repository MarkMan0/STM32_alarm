#include "uart_task.h"
#include "globals.h"



static std::array<char, 40> buff{};

void uart_task(void*) {
  while (1) {
    xTaskNotifyWait(0, UINT32_MAX, nullptr, portMAX_DELAY);


    if (uart2.available()) {
      buff[0] = '\0';
      strncat(buff.data(), "Got 2: \"", buff.size() - 1);
      while (uart2.available()) {
        auto c = uart2.get_one();
        if (c == '\n' || c == '\r') {
          continue;
        }
        char s[] = { c, '\0' };
        strncat(buff.data(), s, buff.size() - 1);
      }
      strncat(buff.data(), "\"", buff.size() - 1);
      strncat(buff.data(), "\n", buff.size() - 1);

      uart2.printf("%s", buff.data());
    }
  }
}
