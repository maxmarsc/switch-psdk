#include <format>
#include <optional>
#include <string_view>
#include <filesystem>

#include <switch.h>

namespace fs = std::filesystem;


#pragma once

namespace lognx {

enum class Level { kDebug, kInfo, kWarning, kError, kFatal, kRaw };

//==============================================================================
void setLevel(Level level);
int enableConsole(PrintConsole* console);
int disableConsole();
int enableNxLink(bool redirect_std_out, bool redirect_std_err);
int disableNxLink();
int enableFile(const fs::path& log_path);
int disableFile();

//==============================================================================
void debug(std::string_view text);
void info(std::string_view text);
void warning(std::string_view text);
void error(std::string_view text);
void fatal(std::string_view text);

}  // namespace lognx