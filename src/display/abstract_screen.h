/**
 * @file abstract_screen.h
 */

#pragma once
#include <cstdint>

/// Abstract screen class
class AbstractScreen {
public:
  /// Called once, when this screen is activated
  virtual void onEntry() {
  }
  /// Called before switch to new screen
  virtual void onExit() {
  }
  /// Called on pressed event of the button
  virtual bool onClickDown() {
    return false;
  }
  /// Called on held event on button
  virtual bool onClickHeld() {
    return false;
  }
  /// Called when button is released, and held was not handled
  virtual bool onClickUp() {
    return false;
  }
  /// Called when encoder is changes, @p inc is the change in encoder position
  virtual void onEncoder(int32_t inc) {
  }

  /// Renders the screen, has to be implemented
  virtual void draw() = 0;


  virtual ~AbstractScreen() {
  }
};
