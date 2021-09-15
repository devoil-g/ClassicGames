#include <fstream>
#include <array>
#include <vector>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <iomanip>
#include <tuple>
#include <algorithm>

#include "Math/Math.hpp"
#include "System/Midi.hpp"
#include "System/Config.hpp"

// TODO: remove this
struct Note
{
  uint8_t   key = 0;
  uint8_t   velocity = 0;
  uint64_t  clock = 0;
  bool      pressed = false;
};

// TODO: remove this
std::list<Note> notes;

// TODO: remove this
void  toWave(const std::string& filename, const std::vector<int16_t>& samples16)
{
  std::ofstream  out(filename, std::ofstream::binary);

  uint32_t byte4;
  uint16_t byte2;

  out.write("RIFF", 4);
  byte4 = (int)samples16.size() * 2 + 36;
  out.write((char*)&byte4, sizeof(byte4));
  out.write("WAVE", 4);
  out.write("fmt ", 4);
  byte4 = 16;
  out.write((char*)&byte4, sizeof(byte4));
  byte2 = 1;
  out.write((char*)&byte2, sizeof(byte2));
  byte2 = 1;
  out.write((char*)&byte2, sizeof(byte2));
  byte4 = 22050;
  out.write((char*)&byte4, sizeof(byte4));
  byte4 = 22050 * 2;
  out.write((char*)&byte4, sizeof(byte4));
  byte2 = 2;
  out.write((char*)&byte2, sizeof(byte2));
  byte2 = 16;
  out.write((char*)&byte2, sizeof(byte2));
  out.write("data", 4);
  byte4 = (int)samples16.size() * 2;
  out.write((char*)&byte4, sizeof(byte4));
  out.write((char*)samples16.data(), samples16.size() * 2);
}

// TODO: remove this
void  Game::Midi::generate(const Game::Midi::Sequence& sequence)
{
  std::array<Note, 120> piano;
  std::vector<float>  samples((int)(duration(sequence, sequence.metadata.end).asSeconds() * 22050) + 1, 0.f);

  piano.fill(Note{ 0, 0, 0, false });

  std::cout << "generating" << std::endl;

  for (uint64_t tick = 0; tick < sequence.metadata.end; tick++) {
    if ((int)(duration(sequence, tick).asSeconds() / 1.f) != (int)(duration(sequence, tick + 1).asSeconds() / 1.f)) {
      static int i = 0;
      i++;
      std::cout << i / 60 << "m " << i % 60 << "s\r" << std::flush;
    }

    float sample = 0.f;

    while (notes.empty() == false && notes.front().clock <= tick) {
      piano[notes.front().key] = notes.front();
      notes.pop_front();
    }

    for (const auto& note : piano) {
      if (note.velocity > 0.f && note.pressed == true) {
        for (int sample_idx = (int)(duration(sequence, tick).asSeconds() * 22050); sample_idx < (int)(duration(sequence, tick + 1).asSeconds() * 22050); sample_idx++) {
          float note_frequency = (261.626f * (float)std::pow(2.f, note.key / 12 - 5)) * (1.f + (note.key % 12) / 12.f);
          float note_duration = std::max(0.f, (sample_idx / 22050.f) - duration(sequence, note.clock).asSeconds());

          samples[sample_idx] += std::sin(note_duration * note_frequency * (2.f * Math::Pi)) * (note.velocity / 127.f);// *std::max(0.f, 0.5f - note_duration);
        }
      }
    }
  }

  std::vector<int16_t>  samples16;
  for (const auto& sample : samples) {
    samples16.push_back((int16_t)(std::clamp(sample, -1.f, +1.f) * 32767.f));
  }

  std::cout << std::endl;

  ::toWave(Game::Config::ExecutablePath + "/generated.wav", samples16);
}

Game::Midi::Midi(const std::string& filename) :
  _soundfont(Game::Config::ExecutablePath + "/assets/levels/gzdoom.sf2")
{
  // TODO: remove this
  std::cout << std::endl << "[MIDI] filename: " << filename << ":" << std::endl << std::endl;

  // Load MIDI file
  load(filename);
}

void  Game::Midi::load(const std::string& filename)
{
  std::ifstream file(filename, std::ifstream::binary);

  // Check that file is correctly open
  if (file.good() == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load initial chunk
  loadHeader(file);

  // Load tracks
  loadTracks(file);
}

void  Game::Midi::loadHeader(std::ifstream& file)
{
  Game::Midi::MidiHeader  header = {};

  // Read first header
  read(file, &header);

  // Swap endianness
  header.size = Game::Midi::swap_endianness(header.size);

  // Check for invalid header
  if (header.type != Game::Midi::str_to_key("MThd") || header.size != 6)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  uint16_t  time = 0;

  // Read header datas
  read(file, &_format);
  read(file, &_trackNumber);
  read(file, &time);
  
  // Swap endianness
  _format = Game::Midi::swap_endianness(_format);
  _trackNumber = Game::Midi::swap_endianness(_trackNumber);

  // Check header infos
  if (_format > Game::Midi::MidiTrackFormat::SimultaneousTracks ||
    (_format == Game::Midi::MidiTrackFormat::SingleTrack && _trackNumber != 1) ||
    _trackNumber < 1)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // SMPTE time format
  if ((int)((uint8_t*)&time)[0] & 0b10000000) {
    uint16_t  smpte = (int)((uint8_t*)&time)[0] & 0b01111111;
    uint16_t  tick = (int)((uint8_t*)&time)[1];

    _timeFormat = Game::Midi::MidiTimeFormat::SMPTE;
    _timeValue = smpte * tick;

    std::cout << (int)((uint8_t*)&time)[0] << " " << (int)((uint8_t*)&time)[1] << std::endl;

    // TODO: test SMPTE time format
  }

  // Quarter-note time format
  else {
    _timeFormat = Game::Midi::MidiTimeFormat::QuarterNote;
    _timeValue = Game::Midi::swap_endianness((uint16_t)(time)) & 0b0111111111111111;
  }

  // TODO: remove this
  std::cout
    << "  [MTHD] format:      " << _format << std::endl
    << "  [MTHD] tracks:      " << _trackNumber << std::endl
    << "  [MTHD] time format: " << _timeFormat << std::endl
    << "  [MTHD] time value:  " << _timeValue << std::endl
    ;
}

void  Game::Midi::loadTracks(std::ifstream& file)
{
  // Push only track for non multiple tracks format
  if (_format != Game::Midi::MidiTrackFormat::MultipleTracks)
    sequences.push_back({});

  // Read each track
  for (unsigned int track = 0; track < _trackNumber;) {
    Game::Midi::MidiHeader  header = {};

    // Read chunk header
    read(file, &header);

    // Swap endianness
    header.size = Game::Midi::swap_endianness(header.size);

    // Skip unknow chunk
    if (header.type != Game::Midi::str_to_key("MTrk")) {
      std::cerr << "[Game::Midi::load]: Warning, unknow chunk type '" << Game::Midi::key_to_str(header.type) << "' (chunk ignored)." << std::endl;
      file.seekg(header.size, file.cur);
      continue;
    }

    // In multiple tracks format, create a new track
    if (_format == Game::Midi::MidiTrackFormat::MultipleTracks)
      sequences.push_back({});

    // Load track
    loadTrack(file, header, sequences.back(), (_format == Game::Midi::MidiTrackFormat::MultipleTracks) ? 0 : track);
    track++;
  }

  // TODO: remove this
  generate(sequences.front());
}

void  Game::Midi::loadTrack(std::ifstream& file, const Game::Midi::MidiHeader& header, Game::Midi::Sequence& sequence, std::size_t track)
{
  // TODO: remove this
  std::cout << std::endl << "  [SUBTRACK] " << track << std::endl;

  using Command = std::tuple<uint8_t, uint8_t, void(Game::Midi::*)(std::ifstream&, Game::Midi::Sequence&, std::size_t, uint8_t, std::size_t)>;

  // MIDI events
  static const std::array<Command, 23>  midi_commands =
  {
    // Channel Voice Messages
    Command{ 4, 0b10000000, &Game::Midi::loadTrackMidiNoteOff },
    Command{ 4, 0b10010000, &Game::Midi::loadTrackMidiNoteOn },
    Command{ 4, 0b10100000, &Game::Midi::loadTrackMidiPolyphonicKeyPressure },
    Command{ 4, 0b10110000, &Game::Midi::loadTrackMidiControlChange },
    Command{ 4, 0b11000000, &Game::Midi::loadTrackMidiProgramChange },
    Command{ 4, 0b11010000, &Game::Midi::loadTrackMidiChannelPressure },
    Command{ 4, 0b11100000, &Game::Midi::loadTrackMidiPitchWheelChange },

    // System Common Messages
    Command{ 8, 0b11110000, &Game::Midi::loadTrackSysexF0 },
    Command{ 8, 0b11110001, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11110010, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11110011, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11110100, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11110101, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11110110, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11110111, &Game::Midi::loadTrackSysexF7 },

    // System Real-Time Messages
    Command{ 8, 0b11111000, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11111001, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11111010, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11111011, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11111100, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11111101, &Game::Midi::loadTrackUndefined },
    Command{ 8, 0b11111110, &Game::Midi::loadTrackUndefined },

    // Meta events
    Command{ 8, 0b11111111, &Game::Midi::loadTrackMeta }
  };

  std::size_t clock = 0;
  uint8_t     status = 0;

  // Set flag end value
  sequence.metadata.end = (std::size_t)-1;

  // Read until track end event
  while (sequence.metadata.end == (std::size_t)-1)
  {
    // Get delta time of event
    clock += loadVariableLengthQuantity(file);

    uint8_t byte = 0;

    // Read next byte of file
    read(file, &byte);

    // New status
    if (byte & 0b10000000)
      status = byte;

    // Running command, repeat last status
    else
      file.seekg(-1, file.cur);

    // Find command to execute
    auto command = std::find_if(midi_commands.begin(), midi_commands.end(),
      [status](const auto& instruction) {
        auto s = status;

        // Keep only significant bits
        if (std::get<0>(instruction) < 8) {
          s = s >> (8 - std::get<0>(instruction));
          s = s << (8 - std::get<0>(instruction));
        }

        return std::get<1>(instruction) == s;
      });

    // Invalid command
    if (status == 0 || command == midi_commands.end())
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Execute command
    (*this.*std::get<2>(*command))(file, sequence, track, status, clock);
  }
}

void  Game::Midi::loadTrackSysexF0(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  // Get length of message
  uint64_t  length = loadVariableLengthQuantity(file);

  std::vector<uint8_t>  buffer(length);

  // Read message (ignored)
  read(file, buffer.data(), buffer.size());

  // TODO: remove this
  std::ios_base::fmtflags save(std::cout.flags());
  std::cout << "  [SYSEX] F0";
  for (const auto& byte : buffer)
    std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  std::cout << std::endl;
  std::cout.flags(save);
}

void  Game::Midi::loadTrackSysexF7(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  // Get length of message
  uint64_t  length = loadVariableLengthQuantity(file);

  std::vector<uint8_t>  buffer(length);

  // Read message (ignored)
  read(file, buffer.data(), buffer.size());

  // TODO: remove this
  std::ios_base::fmtflags save(std::cout.flags());
  std::cout << "  [SYSEX]";
  for (const auto& byte : buffer)
    std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  std::cout << std::endl;
  std::cout.flags(save);
}

void  Game::Midi::loadTrackMeta(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  using Command = std::pair<uint8_t, void(Game::Midi::*)(std::ifstream&, Game::Midi::Sequence&, std::size_t, std::size_t)>;

  // Meta events
  static const std::array<Command, 19> meta_commands =
  {
    Command{ 0x00, &Game::Midi::loadTrackMetaSequence },
    Command{ 0x01, &Game::Midi::loadTrackMetaTexts },
    Command{ 0x02, &Game::Midi::loadTrackMetaCopyright },
    Command{ 0x03, &Game::Midi::loadTrackMetaName },
    Command{ 0x04, &Game::Midi::loadTrackMetaInstrument },
    Command{ 0x05, &Game::Midi::loadTrackMetaLyrics },
    Command{ 0x06, &Game::Midi::loadTrackMetaMarkers },
    Command{ 0x07, &Game::Midi::loadTrackMetaCues },
    Command{ 0x08, &Game::Midi::loadTrackMetaProgram },
    Command{ 0x09, &Game::Midi::loadTrackMetaDevices },
    Command{ 0x20, &Game::Midi::loadTrackMetaChannels },
    Command{ 0x21, &Game::Midi::loadTrackMetaPorts },
    Command{ 0x2F, &Game::Midi::loadTrackMetaEnd },
    Command{ 0x4B, &Game::Midi::loadTrackMetaTags },
    Command{ 0x51, &Game::Midi::loadTrackMetaTempos },
    Command{ 0x54, &Game::Midi::loadTrackMetaSmpte },
    Command{ 0x58, &Game::Midi::loadTrackMetaSignatures },
    Command{ 0x59, &Game::Midi::loadTrackMetaKeys },
    Command{ 0x7F, &Game::Midi::loadTrackMetaDatas }
  };
  
  uint8_t meta = 0;

  // Read Meta instruction byte
  read(file, &meta);

  // TODO: remove this
  std::ios_base::fmtflags save(std::cout.flags());
  std::cout << "  [META][" << std::hex << std::setw(2) << std::setfill('0') << (int)meta << "] " << std::flush;
  std::cout.flags(save);

  // Find command to execute
  auto command = std::find_if(meta_commands.begin(), meta_commands.end(),
    [meta](const auto& instruction) {
      return std::get<0>(instruction) == meta;
    });

  // Invalid command
  if (command == meta_commands.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Execute command
  (*this.*std::get<1>(*command))(file, sequence, track, clock);
}

void  Game::Midi::loadTrackMetaName(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  std::string name = loadText(file);

  // Sequence name
  if (track == 0)
    sequence.metadata.name = name;

  // Assign name to track
  sequence.tracks[track].name = name;

  // TODO: remove this
  if (track == 0)
    std::cout << "sequence name: " << sequence.metadata.name << std::endl;
  else
    std::cout << "track " << track << " name: " << sequence.tracks[track].name << std::endl;
}

void  Game::Midi::loadTrackMetaCopyright(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.copyright = loadText(file);

  // TODO: remove this
  std::cout << "copyright: " << sequence.metadata.copyright << std::endl;
}

void  Game::Midi::loadTrackMetaInstrument(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.instrument = loadText(file);

  // TODO: remove this
  std::cout << "instrument: " << sequence.metadata.instrument << std::endl;
}

void  Game::Midi::loadTrackMetaLyrics(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.lyrics.push_back({ clock, loadText(file) });

  // TODO: remove this
  std::cout << "lyric: '" << sequence.metadata.lyrics.back().second << "' at " << sequence.metadata.lyrics.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaMarkers(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.markers.push_back({ clock, loadText(file) });

    // TODO: remove this
    std::cout << "marker: '" << sequence.metadata.markers.back().second << "' at " << sequence.metadata.markers.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaCues(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.cues.push_back({ clock, loadText(file) });

  // TODO: remove this
  std::cout << "cue: '" << sequence.metadata.cues.back().second << "' at " << sequence.metadata.cues.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaTexts(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.texts.push_back({ clock, loadText(file) });

  // TODO: remove this
  std::cout << "text: '" << sequence.metadata.texts.back().second << "' at " << sequence.metadata.texts.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaSequence(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;

  // Read sequence mode
  read(file, &mode);

  // Default mode
  if (mode == 0x00)
    sequence.metadata.sequence = (uint16_t)(sequences.size() - 1);

  // Get value from file
  else if (mode == 0x02) {
    read(file, &sequence.metadata.sequence);
    sequence.metadata.sequence = Game::Midi::swap_endianness(sequence.metadata.sequence);
  }

  // Unsupported
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // TODO: remove this
  std::cout << "sequence: " << sequence.metadata.sequence << std::endl;
}

void  Game::Midi::loadTrackMetaTempos(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t   mode = 0;
  uint32_t  tempo = 0;

  // Get mode
  read(file, &mode);

  // Invalid mode
  if (mode != 0x03)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  read(file, ((char*)&tempo) + 1, 3);
  sequence.metadata.tempos.push_back({ clock, Game::Midi::swap_endianness(tempo) });

  // TODO: remove this
  std::cout << "tempo: " << 60000000 / sequence.metadata.tempos.back().second << " BPM at " << sequence.metadata.tempos.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaSmpte(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;

  // Get mode
  read(file, &mode);

  // Invalid mode
  if (mode != 0x05)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  read(file, &sequence.metadata.smpte.hours);
  read(file, &sequence.metadata.smpte.minutes);
  read(file, &sequence.metadata.smpte.seconds);
  read(file, &sequence.metadata.smpte.frames);
  read(file, &sequence.metadata.smpte.fractionals);

  const std::array<uint8_t, 4>  framerates = { 24, 25, 30, 30 };

  // Extract framerate from bit 6-5 of hours byte
  sequence.metadata.smpte.framerate = framerates[((sequence.metadata.smpte.hours & 0b01100000) >> 5)];
  sequence.metadata.smpte.hours = (sequence.metadata.smpte.hours & 0b00011111);

  // TODO: remove this
  std::cout << "smpte: "
    << (int)sequence.metadata.smpte.hours << "h "
    << (int)sequence.metadata.smpte.minutes << "m "
    << (int)sequence.metadata.smpte.seconds << "s "
    << (int)sequence.metadata.smpte.frames << "/" << (int)sequence.metadata.smpte.framerate << "f "
    << (int)sequence.metadata.smpte.fractionals << " 100e"
    << std::endl;
}

void  Game::Midi::loadTrackMetaSignatures(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t                                   mode = 0;
  Game::Midi::Sequence::MetaData::Signature signature = {};
  
  // Get mode
  read(file, &mode);

  // Invalid mode
  if (mode != 0x04)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  read(file, &signature.numerator);
  read(file, &signature.denominator);
  read(file, &signature.metronome);
  read(file, &signature.quarter);

  // Denominator is given as a power of 2
  signature.denominator = (uint8_t)std::pow(2, signature.denominator);

  sequence.metadata.signatures.push_back({ clock, signature });

  // TODO: remove this
  std::cout << "signature: " << (int)sequence.metadata.signatures.back().second.numerator << "/" << (int)sequence.metadata.signatures.back().second.denominator << " at " << sequence.metadata.signatures.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaKeys(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t                             mode = 0;
  Game::Midi::Sequence::MetaData::Key key = {};

  // Get mode
  read(file, &mode);

  // Invalid mode
  if (mode != 0x02)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  read(file, &key.key);
  read(file, &key.major);

  sequence.metadata.keys.push_back({ clock, key });

  // TODO: remove this
  std::cout << "key: " << (int)sequence.metadata.keys.back().second.key << " " << (sequence.metadata.keys.back().second.major ? "major" : "minor") << " at " << sequence.metadata.keys.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaProgram(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.program = loadText(file);

  // TODO: remove this
  std::cout << "program: " << sequence.metadata.program << std::endl;
}

void  Game::Midi::loadTrackMetaDevices(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.devices.push_back({ clock, loadText(file) });

  // TODO: remove this
  std::cout << "device: '" << sequence.metadata.devices.back().second << "' at " << sequence.metadata.devices.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaChannels(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;
  uint8_t channel = 0;
  
  // Get mode
  read(file, &mode);

  // Invalid mode
  if (mode != 0x01)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  read(file, &channel);
  std::cerr << "[Game::Midi::load]: Warning, ignored MIDI track metadata (channel prefix " << (int)channel << ")." << std::endl;
}

void  Game::Midi::loadTrackMetaPorts(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;
  uint8_t port = 0;

  // Get mode
  read(file, &mode);

  // Invalid mode
  if (mode != 0x01)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  read(file, &port);
  std::cerr << "[Game::Midi::load]: Warning, ignored MIDI track metadata (MIDI port " << (int)port << ")." << std::endl;
}

void  Game::Midi::loadTrackMetaDatas(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint64_t  length(loadVariableLengthQuantity(file));

  sequence.metadata.datas.resize(length, 0);
  read(file, sequence.metadata.datas.data(), sequence.metadata.datas.size());

  // TODO: remove this
  std::cout << "datas: " << length << " bytes" << std::endl;
}

void  Game::Midi::loadTrackMetaTags(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint64_t                            length(loadVariableLengthQuantity(file));
  std::vector<char>                   text(length + 1, 0);
  Game::Midi::Sequence::MetaData::Tag tag;

  read(file, &tag);
  read(file, text.data(), text.size() - 1);
  sequence.metadata.tags[tag] = text.data();

  // TODO: remove this
  std::cout << "tag: [" << (int)tag << "] '" << sequence.metadata.tags[tag] << "'" << std::endl;
}

void  Game::Midi::loadTrackMetaEnd(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;

  // Get mode
  read(file, &mode);

  // Invalid mode
  if (mode != 0x00)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Save end time
  sequence.metadata.end = (sequence.metadata.end == (std::size_t)-1) ? clock : std::max(sequence.metadata.end, clock);

  // TODO: remove this
  std::cout << "end clock: " << clock << std::endl;
}

void  Game::Midi::loadTrackMidiNoteOff(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t key = 0;
  int8_t  velocity = 0;
  
  // Get instruction parameters
  read(file, &key);
  read(file, &velocity);

  // Add new key released
  sequence.tracks[track].channel[channel].note.push_back({ clock, { key, (int8_t)-velocity } });

  // Force key pressure to 0
  sequence.tracks[track].channel[channel].polyphonic.push_back({ clock, { key, 0 } });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] note off at " << clock << ":" << std::endl
    << "    channel:  " << (int)channel << std::endl
    << "    key:      " << (int)key << std::endl
    << "    velocity: " << (int)velocity << std::endl;
}

void  Game::Midi::loadTrackMidiNoteOn(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t key = 0;
  int8_t  velocity = 0;

  // Get instruction parameters
  read(file, &key);
  read(file, &velocity);

  // Add new key press
  sequence.tracks[track].channel[channel].note.push_back({ clock, { key, (int8_t)+velocity } });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] note on at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    key:        " << (int)key << std::endl
    << "    velocity:   " << (int)velocity << std::endl;
}

void  Game::Midi::loadTrackMidiPolyphonicKeyPressure(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t key = 0;
  uint8_t pressure = 0;

  // Get instruction parameters
  read(file, &key);
  read(file, &pressure);

  // Add new polyphonic key pressure
  sequence.tracks[track].channel[channel].polyphonic.push_back({ clock, { key, pressure } });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] polyphonic key pressure at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    key:        " << (int)key << std::endl
    << "    pressure:   " << (int)pressure << std::endl;
}

void  Game::Midi::loadTrackMidiControlChange(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t controller = 0;
  uint8_t value = 0;

  // Get instruction parameters
  read(file, &controller);
  read(file, &value);

  // New new controller value
  sequence.tracks[track].channel[channel].controller[(Game::Midi::Sequence::Track::Channel::Controller)controller].push_back({ clock, value });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] control change at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    controller: " << (int)controller << std::endl
    << "    value:      " << (int)value << std::endl;
}

void  Game::Midi::loadTrackMidiProgramChange(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t program = 0;

  // Get instruction parameters
  read(file, &program);

  // Add program change
  sequence.tracks[track].channel[channel].program.push_back({ clock, program });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] program change at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    program:    " << (int)program << std::endl;
}

void  Game::Midi::loadTrackMidiChannelPressure(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t pressure = 0;

  // Get instruction parameters
  read(file, &pressure);

  // Add new polyphonic key pressure for every key of the channel
  for (uint8_t key = 0; key < 128; key++)
    sequence.tracks[track].channel[channel].polyphonic.push_back({ clock, { key, pressure } });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] channel pressure at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    pressure:   " << (int)pressure << std::endl;
}

void  Game::Midi::loadTrackMidiPitchWheelChange(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t   channel = status & 0b00001111;
  uint16_t  pitch = 0;

  // Get instruction parameters
  read(file, &pitch);

  // Only keep significant bits
  pitch = (((uint8_t*)&pitch)[0] & 0b01111111) * 128 + (((uint8_t*)&pitch)[1] & 0b01111111);

  // Add new pitch wheel value
  sequence.tracks[track].channel[channel].pitch.push_back({ clock, pitch });
  
  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] channel pitch wheel change at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    pitch:      " << (int)pitch << std::endl;
}

void  Game::Midi::loadTrackUndefined(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  std::vector<uint8_t>  unused(1, 0);

  // Skip unused bytes
  read(file, unused.data(), unused.size());

  // Display instruction byte
  std::ios_base::fmtflags save(std::cerr.flags());
  std::cerr << "[Game::Midi::load]: Warning, undefined instruction '" << std::hex << std::setw(2) << std::setfill('0') << (int)status << "' (1 bytes skipped)." << std::endl;
  std::cerr.flags(save);
}

uint64_t  Game::Midi::loadVariableLengthQuantity(std::ifstream& file)
{
  uint64_t  result = 0;

  while (true) {
    uint8_t byte = 0;

    // Read byte from file
    read(file, &byte);

    // Number is the first 7 bits
    result = (result * 128) + (byte & 0b01111111);

    // Stop if final bit reached
    if ((byte & 0b10000000) == 0)
      break;
  }

  return result;
}

std::string Game::Midi::loadText(std::ifstream& file)
{
  uint64_t                              length(loadVariableLengthQuantity(file));
  std::vector<char>                     str(length + 1, '\0');

  // Read string
  read(file, str.data(), str.size() - 1);

  return str.data();
}

sf::Time  Game::Midi::duration(const Game::Midi::Sequence& sequence, std::size_t clock)
{
  // Translate time according to format
  switch (_timeFormat)
  {
  case Game::Midi::QuarterNote:
  {
    // TODO: support multiple tempos through track
    uint32_t  tempo = ((sequence.metadata.tempos.empty() == true) ? (120) : (sequence.metadata.tempos.front().second));

    return sf::seconds(((float)clock / (float)_timeValue) * (60.f / (60000000.f / (float)tempo)));
  }
  case Game::Midi::SMPTE:
    return sf::seconds((float)clock / (float)_timeValue);
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    break;
  }
}

Game::Midi::SoundFont::SoundFont(const std::string& filename) :
  info(),
  samples16(),
  samples24()
{
  // Load file
  load(filename);
}

void  Game::Midi::SoundFont::load(const std::string& filename)
{
  std::ifstream file(filename, std::ifstream::binary);

  // Check that file is correctly open
  if (file.good() == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  SoundFont::Sf2Header  header = {};

  // Read RIFF header
  read(file, &header);

  // TODO: remove this
  std::cout
    << "[SOUNDFONT] filename: " << filename << std::endl
    << std::endl
    << "  [HEADER] magic: " << Game::Midi::key_to_str(header.magic) << std::endl
    << "  [HEADER] size:  " << header.size << std::endl
    << "  [HEADER] type:  " << Game::Midi::key_to_str(header.type) << std::endl
    ;

  // Check header data
  if (std::memcmp(&header.magic, "RIFF", sizeof(header.magic)) != 0 ||
    std::memcmp(&header.type, "sfbk", sizeof(header.magic)) != 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load sections of SoundFont
  loadSections(file, header);
}

void  Game::Midi::SoundFont::loadSections(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header)
{
  SoundFont::Sf2SectionHeader section = {};

  for (std::streampos section_position = sizeof(header); section_position < header.size + 8; section_position += section.size + 8)
  {
    section = {};

    // Read section header
    file.seekg(section_position, file.beg);
    read(file, &section);

    // TODO: remove this
    std::cout
      << std::endl
      << "  [SECTION] type: " << Game::Midi::key_to_str(section.type) << std::endl
      << "  [SECTION] size: " << section.size << std::endl
      << "  [SECTION] name: " << Game::Midi::key_to_str(section.name) << std::endl
      ;

    // Matching sections
    std::unordered_map<uint32_t, std::function<void()>> section_commands = {
      { Game::Midi::str_to_key("LIST"), std::bind(&Game::Midi::SoundFont::loadSectionList, this, std::ref(file), std::ref(header), std::ref(section), section_position) }
    };

    auto command = section_commands.find(section.type);

    // Execute matching command
    if (command != section_commands.end())
      command->second();
    else
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow section type '" << Game::Midi::key_to_str(section.type) << "' (section ignored)." << std::endl;
  }
}

void  Game::Midi::SoundFont::loadSectionList(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position)
{
  // Matching sections
  std::unordered_map<uint32_t, std::function<void()>> list_commands = {
    { Game::Midi::str_to_key("INFO"), std::bind(&SoundFont::loadSectionListInfo, this, std::ref(file), std::ref(header), std::ref(section), position) },
    { Game::Midi::str_to_key("sdta"), std::bind(&SoundFont::loadSectionListSdta, this, std::ref(file), std::ref(header), std::ref(section), position) },
    { Game::Midi::str_to_key("pdta"), std::bind(&SoundFont::loadSectionListPdta, this, std::ref(file), std::ref(header), std::ref(section), position) }
  };

  auto command = list_commands.find(section.name);

  // Execute matching command
  if (command != list_commands.end())
    command->second();
  else
    std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow section name '" << Game::Midi::key_to_str(section.name) << "' (section ignored)." << std::endl;
}

void  Game::Midi::SoundFont::loadSectionListInfo(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position)
{
  SoundFont::Sf2SubsectionHeader  info = {};

  for (std::streampos info_position = sizeof(section); info_position < section.size + 8; info_position += sizeof(info) + info.size)
  {
    info = {};

    // Read info header
    file.seekg(position + info_position, file.beg);
    read(file, &info);
    
    // Matching subsections
    std::unordered_map<uint32_t, std::function<void()>> info_commands = {
      { Game::Midi::str_to_key("ifil"), std::bind(&SoundFont::loadSectionListInfoIfil, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("isng"), std::bind(&SoundFont::loadSectionListInfoIsng, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("INAM"), std::bind(&SoundFont::loadSectionListInfoInam, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("IROM"), std::bind(&SoundFont::loadSectionListInfoIrom, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("VER"), std::bind(&SoundFont::loadSectionListInfoVer, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("ICRD"), std::bind(&SoundFont::loadSectionListInfoIcrd, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("IENG"), std::bind(&SoundFont::loadSectionListInfoIeng, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("IPRD"), std::bind(&SoundFont::loadSectionListInfoIprd, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("ICOP"), std::bind(&SoundFont::loadSectionListInfoIcop, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("ICMT"), std::bind(&SoundFont::loadSectionListInfoIcmt, this, std::ref(file), std::ref(info)) },
      { Game::Midi::str_to_key("ISFT"), std::bind(&SoundFont::loadSectionListInfoIsft, this, std::ref(file), std::ref(info)) }
    };

    auto  command = info_commands.find(info.name);

    // Execute matching command
    if (command != info_commands.end())
      command->second();
    else
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow info name '" << Game::Midi::key_to_str(info.name) << "' (info ignored)." << std::endl;
  }

  // TODO: remove this
  std::cout
    << "    [INFO] version: " << this->info.version.first << "." << this->info.version.second << std::endl
    << "    [INFO] engine: " << this->info.engine << std::endl
    << "    [INFO] name: " << this->info.name << std::endl
    << "    [INFO] rom: " << this->info.rom << std::endl
    << "    [INFO] rom version: " << this->info.romVersion.first << "." << this->info.romVersion.second << std::endl
    << "    [INFO] creation: " << this->info.creation << std::endl
    << "    [INFO] author: " << this->info.author << std::endl
    << "    [INFO] product: " << this->info.product << std::endl
    << "    [INFO] copyright: " << this->info.copyright << std::endl
    << "    [INFO] comment: " << this->info.comment << std::endl
    << "    [INFO] tool: " << this->info.tool << std::endl
    ;
}

void  Game::Midi::SoundFont::loadSectionListInfoIfil(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // SoundFont version (major/minor)
  read(file, &this->info.version.first);
  read(file, &this->info.version.second);
}

void  Game::Midi::SoundFont::loadSectionListInfoIsng(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Optimized audio engine
  this->info.engine = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoInam(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Name of sound bank
  this->info.name = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIrom(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Name of ROM to use
  this->info.rom = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoVer(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // ROM version to use
  read(file, &this->info.romVersion.first);
  read(file, &this->info.romVersion.second);
}

void  Game::Midi::SoundFont::loadSectionListInfoIcrd(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Date of creation
  this->info.creation = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIeng(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Name of the author(s)
  this->info.author = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIprd(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Name of the target product
  this->info.author = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIcop(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Copyright license
  this->info.copyright = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIcmt(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Comment
  this->info.comment = loadString(file, info.size);
}

void  Game::Midi::SoundFont::loadSectionListInfoIsft(std::ifstream& file, const Game::Midi::SoundFont::Sf2SubsectionHeader& info)
{
  // Tool used to create the sound bank
  this->info.tool = loadString(file, info.size);
}

std::string Game::Midi::SoundFont::loadString(std::ifstream& file, std::size_t size)
{
  std::vector<char> str(size + 1, '\0');
  
  // Read string
  read(file, str.data(), str.size() - 1);

  return str.data();
}

void  Game::Midi::SoundFont::loadSectionListSdta(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position)
{
  Game::Midi::SoundFont::Sf2SubsectionHeader  sdta = {};

  for (std::streampos sdta_position = sizeof(section); sdta_position < section.size + 8; sdta_position += sizeof(sdta) + sdta.size)
  {
    sdta = {};

    // Read info header
    file.seekg(position + sdta_position, file.beg);
    read(file, &sdta);

    // Matching subsections
    std::unordered_map<uint32_t, std::function<void()>> sdta_commands = {
      { Game::Midi::str_to_key("smpl"), std::bind(&Game::Midi::SoundFont::loadSubsection<int16_t>, this, std::ref(file), sdta.size, std::ref(samples16)) },
      { Game::Midi::str_to_key("sm24"), std::bind(&Game::Midi::SoundFont::loadSubsection<int8_t>, this, std::ref(file), sdta.size, std::ref(samples24)) }
    };

    auto command = sdta_commands.find(sdta.name);

    // Execute matching regex if only one found
    if (command != sdta_commands.end())
      command->second();
    else
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow sdta name '" << Game::Midi::key_to_str(sdta.name) << "' (sdta ignored)." << std::endl;
  }

  // TODO: remove this
  std::cout
    << "    [SDTA] samples16: " << samples16.size() << std::endl
    << "    [SDTA] samples24: " << samples24.size() << std::endl
    ;
}

void  Game::Midi::SoundFont::loadSectionListPdta(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::streampos position)
{
  SoundFont::Sf2SubsectionHeader  pdta = {};

  std::vector<SoundFont::Sf2Preset>     presetHeaders;        // Header of each preset
  std::vector<SoundFont::Sf2Bag>        presetBags;           // List of preset bags
  std::vector<SoundFont::Sf2GenList>    presetGenerators;     // List of preset generators
  std::vector<SoundFont::Sf2ModList>    presetModulators;     // List of preset modulators
  std::vector<SoundFont::Sf2Instrument> instrumentHeaders;    // Header of each instrument
  std::vector<SoundFont::Sf2Bag>        instrumentBags;       // List of instrument bags
  std::vector<SoundFont::Sf2GenList>    instrumentGenerators; // List of instrument generators
  std::vector<SoundFont::Sf2ModList>    instrumentModulators; // List of instrument modulators
  std::vector<SoundFont::Sf2Sample>     sampleHeaders;        // Description of each sample

  for (std::streampos pdta_position = sizeof(section); pdta_position < section.size + 8; pdta_position += sizeof(pdta) + pdta.size)
  {
    pdta = {};

    // Read info header
    file.seekg(position + pdta_position, file.beg);
    read(file, &pdta);

    // Matching subsections
    std::unordered_map<uint32_t, std::function<void()>> pdta_commands = {
      { Game::Midi::str_to_key("phdr"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Preset>, this, std::ref(file), pdta.size, std::ref(presetHeaders)) },
      { Game::Midi::str_to_key("pbag"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Bag>, this, std::ref(file), pdta.size, std::ref(presetBags)) },
      { Game::Midi::str_to_key("pmod"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2ModList>, this, std::ref(file), pdta.size, std::ref(presetModulators)) },
      { Game::Midi::str_to_key("pgen"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2GenList>, this, std::ref(file), pdta.size, std::ref(presetGenerators)) },
      { Game::Midi::str_to_key("inst"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Instrument>, this, std::ref(file), pdta.size, std::ref(instrumentHeaders)) },
      { Game::Midi::str_to_key("ibag"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Bag>, this, std::ref(file), pdta.size, std::ref(instrumentBags)) },
      { Game::Midi::str_to_key("imod"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2ModList>, this, std::ref(file), pdta.size, std::ref(instrumentModulators)) },
      { Game::Midi::str_to_key("igen"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2GenList>, this, std::ref(file), pdta.size, std::ref(instrumentGenerators)) },
      { Game::Midi::str_to_key("shdr"), std::bind(&SoundFont::loadSubsection<SoundFont::Sf2Sample>, this, std::ref(file), pdta.size, std::ref(sampleHeaders)) }
    };

    auto command = pdta_commands.find(pdta.name);

    // Execute matching command
    if (command != pdta_commands.end())
      command->second();
    else
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow pdta name '" << Game::Midi::key_to_str(pdta.name) << "' (sdta ignored)." << std::endl;
  }

  // TODO: remove this
  std::cout
    << "    [PDTA] presets:               " << presetHeaders.size() << std::endl
    << "    [PDTA] preset bags:           " << presetBags.size() << std::endl
    << "    [PDTA] preset modulators:     " << presetModulators.size() << std::endl
    << "    [PDTA] preset generators:     " << presetGenerators.size() << std::endl
    << "    [PDTA] instruments:           " << instrumentHeaders.size() << std::endl
    << "    [PDTA] instrument bags:       " << instrumentBags.size() << std::endl
    << "    [PDTA] instrument modulators: " << instrumentModulators.size() << std::endl
    << "    [PDTA] instrument generators: " << instrumentGenerators.size() << std::endl
    << "    [PDTA] samples:               " << sampleHeaders.size() << std::endl
    ;

  // TODO: build presets, instruments and samples
}