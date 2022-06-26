#include "screens.h"
#include "display_task.h"
#include "globals.h"


void MainScreen::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 2 });
  gfx.printf("Main screen");
  gfx.draw();
}


void MainScreen::onClickUp() {
  menu.goto_screen(ScreenAllocator::allocate(Screen2()));
}


void Screen2::draw() {
  gfx.clear_canvas();
  gfx.move_cursor({ 0, 2 });
  gfx.printf("Screen2 screen");
  gfx.draw();
}


void Screen2::onClickUp() {
  menu.goto_screen(ScreenAllocator::allocate(MainScreen()));
}
