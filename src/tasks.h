#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



namespace rtos_tasks {

  void gpio_task(void*);
  void command_task(void*);
  void display_task(void*);
  void monitor_task(void*);

};  // namespace rtos_tasks


namespace rtos_obj {
  extern QueueHandle_t gpio_queue;
  extern TaskHandle_t display_handle;
  extern TaskHandle_t led_handle;
  extern TaskHandle_t command_handle;
  extern TaskHandle_t gpio_handle;
  extern TaskHandle_t monitor_handle;
  extern TaskHandle_t uart2_tx_handle;
};  // namespace rtos_obj
