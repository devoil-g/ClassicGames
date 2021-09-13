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
  std::size_t clock = 0;
  bool        end = false;

  std::cout << std::endl << "  [SUBTRACK] " << track << std::endl;

  // Read until track end event
  while (end == false)
  {
    // Get delta time of event
    clock += loadVariableLengthQuantity(file);

    std::list<std::tuple<uint8_t, std::vector<uint8_t>, std::function<void()>>> midi_commands =
    {
      // Sysex events
      { 8, { 0xF0 }, std::bind(&Game::Midi::loadTrackSysexF0, this, std::ref(file)) },
      { 8, { 0xF7 }, std::bind(&Game::Midi::loadTrackSysexF7, this, std::ref(file)) },

      // Meta events
      { 24, { 0xFF, 0x00, 0x00 }, std::bind(&Game::Midi::loadTrackMetaSequenceDefault, this, std::ref(file), std::ref(sequence), (int16_t)sequences.size()) },
      { 24, { 0xFF, 0x00, 0x02 }, std::bind(&Game::Midi::loadTrackMetaSequence, this, std::ref(file), std::ref(sequence)) },
      { 16, { 0xFF, 0x01 }, std::bind(&Game::Midi::loadTrackMetaTexts, this, std::ref(file), std::ref(sequence), clock) },
      { 16, { 0xFF, 0x02 }, std::bind(&Game::Midi::loadTrackMetaCopyright, this, std::ref(file), std::ref(sequence)) },
      { 16, { 0xFF, 0x03 }, std::bind(&Game::Midi::loadTrackMetaName, this, std::ref(file), std::ref(sequence), track) },
      { 16, { 0xFF, 0x04 }, std::bind(&Game::Midi::loadTrackMetaInstrument, this, std::ref(file), std::ref(sequence)) },
      { 16, { 0xFF, 0x05 }, std::bind(&Game::Midi::loadTrackMetaLyrics, this, std::ref(file), std::ref(sequence), clock) },
      { 16, { 0xFF, 0x06 }, std::bind(&Game::Midi::loadTrackMetaMarkers, this, std::ref(file), std::ref(sequence), clock) },
      { 16, { 0xFF, 0x07 }, std::bind(&Game::Midi::loadTrackMetaCues, this, std::ref(file), std::ref(sequence), clock) },
      { 16, { 0xFF, 0x08 }, std::bind(&Game::Midi::loadTrackMetaProgram, this, std::ref(file), std::ref(sequence)) },
      { 16, { 0xFF, 0x09 }, std::bind(&Game::Midi::loadTrackMetaDevices, this, std::ref(file), std::ref(sequence), clock) },
      { 24, { 0xFF, 0x20, 0x01 }, std::bind(&Game::Midi::loadTrackMetaChannels, this, std::ref(file), std::ref(sequence), clock) },
      { 24, { 0xFF, 0x21, 0x01 }, std::bind(&Game::Midi::loadTrackMetaPorts, this, std::ref(file), std::ref(sequence), clock) },
      { 24, { 0xFF, 0x2F, 0x00 }, std::bind(&Game::Midi::loadTrackMetaEnd, this, std::ref(file), std::ref(sequence), clock, std::ref(end)) },
      { 16, { 0xFF, 0x4B }, std::bind(&Game::Midi::loadTrackMetaTags, this, std::ref(file), std::ref(sequence)) },
      { 24, { 0xFF, 0x51, 0x03 }, std::bind(&Game::Midi::loadTrackMetaTempos, this, std::ref(file), std::ref(sequence), clock) },
      { 24, { 0xFF, 0x54, 0x05 }, std::bind(&Game::Midi::loadTrackMetaSmpte, this, std::ref(file), std::ref(sequence)) },
      { 24, { 0xFF, 0x58, 0x04 }, std::bind(&Game::Midi::loadTrackMetaSignatures, this, std::ref(file), std::ref(sequence), clock) },
      { 24, { 0xFF, 0x59, 0x02 }, std::bind(&Game::Midi::loadTrackMetaKeys, this, std::ref(file), std::ref(sequence), clock) },
      { 16, { 0xFF, 0x7F }, std::bind(&Game::Midi::loadTrackMetaDatas, this, std::ref(file), std::ref(sequence)) },
      
      // MIDI events, Channel Voice Messages
      { 4, { 0b10000000 }, std::bind(&Game::Midi::loadTrackMidiNoteOff, this, std::ref(file), std::ref(sequence), track, clock) },
      { 4, { 0b10010000 }, std::bind(&Game::Midi::loadTrackMidiNoteOn, this, std::ref(file), std::ref(sequence), track, clock) },
      { 4, { 0b10100000 }, std::bind(&Game::Midi::loadTrackMidiPolyphonicKeyPressure, this, std::ref(file), std::ref(sequence), track, clock) },
      { 4, { 0b10110000 }, std::bind(&Game::Midi::loadTrackMidiControlChange, this, std::ref(file), std::ref(sequence), clock) },
      { 4, { 0b11000000 }, std::bind(&Game::Midi::loadTrackMidiProgramChange, this, std::ref(file), std::ref(sequence), track, clock) },
      { 4, { 0b11010000 }, std::bind(&Game::Midi::loadTrackMidiChannelPressure, this, std::ref(file), std::ref(sequence), track, clock) },
      { 4, { 0b11100000 }, std::bind(&Game::Midi::loadTrackMidiPitchWheelChange, this, std::ref(file), std::ref(sequence), track, clock) },

      // MIDI events, System Common Messages
      { 8, { 0b11110001 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xF1 }), 0) },
      { 8, { 0b11110010 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xF2 }), 0) },
      { 8, { 0b11110011 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xF3 }), 0) },
      { 8, { 0b11110100 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xF4 }), 0) },
      { 8, { 0b11110101 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xF5 }), 0) },
      { 8, { 0b11110110 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xF6 }), 0) },

      // MIDI events, System Real-Time Messages
      { 8, { 0b11111000 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xF8 }), 0) },
      { 8, { 0b11111001 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xF9 }), 0) },
      { 8, { 0b11111010 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xFA }), 0) },
      { 8, { 0b11111011 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xFB }), 0) },
      { 8, { 0b11111100 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xFC }), 0) },
      { 8, { 0b11111101 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xFD }), 0) },
      { 8, { 0b11111110 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0xFE }), 0) },

      // MIDI Controller Messages
      { 8, { 0x00 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x00 }), 1) },
      { 8, { 0x01 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x01 }), 1) },
      { 8, { 0x02 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x02 }), 1) },
      { 8, { 0x03 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x03 }), 1) },
      { 8, { 0x04 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x04 }), 1) },
      { 8, { 0x05 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x05 }), 1) },
      { 8, { 0x06 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x06 }), 1) },
      { 8, { 0x07 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x07 }), 1) },
      { 8, { 0x08 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x08 }), 1) },
      { 8, { 0x09 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x09 }), 1) },
      { 8, { 0x0A }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x0A }), 1) },
      { 8, { 0x0B }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x0B }), 1) },
      { 8, { 0x0C }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x0C }), 1) },
      { 8, { 0x0D }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x0D }), 1) },
      { 8, { 0x0E }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x0E }), 1) },
      { 8, { 0x0F }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x0F }), 1) },

      { 8, { 0x10 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x10 }), 1) },
      { 8, { 0x11 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x11 }), 1) },
      { 8, { 0x12 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x12 }), 1) },
      { 8, { 0x13 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x13 }), 1) },
      { 8, { 0x14 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x14 }), 1) },
      { 8, { 0x15 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x15 }), 1) },
      { 8, { 0x16 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x16 }), 1) },
      { 8, { 0x17 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x17 }), 1) },
      { 8, { 0x18 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x18 }), 1) },
      { 8, { 0x19 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x19 }), 1) },
      { 8, { 0x1A }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x1A }), 1) },
      { 8, { 0x1B }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x1B }), 1) },
      { 8, { 0x1C }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x1C }), 1) },
      { 8, { 0x1D }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x1D }), 1) },
      { 8, { 0x1E }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x1E }), 1) },
      { 8, { 0x1F }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x1F }), 1) },

      { 8, { 0x20 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x20 }), 1) },
      { 8, { 0x21 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x21 }), 1) },
      { 8, { 0x22 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x22 }), 1) },
      { 8, { 0x23 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x23 }), 1) },
      { 8, { 0x24 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x24 }), 1) },
      { 8, { 0x25 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x25 }), 1) },
      { 8, { 0x26 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x26 }), 1) },
      { 8, { 0x27 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x27 }), 1) },
      { 8, { 0x28 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x28 }), 1) },
      { 8, { 0x29 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x29 }), 1) },
      { 8, { 0x2A }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x2A }), 1) },
      { 8, { 0x2B }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x2B }), 1) },
      { 8, { 0x2C }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x2C }), 1) },
      { 8, { 0x2D }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x2D }), 1) },
      { 8, { 0x2E }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x2E }), 1) },
      { 8, { 0x2F }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x2F }), 1) },

      { 8, { 0x30 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x30 }), 1) },
      { 8, { 0x31 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x31 }), 1) },
      { 8, { 0x32 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x32 }), 1) },
      { 8, { 0x33 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x33 }), 1) },
      { 8, { 0x34 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x34 }), 1) },
      { 8, { 0x35 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x35 }), 1) },
      { 8, { 0x36 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x36 }), 1) },
      { 8, { 0x37 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x37 }), 1) },
      { 8, { 0x38 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x38 }), 1) },
      { 8, { 0x39 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x39 }), 1) },
      { 8, { 0x3A }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x3A }), 1) },
      { 8, { 0x3B }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x3B }), 1) },
      { 8, { 0x3C }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x3C }), 1) },
      { 8, { 0x3D }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x3D }), 1) },
      { 8, { 0x3E }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x3E }), 1) },
      { 8, { 0x3F }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x3F }), 1) },

      { 8, { 0x40 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x40 }), 1) },
      { 8, { 0x41 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x41 }), 1) },
      { 8, { 0x42 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x42 }), 1) },
      { 8, { 0x43 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x43 }), 1) },
      { 8, { 0x44 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x44 }), 1) },
      { 8, { 0x45 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x45 }), 1) },
      { 8, { 0x46 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x46 }), 1) },
      { 8, { 0x47 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x47 }), 1) },
      { 8, { 0x48 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x48 }), 1) },
      { 8, { 0x49 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x49 }), 1) },
      { 8, { 0x4A }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x4A }), 1) },
      { 8, { 0x4B }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x4B }), 1) },
      { 8, { 0x4C }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x4C }), 1) },
      { 8, { 0x4D }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x4D }), 1) },
      { 8, { 0x4E }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x4E }), 1) },
      { 8, { 0x4F }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x4F }), 1) },

      { 8, { 0x50 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x50 }), 1) },
      { 8, { 0x51 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x51 }), 1) },
      { 8, { 0x52 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x52 }), 1) },
      { 8, { 0x53 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x53 }), 1) },
      { 8, { 0x54 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x54 }), 1) },
      { 8, { 0x55 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x55 }), 1) },
      { 8, { 0x56 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x56 }), 1) },
      { 8, { 0x57 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x57 }), 1) },
      { 8, { 0x58 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x58 }), 1) },
      { 8, { 0x59 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x59 }), 1) },
      { 8, { 0x5A }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x5A }), 1) },
      { 8, { 0x5B }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x5B }), 1) },
      { 8, { 0x5C }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x5C }), 1) },
      { 8, { 0x5D }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x5D }), 1) },
      { 8, { 0x5E }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x5E }), 1) },
      { 8, { 0x5F }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x5F }), 1) },

      { 8, { 0x60 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x60 }), 1) },
      { 8, { 0x61 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x61 }), 1) },
      { 8, { 0x62 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x62 }), 1) },
      { 8, { 0x63 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x63 }), 1) },
      { 8, { 0x64 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x64 }), 1) },
      { 8, { 0x65 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x65 }), 1) },
      { 8, { 0x66 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x66 }), 1) },
      { 8, { 0x67 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x67 }), 1) },
      { 8, { 0x68 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x68 }), 1) },
      { 8, { 0x69 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x69 }), 1) },
      { 8, { 0x6A }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x6A }), 1) },
      { 8, { 0x6B }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x6B }), 1) },
      { 8, { 0x6C }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x6C }), 1) },
      { 8, { 0x6D }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x6D }), 1) },
      { 8, { 0x6E }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x6E }), 1) },
      { 8, { 0x6F }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x6F }), 1) },

      { 8, { 0x70 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x70 }), 1) },
      { 8, { 0x71 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x71 }), 1) },
      { 8, { 0x72 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x72 }), 1) },
      { 8, { 0x73 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x73 }), 1) },
      { 8, { 0x74 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x74 }), 1) },
      { 8, { 0x75 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x75 }), 1) },
      { 8, { 0x76 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x76 }), 1) },
      { 8, { 0x77 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x77 }), 1) },
      { 8, { 0x78 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x78 }), 1) },
      { 8, { 0x79 }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x79 }), 1) },
      { 8, { 0x7A }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x7A }), 1) },
      { 8, { 0x7B }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x7B }), 1) },
      { 8, { 0x7C }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x7C }), 1) },
      { 8, { 0x7D }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x7D }), 1) },
      { 8, { 0x7E }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x7E }), 1) },
      { 8, { 0x7F }, std::bind(&Game::Midi::loadTrackMidiUndefined, this, std::ref(file), std::vector<uint8_t>({ 0x7F }), 1) },
    };

    std::cout << "  [READ] (clock: " << clock << ") " << std::flush;

    // Find matching command
    while (true) {
      uint8_t byte = 0;

      // Read one byte from input
      read(file, &byte);

      // TODO: remove this
      std::ios_base::fmtflags save(std::cout.flags());
      std::cout << " " << std::hex << std::setw(2) << std::setfill('0') << (int)byte << std::flush;
      std::cout.flags(save);

      // Remove non-matching commands
      midi_commands.remove_if([byte](const auto& instruction) {
        if (std::get<1>(instruction).empty() == true)
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

        auto b = byte;
        if (std::get<0>(instruction) < 8) {
          b = b >> (8 - std::get<0>(instruction));
          b = b << (8 - std::get<0>(instruction));
        }

        return std::get<1>(instruction).front() != b;
      });

      // No match
      if (midi_commands.empty() == true)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      // Remove first byte of each remaining instruction
      for (auto& instruction : midi_commands) {
        std::get<0>(instruction) -= 8;
        std::get<1>(instruction).erase(std::get<1>(instruction).begin());
      }

      // Execute instruction if only an empty one remains
      if (midi_commands.size() == 1 && std::get<1>(midi_commands.front()).empty() == true) {
        std::cout << std::endl;
        std::get<2>(midi_commands.front())();
        break;
      }
    }
  }
}

void  Game::Midi::loadTrackSysexF0(std::ifstream& file)
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

void  Game::Midi::loadTrackSysexF7(std::ifstream& file)
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

void  Game::Midi::loadTrackMetaName(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track)
{
  std::string name = loadText(file);

  // Check if general name of track name
  if (track == 0)
    sequence.metadata.name = name;
  else
    sequence.tracks[track].name = name;

  // TODO: remove this
  if (track == 0)
    std::cout << "  [META] sequence name: " << sequence.metadata.name << std::endl;
  else
    std::cout << "  [META] track " << track << " name: " << sequence.metadata.name << std::endl;
}

void  Game::Midi::loadTrackMetaCopyright(std::ifstream& file, Game::Midi::Sequence& sequence)
{
  sequence.metadata.copyright = loadText(file);

  // TODO: remove this
  std::cout << "  [META] copyright: " << sequence.metadata.copyright << std::endl;
}

void  Game::Midi::loadTrackMetaInstrument(std::ifstream& file, Game::Midi::Sequence& sequence)
{
  sequence.metadata.instrument = loadText(file);

  // TODO: remove this
  std::cout << "  [META] instrument: " << sequence.metadata.instrument << std::endl;
}

void  Game::Midi::loadTrackMetaLyrics(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  sequence.metadata.lyrics.push_back({ clock, loadText(file) });

  // TODO: remove this
  std::cout << "  [META] lyric: '" << sequence.metadata.lyrics.back().second << "' at " << sequence.metadata.lyrics.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaMarkers(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  sequence.metadata.markers.push_back({ clock, loadText(file) });

    // TODO: remove this
    std::cout << "  [META] marker: '" << sequence.metadata.markers.back().second << "' at " << sequence.metadata.markers.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaCues(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  sequence.metadata.cues.push_back({ clock, loadText(file) });

  // TODO: remove this
  std::cout << "  [META] cue: '" << sequence.metadata.cues.back().second << "' at " << sequence.metadata.cues.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaTexts(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  sequence.metadata.texts.push_back({ clock, loadText(file) });

  // TODO: remove this
  std::cout << "  [META] text: '" << sequence.metadata.texts.back().second << "' at " << sequence.metadata.texts.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaSequence(std::ifstream& file, Game::Midi::Sequence& sequence)
{
  read(file, &sequence.metadata.sequence);
  sequence.metadata.sequence = Game::Midi::swap_endianness(sequence.metadata.sequence);

  // TODO: remove this
  std::cout << "  [META] sequence: " << sequence.metadata.sequence << std::endl;
}

void  Game::Midi::loadTrackMetaSequenceDefault(std::ifstream& file, Game::Midi::Sequence& sequence, uint16_t value)
{
  sequence.metadata.sequence = value;

  // TODO: remove this
  std::cout << "  [META] sequence: " << sequence.metadata.sequence << " (default)" << std::endl;
}

void  Game::Midi::loadTrackMetaTempos(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  uint32_t  tempo = 0;

  read(file, ((char*)&tempo) + 1, 3);
  sequence.metadata.tempos.push_back({ clock, Game::Midi::swap_endianness(tempo) });

  // TODO: remove this
  std::cout << "  [META] tempo: " << 60000000 / sequence.metadata.tempos.back().second << " BPM at " << sequence.metadata.tempos.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaSmpte(std::ifstream& file, Game::Midi::Sequence& sequence)
{
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
  std::cout << "  [META] smpte: "
    << (int)sequence.metadata.smpte.hours << "h "
    << (int)sequence.metadata.smpte.minutes << "m "
    << (int)sequence.metadata.smpte.seconds << "s "
    << (int)sequence.metadata.smpte.frames << "/" << (int)sequence.metadata.smpte.framerate << "f "
    << (int)sequence.metadata.smpte.fractionals << " 100e"
    << std::endl;
}

void  Game::Midi::loadTrackMetaSignatures(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  Game::Midi::Sequence::MetaData::Signature  signature = {};

  read(file, &signature.numerator);
  read(file, &signature.denominator);
  read(file, &signature.metronome);
  read(file, &signature.quarter);

  // Denominator is given as a power of 2
  signature.denominator = (uint8_t)std::pow(2, signature.denominator);

  sequence.metadata.signatures.push_back({ clock, signature });

  // TODO: remove this
  std::cout << "  [META] signature: " << (int)sequence.metadata.signatures.back().second.numerator << "/" << (int)sequence.metadata.signatures.back().second.denominator << " at " << sequence.metadata.signatures.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaKeys(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  Game::Midi::Sequence::MetaData::Key  key = {};

  read(file, &key.key);
  read(file, &key.major);

  sequence.metadata.keys.push_back({ clock, key });

  // TODO: remove this
  std::cout << "  [META] key: " << (int)sequence.metadata.keys.back().second.key << " " << (sequence.metadata.keys.back().second.major ? "major" : "minor") << " at " << sequence.metadata.keys.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaProgram(std::ifstream& file, Game::Midi::Sequence& sequence)
{
  sequence.metadata.program = loadText(file);

  // TODO: remove this
  std::cout << "  [META] program: " << sequence.metadata.program << std::endl;
}

void  Game::Midi::loadTrackMetaDevices(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  sequence.metadata.devices.push_back({ clock, loadText(file) });

  // TODO: remove this
  std::cout << "  [META] device: '" << sequence.metadata.devices.back().second << "' at " << sequence.metadata.devices.back().first << std::endl;
}

void  Game::Midi::loadTrackMetaChannels(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  uint8_t channel;

  read(file, &channel);
  std::cerr << "[Game::Midi::load]: Warning, ignored MIDI track metadata (channel prefix " << (int)channel << ")." << std::endl;
}

void  Game::Midi::loadTrackMetaPorts(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  uint8_t port;

  read(file, &port);
  std::cerr << "[Game::Midi::load]: Warning, ignored MIDI track metadata (MIDI port " << (int)port << ")." << std::endl;
}

void  Game::Midi::loadTrackMetaDatas(std::ifstream& file, Game::Midi::Sequence& sequence)
{
  uint64_t  length(loadVariableLengthQuantity(file));

  sequence.metadata.datas.resize(length, 0);
  read(file, sequence.metadata.datas.data(), sequence.metadata.datas.size());

  // TODO: remove this
  std::cout << "  [META] datas: " << length << " bytes" << std::endl;
}

void  Game::Midi::loadTrackMetaTags(std::ifstream& file, Game::Midi::Sequence& sequence)
{
  uint64_t                            length(loadVariableLengthQuantity(file));
  std::vector<char>                   text(length + 1, 0);
  Game::Midi::Sequence::MetaData::Tag tag;

  read(file, &tag);
  read(file, text.data(), text.size() - 1);
  sequence.metadata.tags[tag] = text.data();

  // TODO: remove this
  std::cout << "  [META] tag: [" << (int)tag << "] '" << sequence.metadata.tags[tag] << "'" << std::endl;
}

void  Game::Midi::loadTrackMetaEnd(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock, bool& end)
{
  // Stop track recording
  end = true;

  // Save end time
  sequence.metadata.end = std::max(sequence.metadata.end, clock);

  // TODO: remove this
  std::cout << "  [META] end clock: " << clock << std::endl;
}

void  Game::Midi::loadTrackMidiNoteOff(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t channel = 0;
  uint8_t key = 0;
  uint8_t velocity = 0;
  
  // Get instruction parameters
  channel = loadChannel(file);
  read(file, &key);
  read(file, &velocity);

  // Add new key released
  sequence.tracks[track].program[channel].released.push_back({ clock, { key, velocity } });

  // Force key pressure to 0
  sequence.tracks[track].program[channel].polyphonic.push_back({ clock, { key, 0 } });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] note off at " << clock << ":" << std::endl
    << "    channel:  " << (int)channel << std::endl
    << "    key:      " << (int)key << std::endl
    << "    velocity: " << (int)velocity << std::endl;
}

void  Game::Midi::loadTrackMidiNoteOn(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t channel = 0;
  uint8_t key = 0;
  uint8_t velocity = 0;

  // Get instruction parameters
  channel = loadChannel(file);
  read(file, &key);
  read(file, &velocity);

  // Add new key press
  sequence.tracks[track].program[channel].pressed.push_back({ clock, { key, velocity } });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] note on at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    key:        " << (int)key << std::endl
    << "    velocity:   " << (int)velocity << std::endl;
}

void  Game::Midi::loadTrackMidiPolyphonicKeyPressure(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t channel = 0;
  uint8_t key = 0;
  uint8_t pressure = 0;

  // Get instruction parameters
  channel = loadChannel(file);
  read(file, &key);
  read(file, &pressure);

  // Add new polyphonic key pressure
  sequence.tracks[track].program[channel].polyphonic.push_back({ clock, { key, pressure } });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] polyphonic key pressure at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    key:        " << (int)key << std::endl
    << "    pressure:   " << (int)pressure << std::endl;
}

void  Game::Midi::loadTrackMidiControlChange(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t clock)
{
  uint8_t channel = 0;
  uint8_t controller = 0;
  uint8_t value = 0;

  // Get instruction parameters
  channel = loadChannel(file);
  read(file, &controller);
  read(file, &value);

  // TODO: remove this
  std::cout
    << "  [MIDI] control change at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    controller: " << (int)controller << std::endl
    << "    value:      " << (int)value << std::endl;
}

void  Game::Midi::loadTrackMidiProgramChange(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t channel = 0;
  uint8_t program = 0;

  // Get instruction parameters
  channel = loadChannel(file);
  read(file, &program);

  // Add program change
  sequence.tracks[track].program[channel].program.push_back({ clock, program });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] program change at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    program:    " << (int)program << std::endl;
}

void  Game::Midi::loadTrackMidiChannelPressure(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t channel = 0;
  uint8_t pressure = 0;

  // Get instruction parameters
  channel = loadChannel(file);
  read(file, &pressure);

  // Add new polyphonic key pressure for every key of the channel
  for (uint8_t key = 0; key < 128; key++)
    sequence.tracks[track].program[channel].polyphonic.push_back({ clock, { key, pressure } });

  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] channel pressure at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    pressure:   " << (int)pressure << std::endl;
}

void  Game::Midi::loadTrackMidiPitchWheelChange(std::ifstream& file, Game::Midi::Sequence& sequence, std::size_t track, std::size_t clock)
{
  uint8_t   channel = 0;
  uint16_t  pitch = 0;

  // Get instruction parameters
  channel = loadChannel(file);
  read(file, &pitch);

  // Only keep significant bits
  pitch = (((uint8_t*)&pitch)[0] & 0b01111111) * 128 + (((uint8_t*)&pitch)[1] & 0b01111111);

  // Add new pitch wheel value
  sequence.tracks[track].program[channel].pitch.push_back({ clock, pitch });
  
  return;

  // TODO: remove this
  std::cout
    << "  [MIDI] channel pitch wheel change at " << clock << ":" << std::endl
    << "    channel:    " << (int)channel << std::endl
    << "    pitch:      " << (int)pitch << std::endl;
}

void  Game::Midi::loadTrackMidiUndefined(std::ifstream& file, const std::vector<uint8_t>& instruction, std::size_t skip)
{
  std::vector<uint8_t>  unused(skip, 0);

  // Skip unused bytes
  read(file, unused.data(), unused.size());

  std::cerr << "[Game::Midi::load]: Warning, undefined instruction '";

  // Display instruction bytes
  std::ios_base::fmtflags save(std::cerr.flags());
  for (auto it = instruction.begin(); it != instruction.end(); it++)
    std::cerr << ((it == instruction.begin()) ? "" : " ") << std::hex << std::setw(2) << std::setfill('0') << (int)*it << std::flush;
  std::cerr.flags(save);

  std::cerr << "' (" << skip << " bytes skipped)." << std::endl;
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

uint8_t Game::Midi::loadChannel(std::ifstream& file)
{
  uint8_t byte = 0;

  // Re-read instruction byte
  file.seekg(-1, file.cur);
  read(file, &byte);

  return byte & 0b00001111;
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