#include "FreeRTOS.h"
#include "GFX.h"
#include "SSD1306.h"
#include "rtos_i2c.h"
#include "globals.h"
#include "DS3231.h"
#include <array>

Menu menu;
static std::array<char, 15> time_str;

void display_task(void* ptr_in) {
  menu.init();
  while (1) {
    menu.tick();
    xTaskNotifyWait(0, UINT32_MAX, nullptr, pdMS_TO_TICKS(100));
  }
}
