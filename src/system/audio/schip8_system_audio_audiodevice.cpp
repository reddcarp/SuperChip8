#include <raylib.h>

#include "schip8_error.hpp"
#include "schip8_system_audio_audiodevice.hpp"

namespace SuperChip8::System::Audio {

void AudioDevice::open(std::error_code &ec) {
  InitAudioDevice();
  if (!IsAudioDeviceReady()) {
    ec = Error::FAILED_TO_OPEN_AUDIO_DEVICE;
  }
}
void AudioDevice::close() { CloseAudioDevice(); }

void AudioDevice::registerSound(SoundType type, const std::string &filename,
                                std::error_code &ec) {
  Sound sound = LoadSound(filename.c_str());
  if (sound.frameCount == 0) {
    ec = Error::FAILED_TO_LOAD_SOUND;
    return;
  }
  _sounds[type] = sound;
}

void AudioDevice::playSound(SoundType type, std::error_code &ec) {
  if (!_sounds.contains(type)) {
    ec = Error::SOUND_NOT_FOUND;
    return;
  }
  PlaySound(_sounds[type]);
}

void AudioDevice::stopSound(SoundType type, std::error_code &ec) {
  if (!_sounds.contains(type)) {
    ec = Error::SOUND_NOT_FOUND;
    return;
  }
  StopSound(_sounds[type]);
}

}  // namespace SuperChip8::System::Audio
