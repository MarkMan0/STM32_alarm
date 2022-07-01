/**
 * @file GFX.cpp
 * @brief GFX class implementation
 *
 */

#include "GFX.h"
#include "SSD1306/fonts.h"

#include <cstring>
#include "utils.h"
#include "main.h"
#include "SSD1306.h"
#include "nanoprintf.h"


std::pair<uint8_t, uint8_t> GFX::get_page_and_mask(uint8_t row) const {
  std::pair<uint8_t, uint8_t> ret{ 7 - (row / 8), (1 << (7 - row % 8)) };
  return ret;
}

uint8_t& GFX::canvas_access(uint8_t i, uint8_t j) {
  i = utils::constrain(i, 0, canvas_.size());
  j = utils::constrain(j, 0, canvas_[0].size());
  return canvas_[i][j];
}


void GFX::set_pixel(const Pixel& pix, bool val) {
  auto [page, mask] = get_page_and_mask(pix.y_);
  if (val) {
    canvas_access(pix.x_, page) |= mask;
  } else {
    canvas_access(pix.x_, page) &= ~mask;
  }
}


void GFX::reset_pixel(const Pixel& pix) {
  set_pixel(pix, false);
}


bool GFX::get_pixel(const Pixel& pix) {
  auto [page, mask] = get_page_and_mask(pix.y_);
  return (canvas_access(pix.x_, page) & mask);
}

void GFX::toggle_pixel(const Pixel& pix) {
  set_pixel(pix, !get_pixel(pix));
}

void GFX::draw() {
  assert_param(ssd_1306_ != nullptr);

  if (ssd_1306_) {
    ssd_1306_->draw_canvas(canvas_);
  }
}


void GFX::clear_canvas() {
  // we can use memset instead of a double for
  memset(canvas_.data(), 0, 128 * 8);
  move_cursor({ 0, 0 });
}



void GFX::draw_circle(const Pixel& pix, uint8_t radius, bool val) {
  // create a slightly larger box around the circle
  static constexpr uint8_t margin = 2;
  const Pixel start{ static_cast<uint8_t>(std::max(0, pix.x_ - radius - margin)),
                     static_cast<uint8_t>(std::max(0, pix.y_ - radius - margin)) },
      end{ static_cast<uint8_t>(std::min(127, pix.x_ + radius + margin)),
           static_cast<uint8_t>(std::min(63, pix.y_ + radius + margin)) };

  // check each pixel in the bo, if it is inside the circle
  // not the most efficient way

  auto callback = [&pix, radius, val, this](const Pixel& curr) {
    if (pix.distance(curr) <= radius) {
      this->set_pixel(curr, val);
    }
  };

  pixel_iterate(start, end, callback);
}


void GFX::draw_rectangle(const Pixel& top_left, const Pixel& bottom_right, bool val) {
  auto callback = [this, val](const Pixel& curr) { this->set_pixel(curr, val); };

  pixel_iterate(top_left, bottom_right, callback);
}

/// \todo rework this mess
void GFX::render_glyph(const Pixel& pos, char c) {
  const auto x_offset = pos.x_;
  const auto page = 7 - utils::constrain(pos.y_, 0, 7);
  const auto& curr_font = fonts::font1;

  if (c < curr_font.offset_) {
    // cant render
    return;
  }
  const auto index = c - curr_font.offset_;
  if (index >= curr_font.num_glyphs_) {
    // cant render
    return;
  }

  /*
   * we can only set columns in the canvas, not rows
   * so we need to extract columns from the bitmap
   * bitmap data is in binary, in the format:
   * u8, u8, u8
   * u8, u8, u8
   * with 8 rows
   */
  for (int col = 7; col >= 0; --col) {
    // construct columns in reverse to display correctly
    uint8_t column_val{ 0 };

    for (int glyph_row = 0; glyph_row < 8; ++glyph_row) {
      // get the index for the row of this char
      auto ind = index + glyph_row * curr_font.num_glyphs_;
      auto a = curr_font.font_[ind];
      // extract the bit, and place it into the row
      column_val |= (!!(a & (uint8_t)(1 << col))) << (7 - glyph_row);
    }
    // finally write the column
    canvas_access(7 - col + x_offset, page) = column_val;
  }
}


void GFX::draw_text(const char* txt) {
  const auto len = strlen(txt);
  const auto increment = fonts::font1.width;

  bool state{ true };

  for (unsigned int i = 0; i < len; ++i) {
    if (!state) {
      return;
    }
    render_one(txt[i], increment, state);
  }
}


void GFX::render_one(char c, uint8_t increment, bool& state) {
  if (!state) {
    return;
  }

  switch (c) {
    case '\r':
      cursor_.x_ = 0;
      return;
    case '\n':
      cursor_.x_ = 0;
      cursor_.y_++;
      if (cursor_.y_ > 7) {
        cursor_.y_ = 0;
        return;
      }
      return;
    case '\t':
      // render tab as 2 spaces
      render_one(' ', increment, state);
      render_one(' ', increment, state);
      return;
  }

  render_glyph(cursor_, c);
  cursor_.x_ += increment;
  if (cursor_.x_ > 127 - increment) {
    // next char won't fit
    cursor_.x_ = 0;
    cursor_.y_++;
    if (cursor_.y_ > 7) {
      // screen is full
      cursor_.y_ = 0;
      state = false;
      return;
    }
  }
}

void GFX::move_cursor(const Pixel& to) {
  cursor_ = to;
}

void GFX::printf(const char* fmt, ...) {
  std::va_list args;
  va_start(args, fmt);
  npf_vpprintf(putc, this, fmt, args);
  va_end(args);
}


void GFX::putc(int c, void* ptr) {
  GFX& gfx = *reinterpret_cast<GFX*>(ptr);
  bool b = true;
  gfx.render_one(c, 8, b);
}
