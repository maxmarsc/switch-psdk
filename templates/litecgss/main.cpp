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
// LiteCGSS
#include <LiteCGSS/Configuration/DisplayWindowSettings.h>
#include <LiteCGSS/Graphics/Serializers/TextureSerializer.h>
#include <LiteCGSS/Graphics/Sprite.h>
#include <LiteCGSS/Graphics/Texture.h>
#include <LiteCGSS/Views/DisplayWindow.h>
#include <LiteCGSS/Views/Viewport.h>

#include "sample_png.h"

// static void memTest() {
//   auto settings        = cgss::DisplayWindowSettings{};
//   settings.video.scale = 1;
//   // settings.visibleMouse = true;
//   settings.video.width  = 400;
//   settings.video.height = 300;
//   settings.title        = "MemTest";
//   auto window           = cgss::DisplayWindow{settings};

//   auto viewport = window.addView<cgss::Viewport>();
//   auto texture  = cgss::Texture::create(window, "sdmc:/dev_assets/psdk.png");
//   viewport.resize(200, 200);

//   printf("Before mem alloc\n");
//   for (std::size_t i = 0; i < 1000000; i++) {
//     auto sprite = cgss::Sprite::create(viewport);
//     sprite.setTexture(texture);
//     if (i % 10000 == 0) {
//       printf("%f \x0a\n", (static_cast<float>(i) / 10000.F));
//     }
//   }
//   printf("Drawing...\n");
//   window.draw();
//   printf("Before mem release\n");
//   viewport.detach();
//   printf("After mem release\n");
// }

static bool g_dispose_view = false;
static sf::Vector2i g_window_position{100, 100};

static void handleEvents(cgss::DisplayWindow& window,
                         cgss::DisplayWindowSettings& /*settings*/) {
  sf::Event event{};
  while (window.popEvent(event)) {
    switch (event.type) {
      case sf::Event::JoystickButtonPressed: {
        if (event.joystickButton.button == 10) {
          printf("Closing window...\n");
          window.stop();
        }
        break;
      }
      // case sf::Event::EventType::Closed:
      //   window.stop();
      //   break;
      // case sf::Event::EventType::KeyPressed:
      //   switch (event.key.code) {
      //     case sf::Keyboard::F1:
      //       if (window.screenHeight() > 15 && window.screenWidth() > 20) {
      //         window.resizeScreen(window.screenWidth() - 20,
      //                             window.screenHeight() - 15);
      //         settings.video.height = window.screenHeight();
      //         settings.video.width  = window.screenWidth();
      //       }
      //       break;
      //     case sf::Keyboard::F2:
      //       window.resizeScreen(window.screenWidth() + 20,
      //                           window.screenHeight() + 15);
      //       settings.video.height = window.screenHeight();
      //       settings.video.width  = window.screenWidth();
      //       break;
      //     case sf::Keyboard::F3:
      //       g_disposeView = true;
      //       break;
      //     case sf::Keyboard::F5:
      //       window.move(window.getX() - 10, window.getY() - 10);
      //       break;
      //     case sf::Keyboard::F6:
      //       window.move(window.getX() + 10, window.getY() + 10);
      //       break;
      //     case sf::Keyboard::Escape:
      //       window.stop();
      //       break;
      //     case sf::Keyboard::Enter:
      //       if (event.key.alt) {
      //         settings.fullscreen = !settings.fullscreen;
      //         window.reload(settings);
      //       }
      //       break;
      //     default:
      //       break;
      //   }
      //   break;
      // case sf::Event::EventType::KeyReleased:
      //   switch (event.key.code) {
      //     case sf::Keyboard::Escape:
      //       g_disposeView = false;
      //       break;
      //     default:
      //       break;
      //   }
      //   break;
      default:
        break;
    }
  }
}

static int viewTest() {
  printf("Creating cgss::DisplayWindowSettings...\n");
  svcSleepThread(100000000);
  auto settings         = cgss::DisplayWindowSettings{};
  settings.video.scale  = 1;
  settings.visibleMouse = true;
  settings.video.width  = 400;
  settings.video.height = 300;
  settings.title        = "Playground";
  printf("Creating cgss::DisplayWindow...\n");
  svcSleepThread(100000000);
  auto window = cgss::DisplayWindow{};

  printf("Creating window...\n");
  svcSleepThread(100000000);
  const auto window_pos =
      sf::Vector2i{static_cast<int>(cgss::DisplayWindow::DesktopWidth() -
                                    settings.video.width) /
                       2,
                   static_cast<int>(cgss::DisplayWindow::DesktopHeight() -
                                    settings.video.height) /
                       2};
  window.move(window_pos.x, window_pos.y);

  window.reload(std::move(settings), true);
  printf("Flag 2\n");
  svcSleepThread(100000000);

  auto viewport = window.addView<cgss::Viewport>();
  auto serializer =
      cgss::TextureMemorySerializer{{g_sample_png, g_sample_png_length}, false};
  auto texture = cgss::Texture{};
  if (!texture.load(serializer)) {
    printf("Failed to load bitmap from memory.\n");
    return 1;
    // throw std::runtime_error("Failed to load bitmap from memory.");
  }
  auto framed_view = viewport.addView<cgss::FramedView>();
  printf("Flag 3\n");

  svcSleepThread(100000000);

  auto fill_texture      = cgss::Texture{};
  auto memory_serializer = cgss::TextureEmptySerializer{1000, 1000};
  fill_texture.load(memory_serializer);
  fill_texture.fillRect(0, 0, 1000, 1000, sf::Color::White);
  printf("Flag 4\n");

  //Move the INSIDE content (i.e. what is drawn), on SOURCE
  framed_view.moveOrigin(60, 50);

  //Move the place where it's rendered on screen, on DESTINATION
  framed_view.move(100, 100);

  //Resize portion of drawn content, on DESTINATION
  framed_view.resize(100, 200);

  printf("Flag 5\n");
  svcSleepThread(100000000);

  auto sprite3 = cgss::Sprite::create(viewport);
  auto sprite4 = cgss::Sprite::create(framed_view);
  sprite3.setTexture(texture);
  sprite3.setColor({255, 120, 120});
  //sprite3.move(20, 0);
  //sprite3.move(50, 100);

  sprite4.setTexture(fill_texture);
  //sprite4.setZ(0);

  auto sprite = cgss::Sprite::create(framed_view);
  sprite.setTexture(texture);
  sprite.move(50, 100);
  sprite.setColor({120, 255, 120});

  auto sprite2 = cgss::Sprite::create(window);
  sprite2.setTexture(texture);
  sprite2.move(200, 100);
  sprite2.setColor({128, 128, 255});

  viewport.move(50, 50);

  printf("Flag 6\n");
  svcSleepThread(100000000);

  auto i = 0;
  {

    while (appletMainLoop() && window.isOpen()) {
      window.draw();
      handleEvents(window, settings);

      sprite.setAngle(sprite.getAngle() + 1.F);

      if (g_dispose_view) {
        framed_view.detach();
      }

      i++;
    }
  }

  return 0;
}

int main(int /*argc*/, char* /*argv*/[]) {
  socketInitializeDefault();
  nxlinkStdio();

  printf("Starting application...\n");
  svcSleepThread(100000000);

  viewTest();
  {

    // // Create a window (Switch resolution is 1280x720)
    // constexpr auto kWidth  = 1280;
    // constexpr auto kHeight = 720;
    // sf::RenderWindow window(sf::VideoMode(kWidth, kHeight), "SFML Switch Test");

    // // Create a simple shape to verify rendering
    // auto texture = cgss::Texture::create("sdmc:/dev_assets/psdk.png");
    // printf("file loaded\n");
    // auto sprite      = cgss::Sprite();
    // auto sprite_item = cgss::SpriteItem(&sprite);
    // printf("sprite created\n");
    // sprite.setTexture(texture);
    // printf("texture set\n");
    // printf("sprite position: (%f, %f)\n", sprite.getX(), sprite.getY());

    // // Drive the loop by the OS status
    // auto positions  = std::array{std::pair<float, float>{100.F, 100.F},
    //                             std::pair<float, float>{200.F, 200.F},
    //                             std::pair<float, float>{300.F, 300.F},
    //                             std::pair<float, float>{400.F, 400.F}};
    // int color_index = 0;

    // while (appletMainLoop() && window.isOpen()) {
    //   sf::Event event{};
    //   while (window.pollEvent(event)) {
    //     if (event.type == sf::Event::Closed) {
    //       window.close();
    //     }

    //     // Change color on ANY button press
    //     if (event.type == sf::Event::JoystickButtonPressed) {
    //       printf("Moving texture %d\n", color_index);
    //       auto&& [x, y] = positions.at(color_index);
    //       sprite.move(x, y);

    //       if (event.joystickButton.button == 10) {
    //         printf("Closing window...\n");
    //         window.close();
    //       }
    //     }

    //     window.clear();
    //     sprite_item.draw(window);
    //     window.display();
    //   }
    // }
  }

  socketExit();
  return 0;
}