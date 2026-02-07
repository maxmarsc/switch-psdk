// Include the most common headers from the C standard library
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

// // Main program entrypoint
// int main(int /*argc*/, char* /*argv*/[]) {
//   // This example uses a text console, as a simple way to output text to the screen.
//   // If you want to write a software-rendered graphics application,
//   //   take a look at the graphics/simplegfx example, which uses the libnx Framebuffer API instead.
//   // If on the other hand you want to write an OpenGL based application,
//   //   take a look at the graphics/opengl set of examples, which uses EGL instead.
//   // consoleInit(nullptr);

//   // Configure our supported input layout: a single player with standard controller styles
//   padConfigureInput(1, HidNpadStyleSet_NpadStandard);

//   // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
//   PadState pad;
//   padInitializeDefault(&pad);

//   // // Other initialization goes here. As a demonstration, we print hello world.
//   // printf("Hello World!\n");

//   // Main loop
//   while (appletMainLoop()) {
//     // Scan the gamepad. This should be done once for each frame
//     padUpdate(&pad);

//     // padGetButtonsDown returns the set of buttons that have been
//     // newly pressed in this frame compared to the previous one
//     u64 k_down = padGetButtonsDown(&pad);

//     if (k_down & HidNpadButton_Plus) {
//       break;  // break in order to return to hbmenu
//     }

//     // Your code goes here

//     // Update the console, sending a new frame to the display
//     consoleUpdate(nullptr);
//   }

//   // Deinitialize and clean up resources used by the console (important!)
//   consoleExit(nullptr);
//   return 0;
// }

int main(int /*argc*/, char* /*argv*/[]) {
  // Configure our supported input layout: a single player with standard controller styles
  padConfigureInput(1, HidNpadStyleSet_NpadStandard);

  // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
  PadState pad;
  padInitializeDefault(&pad);

  // 1. Create a window (Switch resolution is 1280x720)
  constexpr auto kWidth  = 1280;
  constexpr auto kHeight = 720;
  sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "SFML Switch Test");

  // 2. Create a simple shape to verify rendering
  sf::CircleShape shape(100.F);
  shape.setFillColor(sf::Color::Green);
  shape.setOrigin(100.F, 100.F);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
  shape.setPosition(640.F, 360.F);  // Center of the screen

  // Drive the loop by the OS status
  while (appletMainLoop() && window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      // Exit back to hbmenu if '+' is pressed
      // SFML maps Switch buttons to Joypad buttons
      if (event.type == sf::Event::KeyPressed) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
        if (event.key.code == sf::Keyboard::Escape ||
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
            sf::Joystick::isButtonPressed(0, 10)) {  // 10 is typically '+'
          window.close();
        }
      }
    }

    window.clear();
    window.draw(shape);
    window.display();
  }

  return 0;
}