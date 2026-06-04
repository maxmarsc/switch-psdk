# audio_test.rb — run via your Switch Ruby host
require 'SFMLAudio'
include SFMLAudio

# Dir.chdir("romfs:/") rescue nil

def frames(n) = n.times { svc_sleep_frame() rescue sleep(1.0/60) }

# --- 1. Streamed OGG (Music) — the BGM path --------------------------------
puts "\n=== Music / OGG ==="
m = SFMLAudio::Music.new
if m.open_from_file("romfs:/assets/test.ogg")
  puts "[ OK ] OGG opened: dur=#{m.get_duration.inspect}, " \
       "#{m.get_channel_count} ch, #{m.get_sample_rate} Hz"
  m.set_loop(false)
  m.set_volume(100)
  m.play
  puts "playing? #{m.playing?}"
  frames(60 * 3)        # ~3s — LISTEN
  m.stop
  puts "stopped? #{m.stopped?}"
else
  puts "[FAIL] open_from_file assets/test.ogg"
end

# --- 2. In-RAM WAV (SoundBuffer + Sound) — the SE path ---------------------
puts "\n=== Sound / WAV ==="
buf = SFMLAudio::SoundBuffer.new
if buf.load_from_file("romfs:/assets/test.wav")    # change to load_from_file if that's the real name
  puts "[ OK ] WAV loaded: #{buf.get_sample_count} samples, #{buf.get_sample_rate} Hz"
  s = SFMLAudio::Sound.new
  s.set_buffer(buf)        # buffer is attached, not passed to the constructor
  s.set_volume(100)
  s.play
  puts "playing? #{s.playing?}"
  frames(120)
  s.stop
else
  puts "[FAIL] open_from_file assets/test.wav"
end

puts "\n=== done ==="