#ifndef SUPERCHIP8_EMULATOR_MEMORY_REGISTERS_HPP
#define SUPERCHIP8_EMULATOR_MEMORY_REGISTERS_HPP

#include "schip8_emulator_memory_ram.hpp"

#include <array>
#include <atomic>
#include <cstdint>

namespace SuperChip8::Emulator::Memory {

constexpr std::uint8_t REGISTERS = 16;
// Max 16 levels of nested subroutines
constexpr std::uint8_t STACK_SIZE = 16;

class Registers {
 public:
  /// @brief Set all registers, I, delay_timer, sound_timer to 0 and pc to
  /// ROM_START
  void clear();

  /// @brief Push a value to the stack
  /// @param value Value to push
  /// @param ec Error::STACK_OVERFLOW
  ///
  /// - If the stack is full
  void pushToStack(std::uint16_t value, std::error_code &ec);

  /// @brief Pop a value from the stack
  /// @param ec Error::STACK_UNDERFLOW
  ///
  /// - If the stack is empty
  /// @return the value popped from the stack
  std::uint16_t popFromStack(std::error_code &ec);

  // General purpose registers: V0 to VF [VF is used as a flag]
  std::array<std::uint8_t, REGISTERS> V = {0};
  // RPL flags
  std::array<std::uint8_t, REGISTERS> RPL = {0};
  // Index register
  std::uint16_t I = 0;
  // decrement at 60Hz until it reaches 0
  std::atomic<std::uint8_t> delay_timer = 0;
  // decrement at 60Hz, beeping until it reaches 0
  std::atomic<std::uint8_t> sound_timer = 0;

  // Program counter [address of the current instruction]
  // an instruction is 2 bytes long => [sp, sp+1]
  std::uint16_t pc = ROM_START;
  // Stack pointer [address of the top of the stack]
  std::uint8_t sp = 0;
  // Stack [used to store the address that the interpreter should return to
  // after finishing a subroutine]
  std::array<std::uint16_t, STACK_SIZE> stack = {0};
};

}  // namespace SuperChip8::Emulator::Memory

#endif  // SUPERCHIP8_EMULATOR_MEMORY_REGISTERS_HPP