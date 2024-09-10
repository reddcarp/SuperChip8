#ifndef SUPERCHIP8_EMULATOR_VM_HPP
#define SUPERCHIP8_EMULATOR_VM_HPP

#include "schip8_emulator_memory_ram.hpp"
#include "schip8_emulator_memory_registers.hpp"
#include "schip8_emulator_opcode.hpp"
#include "schip8_system_audio_audiodevice.hpp"
#include "schip8_system_graphics_display.hpp"
#include "schip8_system_input_keyboard.hpp"

#include <atomic>
#include <condition_variable>
#include <random>
#include <system_error>
#include <thread>

namespace SuperChip8::Emulator {

/// @brief SuperChip-8 Virtual Machine, responsible for running the emulator
/// (CPU and external devices)
class VM {
 public:
  /// @param target_cycles Target CPU cycles per frame
  VM(std::uint16_t target_cycles);

  /// @brief Initialize the VM, load the program and start running
  ///
  /// @details This function initializes the memory, loads the fontset, loads
  /// the program, initializes the audio device, creates the display window, and
  /// starts the CPU thread.
  /// @param program_path Path to the program to load
  /// @param ec error_code
  void turnOn(const std::string &program_path, std::error_code &ec);

  /// @brief Turn off the VM and clean up resources
  /// @details This function stops the CPU thread, closes the audio device, and
  /// closes the display window.
  void turnOff();

  /// @brief Load a program into memory
  /// @param program_path Path to the program to load
  /// @param ec error_code
  ///
  /// - Error::FILE_NOT_FOUND | If the file cannot be opened
  ///
  /// - Error::OUT_OF_RANGE | If the program is too large to fit in memory
  void loadProgram(const std::string &program_path, std::error_code &ec);

 private:
  /// @brief Main CPU loop
  void run(std::error_code &ec);

  /// @brief Draw loop
  void drawLoop();

  void executeOpcode(const Opcode &opcode, std::error_code &ec);

  void executeCategory0(const Opcode &opcode, std::error_code &ec);
  void executeCategory1(const Opcode &opcode, std::error_code &ec);
  void executeCategory2(const Opcode &opcode, std::error_code &ec);
  void executeCategory3(const Opcode &opcode, std::error_code &ec);
  void executeCategory4(const Opcode &opcode, std::error_code &ec);
  void executeCategory5(const Opcode &opcode, std::error_code &ec);
  void executeCategory6(const Opcode &opcode, std::error_code &ec);
  void executeCategory7(const Opcode &opcode, std::error_code &ec);
  void executeCategory8(const Opcode &opcode, std::error_code &ec);
  void executeCategory9(const Opcode &opcode, std::error_code &ec);
  void executeCategoryA(const Opcode &opcode, std::error_code &ec);
  void executeCategoryB(const Opcode &opcode, std::error_code &ec);
  void executeCategoryC(const Opcode &opcode, std::error_code &ec);
  void executeCategoryD(const Opcode &opcode, std::error_code &ec);
  void executeCategoryE(const Opcode &opcode, std::error_code &ec);
  void executeCategoryF(const Opcode &opcode, std::error_code &ec);

  Memory::RAM _ram;
  Memory::Registers _registers;
  System::Audio::AudioDevice _audioDevice;
  System::Graphics::Display _display;
  System::Input::Keyboard _keyboard;

  // Random number generator
  std::random_device _rd;
  std::mt19937 _gen;
  std::uniform_int_distribution<std::uint16_t> _dist;

  // vm state
  std::atomic<bool> _running = false;
  std::atomic<bool> _program_loaded = false;

  // For Vblank [executed between each frame] (ie: 60Hz)
  void handleVBlankInterrupt();
  void updateTimers(std::error_code &ec);
  void processInput();

  // array used to store which keys are pressed
  std::array<bool, 16> _keyPressed = {false};

  // for CPU cycles
  std::atomic<std::uint16_t> _cycle = 0;
  std::atomic<std::uint16_t> _target_cycles;
  std::jthread _cpu_thread;
  std::condition_variable _cpu_sleep_cv;
};

}  // namespace SuperChip8::Emulator

#endif  // SUPERCHIP8_EMULATOR_VM_HPP