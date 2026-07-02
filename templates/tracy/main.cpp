// Include the most common headers from the C standard library
#include <format>

#if defined(__CLANGD__)
#include "arm_acle_clangd_fix.h"  // IWYU pragma: keep
#include "arm_neon_clangd_fix.h"  // IWYU pragma: keep
#endif

// Include the main libnx system header, for Switch development
#include <switch.h>
// lognx
#include <lognx.hpp>
#include <tracy/Tracy.hpp>

namespace {

// NOLINTNEXTLINE(misc-no-recursion)
void workFunction(int amount_ms, int recursion_depth = 0) {
  ZoneScoped;
  lognx::info(std::format("Doing work for {} ms at recursion depth {}",
                          amount_ms, recursion_depth));
  // Sleep a bit
  svcSleepThread(static_cast<int64_t>(amount_ms) * 1'000'000);
  TracyMessageL("About to start calling child functions");
  if (recursion_depth > 0) {
    // Then call child functions to simulate a call stack
    workFunction(amount_ms / 2, recursion_depth - 1);
    workFunction(amount_ms / 4, recursion_depth - 1);
  }
}
}  // namespace

// Main program entrypoint
int main(int /*argc*/, char* /*argv*/[]) {
  lognx::enableConsole(nullptr);
  if(socketInitializeDefault() != 0) {
    lognx::error("Failed to initialize socket driver");
    return 1;
  }
  lognx::enableNxLink(true, true);
  lognx::setLevel(lognx::Level::kDebug);
  tracy::StartupProfiler();

  // Configure our supported input layout: a single player with standard controller styles
  padConfigureInput(1, HidNpadStyleSet_NpadStandard);

  // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
  PadState pad;
  padInitializeDefault(&pad);

  svcSleepThread(1'000'000'000);  // 1s
  lognx::info("Hello World! About to do some work...");

  for (int i = 0; i < 5; ++i) {
    FrameMark;
    workFunction(500, 3);
  }

  // Exit loop
  lognx::info("Waiting for user input... Press + to exit.");
  while (appletMainLoop()) {
    // Scan the gamepad. This should be done once for each frame
    padUpdate(&pad);

    // padGetButtonsDown returns the set of buttons that have been
    // newly pressed in this frame compared to the previous one
    u64 k_down = padGetButtonsDown(&pad);

    if (k_down & HidNpadButton_Plus) {
      break;  // break in order to return to hbmenu
    }

    svcSleepThread(100'000'000);  // 100ms
  }

  // Deinitialize and clean up resources used by the console (important!)
  tracy::ShutdownProfiler();
  lognx::disableConsole();
  lognx::disableNxLink(false);
  socketExit();
  return 0;
}
