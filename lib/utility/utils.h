#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

namespace utils {
  struct Lock {
  private:
    const SemaphoreHandle_t mtx_{};
    bool is_locked_{ true };

  public:
    Lock(SemaphoreHandle_t mtx) : mtx_(mtx) {
      xSemaphoreTake(mtx_, portMAX_DELAY);
    }
    ~Lock() {
      if (is_locked_) {
        xSemaphoreGive(mtx_);
      }
    }

    Lock(const Lock&) = delete;
    Lock& operator=(const Lock&) = delete;

    Lock(Lock&& other) : mtx_(other.mtx_) {
      other.is_locked_ = false;
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


  template <class T, class U, class V>
  constexpr inline T constrain(T val, U low, V high) {
    if (val <= low) {
      return low;
    } else if (val >= high) {
      return high;
    } else {
      return val;
    }
  }


  template <class T, class K, class L>
  inline constexpr bool within(T val, K low, L high) {
    return (val >= low) && (val <= high);
  }


  template <class T, class K, class L>
  inline constexpr bool between(T val, K low, L high) {
    return (val > low) && (val < high);
  }
}  // namespace utils
