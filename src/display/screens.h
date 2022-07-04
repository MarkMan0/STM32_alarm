/**
 * @file screens.h
 * @brief Declaration of Screens
 *
 */

#pragma once

#include "abstract_screen.h"
#include "DS3231.h"
#include <algorithm>


/// The default screen
class MainScreen : public AbstractScreen {
public:
  void draw() override;
  bool onClickUp() override;
};

/// Main menu
class MainMenuScreen : public AbstractScreen {
public:
  void draw() override;
  void onEncoder(int32_t) override;
  bool onClickUp() override;

private:
  static constexpr const char* menu_items[] = { "Back", "Set alarm 1", "Set alarm 2", "Alarm" };
  static constexpr unsigned num_items = sizeof(menu_items) / sizeof(menu_items[0]);

  unsigned current_item_ = 0;
};

/// Example screen
class SetAlarmScreen : public AbstractScreen {
public:
  SetAlarmScreen(int n) : alarm_no_(n) {
    next_blink_ = HAL_GetTick() + 1000;
  }
  void onEntry() override;
  void onExit() override;
  void draw() override;
  bool onClickDown() override;
  bool onClickUp() override;
  bool onClickHeld() override;
  void onEncoder(int32_t) override;

private:
  DS3231::alarm_t alarm_;        ///< Alarm read on entry and written on exit
  bool valid_ = false;           ///< if alarm read was successful
  int alarm_no_{ 0 };            ///< Alarm being edited
  bool editing_hour_ = false;    ///< User is editing the hour
  bool editing_minute_ = false;  ///< User is editing the minute
  bool editing_en_ = false;      ///< User is editing the hour
  bool editing_dow_ = true;      ///< User is editing the DOW of the alarm
  bool blink_on_ = true;         ///< blink state while editing
  uint32_t next_blink_ = 0;      ///< time to change blink_
  bool held_released_ = true;    ///< so holding the button doesn't skip settings
};


/// Example screen
class AlarmScreen : public AbstractScreen {
public:
  AlarmScreen(int n) : alarm_no_(n) {
  }
  void onEntry() override;    ///< start blinking of LED
  void onExit() override;     ///< stop blinking of LED
  void draw() override;       ///< blinking alarm screen
  bool onClickUp() override;  ///< end alarm

private:
  bool blink_flag_ = false;
  uint32_t next_blink_ = 0;
  int alarm_no_{ 0 };
};


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

    void* const ptr = flag_ ? storage_1_ : storage_2_;
    flag_ = not flag_;

    new (ptr) T(std::forward<Args>(args)...);
    return reinterpret_cast<AbstractScreen*>(ptr);
  }
};
