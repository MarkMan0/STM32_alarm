#pragma once

/**
 * @file GFX.h
 * @brief Graphics driver
 *
 */

#include <cstdint>

#include <array>
#include <bitset>
#include <cstdarg>
#include <cmath>

class SSD1306;

struct Pixel {
  using data_t = int;
  data_t x_{ 0 };
  data_t y_{ 0 };

  Pixel() = default;
  Pixel(data_t x, data_t y) : x_{ x }, y_{ y } {
  }
  Pixel(const Pixel& other) = default;
  Pixel& operator=(const Pixel& lhs) = default;

  float distance(const Pixel& other) const {
    return std::hypot(x_ - other.x_, y_ - other.y_);
  }
};

class GFX {
public:
  using canvas_t = std::array<std::array<uint8_t, 8>, 128>; /*!< canvas, where each bit is one pixel */

  void set_pixel(const Pixel& pix, bool val = true);
  void reset_pixel(const Pixel& pix);
  bool get_pixel(const Pixel& pix);
  void toggle_pixel(const Pixel& pix);

  void clear_canvas();

  void draw_circle(const Pixel& pix, uint8_t radius, bool val = true);

  void draw_rectangle(const Pixel& top_left, const Pixel& bottom_right, bool val = true);

  /**
   * @brief Draws the character to the canvas
   *
   * @param pos x is the x pos, y is the page/line(0-7)
   * @param c the char to render (0-127)
   */
  void render_glyph(const Pixel& pos, char c);

  /**
   * @brief Draws text to the current cursor_ pos
   *
   * @param txt
   */
  void draw_text(const char* txt);

  void move_cursor(const Pixel& to);

  /**
   * @brief Draws printf-style format text, very limited capabilities
   * @see vprintf
   * @param fmt Only use %d and %s
   * @param ...
   */
  void printf(const char* fmt, ...);

  void draw();

  void insert_ssd1306_dependency(SSD1306* ssd) {
    ssd_1306_ = ssd;
  }

private:
  canvas_t canvas_{ 0 }; /*!< drawing canvas */
  Pixel cursor_;         /*!< cursor for text drawing*/

  SSD1306* ssd_1306_{ nullptr };

  /**
   * @brief For a given row, returns the page number and bit mask
   *
   * @param row give in pixel coordinate
   * @return std::pair<uint8_t, uint8_t>, first is the page number, second is the mask
   */
  std::pair<uint8_t, uint8_t> get_page_and_mask(uint8_t row) const;

  /**
   * @brief Bound checks and returns reference to byte in canvas_
   *
   * @param i index 1
   * @param j index 2
   * @return uint8_t&
   */
  uint8_t& canvas_access(uint8_t i, uint8_t j);


  /**
   * @brief Iterates through pixels from \p from to \p to
   * @details IMPORTANT: \p from should be LESS than \p to
   *
   * @tparam LAMBDA type of lambda, this allows the lambda to have a capture list
   * @param from
   * @param to
   * @param callback callback, can be a simple function pointer or a lambda
   */
  template <class LAMBDA>
  void pixel_iterate(const Pixel& from, const Pixel& to, LAMBDA& callback) {
    for (Pixel curr{ from }; curr.x_ <= to.x_; ++curr.x_) {
      for (curr.y_ = from.y_; curr.y_ <= to.y_; ++curr.y_) {
        callback(curr);
      }
    }
  }

  /**
   * @brief draw the formatted string to the canvas, limited capabilities
   * @details Currently only supports %d and %s, without width specifiers
   * @param fmt
   * @param args
   */
  void vprintf(const char* fmt, va_list args);

  /**
   * @brief Render one character and advances the cursor
   * @details success is not returned, but modified in a parameter, so code which
   * uses this method will be shorter, no need to wrap it in if, just call with the same \p state parameter
   * @param c char to render
   * @param increment width of the char
   * @param state only render if true, method will set this to false if end of screen is reached
   */
  void render_one(char c, uint8_t increment, bool& state);
};
