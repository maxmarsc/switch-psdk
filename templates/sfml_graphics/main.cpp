// Include the most common headers from the C standard library
#include <array>
#include <format>

#if defined(__CLANGD__)
#include "arm_acle_clangd_fix.h"  // IWYU pragma: keep
#include "arm_neon_clangd_fix.h"  // IWYU pragma: keep
#endif

// Include the main libnx system header, for Switch development
#include <switch.h>
// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <lognx.hpp>

#ifndef NDEBUG
namespace {
//NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
volatile bool gdb_wait = true;

int waitForDebugger() {
  padConfigureInput(1, HidNpadStyleSet_NpadStandard);
  PadState pad;
  padInitializeDefault(&pad);
  lognx::debug(
      "Waiting for debugger or [-] button to start or [+] button to exit");
  while (appletMainLoop() && gdb_wait) {
    padUpdate(&pad);
    u64 k_down = padGetButtonsDown(&pad);

    if (k_down & HidNpadButton_Minus) {
      break;  // break in order to start the application
    }

    if (k_down & HidNpadButton_Plus) {
      lognx::debug("Exiting to hbmenu...");
      return 1;
    }
    svcSleepThread(100'000'000);  // 100ms
  }
  return 0;
}
}  // namespace
#endif

int main(int /*argc*/, char* /*argv*/[]) {
  lognx::setLevel(lognx::Level::kDebug);
  lognx::enableFile("sfml_app");
#ifndef NDEBUG
  if (lognx::enableConsole(nullptr)) {
    return 1;
  }
  if (waitForDebugger()) {
    return 0;
  }
  lognx::disableConsole();
#endif
  lognx::enableNxLink(true, true);

  {
    // Create a window (Switch resolution is 1280x720)
    constexpr auto kWidth  = 1280;
    constexpr auto kHeight = 720;
    sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "SFML Switch Test");

    // Create a simple shape to verify rendering
    sf::CircleShape shape(100.F);
    shape.setFillColor(sf::Color::Yellow);
    shape.setOrigin(100.F, 100.F);
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    shape.setPosition(640.F, 360.F);  // Center of the screen

    // Drive the loop by the OS status
    auto colors     = std::array{sf::Color::Yellow, sf::Color::Green,
                             sf::Color::Blue, sf::Color::Red};
    int color_index = 0;

    while (appletMainLoop() && window.isOpen()) {
      sf::Event event{};
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
          window.close();
        }

        // Change color on ANY button press
        if (event.type == sf::Event::JoystickButtonPressed) {
          color_index = (color_index + 1) % 4;
          lognx::debug(std::format("Changing color to {}", color_index));
          shape.setFillColor(colors.at(color_index));

          // Specifically check for PLUS to exit
          // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
          if (event.joystickButton.button == 10) {
            lognx::info("Closing window...");
            window.close();
          }
        }

        window.clear();
        window.draw(shape);
        window.display();
      }
    }
  }

  return 0;
}