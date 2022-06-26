#pragma once
#include "pin_api.h"
#include "utils.h"

class Encoder {
public:
  void init(bool a_state) {
    prev_a_state_ = a_state;
    counter_ = 0;
  }

  bool update(bool a_now, bool b_now) {
    bool retval = false;
    if (a_now != prev_a_state_) {
      if (a_now != b_now) {
        ++counter_;
      } else {
        --counter_;
      }
      retval = true;
    }
    prev_a_state_ = a_now;
    return retval;
  }

  void reset() {
    counter_ = 0;
  }

  int32_t get() const {
    return counter_;
  }

private:
  int32_t counter_{ 0 };
  bool prev_a_state_;
};



enum class btn_event_t {
  NONE,
  PRESSED,
  HELD,
  RELEASED,
};
template <bool PRESSED_STATE = false>
class BtnTracker {
public:
  void update_btn(bool btn) {
    button_state_ = btn;
  }

  btn_event_t get_state() {
    switch (tracker_state_) {
      case BTN_UP: {
        if (button_state_ == DOWN_STATE) {
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = BTN_DEBOUNCE_DOWN;
        }
        break;
      }
      case BTN_DEBOUNCE_DOWN: {
        if (utils::elapsed(HAL_GetTick(), event_time_)) {
          if (button_state_ == DOWN_STATE) {
            tracker_state_ = BTN_WAIT_DOWN;              // actual button press
            event_time_ += HELD_DELAY - DEBOUNCE_DELAY;  // held delay since first button detection
            return btn_event_t::PRESSED;
          } else {
            tracker_state_ = BTN_UP;  // false signal / noise
          }
        }
        break;
      }
      case BTN_WAIT_DOWN: {
        if (utils::elapsed(HAL_GetTick(), event_time_)) {
          tracker_state_ = BTN_DOWN;
          return btn_event_t::HELD;
        }
        break;
      }
      case BTN_DOWN: {
        if (button_state_ != DOWN_STATE) {
          tracker_state_ = BTN_DEBOUNCE_UP;
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
        }
        return btn_event_t::HELD;
        break;
      }
      case BTN_DEBOUNCE_UP: {
        if (utils::elapsed(HAL_GetTick(), event_time_)) {
          if (button_state_ != DOWN_STATE) {
            tracker_state_ = BTN_UP;  // actual button press
            return btn_event_t::RELEASED;
          } else {
            tracker_state_ = BTN_DOWN;  // false signal / noise
            return btn_event_t::HELD;
          }
        }
        break;
      }
    }
    return btn_event_t::NONE;
  };



private:
  static constexpr uint32_t HELD_DELAY = 300;
  static constexpr uint32_t DEBOUNCE_DELAY = 50;
  static constexpr bool DOWN_STATE = PRESSED_STATE;

  uint32_t event_time_{};

  bool button_state_ = !DOWN_STATE;

  enum btn_state : uint8_t {
    BTN_UP,
    BTN_DEBOUNCE_DOWN,
    BTN_WAIT_DOWN,
    BTN_DOWN,
    BTN_DEBOUNCE_UP,
  } tracker_state_ = BTN_UP;
};
