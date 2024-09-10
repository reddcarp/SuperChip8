#ifndef SUPERCHIP8_SYSTEM_AUDIO_AUDIODEVICE_HPP
#define SUPERCHIP8_SYSTEM_AUDIO_AUDIODEVICE_HPP

#include <map>
#include <raylib.h>
#include <system_error>

#include "schip8_system_audio_soundtype.hpp"

namespace SuperChip8::System::Audio {

class AudioDevice {
 public:
  void open(std::error_code &ec);
  void close();

  void registerSound(SoundType type, const std::string &filename,
                     std::error_code &ec);
  void playSound(SoundType type, std::error_code &ec);
  void stopSound(SoundType type, std::error_code &ec);

 private:
  std::map<SoundType, Sound> _sounds;
};

}  // namespace SuperChip8::System::Audio

#endif  // SUPERCHIP8_SYSTEM_AUDIO_AUDIODEVICE_HPP