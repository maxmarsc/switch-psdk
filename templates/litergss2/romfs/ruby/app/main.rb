require 'rbconfig'
require 'continuation'
require 'date'
require 'delegate'
require 'digest'
require 'fcntl'
require 'fileutils'
require 'find'
require 'forwardable'
require 'ipaddr'
require 'io/wait'
require 'io/nonblock'
require 'json'
require 'monitor'
require 'objspace'
require 'optparse'
require 'ostruct'
require 'pathname'
require 'pp'
require 'prettyprint'
require 'ripper'
require 'securerandom'
require 'set'
require 'shellwords'
require 'singleton'
require 'socket'
require 'strscan'
require 'stringio'
require 'time'
require 'timeout'
require 'tempfile'
require 'tmpdir'
require 'tsort'
require 'weakref'
require 'zlib'

require 'LiteRGSS'
include LiteRGSS
require 'tracy_ruby'

Tracy.startup()

w, h = 1280, 720
window = DisplayWindow.new("Input test", w, h, 1)
viewport = Viewport.new(window, w, h)
sprite = Sprite.new(viewport)
sprite.bitmap = Bitmap.new(File.binread("romfs:/assets/image.png"), true)
sprite.x = w / 2
sprite.y = h / 2

# --- I/O demo ---
ax_x = 0
ax_y = 0
rotating = false
running = true
exit_button = 10 # button + / Start

window.on_joystick_connected    = proc { |id| puts "connected #{id}" }
window.on_joystick_disconnected = proc { |id| puts "disconnected #{id}" }

window.on_joystick_button_pressed = proc do |id, b|
  Tracy.zone("Joystick button pressed") {
    puts "press   id=#{id} btn=#{b}"
    rotating = !rotating if b == 0
    running = false if b == exit_button
  }
end

window.on_joystick_button_released = proc do |id, b|
  Tracy.zone("Joystick button released") {
    puts "release id=#{id} btn=#{b}"
  }
end

window.on_joystick_moved = proc do |id, axis, pos|
  Tracy.zone("Joystick axis moved") {
    puts "axis    id=#{id} ax=#{axis} pos=#{pos}"
    ax_x = pos if axis == 0
    ax_y = pos if axis == 1
  }
end

window.on_touch_began = proc do |finger_id, x, y|
  Tracy.zone("Touch begin") {
    puts "touch begin finger=#{finger_id} x=#{x} y=#{y}"
    sprite.x = x
    sprite.y = y
  }
end

window.on_touch_moved = proc do |finger_id, x, y|
  Tracy.zone("Touch move") {
    puts "touch move  finger=#{finger_id} x=#{x} y=#{y}"
    sprite.x = x
    sprite.y = y
  }
end

window.on_touch_ended = proc do |finger_id, x, y|
  Tracy.zone("Touch end") {
    puts "touch end   finger=#{finger_id} x=#{x} y=#{y}"
  }
end

while running
  # sleep(0.01)
  sprite.x += ax_x * 0.05
  sprite.y += ax_y * 0.05
  sprite.angle = (sprite.angle + 5) % 360 if rotating
  Tracy.zone("Update window") {
    window.update
  }
  Tracy.frame_mark
end

window.dispose
Tracy.shutdown()