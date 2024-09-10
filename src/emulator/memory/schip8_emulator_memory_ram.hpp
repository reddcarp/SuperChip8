#ifndef SUPERCHIP8_EMULATOR_MEMORY_RAM_HPP
#define SUPERCHIP8_EMULATOR_MEMORY_RAM_HPP

#include <array>
#include <cstdint>
#include <system_error>

namespace SuperChip8::Emulator::Memory {

/** RAM Representation
 * +-----------------+ => 0xFFF (4095) End of Chip-8 RAM
 * |                 |
 * |                 |
 * |                 |
 * |                 |
 * |                 |
 * | 0x200 to 0xFFF  | [Where we store the Schip-8 program
 * | SChip-8 program |  Read from file]
 * |                 |
 * |                 |
 * |                 |
 * |                 |
 * |                 |
 * +-----------------+ => 0x200 (512) Start of SChip-8 programs
 * | 0x000 to 0x1FF  |
 * |  Reserved for   | [Where we store the fontset]
 * | the interpreter |
 * +-----------------+ => 0x000 (0) Start of SChip-8 RAM
 */

constexpr std::uint16_t RAM_SIZE = 4096;
constexpr std::uint16_t ROM_START = 0x200;

class RAM {
 public:
  /// @brief Zeroes out the memory (0x000 to 0xFFF)
  void clear();

  /// @brief Zeroes out the program memory (0x200 to 0xFFF)
  void clearProgram();

  /// @brief Load data into memory
  /// @param data Pointer to the data
  /// @param size Size of the data (in bytes)
  /// @param offset Offset in RAM to start writing the data
  /// @param ec Error::OUT_OF_RANGE
  ///
  /// - If the data cannot fit in the memory
  void loadData(const std::uint8_t *data, std::size_t size,
                std::uint16_t offset, std::error_code &ec);

  /// @brief Read a byte from memory
  /// @param address Address to read from
  /// @param ec Error::OUT_OF_RANGE
  ///
  /// - If the address is beyond the memory size
  /// @return the byte at the address
  std::uint8_t readByte(std::uint16_t address, std::error_code &ec) const;

  /// @brief Read a word from memory
  /// @param address Address to read from
  /// @param ec Error::OUT_OF_RANGE
  ///
  /// - If the address is beyond the memory size
  /// @return the word at the address
  std::uint16_t readWord(std::uint16_t address, std::error_code &ec) const;

  /// @brief Write a byte to memory
  /// @param address Address to write to
  /// @param value Value to write
  /// @param ec Error::OUT_OF_RANGE
  ///
  /// - If the data cannot fit in the memory
  void writeByte(std::uint16_t address, std::uint8_t value,
                 std::error_code &ec);

  /// @brief Get a pointer to the byte at the address
  /// @param address Address to get the pointer from
  /// @param ec Error::OUT_OF_RANGE
  ///
  /// - If the address is beyond the memory size
  /// @return Pointer to the byte at the address (nullptr if error)
  std::uint8_t *getBytePointer(std::uint16_t address, std::error_code &ec);

  /// @brief Check if the address and size are within the memory bounds
  /// @param address Address to check
  /// @param size Size to check
  /// @return `true` if the address and size are within the memory bounds
  bool isSizeReadable(std::uint16_t address, std::size_t size) const;

 private:
  std::array<std::uint8_t, RAM_SIZE> _memory = {0};
};

}  // namespace SuperChip8::Emulator::Memory

#endif  // SUPERCHIP8_EMULATOR_MEMORY_RAM_HPP