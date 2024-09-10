#ifndef SUPERCHIP8_SYSTEM_GRAPHICS_SPRITE_HPP
#define SUPERCHIP8_SYSTEM_GRAPHICS_SPRITE_HPP

#include <cstdint>

namespace SuperChip8::System::Graphics {

constexpr std::uint8_t SPRITE_WIDTH_LOW_RES = 8;
constexpr std::uint8_t SPRITE_WIDTH_HIGH_RES = 16;

class Sprite {
 public:
  Sprite(std::uint8_t height, std::uint8_t width, const std::uint8_t* data)
      : _height(height), _width(width), _data(data) {}

  std::uint8_t getHeight() const { return _height; }
  std::uint8_t getWidth() const { return _width; }
  const std::uint8_t* getData() const { return _data; }

 private:
  const std::uint8_t _height;
  const std::uint8_t _width;
  const std::uint8_t* _data;
};
}  // namespace SuperChip8::System::Graphics

#endif  // SUPERCHIP8_SYSTEM_GRAPHICS_SPRITE_HPP