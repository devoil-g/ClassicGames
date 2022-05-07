#include <fstream>
#include <cmath>
#include <array>
#include <vector>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <iomanip>
#include <tuple>
#include <algorithm>

#include "System/Audio/Midi.hpp"
#include "System/Config.hpp"
#include "System/Utilities.hpp"

Game::Audio::Midi::Midi(const std::string& filename)
{
  // TODO: remove this
#ifdef _DEBUG
  std::cout << std::endl << "[MIDI] filename: " << filename << ":" << std::endl << std::endl;
#endif

  // Load MIDI file
  load(filename);
}

void  Game::Audio::Midi::load(const std::string& filename)
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

void  Game::Audio::Midi::loadHeader(std::ifstream& file)
{
  Game::Audio::Midi::MidiHeader header = {};

  // Read first header
  Game::Utilities::read(file, &header);

  // Swap endianness
  header.size = Game::Utilities::swap_endianness(header.size);

  // Check for invalid header
  if (header.type != Game::Utilities::str_to_key<uint32_t>("MThd") || header.size != 6)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  uint16_t  time = 0;

  // Read header datas
  Game::Utilities::read(file, &_format);
  Game::Utilities::read(file, &_trackNumber);
  Game::Utilities::read(file, &time);
  
  // Swap endianness
  _format = Game::Utilities::swap_endianness(_format);
  _trackNumber = Game::Utilities::swap_endianness(_trackNumber);

  // Check header infos
  if (_format > Game::Audio::Midi::MidiTrackFormat::SimultaneousTracks ||
    (_format == Game::Audio::Midi::MidiTrackFormat::SingleTrack && _trackNumber != 1) ||
    _trackNumber < 1)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // SMPTE time format
  if ((int)((uint8_t*)&time)[0] & 0b10000000) {
    uint16_t  smpte = (int)((uint8_t*)&time)[0] & 0b01111111;
    uint16_t  tick = (int)((uint8_t*)&time)[1];

    _timeFormat = Game::Audio::Midi::MidiTimeFormat::SMPTE;
    _timeValue = smpte * tick;

    // TODO: test SMPTE time format
  }

  // Quarter-note time format
  else {
    _timeFormat = Game::Audio::Midi::MidiTimeFormat::QuarterNote;
    _timeValue = Game::Utilities::swap_endianness((uint16_t)(time)) & 0b0111111111111111;
  }

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "  [MTHD] format:      " << _format << std::endl
    << "  [MTHD] tracks:      " << _trackNumber << std::endl
    << "  [MTHD] time format: " << _timeFormat << std::endl
    << "  [MTHD] time value:  " << _timeValue << std::endl
    ;
#endif
}

void  Game::Audio::Midi::loadTracks(std::ifstream& file)
{
  // Push only track for non multiple tracks format
  if (_format != Game::Audio::Midi::MidiTrackFormat::MultipleTracks)
    sequences.push_back({});

  // Read each track
  for (unsigned int track = 0; track < _trackNumber;) {
    Game::Audio::Midi::MidiHeader header = {};

    // Read chunk header
    Game::Utilities::read(file, &header);

    // Swap endianness
    header.size = Game::Utilities::swap_endianness(header.size);

    // Skip unknow chunk
    if (header.type != Game::Utilities::str_to_key<uint32_t>("MTrk")) {
      std::cerr << "[Game::Midi::load]: Warning, unknow chunk type '" << Game::Utilities::key_to_str<uint32_t>(header.type) << "' (chunk ignored)." << std::endl;
      file.seekg(header.size, file.cur);
      continue;
    }

    // In multiple tracks format, create a new track
    if (_format == Game::Audio::Midi::MidiTrackFormat::MultipleTracks)
      sequences.push_back({});

    // Load track
    loadTrack(file, header, sequences.back(), (_format == Game::Audio::Midi::MidiTrackFormat::MultipleTracks) ? 0 : track);
    track++;
  }
}

void  Game::Audio::Midi::loadTrack(std::ifstream& file, const Game::Audio::Midi::MidiHeader& header, Game::Audio::Midi::Sequence& sequence, std::size_t track)
{
  // TODO: remove this
#ifdef _DEBUG
  std::cout << std::endl << "  [SUBTRACK] " << track << std::endl;
#endif

  using Command = std::tuple<uint8_t, uint8_t, void(Game::Audio::Midi::*)(std::ifstream&, Game::Audio::Midi::Sequence&, std::size_t, uint8_t, std::size_t)>;

  // MIDI events
  static const std::array<Command, 23>  midi_commands =
  {
    // Channel Voice Messages
    Command{ 4, 0b10000000, &Game::Audio::Midi::loadTrackMidiNoteOff },
    Command{ 4, 0b10010000, &Game::Audio::Midi::loadTrackMidiNoteOn },
    Command{ 4, 0b10100000, &Game::Audio::Midi::loadTrackMidiPolyphonicKeyPressure },
    Command{ 4, 0b10110000, &Game::Audio::Midi::loadTrackMidiControlChange },
    Command{ 4, 0b11000000, &Game::Audio::Midi::loadTrackMidiProgramChange },
    Command{ 4, 0b11010000, &Game::Audio::Midi::loadTrackMidiChannelPressure },
    Command{ 4, 0b11100000, &Game::Audio::Midi::loadTrackMidiPitchWheelChange },

    // System Common Messages
    Command{ 8, 0b11110000, &Game::Audio::Midi::loadTrackSysexF0 },
    Command{ 8, 0b11110001, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11110010, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11110011, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11110100, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11110101, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11110110, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11110111, &Game::Audio::Midi::loadTrackSysexF7 },

    // System Real-Time Messages
    Command{ 8, 0b11111000, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11111001, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11111010, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11111011, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11111100, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11111101, &Game::Audio::Midi::loadTrackUndefined },
    Command{ 8, 0b11111110, &Game::Audio::Midi::loadTrackUndefined },

    // Meta events
    Command{ 8, 0b11111111, &Game::Audio::Midi::loadTrackMeta }
  };

  std::size_t clock = 0;
  uint8_t     status = 0;

  // Set flag end value
  sequence.metadata.end = std::numeric_limits<float>::quiet_NaN();

  // Read until track end event
  while (std::isnan(sequence.metadata.end) == true)
  {
    // Get delta time of event
    clock += loadVariableLengthQuantity(file);

    uint8_t byte = 0;

    // Read next byte of file
    Game::Utilities::read(file, &byte);

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

void  Game::Audio::Midi::loadTrackSysexF0(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  // Get length of message
  uint64_t  length = loadVariableLengthQuantity(file);

  std::vector<uint8_t>  buffer(length);

  // Read message (ignored)
  Game::Utilities::read(file, buffer.data(), buffer.size());

  // TODO: remove this
#ifdef _DEBUG
  std::ios_base::fmtflags save(std::cout.flags());
  std::cout << "  [SYSEX] F0";
  for (const auto& byte : buffer)
    std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  std::cout << std::endl;
  std::cout.flags(save);
#endif
}

void  Game::Audio::Midi::loadTrackSysexF7(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  // Get length of message
  uint64_t  length = loadVariableLengthQuantity(file);

  std::vector<uint8_t>  buffer(length);

  // Read message (ignored)
  Game::Utilities::read(file, buffer.data(), buffer.size());

  // TODO: remove this
#ifdef _DEBUG
  std::ios_base::fmtflags save(std::cout.flags());
  std::cout << "  [SYSEX]";
  for (const auto& byte : buffer)
    std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
  std::cout << std::endl;
  std::cout.flags(save);
#endif
}

void  Game::Audio::Midi::loadTrackMeta(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  using Command = std::pair<uint8_t, void(Game::Audio::Midi::*)(std::ifstream&, Game::Audio::Midi::Sequence&, std::size_t, std::size_t)>;

  // Meta events
  static const std::array<Command, 19> meta_commands =
  {
    Command{ 0x00, &Game::Audio::Midi::loadTrackMetaSequence },
    Command{ 0x01, &Game::Audio::Midi::loadTrackMetaTexts },
    Command{ 0x02, &Game::Audio::Midi::loadTrackMetaCopyright },
    Command{ 0x03, &Game::Audio::Midi::loadTrackMetaName },
    Command{ 0x04, &Game::Audio::Midi::loadTrackMetaInstrument },
    Command{ 0x05, &Game::Audio::Midi::loadTrackMetaLyrics },
    Command{ 0x06, &Game::Audio::Midi::loadTrackMetaMarkers },
    Command{ 0x07, &Game::Audio::Midi::loadTrackMetaCues },
    Command{ 0x08, &Game::Audio::Midi::loadTrackMetaProgram },
    Command{ 0x09, &Game::Audio::Midi::loadTrackMetaDevices },
    Command{ 0x20, &Game::Audio::Midi::loadTrackMetaChannels },
    Command{ 0x21, &Game::Audio::Midi::loadTrackMetaPorts },
    Command{ 0x2F, &Game::Audio::Midi::loadTrackMetaEnd },
    Command{ 0x4B, &Game::Audio::Midi::loadTrackMetaTags },
    Command{ 0x51, &Game::Audio::Midi::loadTrackMetaTempos },
    Command{ 0x54, &Game::Audio::Midi::loadTrackMetaSmpte },
    Command{ 0x58, &Game::Audio::Midi::loadTrackMetaSignatures },
    Command{ 0x59, &Game::Audio::Midi::loadTrackMetaKeys },
    Command{ 0x7F, &Game::Audio::Midi::loadTrackMetaDatas }
  };
  
  uint8_t meta = 0;

  // Read Meta instruction byte
  Game::Utilities::read(file, &meta);

  // TODO: remove this
#ifdef _DEBUG
  std::ios_base::fmtflags save(std::cout.flags());
  std::cout << "  [META][" << std::hex << std::setw(2) << std::setfill('0') << (int)meta << "] " << std::flush;
  std::cout.flags(save);
#endif

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

void  Game::Audio::Midi::loadTrackMetaName(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  std::string name = loadText(file);

  // Sequence name
  if (track == 0)
    sequence.metadata.name = name;

  // Assign name to track
  sequence.tracks[track].name = name;

  // TODO: remove this
#ifdef _DEBUG
  if (track == 0)
    std::cout << "sequence name: " << sequence.metadata.name << std::endl;
  else
    std::cout << "track " << track << " name: " << sequence.tracks[track].name << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaCopyright(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.copyright = loadText(file);

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "copyright: " << sequence.metadata.copyright << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaInstrument(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.instrument = loadText(file);

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "instrument: " << sequence.metadata.instrument << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaLyrics(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.lyrics.push_back({ duration(sequence, clock), loadText(file) });

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "lyric: '" << sequence.metadata.lyrics.back().second << "' at " << sequence.metadata.lyrics.back().first << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaMarkers(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.markers.push_back({ duration(sequence, clock), loadText(file) });

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "marker: '" << sequence.metadata.markers.back().second << "' at " << sequence.metadata.markers.back().first << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaCues(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.cues.push_back({ duration(sequence, clock), loadText(file) });

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "cue: '" << sequence.metadata.cues.back().second << "' at " << sequence.metadata.cues.back().first << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaTexts(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.texts.push_back({ duration(sequence, clock), loadText(file) });

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "text: '" << sequence.metadata.texts.back().second << "' at " << sequence.metadata.texts.back().first << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaSequence(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;

  // Read sequence mode
  Game::Utilities::read(file, &mode);

  // Default mode
  if (mode == 0x00)
    sequence.metadata.sequence = (uint16_t)(sequences.size() - 1);

  // Get value from file
  else if (mode == 0x02) {
    Game::Utilities::read(file, &sequence.metadata.sequence);
    sequence.metadata.sequence = Game::Utilities::swap_endianness(sequence.metadata.sequence);
  }

  // Unsupported
  else
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "sequence: " << sequence.metadata.sequence << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaTempos(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t   mode = 0;
  uint32_t  tempo = 0;

  // Get mode
  Game::Utilities::read(file, &mode);

  // Invalid mode
  if (mode != 0x03)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  Game::Utilities::read(file, ((char*)&tempo) + 1, 3);
  sequence.metadata.tempos.push_back({ clock, Game::Utilities::swap_endianness(tempo) });

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "tempo: " << 60000000 / sequence.metadata.tempos.back().second << " BPM at " << sequence.metadata.tempos.back().first << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaSmpte(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;

  // Get mode
  Game::Utilities::read(file, &mode);

  // Invalid mode
  if (mode != 0x05)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  Game::Utilities::read(file, &sequence.metadata.smpte.hours);
  Game::Utilities::read(file, &sequence.metadata.smpte.minutes);
  Game::Utilities::read(file, &sequence.metadata.smpte.seconds);
  Game::Utilities::read(file, &sequence.metadata.smpte.frames);
  Game::Utilities::read(file, &sequence.metadata.smpte.fractionals);

  const std::array<uint8_t, 4>  framerates = { 24, 25, 30, 30 };

  // Extract framerate from bit 6-5 of hours byte
  sequence.metadata.smpte.framerate = framerates[((sequence.metadata.smpte.hours & 0b01100000) >> 5)];
  sequence.metadata.smpte.hours = (sequence.metadata.smpte.hours & 0b00011111);

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "smpte: "
    << (int)sequence.metadata.smpte.hours << "h "
    << (int)sequence.metadata.smpte.minutes << "m "
    << (int)sequence.metadata.smpte.seconds << "s "
    << (int)sequence.metadata.smpte.frames << "/" << (int)sequence.metadata.smpte.framerate << "f "
    << (int)sequence.metadata.smpte.fractionals << " 100e"
    << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaSignatures(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t                                           mode = 0;
  Game::Audio::Midi::Sequence::MetaData::Signature  signature = {};
  
  // Get mode
  Game::Utilities::read(file, &mode);

  // Invalid mode
  if (mode != 0x04)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  Game::Utilities::read(file, &signature.numerator);
  Game::Utilities::read(file, &signature.denominator);
  Game::Utilities::read(file, &signature.metronome);
  Game::Utilities::read(file, &signature.quarter);

  // Denominator is given as a power of 2
  signature.denominator = (uint8_t)std::pow(2, signature.denominator);

  sequence.metadata.signatures.push_back({ duration(sequence, clock), signature });

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "signature: " << (int)sequence.metadata.signatures.back().second.numerator << "/" << (int)sequence.metadata.signatures.back().second.denominator << " at " << sequence.metadata.signatures.back().first << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaKeys(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t                                     mode = 0;
  Game::Audio::Midi::Sequence::MetaData::Key  key = {};

  // Get mode
  Game::Utilities::read(file, &mode);

  // Invalid mode
  if (mode != 0x02)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  Game::Utilities::read(file, &key.key);
  Game::Utilities::read(file, &key.major);

  sequence.metadata.keys.push_back({ duration(sequence, clock), key });

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "key: " << (int)sequence.metadata.keys.back().second.key << " " << (sequence.metadata.keys.back().second.major ? "major" : "minor") << " at " << sequence.metadata.keys.back().first << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaProgram(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.program = loadText(file);

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "program: " << sequence.metadata.program << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaDevices(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  sequence.metadata.devices.push_back({ duration(sequence, clock), loadText(file) });

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "device: '" << sequence.metadata.devices.back().second << "' at " << sequence.metadata.devices.back().first << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaChannels(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;
  uint8_t channel = 0;
  
  // Get mode
  Game::Utilities::read(file, &mode);

  // Invalid mode
  if (mode != 0x01)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  Game::Utilities::read(file, &channel);
  std::cerr << "[Game::Midi::load]: Warning, ignored MIDI track metadata (channel prefix " << (int)channel << ")." << std::endl;
}

void  Game::Audio::Midi::loadTrackMetaPorts(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;
  uint8_t port = 0;

  // Get mode
  Game::Utilities::read(file, &mode);

  // Invalid mode
  if (mode != 0x01)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  Game::Utilities::read(file, &port);
  std::cerr << "[Game::Midi::load]: Warning, ignored MIDI track metadata (MIDI port " << (int)port << ")." << std::endl;
}

void  Game::Audio::Midi::loadTrackMetaDatas(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint64_t  length(loadVariableLengthQuantity(file));

  sequence.metadata.datas.resize(length, 0);
  Game::Utilities::read(file, sequence.metadata.datas.data(), sequence.metadata.datas.size());

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "datas: " << length << " bytes" << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaTags(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint64_t                                    length(loadVariableLengthQuantity(file));
  std::vector<char>                           text(length + 1, 0);
  Game::Audio::Midi::Sequence::MetaData::Tag  tag;

  Game::Utilities::read(file, &tag);
  Game::Utilities::read(file, text.data(), text.size() - 1);
  sequence.metadata.tags[tag] = text.data();

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "tag: [" << (int)tag << "] '" << sequence.metadata.tags[tag] << "'" << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMetaEnd(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t mode = 0;
  float   seconds = duration(sequence, clock);

  // Get mode
  Game::Utilities::read(file, &mode);

  // Invalid mode
  if (mode != 0x00)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Save end time
  sequence.metadata.end = (std::isnan(sequence.metadata.end) == true) ? seconds : std::max(sequence.metadata.end, seconds);

  // TODO: remove this
#ifdef _DEBUG
  std::cout << "end clock: " << seconds << "s" << std::endl << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMidiNoteOff(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t key = 0;
  int8_t  velocity = 0;
  
  // Get instruction parameters
  Game::Utilities::read(file, &key);
  Game::Utilities::read(file, &velocity);

  Game::Audio::Midi::Sequence::Track::Channel::Event  event = {};

  event.clock = duration(sequence, clock);
  event.type = Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventKey;
  event.data.note.key = key;
  event.data.note.velocity = -velocity;

  // Add new key released
  sequence.tracks[track].channel[channel].events.push_back(event);

  event.type = Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventPolyphonicKey;
  event.data.polyphonic.key = key;
  event.data.polyphonic.pressure = 0;

  // Force key pressure to 0
  sequence.tracks[track].channel[channel].events.push_back(event);

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "  [MIDI] note off at " << clock << ":" << std::endl
    << "    channel:  " << (int)channel << std::endl
    << "    key:      " << (int)key << std::endl
    << "    velocity: " << (int)velocity << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMidiNoteOn(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t key = 0;
  int8_t  velocity = 0;

  // Get instruction parameters
  Game::Utilities::read(file, &key);
  Game::Utilities::read(file, &velocity);

  Game::Audio::Midi::Sequence::Track::Channel::Event  event = {};

  event.clock = duration(sequence, clock);
  event.type = Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventKey;
  event.data.note.key = key;
  event.data.note.velocity = +velocity;

  // Add new key pressed
  sequence.tracks[track].channel[channel].events.push_back(event);

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "  [MIDI] note on at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    key:        " << (int)key << std::endl
    << "    velocity:   " << (int)velocity << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMidiPolyphonicKeyPressure(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t key = 0;
  uint8_t pressure = 0;

  // Get instruction parameters
  Game::Utilities::read(file, &key);
  Game::Utilities::read(file, &pressure);

  Game::Audio::Midi::Sequence::Track::Channel::Event  event = {};

  event.clock = duration(sequence, clock);
  event.type = Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventPolyphonicKey;
  event.data.polyphonic.key = key;
  event.data.polyphonic.pressure = pressure;

  // Add new polyphonic key pressure
  sequence.tracks[track].channel[channel].events.push_back(event);

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "  [MIDI] polyphonic key pressure at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    key:        " << (int)key << std::endl
    << "    pressure:   " << (int)pressure << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMidiControlChange(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t controller = 0;
  uint8_t value = 0;

  // Get instruction parameters
  Game::Utilities::read(file, &controller);
  Game::Utilities::read(file, &value);

  Game::Audio::Midi::Sequence::Track::Channel::Event  event = {};

  event.clock = duration(sequence, clock);
  event.type = Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventController;
  event.data.controller.type = (Game::Audio::Midi::Sequence::Track::Channel::Controller::Type)controller;
  event.data.controller.value = value;

  // New controller value
  sequence.tracks[track].channel[channel].events.push_back(event);

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "  [MIDI] control change at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    controller: " << (int)controller << std::endl
    << "    value:      " << (int)value << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMidiProgramChange(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t program = 0;

  // Get instruction parameters
  Game::Utilities::read(file, &program);

  Game::Audio::Midi::Sequence::Track::Channel::Event  event = {};

  event.clock = duration(sequence, clock);
  event.type = Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventProgram;
  event.data.program = program;

  // Add program change
  sequence.tracks[track].channel[channel].events.push_back(event);

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "  [MIDI] program change at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    program:    " << (int)program << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMidiChannelPressure(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t channel = status & 0b00001111;
  uint8_t pressure = 0;

  // Get instruction parameters
  Game::Utilities::read(file, &pressure);

  Game::Audio::Midi::Sequence::Track::Channel::Event  event = {};

  event.clock = duration(sequence, clock);
  event.type = Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventPolyphonicKey;
  event.data.polyphonic.pressure = pressure;

  // Add new polyphonic key pressure for every key of the channel
  for (event.data.polyphonic.key = 0; event.data.polyphonic.key < 128; event.data.polyphonic.key++)
    sequence.tracks[track].channel[channel].events.push_back(event);

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "  [MIDI] channel pressure at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    pressure:   " << (int)pressure << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackMidiPitchWheelChange(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  uint8_t   channel = status & 0b00001111;
  uint16_t  pitch = 0;

  // Get instruction parameters
  Game::Utilities::read(file, &pitch);

  // Only keep significant bits
  pitch = (((uint8_t*)&pitch)[1] & 0b01111111) * 128 + (((uint8_t*)&pitch)[0] & 0b01111111);

  Game::Audio::Midi::Sequence::Track::Channel::Event  event = {};

  event.clock = duration(sequence, clock);
  event.type = Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventPitch;
  event.data.pitch = pitch;

  // Add new pitch wheel value
  sequence.tracks[track].channel[channel].events.push_back(event);

  // TODO: remove this
#ifdef _DEBUG
  std::cout
    << "  [MIDI] channel pitch wheel change at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    pitch:      " << (int)pitch << std::endl;
#endif
}

void  Game::Audio::Midi::loadTrackUndefined(std::ifstream& file, Game::Audio::Midi::Sequence& sequence, std::size_t track, uint8_t status, std::size_t clock)
{
  std::vector<uint8_t>  unused(1, 0);

  // Skip unused bytes
  Game::Utilities::read(file, unused.data(), unused.size());

  // Display instruction byte
  std::ios_base::fmtflags save(std::cerr.flags());
  std::cerr << "[Game::Midi::load]: Warning, undefined instruction '" << std::hex << std::setw(2) << std::setfill('0') << (int)status << "' (1 bytes skipped)." << std::endl;
  std::cerr.flags(save);
}

uint64_t  Game::Audio::Midi::loadVariableLengthQuantity(std::ifstream& file)
{
  uint64_t  result = 0;

  while (true) {
    uint8_t byte = 0;

    // Read byte from file
    Game::Utilities::read(file, &byte);

    // Number is the first 7 bits
    result = (result * 128) + (byte & 0b01111111);

    // Stop if final bit reached
    if ((byte & 0b10000000) == 0)
      break;
  }

  return result;
}

std::string Game::Audio::Midi::loadText(std::ifstream& file)
{
  uint64_t          length(loadVariableLengthQuantity(file));
  std::vector<char> str(length + 1, '\0');

  // Read string
  Game::Utilities::read(file, str.data(), str.size() - 1);

  return str.data();
}

float Game::Audio::Midi::duration(const Game::Audio::Midi::Sequence& sequence, std::size_t clock) const
{
  // Get time since beginning of sequence
  return duration(sequence, 0, clock);
}

float Game::Audio::Midi::duration(const Game::Audio::Midi::Sequence& sequence, std::size_t start, std::size_t end) const
{
  // Translate time according to format
  switch (_timeFormat)
  {
  case Game::Audio::Midi::QuarterNote:
  {
    float elapsed = 0.f;

    if (sequence.metadata.tempos.empty() == true)
      return ((float)(end - start) / (float)_timeValue) * (60.f / (60000000.f / 120.f));

    for (auto tempo = sequence.metadata.tempos.begin(); tempo != sequence.metadata.tempos.end(); tempo++)
    {
      // No end of records
      if (std::next(tempo) == sequence.metadata.tempos.end() || std::next(tempo)->first >= end) {
        elapsed += ((float)(end - start) / (float)_timeValue) * (60.f / (60000000.f / (float)tempo->second));
        break;
      }

      // Add time of interval
      elapsed += ((float)(std::next(tempo)->first - start) / (float)_timeValue) * (60.f / (60000000.f / (float)tempo->second));

      // Skip start of interval
      start = std::next(tempo)->first;
    }

    return elapsed;
  }
  case Game::Audio::Midi::SMPTE:
    return (float)(end - start) / (float)_timeValue;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    break;
  }
}