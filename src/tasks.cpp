/**
 * @file tasks.cpp
 * @brief Simple or miscellaneous tasks
 *
 */

#include "FreeRTOS.h"
#include "task.h"
#include "globals.h"
#include "queue.h"
#include "tasks.h"
#include "nanoprintf.h"


namespace rtos_obj {
  QueueHandle_t gpio_queue;
  QueueHandle_t btn_event_queue;
  TaskHandle_t display_handle;
  TaskHandle_t command_handle;
  TaskHandle_t gpio_handle;
#ifdef MONITOR_TASK
  TaskHandle_t monitor_handle;
#endif
  TaskHandle_t uart2_tx_handle;
};  // namespace rtos_obj



void rtos_tasks::command_task(void*) {
  static CommandDispatcher cmd(&uart2);
  while (1) {
    xTaskNotifyWait(0, UINT32_MAX, nullptr, portMAX_DELAY);

    while (uart2.available()) {
      cmd.input_char(uart2.get_one());
    }
  }
}



void rtos_tasks::gpio_task(void*) {
  GPIOStateContainer cont{ 0 };

  /// Timeout for task is based on the current event of the button
  TickType_t next_timeout = portMAX_DELAY;

  while (1) {
    uint32_t ms{ 0 };
    if (pdPASS == xQueueReceive(rtos_obj::gpio_queue, &cont, next_timeout)) {
      /// new data received for GPIO, pass it to classes
      if (cont.pin_A & 2 && cont.pin_B & 2) {
        encoder.enc.update(cont.pin_A & 1, cont.pin_B & 1);
      }
      if (cont.pin_SW & 2) {
        ms = encoder.btn.update_btn(cont.pin_SW & 1);
      }
      if (cont.pin_alarm & 2 && not(cont.pin_alarm & 1)) {
        // alarm
        /// @todo use queues with events to signal display task, not notification
      }

    } else {
      /// timeout elapsed, call update on button to track state
      ms = encoder.btn.update_btn(cont.pin_SW & 1);
    }

    /// ms will be non-zero, if this task should be called without user input
    if (ms) {
      next_timeout = pdMS_TO_TICKS(ms);
    } else {
      next_timeout = portMAX_DELAY;
    }

    /// Something happened, notify the UI task
    xTaskNotify(rtos_obj::display_handle, 0, eNoAction);
  }
}



#ifdef MONITOR_TASK

void rtos_tasks::monitor_task(void*) {
  /// wait for all tasks to start
  vTaskDelay(pdMS_TO_TICKS(10000));
  const auto num_of_tasks = uxTaskGetNumberOfTasks();

  /// Dynamic allocation of space for status of all tasks
  auto statuses = static_cast<TaskStatus_t*>(pvPortMalloc(num_of_tasks * sizeof(TaskStatus_t)));

  /// if task stack is ever lower than this many bytes, a warning is generated
  constexpr size_t memory_low_th{ 20 };
  while (1) {
    if (auto n = uxTaskGetSystemState(statuses, num_of_tasks, NULL)) {
      for (unsigned int i = 0; i < n; ++i) {
        if (statuses[i].usStackHighWaterMark < memory_low_th) {
          uart2.printf("\tMEM_WARN: %s : %d\n", statuses[i].pcTaskName, statuses[i].usStackHighWaterMark);
        }
      }
    } else {
      uart2.printf("Couldn't get system state\n");
    }

    if (xPortGetFreeHeapSize() < memory_low_th) {
      uart2.printf("\tMEM_WARN: HEAP : %d\n", static_cast<int>(xPortGetFreeHeapSize()));
    }

    vTaskDelay(pdMS_TO_TICKS(10000));
  }
}

#endif

/// Called by freertos on stack overflow in any task
void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
  static std::array<char, 50> buff;
  npf_snprintf(buff.data(), buff.size() - 1, "OVERFLOW: %s", pcTaskName);
  while (1) {
    uart2.transmit(buff.data());
    HAL_Delay(2000);
  }

  assert_param(0);
}
