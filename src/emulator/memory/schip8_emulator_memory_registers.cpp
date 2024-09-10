#include "schip8_emulator_memory_registers.hpp"
#include "schip8_error.hpp"

namespace SuperChip8::Emulator::Memory {

void Registers::clear() {
  V.fill(0);
  RPL.fill(0);
  I = 0;
  delay_timer = 0;
  sound_timer = 0;
  pc = ROM_START;
  sp = 0;
  stack.fill(0);
}

void Registers::pushToStack(std::uint16_t value, std::error_code &ec) {
  if (sp >= STACK_SIZE) {
    ec = Error::STACK_OVERFLOW;
    return;
  }

  stack[sp] = value;
  sp += 1;
}

std::uint16_t Registers::popFromStack(std::error_code &ec) {
  if (sp == 0) {
    ec = Error::STACK_UNDERFLOW;
    return 0;
  }

  sp -= 1;
  return stack[sp];
}

}  // namespace SuperChip8::Emulator::Memory
