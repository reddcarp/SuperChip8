#include "schip8_emulator_fontset.hpp"
#include "schip8_emulator_keymapping.hpp"
#include "schip8_emulator_vm.hpp"
#include "schip8_error.hpp"
#include "schip8_system_graphics_sprite.hpp"

#include <chrono>
#include <fstream>
#include <system_error>
#include <iostream>

namespace SuperChip8::Emulator {

VM::VM(std::uint16_t target_cycles)
    : _display([this]() { handleVBlankInterrupt(); }),
      _gen(_rd()),
      _dist(0, 255),
      _target_cycles(target_cycles) {
  // initialize the random number generator
  _gen.seed(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
}

void VM::turnOn(const std::string &program_path, std::error_code &ec) {
  // Initialize the memory
  _registers.clear();
  _ram.clear();

  // load fontset
  _ram.loadData(FONTSET_LOW_RES.data(), FONT_SIZE_LOW_RES, 0, ec);
  _ram.loadData(FONTSET_HIGH_RES.data(), FONT_SIZE_HIGH_RES, FONT_SIZE_LOW_RES,
                ec);
  if (ec) {
    return;
  }

  // Initialize the audio device
  _audioDevice.open(ec);
  if (ec) {
    return;
  }
  // SOUND_FILE_PATH is defined in CMakeLists.txt
  _audioDevice.registerSound(SoundType::BEEP,
                             std::string(SOUND_FILE_PATH) + "/beep.wav", ec);
  if (ec) {
    return;
  }

  // Initialize the display
  _display.clear();
  _display.createWindow("SuperChiP-8", ec);
  if (ec) {
    return;
  }

  loadProgram(program_path, ec);
  if (ec) {
    return;
  }

  _running.store(true);
  _cpu_thread = std::jthread(&VM::run, this, std::ref(ec));

  // Start the draw loop [must be executed in the main thread]
  drawLoop();
}

void VM::turnOff() {
  _running.store(false);
  _cpu_sleep_cv.notify_one();

  _cpu_thread.request_stop();
  _cpu_thread.join();

  _audioDevice.close();
  _display.closeWindow();
}

void VM::loadProgram(const std::string &program_path, std::error_code &ec) {
  _ram.clearProgram();
  std::ifstream file(program_path, std::ios::binary | std::ios::ate);
  if (!file.is_open()) {
    ec = Error::FILE_NOT_FOUND;
    return;
  }
  std::size_t size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<std::uint8_t> buffer(size);
  file.read(reinterpret_cast<char *>(buffer.data()), size);

  _ram.loadData(buffer.data(), size, Memory::ROM_START, ec);
  if (ec) {
    return;
  }

  _program_loaded.store(true);
}

void VM::run(std::error_code &ec) {
  while (!_program_loaded.load() && _running.load()) {
    // wait for the program to be loaded
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  while (_running.load() && _program_loaded.load()) {
    ec.clear();

    Opcode opcode(_ram.readWord(_registers.pc, ec));
    // instructions are 2 bytes long
    _registers.pc += 2;
    if (ec) {
      _running.store(false);
      return;
    }

    executeOpcode(opcode, ec);
    if (ec) {
      _running.store(false);
      return;
    }

    _cycle++;
    if (_cycle >= _target_cycles && _running.load()) {
      std::mutex m;
      std::unique_lock lk(m);
      _cpu_sleep_cv.wait(
          lk, [this] { return !_running.load() || _cycle.load() == 0; });
    }
  }
}

void VM::executeOpcode(const Opcode &opcode, std::error_code &ec) {
  switch (opcode.category) {
    case 0x0:
      executeCategory0(opcode, ec);
      break;
    case 0x1:
      executeCategory1(opcode, ec);
      break;
    case 0x2:
      executeCategory2(opcode, ec);
      break;
    case 0x3:
      executeCategory3(opcode, ec);
      break;
    case 0x4:
      executeCategory4(opcode, ec);
      break;
    case 0x5:
      executeCategory5(opcode, ec);
      break;
    case 0x6:
      executeCategory6(opcode, ec);
      break;
    case 0x7:
      executeCategory7(opcode, ec);
      break;
    case 0x8:
      executeCategory8(opcode, ec);
      break;
    case 0x9:
      executeCategory9(opcode, ec);
      break;
    case 0xA:
      executeCategoryA(opcode, ec);
      break;
    case 0xB:
      executeCategoryB(opcode, ec);
      break;
    case 0xC:
      executeCategoryC(opcode, ec);
      break;
    case 0xD:
      executeCategoryD(opcode, ec);
      break;
    case 0xE:
      executeCategoryE(opcode, ec);
      break;
    case 0xF:
      executeCategoryF(opcode, ec);
      break;
    default:
      ec = Error::UNKNOWN_OPCODE;
      std::cerr << "Unknown opcode: " << std::hex << opcode.raw << std::endl;
      break;
  }
}

void VM::executeCategory0(const Opcode &opcode, std::error_code &ec) {
  switch (opcode.Y) {
    case 0xC:
      // SCROLL_DOWN: 00CN: Scroll the display N pixels down
      _display.scrollDown(opcode.N);
      break;
    case 0xE: {
      switch (opcode.N) {
        case 0x0:
          // CLEAR: 00E0: Clear the screen
          _display.clear();
          break;
        case 0xE:
          // RET: 00EE: Return from a subroutine
          _registers.pc = _registers.popFromStack(ec);
          break;
        default:
          ec = Error::UNKNOWN_OPCODE;
          std::cerr << "Unknown opcode: " << std::setw(4) << std::setfill('0')
                    << std::hex << opcode.raw << std::endl;
          break;
      }
      break;
    }
    case 0xF: {
      switch (opcode.N) {
        case 0xB:
          // SCROLL_RIGHT: 00FB: Scroll the display 4 pixels to the right
          _display.scrollRight(4);
          break;
        case 0xC:
          // SCROLL_LEFT: 00FC: Scroll the display 4 pixels to the left
          _display.scrollLeft(4);
          break;
        case 0xD:
          // EXIT: 00FD: Exit the emulator
          _running.store(false);
          break;
        case 0xE:
          // LOW: 00FE: Set the screen resolution to 64x32
          _display.setResolution(
              SuperChip8::System::Graphics::Display::Resolution::LOW_RES);
          break;
        case 0xF:
          // HIGH: 00FF: Set the screen resolution to 128x64
          _display.setResolution(
              SuperChip8::System::Graphics::Display::Resolution::HIGH_RES);
          break;
        default:
          ec = Error::UNKNOWN_OPCODE;
          std::cerr << "Unknown opcode: " << std::setw(4) << std::setfill('0')
                    << std::hex << opcode.raw << std::endl;
          break;
      }
      break;
    }
    default:
      ec = Error::UNKNOWN_OPCODE;
      std::cerr << "Unknown opcode: " << std::setw(4) << std::setfill('0')
                << std::hex << opcode.raw << std::endl;
      break;
  }
}

void VM::executeCategory1(const Opcode &opcode, std::error_code &ec) {
  // JMP: 1NNN: Jump to address NNN
  _registers.pc = opcode.NNN;
}

void VM::executeCategory2(const Opcode &opcode, std::error_code &ec) {
  // CALL: 2NNN: Call subroutine at NNN
  _registers.pushToStack(_registers.pc, ec);
  _registers.pc = opcode.NNN;
}

void VM::executeCategory3(const Opcode &opcode, std::error_code &ec) {
  // SKIP_EQ: 3XNN: Skip the next instruction if VX == NN
  if (_registers.V[opcode.X] == opcode.NN) {
    _registers.pc += 2;
  }
}

void VM::executeCategory4(const Opcode &opcode, std::error_code &ec) {
  // SKIP_NEQ: 4XNN: Skip the next instruction if VX != NN
  if (_registers.V[opcode.X] != opcode.NN) {
    _registers.pc += 2;
  }
}

void VM::executeCategory5(const Opcode &opcode, std::error_code &ec) {
  // SKIP_EQ_REG: 5XY0: Skip the next instruction if VX == VY
  if (_registers.V[opcode.X] == _registers.V[opcode.Y]) {
    _registers.pc += 2;
  }
}

void VM::executeCategory6(const Opcode &opcode, std::error_code &ec) {
  // SET: 6XNN: Set VX to NN
  _registers.V[opcode.X] = opcode.NN;
}

void VM::executeCategory7(const Opcode &opcode, std::error_code &ec) {
  // ADD: 7XNN: Add NN to VX
  _registers.V[opcode.X] += opcode.NN;
}

void VM::executeCategory8(const Opcode &opcode, std::error_code &ec) {
  switch (opcode.N) {
    case 0x0:
      // SET: 8XY0: VX = VY
      _registers.V[opcode.X] = _registers.V[opcode.Y];
      break;
    case 0x1:
      // OR: 8XY1: VX |= VY
      _registers.V[opcode.X] |= _registers.V[opcode.Y];
      break;
    case 0x2:
      // AND: 8XY2: VX &= VY
      _registers.V[opcode.X] &= _registers.V[opcode.Y];
      break;
    case 0x3:
      // XOR: 8XY3: VX ^= VY
      _registers.V[opcode.X] ^= _registers.V[opcode.Y];
      break;
    case 0x4: {
      // ADD_REG: 8XY4: VX = VX + VY, VF is set to 1 if overflow;
      // here in case VX or VY is VF
      std::uint8_t overflow =
          (_registers.V[opcode.X] + _registers.V[opcode.Y]) >> 8;
      _registers.V[opcode.X] += _registers.V[opcode.Y];
      _registers.V[0xF] = overflow;
      break;
    }
    case 0x5: {
      // SUB_REG: 8XY5: VX = VX - VY, VF is set to 0 if borrow
      std::uint8_t borrow =
          _registers.V[opcode.X] >= _registers.V[opcode.Y] ? 1 : 0;
      _registers.V[opcode.X] -= _registers.V[opcode.Y];
      _registers.V[0xF] = borrow;
      break;
    }
    case 0x6: {
      // SHR: 8XY6: VX >>= 1, VF is set to the least significant bit of VX
      std::uint8_t lsb = _registers.V[opcode.X] & 0x1;
      _registers.V[opcode.X] >>= 1;
      _registers.V[0xF] = lsb;
      break;
    }
    case 0x7: {
      // SUBN_REG: 8XY7: VX = VY - VX, VF is set to 0 if borrow
      std::uint8_t borrow =
          _registers.V[opcode.Y] >= _registers.V[opcode.X] ? 1 : 0;
      _registers.V[opcode.X] = _registers.V[opcode.Y] - _registers.V[opcode.X];
      _registers.V[0xF] = borrow;
      break;
    }
    case 0xE: {
      // SHL: 8XYE: VX <<= 1, VF is set to the most significant bit of VX
      std::uint8_t msb = (_registers.V[opcode.X] & 0x80) >> 7;
      _registers.V[opcode.X] <<= 1;
      _registers.V[0xF] = msb;
      break;
    }
    default:
      ec = Error::UNKNOWN_OPCODE;
      std::cerr << "Unknown opcode: " << std::setw(4) << std::setfill('0')
                << std::hex << opcode.raw << std::endl;
      break;
  }
}

void VM::executeCategory9(const Opcode &opcode, std::error_code &ec) {
  // SKIP_NEQ_REG: 9XY0: Skip the next instruction if VX != VY
  if (_registers.V[opcode.X] != _registers.V[opcode.Y]) {
    _registers.pc += 2;
  }
}

void VM::executeCategoryA(const Opcode &opcode, std::error_code &ec) {
  // SET_I: ANNN: Set I to NNN
  _registers.I = opcode.NNN;
}

void VM::executeCategoryB(const Opcode &opcode, std::error_code &ec) {
  // JMP_V0: BNNN: Jump to address NNN + VX
  _registers.pc = opcode.NNN + _registers.V[opcode.X];
}

void VM::executeCategoryC(const Opcode &opcode, std::error_code &ec) {
  // RAND: CXNN: Set VX to a random number AND NN
  _registers.V[opcode.X] = _dist(_gen) & opcode.NN;
}

void VM::executeCategoryD(const Opcode &opcode, std::error_code &ec) {
  std::uint8_t x = _registers.V[opcode.X];
  std::uint8_t y = _registers.V[opcode.Y];
  const std::uint8_t *sprite_data = _ram.getBytePointer(_registers.I, ec);
  if (ec) {
    return;
  }
  // DISP: DXYN: Draw a sprite at (VX, VY) with width 8 and height N
  std::uint8_t sprite_height = opcode.N;
  std::uint8_t sprite_width = 8;

  // DISP: DXY0: Draw a sprite at (VX, VY) with 16x16 dimensions
  if (opcode.N == 0x0) {
    sprite_height = 16;
    sprite_width = 16;
  }

  // check if all the sprite data is readable (ie: in RAM bounds)
  if (!_ram.isSizeReadable(_registers.I, sprite_height * sprite_width)) {
    ec = Error::OUT_OF_RANGE;
    return;
  }

  SuperChip8::System::Graphics::Sprite sprite(sprite_height, sprite_width,
                                              sprite_data);
  // indicates if a collision occurred
  _registers.V[0xF] = _display.addSprite(sprite, x, y);
}

void VM::executeCategoryE(const Opcode &opcode, std::error_code &ec) {
  switch (opcode.NN) {
    case 0x9E:
      // SKIP_KEY: EX9E: Skip next instruction if the key with the value of
      // VX is pressed
      if (_keyPressed[_registers.V[opcode.X]]) {
        _registers.pc += 2;
      }
      break;
    case 0xA1:
      // SKIP_NKEY: EXA1: Skip next instruction if the key with the value
      // of VX is not pressed
      if (!_keyPressed[_registers.V[opcode.X]]) {
        _registers.pc += 2;
      }
      break;
    default:
      ec = Error::UNKNOWN_OPCODE;
      std::cerr << "Unknown opcode: " << std::setw(4) << std::setfill('0')
                << std::hex << opcode.raw << std::endl;
      break;
  }
}

void VM::executeCategoryF(const Opcode &opcode, std::error_code &ec) {
  switch (opcode.NN) {
    case 0x07:
      // GET_DELAY: FX07: Set VX to the value of the delay timer
      _registers.V[opcode.X] = _registers.delay_timer;
      break;
    case 0x0A: {
      // WAIT_KEY: FX0A: Wait for a key press, store the value of the key in VX
      bool key_pressed = false;
      while (!key_pressed && _running.load()) {
        for (std::uint8_t i = 0; i < Memory::REGISTERS; i++) {
          if (_keyPressed[i]) {
            while (_keyPressed[i] && _running.load()) {
              // wait for the key to be released
            }
            _registers.V[opcode.X] = i;
            key_pressed = true;
            break;
          }
        }
      }
      break;
    }
    case 0x15:
      // SET_DELAY: FX15: Set the delay timer to VX
      _registers.delay_timer = _registers.V[opcode.X];
      break;
    case 0x18:
      // SET_SOUND: FX18: Set the sound timer to VX
      _registers.sound_timer = _registers.V[opcode.X];
      break;
    case 0x1E:
      // ADD_I: FX1E: I = I + VX
      _registers.I += _registers.V[opcode.X];
      break;
    case 0x29:
      // SET_FONT: FX29: Set I to the location of the sprite for the font
      // character in VX of line height 5 (low resolution)
      _registers.I = (_registers.V[opcode.X] % 0x10) * FONT_HEIGHT_LOW_RES;
      break;
    case 0x30:
      // SET_FONT: FX30: Set I to the location of the sprite for the font
      // character in VX of line height 10 (high resolution)
      _registers.I = ((_registers.V[opcode.X] % 0x10) * FONT_HEIGHT_HIGH_RES +
                      FONT_SIZE_LOW_RES);
      break;
    case 0x33:
      // BCD: FX33: Store BCD representation of VX in memory locations I, I+1,
      // I+2
      _ram.writeByte(_registers.I, _registers.V[opcode.X] / 100, ec);
      _ram.writeByte(_registers.I + 1, (_registers.V[opcode.X] / 10) % 10, ec);
      _ram.writeByte(_registers.I + 2, _registers.V[opcode.X] % 10, ec);
      break;
    case 0x55:
      // STORE_REG: FX55: Store V0 to VX in memory starting at I
      for (std::uint8_t i = 0; i <= opcode.X; i++) {
        _ram.writeByte(_registers.I + i, _registers.V[i], ec);
      }
      break;
    case 0x65:
      // LD_REG: FX65: Load V0 to VX from memory starting at I
      for (std::uint8_t i = 0; i <= opcode.X; i++) {
        _registers.V[i] = _ram.readByte(_registers.I + i, ec);
      }
      break;
    case 0x75:
      // SAVE_REG: FX75: Store V0 to VX in the flag register
      for (std::uint8_t i = 0; i <= opcode.X; i++) {
        _registers.RPL[i] = _registers.V[i];
      }
      break;
    case 0x85:
      // LD_REG: FX85: Load V0 to VX from the flag register
      for (std::uint8_t i = 0; i <= opcode.X; i++) {
        _registers.V[i] = _registers.RPL[i];
      }
      break;
    default:
      ec = Error::UNKNOWN_OPCODE;
      std::cerr << "Unknown opcode: " << std::hex << opcode.raw << std::endl;
      break;
  }
}

void VM::handleVBlankInterrupt() {
  std::error_code ec;
  _cycle = 0;
  _cpu_sleep_cv.notify_one();
  updateTimers(ec);
  processInput();
}

void VM::updateTimers(std::error_code &ec) {
  if (_registers.delay_timer > 0) {
    --_registers.delay_timer;
  }

  if (_registers.sound_timer > 0) {
    --_registers.sound_timer;
    if (_registers.sound_timer == 0) {
      _audioDevice.stopSound(SoundType::BEEP, ec);
    } else {
      _audioDevice.playSound(SoundType::BEEP, ec);
    }
  }
}

void VM::processInput() {
  for (std::uint8_t i = 0; i < KEY_MAPPED_COUNT; i++) {
    _keyPressed[i] = _keyboard.isKeyDown(key_map.at(i));
  }
}

void VM::drawLoop() {
  while (!_display.windowShouldClose() && _running.load()) {
    _display.drawFrame();
  }
  _running.store(false);
  _cpu_sleep_cv.notify_one();
}

}  // namespace SuperChip8::Emulator
