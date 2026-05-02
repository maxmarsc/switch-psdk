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

# puts "Hello, world from ruby!"
include LiteRGSS

w = 1280
h = 720
window = DisplayWindow.new("Ruby on Switch", w, h, 2)
v = Viewport.new(window, w, h)

data = File.binread("romfs:/assets/image.png")
bmp = Bitmap.new(data, true)
sp = Sprite.new(v)
sp.bitmap = bmp
sp.x = w / 2
sp.y = h / 2

300.times do |i|
  window.update
  sp.angle = (sp.angle + 5) % 360
end

window.dispose