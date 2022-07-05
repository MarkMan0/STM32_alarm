/**
 * @file menu.h
 * @brief Code for tracking the current screen
 */

#pragma once
#include "abstract_screen.h"
#include <variant>
#include <optional>
#include "screens.h"


/**
 * @brief Return the maximum sizeof of it's template parameters
 *
 * @tparam Ts list of classes/objects
 * @return constexpr size_t size of biggest object in bytes
 */
template <class... Ts>
inline constexpr size_t get_max_size() {
  return std::max({ sizeof(Ts)... });
}

/**
 * @brief Stores 2 screens in memory. On allocate, doesn't destroy the previous
 * @details This is needed for changing screens. Only two screens are instantiated at any time at max. Storage size is
 * known at compile time - no dynamic allocation is needed.
 */
class ScreenAllocator {
public:
  /// storage size in bytes
  static constexpr uint32_t mem_sz = get_max_size<MainScreen, MainMenuScreen, SetAlarmScreen, AlarmScreen>();
  static uint8_t storage_1_[mem_sz], storage_2_[mem_sz];  ///< Screen storages
  static bool flag_;                                      ///< determines which storage will be used

public:
  /**
   * @brief Construct temporary objects in storage spaces
   * @details It is needed, so destructor can be called when creating new screen.
   * Without this, 2 flags would be required to check, if the storage contains a valid object
   * @tparam T type of AbstractScreen
   * @param args constructor arguments to T
   * @return pointer to storage 1 with screen T
   */
  template <class T, class... Args>
  static AbstractScreen* init(Args&&... args) {
    static_assert(sizeof(T) <= mem_sz);
    static_assert(std::is_base_of<AbstractScreen, T>::value);
    new (storage_1_) T(std::forward<Args>(args)...);
    new (storage_2_) T(std::forward<Args>(args)...);

    flag_ = false;  // use storage 2 next
    return reinterpret_cast<AbstractScreen*>(storage_1_);
  }

  /**
   * @brief Construct a screen in preallocated static memory
   *
   * @tparam T type of screen
   * @tparam Args
   * @param args args of constructor of @p t
   * @return AbstractScreen* pointer to allocated screen
   */
  template <class T, class... Args>
  static AbstractScreen* allocate(Args&&... args) {
    static_assert(sizeof(T) <= mem_sz);
    static_assert(std::is_base_of<AbstractScreen, T>::value);

    void* const ptr = flag_ ? storage_1_ : storage_2_;
    flag_ = not flag_;

    // destructor of previous screen
    reinterpret_cast<AbstractScreen*>(ptr)->~AbstractScreen();
    // create new screen
    new (ptr) T(std::forward<Args>(args)...);
    return reinterpret_cast<AbstractScreen*>(ptr);
  }
};



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
