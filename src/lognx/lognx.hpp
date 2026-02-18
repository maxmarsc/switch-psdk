#if defined(__CLANGD__)
#include "arm_acle_clangd_fix.h"  // IWYU pragma: keep
#include "arm_neon_clangd_fix.h"  // IWYU pragma: keep
#endif

#include <switch.h>
#include <string_view>

#pragma once

namespace lognx {

enum class Level : int8_t { kDebug, kInfo, kWarning, kError, kFatal, kRaw };

//==============================================================================
void setLevel(Level level);
int enableConsole(PrintConsole* console);
int disableConsole();
int enableNxLink(bool redirect_std_out, bool redirect_std_err);
int disableNxLink();
int enableFile(std::string_view app_folder);
int disableFile();

//==============================================================================
void debug(std::string_view text);
void info(std::string_view text);
void warning(std::string_view text);
void error(std::string_view text);
void fatal(std::string_view text);

}  // namespace lognx