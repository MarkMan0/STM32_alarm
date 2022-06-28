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
  uint32_t update_btn(bool btn) {
    button_state_ = btn;
    return update_btn();
  }

  uint32_t update_btn() {
    switch (tracker_state_) {
      // Just pressed
      case BTN_UP:
        if (button_state_ == DOWN_STATE) {
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = PRESS_DETECTED;
          return DEBOUNCE_DELAY;  // should be called again after debounce_delay
        }
        break;

      // Wait and see if real press or bounce/noise
      case PRESS_DETECTED:
        if (button_state_ != DOWN_STATE) {
          tracker_state_ = BTN_UP;  // debounce fail, false press
        } else if (utils::elapsed(HAL_GetTick(), event_time_)) {
          tracker_state_ = PRESS_WAIT_HELD;
          event_time_ += HELD_DELAY - DEBOUNCE_DELAY;
          register_press();                    // register the button press
          return HELD_DELAY - DEBOUNCE_DELAY;  // call again after held delay elapsed
        }
        break;

      // Real press, wait until reporting held
      case PRESS_WAIT_HELD:
        if (button_state_ != DOWN_STATE) {
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = RELEASE_DETECTED;
          return DEBOUNCE_DELAY;  // debounce release, call after delay
        } else if (utils::elapsed(HAL_GetTick(), event_time_)) {
          tracker_state_ = BTN_HELD;
          register_held();
        }
        break;

      // button is held down
      case BTN_HELD:
        if (button_state_ != DOWN_STATE) {
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = RELEASE_DETECTED;
          return DEBOUNCE_DELAY;  // debounce release, call after delay
        }

      // button was released
      case RELEASE_DETECTED:
        if (button_state_ == DOWN_STATE) {
          // false positive
          tracker_state_ = BTN_HELD;
        }
        if (utils::elapsed(HAL_GetTick(), event_time_)) {
          // debounced and still released
          tracker_state_ = BTN_UP;
          register_release();
        }


      default:
        break;
    }
    return 0;
  }

  btn_event_t get_state() {
    if (press_event_) {
      press_event_ = false;
      return btn_event_t::PRESSED;
    }
    if (release_event_) {
      release_event_ = false;
      return btn_event_t::RELEASED;
    }
    if (held_event_) {
      return btn_event_t::HELD;
    }

    return btn_event_t::NONE;
  };



private:
  static constexpr uint32_t HELD_DELAY = 300;
  static constexpr uint32_t DEBOUNCE_DELAY = 10;
  static constexpr bool DOWN_STATE = PRESSED_STATE;

  uint32_t event_time_{};

  bool press_event_{ false }, held_event_{ false }, release_event_{ false };

  void register_press() {
    release_event_ = false;
    held_event_ = false;
    press_event_ = true;
  }
  void register_held() {
    held_event_ = true;
  }
  void register_release() {
    held_event_ = false;
    release_event_ = true;
  }

  bool button_state_ = !DOWN_STATE;

  enum report_state_t : uint8_t {
    REPORT_NONE,
    REPORT_PRESSED,
    REPORT_HELD,
    REPORT_RELEASED,
  };
  report_state_t report_state{ REPORT_NONE };

  enum tracker_state_t : uint8_t {
    BTN_UP,
    PRESS_DETECTED,
    PRESS_WAIT_HELD,
    BTN_HELD,
    RELEASE_DETECTED,
  };
  tracker_state_t tracker_state_ = BTN_UP;
};
