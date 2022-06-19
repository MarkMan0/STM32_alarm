#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "GFX.h"
#include <unity.h>
#include "rtos_i2c.h"
#include "SSD1306.h"

static RTOS_I2C i2c;
static SSD1306 display(i2c);
static GFX gfx;

void setUp() {
  gfx.clear_canvas();
  gfx.draw();
  gfx.move_cursor({ 0, 0 });
}
void tearDown() {
}

void test_set_pixel() {
  gfx.set_pixel({ 10, 10 });
  gfx.draw();
  TEST_ASSERT_EQUAL(true, gfx.get_pixel({ 10, 10 }));
  gfx.clear_canvas();
  gfx.draw();
}

void test_draw_rectangle() {
  Pixel start{ 40, 13 };
  Pixel size{ 20, 5 };
  gfx.draw_rectangle(start, { start.x_ + size.x_, start.y_ + size.y_ });
  gfx.draw();

  for (int x = 0; x < 128; ++x) {
    for (int y = 0; y < 64; ++y) {
      if (utils::within(x, start.x_, start.x_ + size.x_) && utils::within(y, start.y_, start.y_ + size.y_)) {
        TEST_ASSERT_TRUE(gfx.get_pixel({ x, y }));
      }
    }
  }
}

void test_draw_text() {
  const char* txt = "Hello world!";

  gfx.move_cursor({ 0, 4 });
  gfx.draw_text(txt);
  gfx.draw();
}

void test_printf() {
  char fmt[] = "%s %d%c";
  gfx.printf(fmt, "Five: ", 5, '?');
  gfx.draw();
}

void test_task(void*) {
  UNITY_BEGIN();

  RUN_TEST(test_set_pixel);
  RUN_TEST(test_draw_rectangle);
  RUN_TEST(test_draw_text);
  RUN_TEST(test_printf);

  UNITY_END();

  while (1) {
  }
}


void pre_test() {
  i2c.init_i2c1();
  display.begin();
  gfx.clear_canvas();
  gfx.insert_ssd1306_dependency(&display);
}
