#include "schip8_system_input_keyboard.hpp"

namespace SuperChip8::System::Input {

bool Keyboard::isKeyDown(Key key) { return IsKeyDown(_keyMap.at(key)); }

bool Keyboard::isKeyReleased(Key key) { return IsKeyReleased(_keyMap.at(key)); }

}  // namespace SuperChip8::System::Input