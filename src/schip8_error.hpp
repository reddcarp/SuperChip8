#ifndef SUPERCHIP8_ERROR_HPP
#define SUPERCHIP8_ERROR_HPP

#include <system_error>

namespace SuperChip8 {

enum class Error {
  FAILED_TO_OPEN_AUDIO_DEVICE,
  FAILED_TO_LOAD_SOUND,
  SOUND_NOT_FOUND,
  WINDOW_CREATION_ERROR,
  BUFFER_OVERFLOW,
  OUT_OF_RANGE,
  STACK_OVERFLOW,
  STACK_UNDERFLOW,
  FILE_NOT_FOUND,
  UNKNOWN_OPCODE
};

class ErrorCategory : public std::error_category {
  const char* name() const noexcept override { return "SuperChip8 error"; }

  virtual std::string message(int condition) const override {
    switch (static_cast<Error>(condition)) {
      case Error::FAILED_TO_OPEN_AUDIO_DEVICE:
        return "Failed to open audio device";
      case Error::FAILED_TO_LOAD_SOUND:
        return "Failed to load sound";
      case Error::SOUND_NOT_FOUND:
        return "Sound not found";
      case Error::WINDOW_CREATION_ERROR:
        return "Window creation error";
      case Error::BUFFER_OVERFLOW:
        return "Buffer overflow";
      case Error::OUT_OF_RANGE:
        return "Out of range";
      case Error::STACK_OVERFLOW:
        return "Stack overflow";
      case Error::STACK_UNDERFLOW:
        return "Stack underflow";
      case Error::FILE_NOT_FOUND:
        return "File not found";
      case Error::UNKNOWN_OPCODE:
        return "Unknown opcode";
      default:
        return "Unknown error";
    }
  }
};

const std::error_category& category();

inline std::error_code make_error_code(Error error) {
  return {static_cast<int>(error), category()};
}

}  // namespace SuperChip8

namespace std {

template <>
struct is_error_code_enum<SuperChip8::Error> : public true_type {};

}  // namespace std

#endif  // SUPERCHIP8_ERROR_HPP