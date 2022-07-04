/**
 * @file screens.h
 * @brief Declaration of Screens
 *
 */

#pragma once

#include "abstract_screen.h"
#include <variant>
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
 * @brief Stores 2 screens in memory. On allocate, doesn't destroy the previous
 * @details This is needed for changing screens. Only two screens are instantiated at any time at max. Storage size is
 * known at compile time - no dynamic allocation is needed.
 */
class ScreenAllocator {
public:
  /// std::variant type, that can store any of the available screens.
  using screen_collection_t = std::variant<MainScreen, MainMenuScreen, SetAlarmScreen, AlarmScreen>;

  /**
   * @brief Allocate a screen of type T()
   * @brief Screen is stored in an alternating pattern in two storages.
   * @tparam T type of screen, must be part of screen_collection_t
   * @param t moving is used to put this into storage
   * @return AbstractScreen* pointer to the newly allocated memory
   */
  template <class T>
  static AbstractScreen* allocate(T&& t) {
    // pointer will be stored here
    AbstractScreen* ptr;
    // get reference to collection based on flag
    auto& coll = flag_ ? coll_1_ : coll_2_;
    // negate flag so next call is to other collection
    flag_ = not flag_;
    // put @p t into collection
    coll = std::move(t);
    // get the pointer to the stored screen
    std::visit([&](auto& arg) { ptr = &arg; }, coll);
    return ptr;
  }

private:
  static bool flag_;                            ///< determines which storage will be used
  static screen_collection_t coll_1_, coll_2_;  ///< Screen storages
};
