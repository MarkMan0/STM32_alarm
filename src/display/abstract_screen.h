#pragma once
#include <cstdint>


class AbstractScreen {
public:
  virtual void onEntry() {
  }
  virtual void onExit() {
  }
  virtual void onClickDown() {
  }
  virtual void onClickHeld() {
  }
  virtual void onClickUp() {
  }

  virtual void onEncoder(int32_t) {
  }

  virtual void draw() = 0;


  virtual ~AbstractScreen() {
  }
};
