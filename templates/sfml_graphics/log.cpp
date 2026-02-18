#if defined(__CLANGD__)
#include "arm_acle_clangd_fix.h"
#include "arm_neon_clangd_fix.h"
#endif

#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <string_view>

namespace fs = std::filesystem;

#include <switch.h>

#include "log.hpp"

namespace lognx {

constexpr std::string_view kResetSuffix = CONSOLE_RESET;
constexpr std::string_view kLogFolder = ".lognx";

constexpr std::string_view colorPrefix(Level level) {
  switch (level) {
    case Level::kDebug:
      return CONSOLE_MAGENTA;
    case Level::kInfo:
      return CONSOLE_WHITE;
    case Level::kWarning:
      return CONSOLE_YELLOW;
    case Level::kError:
    case Level::kFatal:
      return CONSOLE_RED;
    case Level::kRaw:
      return "";
  }
  return "";
}

constexpr std::string_view levelPrefix(Level level) {
  switch (level) {
    case Level::kDebug:
      return "[DEBUG] ";
    case Level::kInfo:
      return "[INFO] ";
    case Level::kWarning:
      return "[WARNING]  ";
    case Level::kError:
      return "[ERROR] ";
    case Level::kFatal:
      return "[FATAL] ";
    case Level::kRaw:
      return "";
  }
  return "";
}

class Logger {
 private:
  Logger() = default;

 public:
  //============================================================================
  static Logger& getInstance() {
    static Logger instance;  // Guaranteed to be destroyed.
                             // Instantiated on first use.
    return instance;
  }

  //============================================================================
  ~Logger() {
    if (console_) {
      consoleExit(console_);
    }

    if (nxLinkFd_ >= 0) {
      socketExit();
    }

    if (log_file_) {
      [[maybe_unused]] auto _ = disableFile();
    }
  }
  Logger(Logger const&)         = delete;
  void operator=(Logger const&) = delete;
  Logger(Logger&&)              = delete;
  void operator=(Logger&&)      = delete;

  //============================================================================
  void setLevel(Level level) { log_level_ = level; }

  [[nodiscard]] inline int enableConsole(PrintConsole* console) {
    if (console_) {
      return 1;  // Console already enabled
    }

    console_ = consoleInit(console);
    return 0;
  }

  [[nodiscard]] inline int disableConsole() {
    if (!console_) {
      return 1;  // No console to disable
    }

    consoleExit(console_);
    console_ = nullptr;
    return 0;
  }

  [[nodiscard]] inline int enableNxLink(bool redirect_std_out,
                                        bool redirect_std_err) {
    if (nxLinkFd_ != -1) {
      return 1;  // NxLink already enabled
    }

    if (socketInitializeDefault() != 0) {
      return 1;  // Failed to initialize socket driver
    }

    nxLinkFd_ = nxlinkConnectToHost(redirect_std_out, redirect_std_err);
    return nxLinkFd_ < 0 ? 1 : 0;
  }

  [[nodiscard]] inline int disableNxLink() {
    if (nxLinkFd_ == -1) {
      return 1;  // No NxLink to disable
    }

    socketExit();

    nxLinkFd_ = -1;
    return 0;
  }

  [[nodiscard]] inline int enableFile(std::string_view app_folder) {
    if (log_file_) {
      return 1;  // File logging already enabled
    }

    fs::path log_path = fs::path(kLogFolder) / app_folder / (getTimeString() + ".log");
    if (ensurePath(log_path)) {
      // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
      log_file_ = fopen(log_path.string().c_str(), "a+");
      if (!log_file_) {
        return 1;  // Failed to open log file
      }
      log_path_ = log_path;
      return 0;
    }
    return 1;  // Failed to ensure log path exists
  }

  [[nodiscard]] inline int disableFile() {
    if (!log_file_) {
      return 1;  // No file logging to disable
    }

    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    fclose(log_file_);
    log_file_ = nullptr;
    log_path_.reset();
    return 0;
  }
  //============================================================================

  void debug(std::string_view text) {
    if (log_level_ > Level::kDebug) {
      return;
    }

    logToStdout(text, levelPrefix(Level::kDebug), colorPrefix(Level::kDebug));
    logToFile(text, levelPrefix(Level::kDebug));
  }

  void info(std::string_view text) {
    if (log_level_ > Level::kInfo) {
      return;
    }

    logToStdout(text, levelPrefix(Level::kInfo), colorPrefix(Level::kInfo));
    logToFile(text, levelPrefix(Level::kInfo));
  }

  void warning(std::string_view text) {
    if (log_level_ > Level::kWarning) {
      return;
    }

    logToStdout(text, levelPrefix(Level::kWarning),
                colorPrefix(Level::kWarning));
    logToFile(text, levelPrefix(Level::kWarning));
  }

  void error(std::string_view text) {
    if (log_level_ > Level::kError) {
      return;
    }

    logToStderr(text, levelPrefix(Level::kError), colorPrefix(Level::kError));
    logToFile(text, levelPrefix(Level::kError));
  }

  void fatal(std::string_view text) {
    if (log_level_ > Level::kFatal) {
      return;
    }

    logToStderr(text, levelPrefix(Level::kFatal), colorPrefix(Level::kFatal));
    logToFile(text, levelPrefix(Level::kFatal));
  }

 private:
  //============================================================================
  void logToFile(std::string_view text, std::string_view level_prefix) {
    if (log_file_) {
      fprintf(log_file_, "%s%s\n", level_prefix.data(), text.data());
    }
  }

  void logToStdout(std::string_view text, std::string_view prefix,
                   std::string_view color_prefix) {
    if (console_ || nxLinkFd_ != -1) {
      std::cout << color_prefix << prefix << text << kResetSuffix << std::endl;
      if (console_) {
        consoleUpdate(console_);
      }
    }
  }

  void logToStderr(std::string_view text, std::string_view prefix,
                   std::string_view color_prefix) {
    if (console_ || nxLinkFd_ != -1) {
      std::cerr << color_prefix << prefix << text << kResetSuffix << std::endl;
      if (console_) {
        consoleUpdate(console_);
      }
    }
  }

  //============================================================================
  static bool ensurePath(const fs::path& log_path) {
    if (!fs::create_directories(log_path.parent_path())) {
      // Check the directory doesn't already exist
      return fs::exists(log_path.parent_path());
    }

    return true;
  }

  static std::string getTimeString() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm local_tm{};
    localtime_r(&now, &local_tm);

    // We use dashes (-) instead of colons (:) because Switch SD cards 
    // are typically FAT32/exFAT, which forbid colons in filenames.
    return std::format("{:04}-{:02}-{:02}_{:02}-{:02}-{:02}",
                       local_tm.tm_year + 1900,
                       local_tm.tm_mon + 1,
                       local_tm.tm_mday,
                       local_tm.tm_hour,
                       local_tm.tm_min,
                       local_tm.tm_sec);
}

  //============================================================================

  Level log_level_                  = Level::kWarning;
  PrintConsole* console_            = nullptr;
  int nxLinkFd_                     = -1;
  FILE* log_file_                   = nullptr;
  std::optional<fs::path> log_path_ = std::nullopt;
};

//==============================================================================
void setLevel(Level level) {
  Logger::getInstance().setLevel(level);
}

int enableConsole(PrintConsole* console) {
  return Logger::getInstance().enableConsole(console);
}

int disableConsole() {
  return Logger::getInstance().disableConsole();
}

int enableNxLink(bool redirect_std_out, bool redirect_std_err) {
  return Logger::getInstance().enableNxLink(redirect_std_out, redirect_std_err);
}

int disableNxLink() {
  return Logger::getInstance().disableNxLink();
}

int enableFile(std::string_view app_folder) {
  return Logger::getInstance().enableFile(app_folder);
}

int disableFile() {
  return Logger::getInstance().disableFile();
}

//==============================================================================
void debug(std::string_view text) {
  Logger::getInstance().debug(text);
}

void info(std::string_view text) {
  Logger::getInstance().info(text);
}

void warning(std::string_view text) {
  Logger::getInstance().warning(text);
}

void error(std::string_view text) {
  Logger::getInstance().error(text);
}

void fatal(std::string_view text) {
  Logger::getInstance().fatal(text);
}

}  // namespace lognx