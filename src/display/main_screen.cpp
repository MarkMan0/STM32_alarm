#include "screens.h"
#include "display_objects.h"
#include "globals.h"
#include "nanoprintf.h"

static std::array<char, 15> time_str;
void MainScreen::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 0 });
  DS3231::time t;
  if (0 == rtc.get_time(t)) {
    constexpr char fmt[] = "%02d:%02d:%02d";
    int len = npf_snprintf(time_str.data(), time_str.size(), fmt, t.hour, t.min, t.sec);
    ++len;
    len *= 8;  // width of font
    gfx.move_cursor({ 128 - len, 0 });
    gfx.draw_text(time_str.data());
  } else {
    gfx.printf("ERR rtc");
  }

  gfx.move_cursor({ 0, 2 });
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
  menu.goto_screen(ScreenAllocator::allocate(MainMenuScreen()));
  return true;
}

void MainMenuScreen::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 0 });
  for (int i = 0; i < num_items; ++i) {
    gfx.move_cursor({ 0, i });
    if (i == current_item_) {
      gfx.printf("*");
    } else {
      gfx.printf(" ");
    }
    gfx.printf(menu_items[i]);
  }
  gfx.draw();
}

void MainMenuScreen::onEncoder(int32_t increment) {
  if (increment > 0) {
    ++current_item_;
  } else if (increment < 0 && current_item_) {  // do not decrement below zero
    --current_item_;
  }
  current_item_ = utils::constrain(current_item_, 0, num_items - 1);
}

bool MainMenuScreen::onClickUp() {
  switch (current_item_) {
    case 0:
      menu.goto_screen(ScreenAllocator::allocate(MainScreen()));
      break;
    case 1:
      menu.goto_screen(ScreenAllocator::allocate(Screen2()));
      break;
    case 2:
      menu.goto_screen(ScreenAllocator::allocate(Screen3()));
      break;
    case 3:
      menu.goto_screen(ScreenAllocator::allocate(Screen4()));
      break;
    default:
      break;
  }
  return true;
}

void Screen2::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 2 });
  gfx.printf("Screen2 screen\n%d", counter_);
  gfx.draw();
}
bool Screen2::onClickUp() {
  menu.goto_screen(ScreenAllocator::allocate(MainScreen()));
  return true;
}
bool Screen2::onClickHeld() {
  ++counter_;
  return true;
}

void Screen3::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 2 });
  gfx.printf("Screen3 screen");
  gfx.draw();
}
bool Screen3::onClickUp() {
  menu.goto_screen(ScreenAllocator::allocate(MainScreen()));
  return true;
}

void Screen4::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 2 });
  gfx.printf("Screen4 screen");
  gfx.draw();
}
bool Screen4::onClickUp() {
  menu.goto_screen(ScreenAllocator::allocate(MainScreen()));
  return true;
}
