#include "screens.h"
#include "../globals.h"
#include "display_objects.h"


void SetAlarmScreen::onEntry() {
  valid_ = 0 == rtc.get_alarm(alarm_no_, alarm_);
}

void SetAlarmScreen::onExit() {
  auto retval = rtc.set_alarm(alarm_no_, alarm_);
  if (retval != 0) {
    uart2.printf("Set alarm failed for: %d, code %d\n", alarm_no_, retval);
  }
}


void SetAlarmScreen::draw() {
  if (utils::elapsed(HAL_GetTick(), next_blink_)) {
    next_blink_ = HAL_GetTick() + 1000;
    blink_on_ = not blink_on_;
  }

  gfx.clear_canvas();
  gfx.printf("Alarm: %d", alarm_no_);


  if (not valid_) {
    gfx.printf("\n\nCouldn't get alarm");
    return;
  }

  gfx.move_cursor({ 0, 2 });

  // default format is:
  // %d: %2d:%2d->%d, DOW, hour, minute, en


  const int dow_num = alarm_.alarm_type == DS3231::alarm_t::DAILY ? 0 : alarm_.dow;
  if (editing_dow_ && !blink_on_) {
    gfx.printf(" : ");
  } else {
    gfx.printf("%d: ", dow_num);
  }

  if (editing_hour_ && !blink_on_) {
    gfx.printf("  :");
  } else {
    gfx.printf("%02d:", alarm_.hour);
  }

  if (editing_minute_ && !blink_on_) {
    gfx.printf("  ");
  } else {
    gfx.printf("%02d", alarm_.min);
  }

  gfx.printf("->");

  if (editing_en_ && !blink_on_) {
  } else {
    if (alarm_.en) {
      gfx.printf("ON");
    } else {
      gfx.printf("OFF");
    }
  }

  gfx.draw();
}



bool SetAlarmScreen::onClickUp() {
  menu.goto_screen<MainMenuScreen>();
  return true;
}

bool SetAlarmScreen::onClickDown() {
  held_released_ = true;
  return true;
}


bool SetAlarmScreen::onClickHeld() {
  if (not held_released_) {
    return true;
  }

  held_released_ = false;

  if (editing_dow_) {
    editing_dow_ = false;
    editing_hour_ = true;
  } else if (editing_hour_) {
    editing_hour_ = false;
    editing_minute_ = true;
  } else if (editing_minute_) {
    editing_minute_ = false;
    editing_en_ = true;
  } else if (editing_en_) {
    editing_en_ = false;
    editing_dow_ = true;
  }

  return true;
}

void SetAlarmScreen::onEncoder(int32_t inc) {
  auto increment = [inc](auto& val, int min, int max) {
    if (val == min && inc < 0) {
      val = max;
    } else if (val == max && inc > 0) {
      val = min;
    } else {
      val += utils::sign(inc);
    }
  };

  if (editing_dow_) {
    increment(alarm_.dow, 0, 7);
    if (alarm_.dow != 0) {
      alarm_.alarm_type = DS3231::alarm_t::ON_DOW;
    } else {
      alarm_.alarm_type = DS3231::alarm_t::DAILY;
    }
  }
  if (editing_hour_) {
    increment(alarm_.hour, 0, 23);
  }
  if (editing_minute_) {
    increment(alarm_.min, 0, 59);
  }
  if (editing_en_) {
    increment(alarm_.en, 0, 1);
  }
}
