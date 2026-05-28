#include <cstdio>
#include <format>

#if defined(__CLANGD__)
#include "arm_acle_clangd_fix.h"  // IWYU pragma: keep
#include "arm_neon_clangd_fix.h"  // IWYU pragma: keep
#endif

// Include the main libnx system header, for Switch development
#include <switch.h>
// SFML
#include <SFML/Audio.hpp>

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

namespace {
void waitFrames(int n) {
  for (int i = 0; i < n; i++) {
    svcSleepThread(16'666'667);
  }
}

}  // namespace

int main(int /*argc*/, char* /*argv*/[]) {
  // initialize console and socket for nxlink
  lognx::setLevel(lognx::Level::kDebug);
  lognx::enableFile("litergss_app");
  if (lognx::enableConsole(nullptr)) {
    return 1;
  }
  lognx::enableNxLink(true, true);
  padConfigureInput(1, HidNpadStyleSet_NpadStandard);
  PadState pad;
  padInitializeDefault(&pad);
  romfsInit();

#ifndef NDEBUG
  if (waitForDebugger()) {
    return 0;
  }
#endif

  {
    // 1) Streamed OGG (sf::Music) — the BGM path
    {
      sf::Music music;
      if (!music.openFromFile("romfs:/assets/test.ogg")) {
        lognx::error("[FAIL] openFromFile test.ogg");
      } else {
        lognx::info(std::format("[ OK ] OGG opened: %.1fs, %u ch, %u Hz",
                                music.getDuration().asSeconds(),
                                music.getChannelCount(),
                                music.getSampleRate()));
        music.setLoop(false);

        music.play();
        lognx::info(std::format("status after play() = {} (1==Playing)",
                                static_cast<int>(music.getStatus())));
        waitFrames(180 * 4);  // ~3s; LISTEN for sound here
        music.stop();
      }
    }

    // 2) In-RAM WAV (sf::SoundBuffer + sf::Sound) — the SE path
    {
      sf::SoundBuffer buf;
      if (!buf.loadFromFile("romfs:/assets/test.wav")) {
        lognx::error("[FAIL] loadFromFile test.wav (PCM only!)");
      } else {
        lognx::info(std::format("[ OK ] WAV loaded: {} samples, {} Hz",
                                static_cast<size_t>(buf.getSampleCount()),
                                buf.getSampleRate()));
        sf::Sound snd(buf);
        snd.play();
        waitFrames(120 * 4);
      }
    }

    lognx::info("=== done; press + to exit ===");

    while (appletMainLoop()) {
      padUpdate(&pad);
      if (padGetButtonsDown(&pad) & HidNpadButton_Plus) {
        break;
      }
      waitFrames(1);
    }
  }

  romfsExit();
  lognx::disableConsole();
  lognx::disableNxLink();
  return 0;
}