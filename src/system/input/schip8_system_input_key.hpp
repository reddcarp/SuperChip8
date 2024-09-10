#ifndef SUPERCHIP8_SYSTEM_INPUT_KEY_HPP
#define SUPERCHIP8_SYSTEM_INPUT_KEY_HPP

#include <cstdint>

namespace SuperChip8::System::Input {

constexpr uint8_t KEY_COUNT = 16;

/// @brief SuperChip-8 keypad keys
enum class Key {
  RIGHT,
  ONE,
  TWO,
  THREE,
  FOUR,
  Q,
  W,
  E,
  R,
  A,
  S,
  D,
  F,
  Z,
  X,
  C,
  V
};

}  // namespace SuperChip8::System::Input

#endif  // SUPERCHIP8_SYSTEM_INPUT_KEY_HPP