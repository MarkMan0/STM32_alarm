#pragma once
#include <cstdint>


class AbstractScreen {
public:
  virtual void onEntry() {
  }
  virtual void onExit() {
  }
  virtual bool onClickDown() {
    return false;
  }
  virtual bool onClickHeld() {
    return false;
  }
  virtual bool onClickUp() {
    return false;
  }

  virtual void onEncoder(int32_t) {
  }

  virtual void draw() = 0;


  virtual ~AbstractScreen() {
  }
};
