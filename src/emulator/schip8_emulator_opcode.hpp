#ifndef SUPERCHIP8_EMULATOR_OPCODE_HPP
#define SUPERCHIP8_EMULATOR_OPCODE_HPP

#include <cstdint>

namespace SuperChip8::Emulator {

constexpr std::uint16_t MASK_CATEGORY = 0xF000;
constexpr std::uint16_t MASK_X = 0x0F00;
constexpr std::uint16_t MASK_Y = 0x00F0;
constexpr std::uint16_t MASK_N = 0x000F;
constexpr std::uint16_t MASK_NN = 0x00FF;
constexpr std::uint16_t MASK_NNN = 0x0FFF;

/// @brief Opcode class, represents a SuperChip-8 opcode
class Opcode {
 public:
  explicit Opcode(std::uint16_t raw) : raw(raw) {
    category = (raw & MASK_CATEGORY) >> 12;
    X = (raw & MASK_X) >> 8;
    Y = (raw & MASK_Y) >> 4;
    N = raw & MASK_N;
    NN = raw & MASK_NN;
    NNN = raw & MASK_NNN;
  }

  std::uint16_t raw;
  std::uint8_t category;
  std::uint8_t X;
  std::uint8_t Y;
  std::uint8_t N;
  std::uint8_t NN;
  std::uint16_t NNN;
};

}  // namespace SuperChip8::Emulator

#endif  // SUPERCHIP8_EMULATOR_OPCODE_HPP