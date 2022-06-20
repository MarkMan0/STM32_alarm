#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "GFX.h"
#include "SSD1306.h"
#include "rtos_i2c.h"
#include "globals.h"
#include "DS3231.h"
#include <array>

GFX gfx;
SSD1306 display(i2c);
DS3231 rtc(i2c);

static std::array<char, 15> time_str;

void display_task(void* ptr_in) {
  while (1) {
    DS3231::time t;
    if (rtc.get_time(t)) {
      constexpr char fmt[] = "%02d:%02d:%02d";
      int len = snprintf(time_str.data(), time_str.size(), fmt, t.hour, t.min, t.sec);
      ++len;
      len *= 8;  // width of font
      gfx.clear_canvas();
      gfx.move_cursor({ 128 - len, 0 });
      gfx.draw_text(time_str.data());
      gfx.draw();
    }

    osDelay(pdMS_TO_TICKS(1000));
  }
}
