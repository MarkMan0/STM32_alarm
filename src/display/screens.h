/**
 * @file screens.h
 * @brief Declaration of Screens
 *
 */

#pragma once
#include "main.h"
#include "abstract_screen.h"
#include "DS3231.h"
#include <algorithm>
#include <type_traits>

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
  void start_beep();  ///< Enable timer
  void beep_tick();   ///< Periodically disable timer
  void stop_beep();   ///< Disable timer

  bool blink_flag_ = false;
  uint32_t next_blink_ = 0;
  uint32_t next_beep_ = 0;
  bool beep_state_ = true;
  int alarm_no_{ 0 };
};
