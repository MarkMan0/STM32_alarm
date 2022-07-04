/**
 * @file encoder.h
 * @brief Rotary encoder and push button drivers
 */
#pragma once
#include "pin_api.h"
#include "utils.h"

/// @brief Tracks the encoder, increments every notch
class Encoder {
public:
  /// Reset the counter and set the current state of the A pin
  void init(bool a_state) {
    prev_a_state_ = a_state;
    counter_ = 0;
  }

  /// Update the counter, if rotation is detected
  /// @return true if rotation was detected
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

  /// Reset the counter to 0
  void reset() {
    counter_ = 0;
  }

  /// Get the value of the counter
  int32_t get() const {
    return counter_;
  }

private:
  int32_t counter_{ 0 };
  bool prev_a_state_;
};


/// The current state of the button
enum class btn_event_t {
  NONE,
  PRESSED,
  HELD,
  RELEASED,
};

/**
 * @brief Button tracker with debounce
 * @details Tracks Pressed, Released and Held events of the button, with debouncing, and delay for Held
 *
 * @tparam PRESSED_STATE the state of the pin, when the button is pressed
 */
template <bool PRESSED_STATE = false>
class BtnTracker {
public:
  /**
   * @brief Update the internal state of the tracker
   * @details The tracker can be in one of 4 states @see tracker_state_t
   * + BTN_UP: If press is detected, start debouncing. Returns DEBOUNCE_DELAY
   * + PRESS_DETECTED: If the button is still pressed after DEBOUNCE_DELAY, report Pressed event. Wait HELD_DELAY
   * + PRESS_WAIT_HELD: If HELD_DELAY elapsed, and still pressed, report Held event, and go to state BTN_HELD
   * + BTN_HELD: While button is pressed, stay in this state
   * + RELEASE_DETECTED: This state is entered from BTN_HELD or PRESS_WAIT_HELD. Start debouncing the release of the
   * button. If DEBOUNCE_DELAY elapsed, report Release event, and go to BTN_UP
   * @return The time, after which this function should be called again or 0
   */
  uint32_t update_btn() {
    switch (tracker_state_) {
      case BTN_UP:
        if (button_state_ == DOWN_STATE) {
          /// Press detected, start debouncing
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = PRESS_DETECTED;
          return DEBOUNCE_DELAY;  // should be called again after debounce_delay
        }
        break;

      case PRESS_DETECTED:
        if (utils::elapsed(HAL_GetTick(), event_time_)) {
          if (button_state_ != DOWN_STATE) {
            /// Debounce failed, was noise only
            tracker_state_ = BTN_UP;
          } else {
            /// Press detected, wait HELD_DELAY
            tracker_state_ = PRESS_WAIT_HELD;
            event_time_ += HELD_DELAY;
            register_press();   // register the button press
            return HELD_DELAY;  // call again after held delay elapsed
          }
        }
        break;

      case PRESS_WAIT_HELD:
        if (button_state_ != DOWN_STATE) {
          /// Button released befor HELD_DELAY, debounce release
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = RELEASE_DETECTED;
          return DEBOUNCE_DELAY;  // debounce release, call after delay
        } else if (utils::elapsed(HAL_GetTick(), event_time_)) {
          /// HELD_DELAY elapsed, button is now in Held state
          tracker_state_ = BTN_HELD;
          register_held();
        }
        break;

      case BTN_HELD:
        if (button_state_ != DOWN_STATE) {
          /// Release of button detected, start debouncing
          event_time_ = HAL_GetTick() + DEBOUNCE_DELAY;
          tracker_state_ = RELEASE_DETECTED;
          return DEBOUNCE_DELAY;  // debounce release, call after delay
        }
        break;

      // button was released
      case RELEASE_DETECTED:
        if (utils::elapsed(HAL_GetTick(), event_time_)) {
          if (button_state_ == DOWN_STATE) {
            /// false positive
            // This returns to BTN_HELD, even if the delay hasn't elapsed yet
            // This rarely occurs, and introducing more code would unnecessarily bloat code
            tracker_state_ = BTN_HELD;
          } else {
            /// Button was released, register event and go to UP state
            tracker_state_ = BTN_UP;
            register_release();
          }
        }
        break;


      default:
        break;
    }
    return 0;
  }

  /// Updates the state of the button, and the state of the tracker @see update_btn()
  uint32_t update_btn(bool btn) {
    button_state_ = btn;
    return update_btn();
  }

  /// Return and consumes the event - Release and Press are reported only once
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



  static constexpr uint32_t HELD_DELAY = 600;        ///< Delay after Held is reported
  static constexpr uint32_t DEBOUNCE_DELAY = 20;     ///< Delay to debounce the button
  static constexpr bool DOWN_STATE = PRESSED_STATE;  ///< The state of the pin when pressed

private:
  uint32_t event_time_{};  ///< ms since start of program for next event @see utils::elapsed(uint32_t, uint32_t)

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

  bool button_state_ = !DOWN_STATE;  ///< The current state of the button

  enum tracker_state_t : uint8_t {
    BTN_UP,            ///< Button is released
    PRESS_DETECTED,    ///< Press detected, not yet debounced
    PRESS_WAIT_HELD,   ///< Press debounced and verified, not yet held
    BTN_HELD,          ///< button is held down
    RELEASE_DETECTED,  ///< Release detected, not yet debounced
  };
  tracker_state_t tracker_state_ = BTN_UP;  ///< Currecnt state of the tracker
};



/// The rotary encoder we are using has a button and an encoder
struct RotaryEncoder {
  Encoder enc;
  BtnTracker<false> btn;
};
