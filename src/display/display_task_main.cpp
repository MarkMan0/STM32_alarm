#include "FreeRTOS.h"
#include "GFX.h"
#include "SSD1306.h"
#include "globals.h"
#include "tasks.h"

Menu menu;
SSD1306 display(i2c);
GFX gfx;

void rtos_tasks::ui_task(void* ptr_in) {
  display.begin();
  gfx.insert_ssd1306_dependency(&display);
  menu.init();

  uint32_t next_sleep = HAL_GetTick() + 10000;

  TickType_t timeout = pdMS_TO_TICKS(100);
  bool was_sleeping = false;
  while (1) {
    if (pdPASS == xTaskNotifyWait(0, UINT32_MAX, nullptr, timeout)) {
      next_sleep = HAL_GetTick() + 10000;
      timeout = pdMS_TO_TICKS(100);
      if (was_sleeping) {
        display.begin();
        was_sleeping = false;
      }
    }
    if (utils::elapsed(HAL_GetTick(), next_sleep)) {
      timeout = portMAX_DELAY;
      menu.sleep();
      gfx.clear_canvas();
      gfx.draw();
      display.sleep();
      was_sleeping = true;
    } else {
      menu.tick();
    }
  }
}
