#ifndef SUPERCHIP8_EMULATOR_KEYMAPPING_HPP
#define SUPERCHIP8_EMULATOR_KEYMAPPING_HPP

#include "schip8_system_input_key.hpp"

#include <cstdint>
#include <map>

namespace SuperChip8::Emulator {

constexpr std::uint8_t KEY_MAPPED_COUNT = 16;

/** Key mapping for the SuperChip-8 keypad to the modern computer keyboard
 *
 *  +-------------------+  +-------------------+
 *  | Original keyboard |  |  Modern keyboard  |
 *  +-------------------+  +-------------------+
 *  |     +-+-+-+-+     |  |     +-+-+-+-+     |
 *  |     |1|2|3|C|     |  |     |1|2|3|4|     |
 *  |     +-+-+-+-+     |  |     +-+-+-+-+     |
 *  |     |4|5|6|D|     |  |     |Q|W|E|R|     |
 *  |     +-+-+-+-+     |  |     +-+-+-+-+     |
 *  |     |7|8|9|E|     |  |     |A|S|D|F|     |
 *  |     +-+-+-+-+     |  |     +-+-+-+-+     |
 *  |     |A|0|B|F|     |  |     |Z|X|C|V|     |
 *  |     +-+-+-+-+     |  |     +-+-+-+-+     |
 *  +-------------------+  +-------------------+
 */
const std::map<std::uint8_t, SuperChip8::System::Input::Key> key_map = {
    {0x1, SuperChip8::System::Input::Key::ONE},
    {0x2, SuperChip8::System::Input::Key::TWO},
    {0x3, SuperChip8::System::Input::Key::THREE},
    {0xC, SuperChip8::System::Input::Key::FOUR},
    {0x4, SuperChip8::System::Input::Key::Q},
    {0x5, SuperChip8::System::Input::Key::W},
    {0x6, SuperChip8::System::Input::Key::E},
    {0xD, SuperChip8::System::Input::Key::R},
    {0x7, SuperChip8::System::Input::Key::A},
    {0x8, SuperChip8::System::Input::Key::S},
    {0x9, SuperChip8::System::Input::Key::D},
    {0xE, SuperChip8::System::Input::Key::F},
    {0xA, SuperChip8::System::Input::Key::Z},
    {0x0, SuperChip8::System::Input::Key::X},
    {0xB, SuperChip8::System::Input::Key::C},
    {0xF, SuperChip8::System::Input::Key::V},
};

}  // namespace SuperChip8::Emulator

#endif  // SUPERCHIP8_EMULATOR_KEYMAPPING_HPP