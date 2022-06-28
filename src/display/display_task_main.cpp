#include "FreeRTOS.h"
#include "GFX.h"
#include "SSD1306.h"
#include "globals.h"
#include "tasks.h"

Menu menu;
SSD1306 display(i2c);
GFX gfx;

void rtos_tasks::display_task(void* ptr_in) {
  display.begin();
  gfx.insert_ssd1306_dependency(&display);
  menu.init();

  while (1) {
    menu.tick();
    xTaskNotifyWait(0, UINT32_MAX, nullptr, pdMS_TO_TICKS(100));
  }
}
