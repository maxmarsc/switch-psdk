#if defined(__CLANGD__)
#include "arm_acle_clangd_fix.h"  // IWYU pragma: keep
#include "arm_neon_clangd_fix.h"  // IWYU pragma: keep
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
// lognx
#include <lognx.hpp>

#include "sample_png.h"

#ifndef NDEBUG
//NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, misc-use-internal-linkage)
volatile bool gdb_wait = true;
#endif

namespace {
#ifndef NDEBUG

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
#endif

void handleEvents(cgss::DisplayWindow& window,
                  cgss::DisplayWindowSettings& /*settings*/) {
  sf::Event event{};
  while (window.popEvent(event)) {
    switch (event.type) {
      case sf::Event::JoystickButtonPressed: {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-union-access)
        if (event.joystickButton.button == 10) {
          lognx::info("Closing window...");
          window.stop();
        }
        break;
      }
      default:
        break;
    }
  }
}

int viewTest() {
  lognx::debug("Creating cgss::DisplayWindowSettings...");
  auto settings         = cgss::DisplayWindowSettings{};
  settings.video.scale  = 1;
  settings.visibleMouse = true;
  settings.video.width  = 1280;
  settings.video.height = 720;
  settings.title        = "Playground";

  lognx::debug("Creating cgss::DisplayWindow...");
  auto window = cgss::DisplayWindow{};

  lognx::debug("Creating window...");
  const auto window_pos =
      sf::Vector2i{static_cast<int>(cgss::DisplayWindow::DesktopWidth() -
                                    settings.video.width) /
                       2,
                   static_cast<int>(cgss::DisplayWindow::DesktopHeight() -
                                    settings.video.height) /
                       2};
  window.move(window_pos.x, window_pos.y);

  window.reload(std::move(settings), true);

  auto viewport = window.addView<cgss::Viewport>();
  auto serializer =
      cgss::TextureMemorySerializer{{g_sample_png, g_sample_png_length}, false};
  auto texture = cgss::Texture{};
  if (!texture.load(serializer)) {
    lognx::error("Failed to load bitmap from memory.");
    return 1;
  }
  auto framed_view = viewport.addView<cgss::FramedView>();

  auto fill_texture      = cgss::Texture{};
  auto memory_serializer = cgss::TextureEmptySerializer{1000, 1000};
  fill_texture.load(memory_serializer);
  fill_texture.fillRect(0, 0, 1000, 1000, sf::Color::White);

  //Move the INSIDE content (i.e. what is drawn), on SOURCE
  framed_view.moveOrigin(60, 50);

  //Move the place where it's rendered on screen, on DESTINATION
  framed_view.move(100, 100);

  //Resize portion of drawn content, on DESTINATION
  framed_view.resize(100, 200);

  auto sprite3 = cgss::Sprite::create(viewport);
  auto sprite4 = cgss::Sprite::create(framed_view);
  sprite3.setTexture(texture);
  sprite3.setColor({255, 120, 120});

  sprite4.setTexture(fill_texture);

  auto sprite = cgss::Sprite::create(framed_view);
  sprite.setTexture(texture);
  sprite.move(50, 100);
  sprite.setColor({120, 255, 120});

  auto sprite2 = cgss::Sprite::create(window);
  sprite2.setTexture(texture);
  sprite2.move(200, 100);
  sprite2.setColor({128, 128, 255});

  viewport.move(50, 50);

  while (appletMainLoop() && window.isOpen()) {
    window.draw();
    handleEvents(window, settings);

    sprite.setAngle(sprite.getAngle() + 1.F);
  }

  return 0;
}
}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  lognx::setLevel(lognx::Level::kDebug);
  lognx::enableFile("litecgss_app");
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
  lognx::info("Starting application...");

  viewTest();

  lognx::disableNxLink();
  return 0;
}