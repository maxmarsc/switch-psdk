// Include the most common headers from the C standard library
#include <dirent.h>
#include <array>
#include <format>

#if defined(__CLANGD__)
#include "arm_acle_clangd_fix.h"  // IWYU pragma: keep
#include "arm_neon_clangd_fix.h"  // IWYU pragma: keep
#endif

// Include the main libnx system header, for Switch development
#include <switch.h>
// lognx
#include <lognx.hpp>

#ifndef NDEBUG
#define RUBY_DEBUG 1
//NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables, misc-use-internal-linkage)
volatile bool gdb_wait = true;
#endif
#include "ruby.h"  // IWYU pragma: keep
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
extern bool rb_free_at_exit;

extern "C" {
// NOLINTBEGIN(readability-identifier-naming)
void Init_LiteRGSS();
void Init_SFMLAudio();
// NOLINTEND(readability-identifier-naming)
}

#include <sys/iosupport.h>

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

int rubyWork() {
  // Add pure-ruby load paths
  lognx::info("Setting up load path...");
  {
    int load_path_state = 0;
    rb_eval_string_protect("$LOAD_PATH.unshift('" RUBY_STDLIB_DIR
                           "', '" RUBY_STDLIB_DIR
                           "/aarch64-elf', 'romfs:/ruby/site_ruby')",
                           &load_path_state);
    if (load_path_state != 0) {
      VALUE err           = rb_errinfo();
      VALUE inspected_err = rb_inspect(err);
      lognx::error(std::format("Failed to set up load path: {}",
                               StringValueCStr(inspected_err)));
      rb_set_errinfo(Qnil);
      return 1;
    }
  }

  // Initialize temp dir
  lognx::info("Setting up temp dir...");
  {
    int tmpdir_state = 0;
    rb_eval_string_protect(
        "require 'tmpdir'; ENV['TMPDIR'] = '" RUBY_TMP_DIR "';", &tmpdir_state);
    if (tmpdir_state != 0) {
      VALUE err           = rb_errinfo();
      VALUE inspected_err = rb_inspect(err);
      lognx::error(std::format("Failed to set up temp dir: {}",
                               StringValueCStr(inspected_err)));
      rb_set_errinfo(Qnil);
      return 1;
    }
  }

  // Initialize home dir
  lognx::info("Setting up home dir...");
  {
    mkdir(PSDK_HOME_DIR, 0755);
    int homedir_state = 0;
    rb_eval_string_protect(
        "require 'tmpdir'; ENV['HOME'] = '" PSDK_HOME_DIR "';", &homedir_state);
    if (homedir_state != 0) {
      VALUE err           = rb_errinfo();
      VALUE inspected_err = rb_inspect(err);
      lognx::error(std::format("Failed to set up home dir: {}",
                               StringValueCStr(inspected_err)));
      rb_set_errinfo(Qnil);
      return 1;
    }
  }

  // Change the working directory to romfs:/ruby/app
  if (chdir("romfs:/ruby/app") != 0) {
    lognx::error("Failed to change working directory to romfs:/ruby/app");
    return 1;
  }

  // Run the ruby code
  lognx::info("Running ruby app...");
  svcSleepThread(1000000000);
  lognx::disableConsole();
  int state = 0;
  rb_load_protect(rb_str_new_cstr(RUBY_MAIN_APP), 0, &state);
  if (state != 0) {
    VALUE err           = rb_errinfo();
    VALUE inspected_err = rb_inspect(err);
    VALUE msg           = rb_funcall(err, rb_intern("message"), 0);
    VALUE klass         = rb_funcall(rb_funcall(err, rb_intern("class"), 0),
                                     rb_intern("name"), 0);
    lognx::enableConsole(nullptr);
    lognx::error(StringValueCStr(inspected_err));
    lognx::log(std::format("Error class: {}, message: {}",
                           StringValueCStr(klass), StringValueCStr(msg)));
    VALUE bt = rb_funcall(err, rb_intern("backtrace"), 0);
    if (RB_TYPE_P(bt, T_ARRAY)) {
      lognx::log("Backtrace:");
      for (int64_t i = 0; i < RARRAY_LEN(bt); i++) {
        VALUE aref = RARRAY_AREF(bt, i);
        lognx::log(std::format("  {}", StringValueCStr(aref)));
      }
    }
    lognx::disableConsole();
    rb_set_errinfo(Qnil);
    return 1;
  }

  return 0;
}
}  // namespace

int main(int argc, char** argv) {
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

#ifndef NDEBUG
  if (waitForDebugger()) {
    return 0;
  }
#endif

  auto rc = romfsInit();
  if (R_FAILED(rc)) {
    lognx::error(std::format("romfsInit: {}", rc));
    lognx::disableConsole();
    lognx::disableNxLink();
    return 1;
  }
  lognx::info("romfs Init Successful!");

  // Setup ruby
  lognx::info("Initializing Ruby...");
  // NOLINTBEGIN(cppcoreguidelines-pro-type-const-cast)
  std::array<char*, 4> ruby_opts{*argv, const_cast<char*>("--disable-gems"),
                                 const_cast<char*>("-e"),
                                 const_cast<char*>("")};
  // NOLINTEND(cppcoreguidelines-pro-type-const-cast)
  ruby_sysinit(&argc, &argv);
  RUBY_INIT_STACK;
  ruby_init();
  // Load the built-in features & extensions
  ruby_options(ruby_opts.size(), ruby_opts.data());
  Init_LiteRGSS();
  rb_provide("LiteRGSS");
  Init_SFMLAudio();
  rb_provide("SFMLAudio");
  // Stubbing these missing extensions are these are not needed to bootstrap
  rb_provide("openssl");
  rb_provide("net/http");
  rb_provide("psych");
  // yaml is provided, but it requires psych which is not, so we need to stub it by providing the absolute path to yaml.rb in LOADED_FEATURES
  // see: https://ruby-doc.org/3.4.1/Kernel.html#method-i-require
  rb_provide(RUBY_STDLIB_DIR "/yaml.rb");
  // Disable buffering for stdout and stderr
  rb_eval_string("STDOUT.sync = true; STDERR.sync = true");

  // don't forget to set this flag to make sure ruby_cleanup() clean everything up
  rb_free_at_exit = true;

  // Setup load paths and temp dir, then run the ruby app
  int result = rubyWork();
  ruby_cleanup(0);

  lognx::enableConsole(nullptr);
  lognx::info("Waiting for [+] button to exit");

  while (appletMainLoop()) {
    svcSleepThread(100000000);  // 100ms
    padUpdate(&pad);
    if (padGetButtonsDown(&pad) & HidNpadButton_Plus) {
      break;
    }
  }

  romfsExit();
  lognx::disableConsole();
  lognx::disableNxLink();
  return result;
}