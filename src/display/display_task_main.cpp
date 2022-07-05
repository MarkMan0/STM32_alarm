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



enum ui_state_t {
  SLEEPING,
  WAKEUP,
  RUNNING,
  PRE_ALARM,
  ALARM,
  POST_ALARM,
};
static ui_state_t ui_state{ RUNNING };


static void check_and_handle_alarm(int n) {
  bool b = false;
  if (0 == rtc.get_and_clear_alarm_flag(n, b) && b) {
    menu.goto_screen(ScreenAllocator::allocate<AlarmScreen>(n));
  }
}
static void check_and_handle_alarms() {
  check_and_handle_alarm(0);
  check_and_handle_alarm(1);
}

static void ui_task_init() {
  display.begin();
  menu.init();

  // clear alarm flags, if any
  check_and_handle_alarms();
}





static void ui_task_tick() {
  static uint32_t next_sleep = 99999;
  static uint32_t timeout = 100;

  uint32_t notif = 0;
  const BaseType_t result = xTaskNotifyWait(0, UINT32_MAX, &notif, timeout);


  if (result == pdPASS) {
    // got notification
    next_sleep = HAL_GetTick() + 10000;  // delay sleep
    timeout = pdMS_TO_TICKS(100);        // 100ms refresh rate until sleep
    if (ui_state == SLEEPING) {
      // was sleeping?
      // wake up
      display.begin();
    }

    ui_state = RUNNING;

    if (notif & rtos_obj::FLAG_ALARM_DETECTED) {
      // is wakeup on alarm?
      // only entered once per alarm
      ui_state = PRE_ALARM;
      next_sleep = HAL_MAX_DELAY;  // no sleep while alarm
    }
  }


  // needs to sleep?
  if (ui_state == RUNNING && utils::elapsed(HAL_GetTick(), next_sleep)) {
    // No user input, go to sleep
    timeout = portMAX_DELAY;
    menu.sleep();
    gfx.clear_canvas();
    gfx.draw();
    display.sleep();
    ui_state = SLEEPING;
  }

  // is pre alarm?
  if (ui_state == PRE_ALARM) {
    check_and_handle_alarms();
    ui_state = ALARM;
  }

  // in running/alarm state nothing special needs to be done


  menu.tick();
}



/// The actual task
void rtos_tasks::ui_task(void* ptr_in) {
  ui_task_init();

  while (1) {
    ui_task_tick();
  }
}
