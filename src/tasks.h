/**
 * @file tasks.h
 * @brief Global FreeRTOS objects and functions
 */

#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



namespace rtos_tasks {

  /// Handle GPIO events. Events are received in a Queue
  void gpio_task(void*);

  /// Handle UART commands
  void command_task(void*);

  /// Draw display
  void ui_task(void*);

#ifdef MONITOR_TASK
  /// Monitor memory consumption of tasks. Can be disabled for "production"
  void monitor_task(void*);
#endif

};  // namespace rtos_tasks


namespace rtos_obj {
  extern QueueHandle_t gpio_queue;     ///< Queue for GPIO events
  extern TaskHandle_t display_handle;  ///< handle for UI task
  extern TaskHandle_t command_handle;  ///< UART RX handler and command dispatcher
  extern TaskHandle_t gpio_handle;     ///< GPIO task handle
#ifdef MONITOR_TASK
  extern TaskHandle_t monitor_handle;  ///< Memory monitor task handle
#endif
  extern TaskHandle_t uart2_tx_handle;  ///< UART2 tx task handle

};  // namespace rtos_obj
