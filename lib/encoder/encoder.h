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
      // Just pressed
      case BTN_UP:
        if (button_state_ == DOWN_STATE) {
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = PRESS_DETECTED;
        }
        break;

      // Wait and see if real press or bounce/noise
      case PRESS_DETECTED:
        if (button_state_ != DOWN_STATE) {
          tracker_state_ = BTN_UP;  // debounce fail, false press
        } else if (utils::elapsed(HAL_GetTick(), event_time_)) {
          tracker_state_ = PRESS_WAIT_HELD;
          event_time_ += HELD_DELAY - DEBOUNCE_DELAY;
          return btn_event_t::PRESSED;  // debounce success
        }
        break;

      // Real press, wait until reporting held
      case PRESS_WAIT_HELD:
        if (button_state_ != DOWN_STATE) {
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = RELEASE_DETECTED_NO_HELD;
        } else if (utils::elapsed(HAL_GetTick(), event_time_)) {
          tracker_state_ = BTN_HELD;
          return btn_event_t::HELD;
        }
        break;

      // button is held down
      case BTN_HELD:
        if (button_state_ != DOWN_STATE) {
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = RELEASE_DETECTED_HELD;
        }
        return btn_event_t::HELD;

      // button was released
      case RELEASE_DETECTED_NO_HELD:
      case RELEASE_DETECTED_HELD:
        if (button_state_ == DOWN_STATE) {
          // false positive
          tracker_state_ = tracker_state_ == RELEASE_DETECTED_HELD ? BTN_HELD : PRESS_WAIT_HELD;
        }
        if (utils::elapsed(HAL_GetTick(), event_time_)) {
          // debounced and still released
          tracker_state_ = BTN_UP;
          return btn_event_t::RELEASED;
        }
        return tracker_state_ == RELEASE_DETECTED_HELD ? btn_event_t::HELD : btn_event_t::NONE;


      default:
        break;
    }
    return btn_event_t::NONE;
  };



private:
  static constexpr uint32_t HELD_DELAY = 300;
  static constexpr uint32_t DEBOUNCE_DELAY = 10;
  static constexpr bool DOWN_STATE = PRESSED_STATE;

  uint32_t event_time_{};

  bool button_state_ = !DOWN_STATE;

  enum btn_state : uint8_t {
    BTN_UP,
    PRESS_DETECTED,
    PRESS_WAIT_HELD,
    BTN_HELD,
    RELEASE_DETECTED_NO_HELD,
    RELEASE_DETECTED_HELD,
  } tracker_state_ = BTN_UP;
};
