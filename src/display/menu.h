/**
 * @file menu.h
 * @brief Code for tracking the current screen
 */

#pragma once
#include "abstract_screen.h"
#include <variant>
#include <optional>
#include "screens.h"


/// Tracks the current screen, and handles events
class Menu {
public:
  void init();  ///< Call once at the beginning
  void tick();  ///< Call periodically to handle events and render screen
  /// Put Menu into sleep mode
  void sleep() {
    was_sleeping_ = true;
  }

  /**
   * @brief Allocate a new screen and switch to it
   *
   * @tparam T type of screen
   * @param args arguments to constructor of T
   */
  template <class T, class... Args>
  void goto_screen(Args&&... args) {
    next_screen_ = ScreenAllocator::allocate<T>(std::forward<Args>(args)...);
  }


private:
  AbstractScreen* curr_screen_;  ///< Not owning pointer to current screen
  AbstractScreen* next_screen_;  ///< Not owning pointer to next screen
  int32_t last_encoder_{ 0 };    ///< Value of encoder in last tick()
  bool held_handled_ = false;    ///< Track if held event was handled, if yes, don't call release
  bool was_sleeping_ = false;    ///< Track if menu was sleeping
};
