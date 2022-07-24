#include "screens.h"
#include "display_objects.h"
#include <array>
#include "globals.h"
#include "nanoprintf.h"


bool ScreenAllocator::flag_{ false };
uint8_t ScreenAllocator::storage_1_[ScreenAllocator::mem_sz];
uint8_t ScreenAllocator::storage_2_[ScreenAllocator::mem_sz];


void MainScreen::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 0 });
  DS3231::time t;
  if (0 == rtc.get_time(t)) {
    // length of the time string 00:00:00
    constexpr int len = 8 * (2 + 1 + 2 + 1 + 2);
    gfx.move_cursor({ 127 - len, 0 });
    gfx.printf("%02d:%02d:%02d", t.hour, t.min, t.sec);

    gfx.move_cursor({ 0, 1 });
    gfx.printf("%d.%2d.%4d", t.date, t.month, t.year);
    gfx.printf(" %.*s", 3, t.dow_str);
  } else {
    gfx.move_cursor({ 0, 1 });
    gfx.printf("ERR rtc");
  }

  float f = 0;
  if (0 == rtc.read_temperature(f)) {
    gfx.move_cursor({ 0, 0 });
    gfx.printf("%d C", static_cast<int>(f));
  }

  gfx.move_cursor({ 0, 3 });
  for (int i = 0; i < 2; ++i) {
    DS3231::alarm_t alarm;
    gfx.printf("Alarm %d ", i);
    if (0 == rtc.get_alarm(i, alarm)) {
      if (alarm.en) {
        gfx.printf("ON\n  ");
        if (alarm.alarm_type == DS3231::alarm_t::DAILY) {
          gfx.printf("*: ");
        } else {
          gfx.printf("%d: ", alarm.dow);
        }
        gfx.printf("%02d:%02d\n", alarm.hour, alarm.min);
      } else {
        gfx.printf("OFF\n");
      }
    }
  }

  gfx.draw();
}


bool MainScreen::onClickUp() {
  menu.goto_screen<MainMenuScreen>();
  return true;
}


void AlarmScreen::start_beep() {
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2);
  next_beep_ = HAL_GetTick() + 500;
}

void AlarmScreen::beep_tick() {
  if (utils::elapsed(HAL_GetTick(), next_beep_)) {
    next_beep_ = HAL_GetTick() + 500;
    if (beep_state_) {
      // HAL_TIM_Base_Stop(&htim2);
      HAL_TIM_OC_Stop(&htim2, TIM_CHANNEL_2);
    } else {
      // HAL_TIM_Base_Start(&htim2);
      HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_2);
    }
    beep_state_ = not beep_state_;
  }
}

void AlarmScreen::stop_beep() {
  HAL_TIM_Base_Stop(&htim2);
  HAL_TIM_OC_Stop(&htim2, TIM_CHANNEL_2);
}

void AlarmScreen::onEntry() {
  start_beep();
  next_blink_ = HAL_GetTick() + 500;
}

void AlarmScreen::onExit() {
  stop_beep();
}


bool AlarmScreen::onClickUp() {
  menu.goto_screen<MainScreen>();
  return true;
}

void AlarmScreen::draw() {
  gfx.clear_canvas();

  bool border = blink_flag_;

  if (utils::elapsed(HAL_GetTick(), next_blink_)) {
    next_blink_ = HAL_GetTick() + 500;
    blink_flag_ = not blink_flag_;
  }

  gfx.draw_rectangle({ 0, 8 * 2 }, { 127, 63 }, border);
  gfx.draw_rectangle({ 0, 0 }, { 127, 8 * 2 }, false);

  int len = npf_snprintf(NULL, 0, "Alarm %d", alarm_no_);

  int start_pos = 127 - (8 * len) * 3 / 2;
  gfx.move_cursor({ start_pos, 0 });
  gfx.printf("Alarm %d", alarm_no_);

  gfx.draw();
  beep_tick();
}
