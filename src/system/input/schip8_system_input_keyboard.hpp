#ifndef SUPERCHIP8_SYSTEM_INPUT_KEYBOARD_HPP
#define SUPERCHIP8_SYSTEM_INPUT_KEYBOARD_HPP

#include "schip8_system_input_key.hpp"

#include <map>
#include <raylib.h>

namespace SuperChip8::System::Input {

class Keyboard {
 public:
  bool isKeyDown(Key key);
  bool isKeyReleased(Key key);

 private:

  /// @brief Map of SuperChip-8 keys to Raylib keys
  const std::map<Key, int> _keyMap = {{Key::RIGHT, KeyboardKey::KEY_RIGHT},
                                      {Key::ONE, KeyboardKey::KEY_ONE},
                                      {Key::TWO, KeyboardKey::KEY_TWO},
                                      {Key::THREE, KeyboardKey::KEY_THREE},
                                      {Key::FOUR, KeyboardKey::KEY_FOUR},
                                      {Key::Q, KeyboardKey::KEY_Q},
                                      {Key::W, KeyboardKey::KEY_W},
                                      {Key::E, KeyboardKey::KEY_E},
                                      {Key::R, KeyboardKey::KEY_R},
                                      {Key::A, KeyboardKey::KEY_A},
                                      {Key::S, KeyboardKey::KEY_S},
                                      {Key::D, KeyboardKey::KEY_D},
                                      {Key::F, KeyboardKey::KEY_F},
                                      {Key::Z, KeyboardKey::KEY_Z},
                                      {Key::X, KeyboardKey::KEY_X},
                                      {Key::C, KeyboardKey::KEY_C},
                                      {Key::V, KeyboardKey::KEY_V}};
};

}  // namespace SuperChip8::System::Input

#endif  // SUPERCHIP8_SYSTEM_INPUT_KEYBOARD_HPP