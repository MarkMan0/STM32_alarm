/**
 * @file rtos_utils.h
 * @brief utility functions to ease working with freertos
 */

#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

namespace utils {

  struct Lock {
  private:
    const SemaphoreHandle_t mtx_{};

  public:
    Lock(SemaphoreHandle_t mtx) : mtx_(mtx) {
      xSemaphoreTake(mtx_, portMAX_DELAY);
    }
    ~Lock() {
      xSemaphoreGive(mtx_);
    }
  };

  struct LockISR {
  private:
    const SemaphoreHandle_t mtx_{};

  public:
    LockISR(SemaphoreHandle_t mtx) : mtx_(mtx) {
      xSemaphoreTakeFromISR(mtx_, nullptr);
    }
    ~LockISR() {
      xSemaphoreGiveFromISR(mtx_, nullptr);
    }
  };


}  // namespace utils
