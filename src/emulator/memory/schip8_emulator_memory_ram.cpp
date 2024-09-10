#include "schip8_emulator_memory_ram.hpp"
#include "schip8_error.hpp"

namespace SuperChip8::Emulator::Memory {

void RAM::clear() { _memory.fill(0); }

void RAM::clearProgram() {
  std::fill(_memory.begin() + ROM_START, _memory.end(), 0);
}

void RAM::loadData(const std::uint8_t *data, std::size_t size,
                   std::uint16_t offset, std::error_code &ec) {
  if (offset + size > RAM_SIZE) {
    ec = Error::OUT_OF_RANGE;
    return;
  }

  std::copy(data, data + size, _memory.begin() + offset);
}

std::uint8_t RAM::readByte(std::uint16_t address, std::error_code &ec) const {
  if (address >= RAM_SIZE) {
    ec = Error::OUT_OF_RANGE;
    return 0;
  }

  return _memory[address];
}

std::uint16_t RAM::readWord(std::uint16_t address, std::error_code &ec) const {
  if (address + 1 >= RAM_SIZE) {
    ec = Error::OUT_OF_RANGE;
    return 0;
  }

  return (_memory[address] << 8) | _memory[address + 1];
}

void RAM::writeByte(std::uint16_t address, std::uint8_t value,
                    std::error_code &ec) {
  if (address >= RAM_SIZE) {
    ec = Error::OUT_OF_RANGE;
    return;
  }

  _memory[address] = value;
}

std::uint8_t *RAM::getBytePointer(std::uint16_t address, std::error_code &ec) {
  if (address >= RAM_SIZE) {
    ec = Error::OUT_OF_RANGE;
    return nullptr;
  }

  return &_memory[address];
}

bool RAM::isSizeReadable(std::uint16_t address, std::size_t size) const {
  return (address + size) < RAM_SIZE;
}

}  // namespace SuperChip8::Emulator::Memory
