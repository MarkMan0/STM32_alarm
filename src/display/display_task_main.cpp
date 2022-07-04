/**
 * @file display_task_main.cpp
 * @brief Implementation of the UI task
 *
 */

#include "FreeRTOS.h"
#include "GFX.h"
#include "SSD1306.h"
#include "globals.h"
#include "tasks.h"

Menu menu;
SSD1306 display(i2c);
GFX gfx(&display);

/// The actual task
void rtos_tasks::ui_task(void* ptr_in) {
  display.begin();
  menu.init();

  uint32_t next_sleep = HAL_GetTick() + 10000;

  TickType_t timeout = pdMS_TO_TICKS(100);
  bool was_sleeping = false;
  bool is_alarm = false;
  while (1) {
    if (pdPASS == xTaskNotifyWait(0, UINT32_MAX, nullptr, timeout)) {
      // Got an event, reset turn off timeout
      next_sleep = HAL_GetTick() + 10000;
      timeout = pdMS_TO_TICKS(100);
      if (was_sleeping) {
        // only call begin once
        display.begin();
        was_sleeping = false;
      }

      if (read_pin(pins::alarm_it) == 0) {
        is_alarm = true;
      }
    }
    if (utils::elapsed(HAL_GetTick(), next_sleep)) {
      // No user input, go to sleep
      timeout = portMAX_DELAY;
      menu.sleep();
      gfx.clear_canvas();
      gfx.draw();
      display.sleep();
      was_sleeping = true;
    } else {
      // refresh, if turn off timeout hasn't elapsed yet

      if (is_alarm) {
        is_alarm = false;
        bool b = false;
        if (0 == rtc.get_and_clear_alarm_flag(0, b)) {
          menu.goto_screen(ScreenAllocator::allocate(AlarmScreen()));
        }
      }


      menu.tick();
    }
  }
}
