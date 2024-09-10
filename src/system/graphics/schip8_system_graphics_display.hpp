#ifndef SUPERCHIP8_SYSTEM_GRAPHICS_DISPLAY_HPP
#define SUPERCHIP8_SYSTEM_GRAPHICS_DISPLAY_HPP

#include "schip8_system_graphics_sprite.hpp"

#include <array>
#include <atomic>
#include <functional>
#include <mutex>
#include <system_error>

namespace SuperChip8::System::Graphics {

constexpr std::uint8_t LOW_RES_VIRTUAL_SCREEN_WIDTH = 64;
constexpr std::uint8_t LOW_RES_VIRTUAL_SCREEN_HEIGHT = 32;

constexpr std::uint8_t HIGH_RES_VIRTUAL_SCREEN_WIDTH = 128;
constexpr std::uint8_t HIGH_RES_VIRTUAL_SCREEN_HEIGHT = 64;

constexpr std::uint8_t TARGET_FPS = 60;

/** Screen coordinates
 *
 * 0,0 +----------------------> x
 *     |
 *     |
 *     |
 *     |
 *     |
 *     v
 *     y
 *
 */

/// @brief Display class, responsible for drawing the screen
///
/// @details This class implements a two buffer system, where the back buffer is
/// used to draw the screen and the front buffer is used to display the screen.
/// The back buffer is updated by the CPU and the front buffer is updated by the
/// display thread. This allows the CPU to draw the screen with little to no
/// interruptions. The display thread is responsible for drawing the screen and
/// handling the display window. It also calls the interrupt handler between
/// frame draws (vblank), as it implements a timer mechanism to limit the frame
/// rate.
class Display {
 public:
  using interrupt_handler_t = std::function<void()>;
  enum class Resolution { LOW_RES, HIGH_RES };

  /// @param interrupt_handler The function to call between frame draws (vblank)
  Display(interrupt_handler_t interrupt_handler);

  /// @brief create the display window
  /// @param title The window title
  /// @param ec Error::WINDOW_CREATION_ERROR
  ///
  /// - if the window could not be created
  void createWindow(const std::string &title, std::error_code &ec);

  /// @brief Check if the window should close
  /// @return `true` if the window should close
  bool windowShouldClose();

  /// @brief Close the display window and clean up resources
  void closeWindow();

  /// @brief Clear the screen
  void clear();

  /// @brief Draw the screen
  ///
  /// @details This fuction checks if the window was resized, computes the new
  /// pixel size, clears the screen, draws the pixels, draws the screen bounds,
  /// and calls the interrupt handler. It then swaps the front and back buffers,
  /// sleeps for the remaining time to reach the target frame rate, and updates
  /// the previous time.
  void drawFrame();

  /// @brief Add a sprite to the screen at the specified position
  ///
  /// @details This function adds a sprite to the back buffer.
  /// It is important to note that unlike stated by Cowgod's documentation, the
  /// sprite do in fact wrap around the screen (needed by some games).
  /// @param sprite The sprite to add
  /// @param x The x position of the sprite
  /// @param y The y position of the sprite
  /// @return `true` if a collision occurred
  bool addSprite(const Sprite &sprite, std::uint8_t x, std::uint8_t y);

  /// @brief Scrolls the screen down by n lines
  ///
  /// @details This function scrolls the screen down by n lines. The lines that
  /// are scrolled down are cleared. (ie: no wrapping). It operates on the back
  /// buffer.
  /// @param n the number of pixels to scroll down by
  void scrollDown(std::uint8_t n);

  /// @brief Scrolls the screen right by n pixels
  ///
  /// @details This function scrolls the screen right by n pixels. The pixels
  /// that are scrolled right are cleared. (ie: no wrapping). It operates on the
  /// back buffer.
  /// @param n the number of pixels to scroll right by
  void scrollRight(std::uint8_t n);

  /// @brief Scrolls the screen left by n pixels
  ///
  /// @details This function scrolls the screen left by n pixels. The pixels
  /// that are scrolled left are cleared. (ie: no wrapping). It operates on the
  /// back buffer.
  /// @param n the number of pixels to scroll left by
  void scrollLeft(std::uint8_t n);

  void setResolution(Resolution resolution);

 private:
  /// @brief Compute the new pixel size based on the screen resolution and the
  /// window size
  void computeNewPixelSize();

  // BACK BUFFER
  Resolution _current_back_resolution = Resolution::LOW_RES;
  std::mutex _virtual_back_screen_mutex;
  std::array<std::array<bool, HIGH_RES_VIRTUAL_SCREEN_WIDTH>,
             HIGH_RES_VIRTUAL_SCREEN_HEIGHT>
      _virtual_back_screen;
  std::uint8_t _virtual_back_screen_height = LOW_RES_VIRTUAL_SCREEN_HEIGHT;
  std::uint8_t _virtual_back_screen_width = LOW_RES_VIRTUAL_SCREEN_WIDTH;

  // FRONT BUFFER
  Resolution _current_front_resolution = Resolution::LOW_RES;
  Resolution _next_front_resolution = Resolution::LOW_RES;
  std::array<std::array<bool, HIGH_RES_VIRTUAL_SCREEN_WIDTH>,
             HIGH_RES_VIRTUAL_SCREEN_HEIGHT>
      _virtual_front_screen;
  std::uint8_t _virtual_front_screen_height = LOW_RES_VIRTUAL_SCREEN_HEIGHT;
  std::uint8_t _virtual_front_screen_width = LOW_RES_VIRTUAL_SCREEN_WIDTH;

  std::uint32_t _pixel_size = 15;
  std::uint32_t _vertical_offset = 0;
  std::uint32_t _horizontal_offset = 0;

  std::atomic<std::uint8_t> _target_fps = TARGET_FPS;
  std::atomic<float> _target_frame_time = 1.0f / (float)_target_fps;
  double _previous_time = 0;

  // called when the display thread finishes drawing the screen (vblank)
  interrupt_handler_t _interrupt_handler;
};

}  // namespace SuperChip8::System::Graphics

#endif  // SUPERCHIP8_SYSTEM_GRAPHICS_DISPLAY_HPP