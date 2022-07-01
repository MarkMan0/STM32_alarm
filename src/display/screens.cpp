#include "screens.h"
#include "display_objects.h"
#include <array>
#include "globals.h"
#include "nanoprintf.h"


bool ScreenAllocator::flag_{ false };
ScreenAllocator::screen_collection_t ScreenAllocator::coll_1_, ScreenAllocator::coll_2_;


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
