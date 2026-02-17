// Include the most common headers from the C standard library
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(__CLANGD__)
#include "arm_acle_clangd_fix.h"
#include "arm_neon_clangd_fix.h"
#endif

// Include the main libnx system header, for Switch development
#include <switch.h>
// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#ifndef NDEBUG
//NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
volatile bool gdb_wait = true;

static int waitForDebugger() {
  consoleInit(nullptr);
  padConfigureInput(1, HidNpadStyleSet_NpadStandard);
  PadState pad;
  padInitializeDefault(&pad);
  printf("Waiting for debugger or [-] button to start or [+] button to exit\n");
  while (appletMainLoop() && gdb_wait) {
    padUpdate(&pad);
    u64 k_down = padGetButtonsDown(&pad);

    if (k_down & HidNpadButton_Minus) {
      break;  // break in order to start the application
    }

    if (k_down & HidNpadButton_Plus) {
      printf("Exiting to hbmenu...\n");
      consoleUpdate(nullptr);
      consoleExit(nullptr);
      return 1; 
    }
    consoleUpdate(nullptr);
    svcSleepThread(100'000'000);  // 100ms
  }
  consoleUpdate(nullptr);
  consoleExit(nullptr);
  return 0;
}
#endif

int main(int /*argc*/, char* /*argv*/[]) {
#ifndef NDEBUG
  if (waitForDebugger()) {
    return 0;
  }
#endif
  socketInitializeDefault();
  bool connected = nxlinkStdio() >= 0;

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
          if (connected) {
            printf("Changing color to %d\n", color_index);
          }
          shape.setFillColor(colors.at(color_index));

          // Specifically check for PLUS to exit
          // NOLINTNEXTLINE
          if (event.joystickButton.button == 10) {
            if(connected) {
              printf("Closing window...\n");
            }
            window.close();
          }
        }

        window.clear();
        window.draw(shape);
        window.display();
      }
    }
  }

  socketExit();
  return 0;
}