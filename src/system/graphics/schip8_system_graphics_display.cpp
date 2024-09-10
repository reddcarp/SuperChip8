#include "schip8_system_graphics_display.hpp"
#include "schip8_error.hpp"

#include <iostream>
#include <raylib.h>

namespace SuperChip8::System::Graphics {

Display::Display(interrupt_handler_t interrupt_handler)
    : _interrupt_handler(interrupt_handler) {
  _virtual_front_screen.fill(
      std::array<bool, HIGH_RES_VIRTUAL_SCREEN_WIDTH>{false});
  _virtual_back_screen.fill(
      std::array<bool, HIGH_RES_VIRTUAL_SCREEN_WIDTH>{false});
}

void Display::createWindow(const std::string &title, std::error_code &ec) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(LOW_RES_VIRTUAL_SCREEN_WIDTH * _pixel_size,
             LOW_RES_VIRTUAL_SCREEN_HEIGHT * _pixel_size, title.c_str());
  if (!IsWindowReady()) {
    ec = Error::WINDOW_CREATION_ERROR;
  }
}

void Display::closeWindow() { CloseWindow(); }

bool Display::windowShouldClose() { return WindowShouldClose(); }

void Display::clear() {
  std::lock_guard lock(_virtual_back_screen_mutex);
  _virtual_back_screen.fill(
      std::array<bool, HIGH_RES_VIRTUAL_SCREEN_WIDTH>{false});
}

void Display::computeNewPixelSize() {
  // calculating max width for pixels
  std::uint32_t max_width = GetScreenWidth() / _virtual_front_screen_width;

  // calculating max height for pixels
  std::uint32_t max_height = GetScreenHeight() / _virtual_front_screen_height;

  // keeping the smallest value (since we want square pixels)
  _pixel_size = std::min(max_width, max_height);

  // calculating possible offsets (to center the screen)
  _horizontal_offset =
      (GetScreenWidth() - (_virtual_front_screen_width * _pixel_size)) / 2;
  _vertical_offset =
      (GetScreenHeight() - (_virtual_front_screen_height * _pixel_size)) / 2;
}

void Display::setResolution(Resolution resolution) {
  std::lock_guard lock(_virtual_back_screen_mutex);
  switch (resolution) {
    case Resolution::LOW_RES:
      _virtual_back_screen_height = LOW_RES_VIRTUAL_SCREEN_HEIGHT;
      _virtual_back_screen_width = LOW_RES_VIRTUAL_SCREEN_WIDTH;
      _current_back_resolution = Resolution::LOW_RES;
      break;
    case Resolution::HIGH_RES:
      _virtual_back_screen_height = HIGH_RES_VIRTUAL_SCREEN_HEIGHT;
      _virtual_back_screen_width = HIGH_RES_VIRTUAL_SCREEN_WIDTH;
      _current_back_resolution = Resolution::HIGH_RES;
      break;
  }
}

void Display::drawFrame() {
  if (IsWindowResized() ||
      _current_front_resolution != _next_front_resolution) {
    computeNewPixelSize();
    _current_front_resolution = _next_front_resolution;
  }

  // clang-format off
  BeginDrawing();
    ClearBackground(BLACK);

    // draw pixels
    // pixel y is the line, from top to bottom
    for (int y = 0; y < _virtual_front_screen_height; y++) {
        // pixel x is the column, from left to right
        for (int x = 0; x < _virtual_front_screen_width; x++) {
            if (_virtual_front_screen[y][x]) {
              DrawRectangle(x * _pixel_size + _horizontal_offset,
                            y * _pixel_size + _vertical_offset,
                            _pixel_size, _pixel_size, WHITE);
            }
        }
    }

    // draw screen bounds
    DrawRectangleLines(_horizontal_offset, _vertical_offset,
                       _virtual_front_screen_width * _pixel_size,
                       _virtual_front_screen_height * _pixel_size, GRAY);
  EndDrawing();
  // clang-format on

  // running VBlank interrupt function
  // updating timers and input polling
  _interrupt_handler();
  // and swapping buffers
  {
    std::lock_guard lock(_virtual_back_screen_mutex);
    _virtual_front_screen = _virtual_back_screen;
    _virtual_front_screen_height = _virtual_back_screen_height;
    _virtual_front_screen_width = _virtual_back_screen_width;
    _next_front_resolution = _current_back_resolution;
  }

  // limiting the frame rate to the target FPS
  double current_time = GetTime();
  double wait_time = _target_frame_time - (current_time - _previous_time);
  if (wait_time > 0) {
    WaitTime((float)wait_time);
  }
  _previous_time = GetTime();
}

bool Display::addSprite(const Sprite &sprite, std::uint8_t x, std::uint8_t y) {
  std::lock_guard lock(_virtual_back_screen_mutex);
  // sprite data
  const std::uint8_t *data = sprite.getData();
  std::uint8_t sprite_height = sprite.getHeight();
  std::uint8_t sprite_width = sprite.getWidth();

  // collision flag
  bool collision = false;

  // drawing sprite
  std::uint8_t data_row = 0;
  for (int row = 0; row < sprite_height; row++) {
    std::uint8_t sprite_byte = data[data_row];
    std::uint16_t sprite_word;
    if (sprite_width == 16) {
      sprite_word = (sprite_byte << 8) | data[data_row + 1];
      data_row++;
    }
    for (int column = 0; column < sprite_width; column++) {
      // getting the bit to draw
      bool bit = sprite_width == 8 ? (sprite_byte & (0x80 >> column))
                                   : (sprite_word & (0x8000 >> column));

      // wrapping around the screen
      int wrap_x = (x + column) % _virtual_back_screen_width;
      int wrap_y = (y + row) % _virtual_back_screen_height;

      // checking for collision
      collision = collision || (_virtual_back_screen[wrap_y][wrap_x] && bit);

      // the sprite's line is XoRed with the screen's pixels
      _virtual_back_screen[wrap_y][wrap_x] ^= bit;
    }
    data_row++;
  }

  return collision;
}

void Display::scrollDown(std::uint8_t n) {
  std::lock_guard lock(_virtual_back_screen_mutex);

  // shifting lines
  for (int i = _virtual_back_screen_height - 1; i >= n; i--) {
    _virtual_back_screen[i] = _virtual_back_screen[i - n];
  }

  // clearing lines that were scrolled
  for (int i = 0; i < n; i++) {
    _virtual_back_screen[i].fill(false);
  }
}

void Display::scrollRight(std::uint8_t n) {
  std::lock_guard lock(_virtual_back_screen_mutex);

  for (int i = 0; i < _virtual_back_screen_height; i++) {
    // shifting pixels to the right
    for (int j = _virtual_back_screen_width - 1; j >= n; j--) {
      _virtual_back_screen[i][j] = _virtual_back_screen[i][j - n];
    }

    // clearing pixels that were shifted
    for (int j = 0; j < n; j++) {
      _virtual_back_screen[i][j] = false;
    }
  }
}

void Display::scrollLeft(std::uint8_t n) {
  std::lock_guard lock(_virtual_back_screen_mutex);

  for (int i = 0; i < _virtual_back_screen_height; i++) {
    // shifting pixels to the left
    for (int j = 0; j < _virtual_back_screen_width - n; j++) {
      _virtual_back_screen[i][j] = _virtual_back_screen[i][j + n];
    }

    // clearing pixels that were shifted
    for (int j = _virtual_back_screen_width - n; j < _virtual_back_screen_width;
         j++) {
      _virtual_back_screen[i][j] = false;
    }
  }
}

}  // namespace SuperChip8::System::Graphics
