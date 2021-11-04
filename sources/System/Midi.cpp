#include <fstream>
#include <array>
#include <vector>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <iomanip>
#include <tuple>
#include <algorithm>
#include <future>
#include <set>

#include "Math/Math.hpp"
#include "System/Midi.hpp"
#include "System/Config.hpp"
#include "System/Pitch.hpp"

// TODO: remove this
#include <SFML/System/Clock.hpp>

// TODO: remove this
void  toWave(const std::string& filename, const std::vector<float>& buffer, std::size_t samplerate)
{
  std::vector<int16_t>  buffer16;
  for (const auto& sample : buffer)
    buffer16.push_back((int16_t)(std::clamp(sample, -1.f, +1.f) * 32767.f));

  std::ofstream  out(filename, std::ofstream::binary);

  uint32_t byte4;
  uint16_t byte2;

  out.write("RIFF", 4);
  byte4 = (int)buffer16.size() * 2 + 36;
  out.write((char*)&byte4, sizeof(byte4));
  out.write("WAVE", 4);
  out.write("fmt ", 4);
  byte4 = 16;
  out.write((char*)&byte4, sizeof(byte4));
  byte2 = 1;
  out.write((char*)&byte2, sizeof(byte2));
  byte2 = 2;
  out.write((char*)&byte2, sizeof(byte2));
  byte4 = (uint32_t)samplerate;
  out.write((char*)&byte4, sizeof(byte4));
  byte4 = (uint32_t)samplerate * 2 * 2;
  out.write((char*)&byte4, sizeof(byte4));
  byte2 = 2;
  out.write((char*)&byte2, sizeof(byte2));
  byte2 = 16;
  out.write((char*)&byte2, sizeof(byte2));
  out.write("data", 4);
  byte4 = (int)buffer16.size() * 2;
  out.write((char*)&byte4, sizeof(byte4));
  out.write((char*)buffer16.data(), buffer16.size() * 2);
}

// TODO: remove this
std::vector<float>  Game::Midi::generate(const Game::Midi::Sequence& sequence, std::size_t sampleRate) const
{
  sf::Clock clock;

  std::vector<float>            buffer((std::size_t)((duration(sequence, sequence.metadata.end) * (float)sampleRate) + 1) * 2, 0.f);
  std::mutex                    buffer_lock;
  std::list<std::future<void>>  tasks;

  // Run a task for each track
  for (const auto& track : sequence.tracks)
  {
    // Execute task
    tasks.push_back(std::async(std::launch::async, [this, &sequence, &track, sampleRate, &buffer_lock, &buffer]
      {
        std::vector<float>  task_buffer(buffer.size(), 0.f);

        // Generate track
        generateTrack(sequence, track.second, task_buffer, sampleRate);

        // Lock buffer
        std::unique_lock  lock(buffer_lock);

        // Add to buffer
        for (std::size_t index = 0; index < task_buffer.size(); index++)
          buffer[index] += task_buffer[index];
      }));
  }

  // Wait for rendering tasks to complete
  for (auto& task : tasks)
    task.wait();

  std::cout << "generation time: " << clock.getElapsedTime().asSeconds() << "s." << std::endl;

  float max = 1.f;

  // Get max value of buffer
  for (float sample : buffer)
    max = std::max(max, std::abs(sample));
  
  // Normalize buffer
  for (float& sample : buffer)
    sample = sample / max;

  puts("Generate WAVE file...");
  ::toWave(Game::Config::ExecutablePath + "/generated.wav", buffer, sampleRate);
  puts("Done.");

  return buffer;
}

void  Game::Midi::generateTrack(const Game::Midi::Sequence& sequence, const Game::Midi::Sequence::Track& track, std::vector<float>& buffer, std::size_t sampleRate) const
{
  std::mutex                    buffer_lock;
  std::list<std::future<void>>  tasks;

  // Iterate manually on channel as some index are specials
  for (std::size_t channel_index = 0; channel_index < track.channel.size(); channel_index++)
  {
    // Get channel
    const auto& channel = track.channel[channel_index];

    // Skip empty channels
    if (channel.note.empty() == true)
      continue;

    // Execute task
    tasks.push_back(std::async(std::launch::async, [this, &sequence, &track, &channel, &buffer, &buffer_lock, sampleRate, channel_index]
      {
        std::vector<float>    channel_buffer(buffer.size(), 0.f);

        // Generate channel
        generateChannel(sequence, track, channel, channel_buffer, sampleRate);

        // Lock buffer
        std::unique_lock  lock(buffer_lock);

        // Add to buffer
        for (std::size_t index = 0; index < channel_buffer.size(); index++)
          buffer[index] += channel_buffer[index];

        // TODO: remove this
        std::cout << "Channel " << channel_index << " done." << std::endl;
      }));
  }

  // Wait for rendering tasks to complete
  for (auto& task : tasks)
    task.wait();

  std::cout << "Track " << track.name << " done." << std::endl;
}

void  Game::Midi::generateChannel(const Game::Midi::Sequence& sequence, const Game::Midi::Sequence::Track& track, const Game::Midi::Sequence::Track::Channel& channel, std::vector<float>& buffer, std::size_t sampleRate) const
{
  std::mutex                    buffer_lock;
  std::list<std::future<void>>  tasks;

  // Process each "positive" note of channel
  for (auto note_start = channel.note.begin(); note_start != channel.note.end(); note_start++)
  {
    // Skip released notes
    if (note_start->second.velocity <= 0)
      continue;

    // Execute task
    tasks.push_back(std::async(std::launch::async, [this, &sequence, &track, &buffer, &buffer_lock, &channel, note_start, sampleRate]
      {
        // Find matching released key event
        auto note_end = std::find_if(std::next(note_start), channel.note.end(), [this, note_start](const auto& end) { return note_start->second.key == end.second.key; });

        std::pair<std::size_t, Game::Midi::Sequence::Track::Channel::Key>  start = *note_start;
        std::pair<std::size_t, Game::Midi::Sequence::Track::Channel::Key>  end = note_end != channel.note.end() ? *note_end : std::pair<std::size_t, Game::Midi::Sequence::Track::Channel::Key>{ sequence.metadata.end, { start.second.key, 0 } };
        
        // Missing note release event
        if (end.second.velocity > 0) {
          std::cerr << "[Game::Midi::generate]: Warning, missing MIDI key release event (key: " << (int)start.second.key << ", time: " << duration(sequence, start.first) << "s)." << std::endl;
          end.second.velocity = 0;
        }

        // Hold pedal
        end.first = generateChannelHold(sequence, track, channel, start, end);

        // Sustenuto pedal
        end.first = generateChannelSustenuto(sequence, track, channel, start, end);
        
        // Soft pedal
        float note_soft = generateChannelSoft(sequence, track, channel, start, end);

        // Get current bank number and program
        uint8_t program = channel.getData<uint8_t>(channel.program, start.first, 0);
        uint8_t bank = channel.getData(Game::Midi::Sequence::Track::Channel::Controller::ControllerBankSelectCoarse, start.first, 0)
          + channel.getData(Game::Midi::Sequence::Track::Channel::Controller::ControllerBankSelectFine, start.first, 0) * 128 + (&channel == &track.channel[9] ? 128 : 0);

        // Check a preset exist for given bank/key
        if (_soundfont.presets.find(bank) == _soundfont.presets.end() || _soundfont.presets.find(bank)->second.find(program) == _soundfont.presets.find(bank)->second.end()) {
          std::cerr << "[Game::Midi::generate]: Warning, no preset for MIDI bank/program (bank: " << (int)bank << ", program: " << (int)program << ", time: " << duration(sequence, start.first) << "s)." << std::endl;
          return;
        }

        Game::Pitch<2048, 4>  pitcher;

        // Get preset to be played
        const auto& preset = _soundfont.presets.find(bank)->second.find(program)->second;

        // Find instrument bag to generate
        for (const auto& instrument : preset.instruments) {
          for (const auto& bag : instrument.bags)
          {
            // Check bag's key range
            if (start.second.key < bag.generator[SoundFont::Sf2Generator::KeyRange].range.low || start.second.key > bag.generator[SoundFont::Sf2Generator::KeyRange].range.high)
              continue;

            // Check bag's velocity range
            if (start.second.key < bag.generator[SoundFont::Sf2Generator::VelocityRange].range.low || start.second.key > bag.generator[SoundFont::Sf2Generator::VelocityRange].range.high)
              continue;

            // Get sample to play
            const auto& sample = _soundfont.samples[bag.generator[SoundFont::Sf2Generator::SampleId].u_amount];

            std::vector<float>  note;
            float               note_start = duration(sequence, start.first);
            float               note_end = duration(sequence, end.first);

            // Pre-allocate memory
            note.reserve((std::size_t)std::ceil((note_end - note_start) * sample.rate) + sample.samples.size());

            // Sample start
            note.insert(note.end(), sample.samples.data(), sample.samples.data() + sample.start);

            // Repeat sample loop at least once and until key is released
            do
            {
              // Sample loop
              note.insert(note.end(), sample.samples.data() + sample.start, sample.samples.data() + sample.end);
            } while (bag.generator[SoundFont::Sf2Generator::SampleMode].u_amount != 0 && note_start + (float)note.size() / (float)sample.rate < note_end);

            // Sample end
            note.insert(note.end(), sample.samples.data() + sample.end, sample.samples.data() + sample.samples.size());

            // Apply soft pedal
            for (std::size_t index = (std::size_t)std::max(0.f, std::ceil((note_soft - note_start) * (float)sample.rate)); index < note.size(); index++)
              note[index] *= 1.f - std::min(((float)index / (float)sample.rate - (note_soft - note_start)) / 2.f * 8.f, 0.5f);

            std::vector<std::pair<std::size_t, float>>  pitchs;

            // Get sample index-pitch map from channel pitch
            pitchs.emplace_back(0, std::pow(2.f, ((float)start.second.key - (float)sample.key + ((float)sample.correction + 0.f) / 100.f) / 12.f));
            for (const auto& pitch : channel.pitch) {
              if (pitch.first < start.first)
                pitchs.front().second = std::pow(2.f, ((float)start.second.key - (float)sample.key + ((float)sample.correction + (((float)pitch.second - (float)0x2000) / (float)0x2000 * 200.f)) / 100.f) / 12.f);
              else if (duration(sequence, pitch.first) < note_start + (float)note.size() / (float)sample.rate)
                pitchs.emplace_back((std::size_t)((duration(sequence, pitch.first) - note_start) * (float)sample.rate), std::pow(2.f, ((float)start.second.key - (float)sample.key + ((float)sample.correction + (((float)pitch.second - (float)0x2000) / (float)0x2000 * 200.f)) / 100.f) / 12.f));
              else
                break;
            }

            // Pitch note
            pitcher.shift(note, sample.rate, pitchs);

            // Lock buffer
            std::unique_lock  lock(buffer_lock);

            // Copy note to buffer
            for (int buffer_index = (int)std::floor(note_start * (float)sampleRate);
              buffer_index < (int)std::ceil((note_start + (float)note.size() / (float)sample.rate) * sampleRate);
              buffer_index++)
            {
              // Index of sample to get
              float note_index = (((float)buffer_index / (float)sampleRate) - note_start) * (float)sample.rate;

              // Get left and right sample
              float note_left = std::floor(note_index + 0) < 0 || std::floor(note_index + 0) >= note.size() ? 0.f : note[(int)std::floor(note_index + 0)];
              float note_right = std::floor(note_index + 1) < 0 || std::floor(note_index + 1) >= note.size() ? 0.f : note[(int)std::floor(note_index + 1)];

              // Add sample to buffer
              buffer[Math::Modulo(buffer_index * 2 + 0, (int)buffer.size())] += note_left * (1.f - Math::Modulo(note_index, 1.f)) + note_right * Math::Modulo(note_index, 1.f);
              buffer[Math::Modulo(buffer_index * 2 + 1, (int)buffer.size())] += note_left * (1.f - Math::Modulo(note_index, 1.f)) + note_right * Math::Modulo(note_index, 1.f);
            }
          }
        }
      }));
  }

  // Wait for rendering tasks to complete
  for (auto& task : tasks)
    task.wait();

  // Apply volume/pan/balance to channel
  generateChannelVolume(sequence, track, channel, buffer, sampleRate);
  generateChannelPan(sequence, track, channel, buffer, sampleRate);
  generateChannelBalance(sequence, track, channel, buffer, sampleRate);
}

std::size_t Game::Midi::generateChannelHold(const Game::Midi::Sequence& sequence, const Game::Midi::Sequence::Track& track, const Game::Midi::Sequence::Track::Channel& channel, const std::pair<std::size_t, Game::Midi::Sequence::Track::Channel::Key>& start, const std::pair<std::size_t, Game::Midi::Sequence::Track::Channel::Key>& end) const
{
  auto  hold_controller = channel.controller.find(Game::Midi::Sequence::Track::Channel::Controller::ControllerHoldPedal);

  if (hold_controller != channel.controller.end())
  {
    auto  hold_pedal = hold_controller->second.begin();

    while (hold_pedal != hold_controller->second.end())
    {
      // Find beginning of hold
      while (hold_pedal != hold_controller->second.end() && hold_pedal->second < 64)
        hold_pedal++;

      // No more pedal
      if (hold_pedal == hold_controller->second.end())
        break;

      // Note already released
      if (hold_pedal->first >= end.first)
        break;

      // Find ending of hold
      while (hold_pedal != hold_controller->second.end() && hold_pedal->second >= 64)
        hold_pedal++;

      // Remember start time of hold
      std::size_t release = (hold_pedal == hold_controller->second.end()) ? sequence.metadata.end : hold_pedal->first;

      // Hold stop before end of the note
      if (release < end.first)
        continue;

      // Remember new note release time
      return release;
    }
  }

  return end.first;
}

std::size_t Game::Midi::generateChannelSustenuto(const Game::Midi::Sequence& sequence, const Game::Midi::Sequence::Track& track, const Game::Midi::Sequence::Track::Channel& channel, const std::pair<std::size_t, Game::Midi::Sequence::Track::Channel::Key>& start, const std::pair<std::size_t, Game::Midi::Sequence::Track::Channel::Key>& end) const
{
  auto  sustenuto_controller = channel.controller.find(Game::Midi::Sequence::Track::Channel::Controller::ControllerSustenutoPedal);

  if (sustenuto_controller != channel.controller.end())
  {
    auto  sustenuto_pedal = sustenuto_controller->second.begin();

    while (sustenuto_pedal != sustenuto_controller->second.end())
    {
      // Find beginning of sustenuto
      while (sustenuto_pedal != sustenuto_controller->second.end() && sustenuto_pedal->second < 64)
        sustenuto_pedal++;

      // No more pedal
      if (sustenuto_pedal == sustenuto_controller->second.end())
        break;

      // Note not pressed yet
      if (sustenuto_pedal->first <= start.first)
      {
        // Skip until pedal release
        while (sustenuto_pedal != sustenuto_controller->second.end() && sustenuto_pedal->second >= 64)
          sustenuto_pedal++;

        continue;
      }

      // Note already released
      if (sustenuto_pedal->first >= end.first)
        break;

      // Find ending of sustenuto
      while (sustenuto_pedal != sustenuto_controller->second.end() && sustenuto_pedal->second >= 64)
        sustenuto_pedal++;

      // Remember end time of sustenuto
      std::size_t release = (sustenuto_pedal == sustenuto_controller->second.end()) ? sequence.metadata.end : sustenuto_pedal->first;

      // Sustenuto stop before end of the note
      if (release < end.first)
        continue;

      // Remember new note release time
      return release;
    }
  }

  return end.first;
}

float Game::Midi::generateChannelSoft(const Game::Midi::Sequence& sequence, const Game::Midi::Sequence::Track& track, const Game::Midi::Sequence::Track::Channel& channel, const std::pair<std::size_t, Game::Midi::Sequence::Track::Channel::Key>& start, const std::pair<std::size_t, Game::Midi::Sequence::Track::Channel::Key>& end) const
{
  auto  soft_controller = channel.controller.find(Game::Midi::Sequence::Track::Channel::Controller::ControllerSoftPedal);

  if (soft_controller != channel.controller.end())
  {
    puts("vagin");
    auto  soft_pedal = soft_controller->second.begin();

    while (soft_pedal != soft_controller->second.end())
    {
      // Find beginning of soft
      while (soft_pedal != soft_controller->second.end() && soft_pedal->second < 64)
        soft_pedal++;

      // No more pedal
      if (soft_pedal == soft_controller->second.end())
        break;

      // Note already released
      if (soft_pedal->first >= end.first)
        break;

      // Remember start time of soft
      std::size_t pressed = (soft_pedal == soft_controller->second.end()) ? sequence.metadata.end : soft_pedal->first;

      // Find ending of soft
      while (soft_pedal != soft_controller->second.end() && soft_pedal->second >= 64)
        soft_pedal++;

      // Remember end time of soft
      std::size_t release = (soft_pedal == soft_controller->second.end()) ? sequence.metadata.end : soft_pedal->first;

      // Soft stopped before start of the note
      if (release < start.first)
        continue;

      // Remember new note release time
      return duration(sequence, pressed);
    }
  }

  return std::numeric_limits<float>::max();
}

void  Game::Midi::generateChannelVolume(const Game::Midi::Sequence& sequence, const Game::Midi::Sequence::Track& track, const Game::Midi::Sequence::Track::Channel& channel, std::vector<float>& buffer, std::size_t sample_rate) const
{
  auto  volume_controller = channel.controller.find(Game::Midi::Sequence::Track::Channel::Controller::ControllerVolumeCoarse);

  // Does nothing if no controller
  if (volume_controller == channel.controller.end())
    return;

  auto  volume_iterator = volume_controller->second.begin();
  float volume_time = (volume_iterator == volume_controller->second.end()) ? std::numeric_limits<float>::max() : duration(sequence, volume_iterator->first);
  float volume_current = 127 / 127.f;

  // Apply volume to buffer
  for (std::size_t index = 0; index < buffer.size() / 2; index++)
  {
    // Volume change
    while ((float)index / (float)sample_rate >= volume_time) {
      volume_current = volume_iterator->second / 127.f;
      volume_iterator++;
      volume_time = (volume_iterator == volume_controller->second.end()) ? std::numeric_limits<float>::max() : duration(sequence, volume_iterator->first);
    }

    // Apply volume
    buffer[index * 2 + 0] *= volume_current;
    buffer[index * 2 + 1] *= volume_current;
  }
}

void  Game::Midi::generateChannelPan(const Game::Midi::Sequence& sequence, const Game::Midi::Sequence::Track& track, const Game::Midi::Sequence::Track::Channel& channel, std::vector<float>& buffer, std::size_t sample_rate) const
{
  auto  pan_controller = channel.controller.find(Game::Midi::Sequence::Track::Channel::Controller::ControllerPanCoarse);

  // Does nothing if no controller
  if (pan_controller == channel.controller.end())
    return;

  auto  pan_iterator = pan_controller->second.begin();
  float pan_time = (pan_iterator == pan_controller->second.end()) ? std::numeric_limits<float>::max() : duration(sequence, pan_iterator->first);
  float pan_current = 64 / 128.f;

  // Apply pan to buffer
  for (std::size_t index = 0; index < buffer.size() / 2; index++)
  {
    // Volume change
    while ((float)index / (float)sample_rate >= pan_time) {
      pan_current = pan_iterator->second / 128.f;
      pan_iterator++;
      pan_time = (pan_iterator == pan_controller->second.end()) ? std::numeric_limits<float>::max() : duration(sequence, pan_iterator->first);
    }

    // Apply pan
    buffer[index * 2 + 0] *= (1 - pan_current);
    buffer[index * 2 + 1] *= pan_current;
  }
}

void  Game::Midi::generateChannelBalance(const Game::Midi::Sequence& sequence, const Game::Midi::Sequence::Track& track, const Game::Midi::Sequence::Track::Channel& channel, std::vector<float>& buffer, std::size_t sample_rate) const
{
  auto  balance_controller = channel.controller.find(Game::Midi::Sequence::Track::Channel::Controller::ControllerBalanceCoarse);

  // Does nothing if no controller
  if (balance_controller == channel.controller.end())
    return;

  // TODO: implement this
  std::cerr << "[Game::Midi::generate]: Warning, balance not implemented." << std::endl;
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

  //return;

  static std::set<uint8_t>  set;

  if (set.find(controller) != set.end())
    return;
  set.insert(controller);

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
  pitch = (((uint8_t*)&pitch)[1] & 0b01111111) * 128 + (((uint8_t*)&pitch)[0] & 0b01111111);

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

float Game::Midi::duration(const Game::Midi::Sequence& sequence, std::size_t clock) const
{
  // Get time since beginning of sequence
  return duration(sequence, 0, clock);
}

float Game::Midi::duration(const Game::Midi::Sequence& sequence, std::size_t start, std::size_t end) const
{
  // Translate time according to format
  switch (_timeFormat)
  {
  case Game::Midi::QuarterNote:
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
  case Game::Midi::SMPTE:
    return (float)(end - start) / (float)_timeValue;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    break;
  }
}

uint8_t Game::Midi::Sequence::Track::Channel::getData(Channel::Controller controller, std::size_t tic, uint8_t default) const  // Retrieve last record of controller at given tic
{
  auto iterator = this->controller.find(controller);

  // No controller event registered
  if (iterator == this->controller.end())
    return default;

  // Find data at tic
  else
    return getData(iterator->second, tic, default);
}

Game::Midi::SoundFont::SoundFont(const std::string& filename) :
  info(),
  presets(),
  samples()
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

  // Buffer for samples
  std::vector<float>  samplesFloat;

  // Load sections of SoundFont
  loadSections(file, header, samplesFloat);
}

void  Game::Midi::SoundFont::loadSections(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, std::vector<float>& samplesFloat)
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
      { Game::Midi::str_to_key("LIST"), std::bind(&Game::Midi::SoundFont::loadSectionList, this, std::ref(file), std::ref(header), std::ref(section), std::ref(samplesFloat), section_position) }
    };

    auto command = section_commands.find(section.type);

    // Execute matching command
    if (command != section_commands.end())
      command->second();
    else
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, unknow section type '" << Game::Midi::key_to_str(section.type) << "' (section ignored)." << std::endl;
  }
}

void  Game::Midi::SoundFont::loadSectionList(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::vector<float>& samplesFloat, std::streampos position)
{
  // Matching sections
  const std::unordered_map<uint32_t, std::function<void()>> list_commands = {
    { Game::Midi::str_to_key("INFO"), std::bind(&SoundFont::loadSectionListInfo, this, std::ref(file), std::ref(header), std::ref(section), position) },
    { Game::Midi::str_to_key("sdta"), std::bind(&SoundFont::loadSectionListSdta, this, std::ref(file), std::ref(header), std::ref(section), std::ref(samplesFloat), position) },
    { Game::Midi::str_to_key("pdta"), std::bind(&SoundFont::loadSectionListPdta, this, std::ref(file), std::ref(header), std::ref(section), std::ref(samplesFloat), position) }
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
    << "    [INFO] version:     " << this->info.version.first << "." << this->info.version.second << std::endl
    << "    [INFO] engine:      " << this->info.engine << std::endl
    << "    [INFO] name:        " << this->info.name << std::endl
    << "    [INFO] rom:         " << this->info.rom << std::endl
    << "    [INFO] rom version: " << this->info.romVersion.first << "." << this->info.romVersion.second << std::endl
    << "    [INFO] creation:    " << this->info.creation << std::endl
    << "    [INFO] author:      " << this->info.author << std::endl
    << "    [INFO] product:     " << this->info.product << std::endl
    << "    [INFO] copyright:   " << this->info.copyright << std::endl
    << "    [INFO] comment:     " << this->info.comment << std::endl
    << "    [INFO] tool:        " << this->info.tool << std::endl
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

void  Game::Midi::SoundFont::loadSectionListSdta(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::vector<float>& samplesFloat, std::streampos position)
{
  Game::Midi::SoundFont::Sf2SubsectionHeader  sdta = {};
  std::vector<int16_t>                        samples16;
  std::vector<int8_t>                         samples24;

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

  // Set 24 bits complementary to 0 if absent or incomplete
  if (samples24.size() < samples16.size())
    samples24.resize(samples16.size(), 0);

  // Allocate sample buffer
  samplesFloat.resize(samples16.size(), 0.f);

  // Convert RAW 24 bits samples to float
  for (auto index = 0; index < samples16.size(); index++)
    samplesFloat[index] = ((float)samples16[index] * 256.f + (float)samples24[index]) / (float)std::pow(2, 23);
}

void  Game::Midi::SoundFont::loadSectionListPdta(std::ifstream& file, const Game::Midi::SoundFont::Sf2Header& header, const Game::Midi::SoundFont::Sf2SectionHeader& section, std::vector<float>& samplesFloat, std::streampos position)
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

  // Build presets
  for (int preset_index = 0; preset_index < presetHeaders.size() - 1; preset_index++) {
    SoundFont::Preset&    preset = presets[(uint8_t)presetHeaders[preset_index].bank][(uint8_t)presetHeaders[preset_index].midi];
    SoundFont::Generator  generator_global;
    int                   preset_bag_start = presetHeaders[preset_index + 0].bag;
    int                   preset_bag_end = presetHeaders[preset_index + 1].bag;

    // Get name of the preset
    preset.name = (const char*)presetHeaders[preset_index].name;

    // Get preset bags
    for (int preset_bag_index = preset_bag_start; preset_bag_index < preset_bag_end; preset_bag_index++) {
      SoundFont::Generator  generator_preset = generator_global;

      // Get generators of preset bag
      for (int preset_generator_index = presetBags[preset_bag_index].generator; preset_generator_index < presetBags[preset_bag_index + 1].generator; preset_generator_index++) {
        switch (presetGenerators[preset_generator_index].operation) {
        case Sf2Generator::StartAddrsOffset:
        case Sf2Generator::EndAddrsOffset:
        case Sf2Generator::StartLoopAddrsOffset:
        case Sf2Generator::EndLoopAddrsOffset:
        case Sf2Generator::StartAddrsCoarseOffset:
        case Sf2Generator::EndAddrsCoarseOffset:
        case Sf2Generator::StartLoopAddrsCoarseOffset:
        case Sf2Generator::EndLoopAddrsCoarseOffset:
        case Sf2Generator::Keynum:
        case Sf2Generator::Velocity:
        case Sf2Generator::SampleMode:
        case Sf2Generator::ExclusiveClass:
        case Sf2Generator::OverridingRootKey:
          std::cerr << "[Game::Midi::SoundFont::load]: Warning, invalid generator in preset zone (" << presetGenerators[preset_generator_index].operation << "), ignored." << std::endl;
          break;

        default:
          generator_preset[presetGenerators[preset_generator_index].operation] = presetGenerators[preset_generator_index].amount;
          break;
        }
      }

      // TODO: get modulators
      for (int preset_modulator_index = presetBags[preset_bag_index].modulator; preset_modulator_index < presetBags[preset_bag_index + 1].modulator; preset_modulator_index++)
        ;

      // First bag without instrument is a global generator, other bags without instrument are ignored
      if (generator_preset[Sf2Generator::Instrument].s_amount == -1) {
        if (preset_bag_index == preset_bag_start)
          generator_global = generator_preset;
        else
          std::cerr << "[Game::Midi::SoundFont::load]: Warning, invalid preset zone (no instrument), ignored." << std::endl;
        continue;
      }

      SoundFont::Preset::Instrument instrument;
      int                           instrument_bag_start = instrumentHeaders[generator_preset[Sf2Generator::Instrument].s_amount + 0].bag;
      int                           instrument_bag_end = instrumentHeaders[generator_preset[Sf2Generator::Instrument].s_amount + 1].bag;

      // Name of the instrument
      instrument.name = (const char*)instrumentHeaders[generator_preset[Sf2Generator::Instrument].s_amount].name;

      // Save current preset generators in instrument
      instrument.generator = generator_preset;

      // Get instrument bags
      for (int instrument_bag_index = instrument_bag_start; instrument_bag_index < instrument_bag_end; instrument_bag_index++) {
        SoundFont::Preset::Instrument::Bag  bag;

        // Get generators of instrument bag
        for (int instrument_generator_index = instrumentBags[instrument_bag_index].generator; instrument_generator_index < instrumentBags[instrument_bag_index + 1].generator; instrument_generator_index++)
          bag.generator[instrumentGenerators[instrument_generator_index].operation] = instrumentGenerators[instrument_generator_index].amount;

        // TODO: get modulators
        for (int instrument_modulator_index = instrumentBags[instrument_bag_index].modulator; instrument_modulator_index < instrumentBags[instrument_bag_index + 1].modulator; instrument_modulator_index++)
          ;

        // Add bag to instrument
        instrument.bags.push_back(std::move(bag));
      }

      // Add new instrument to preset
      preset.instruments.push_back(std::move(instrument));
    }
  }

  // Build samples
  for (const auto& sample : sampleHeaders)
  {
    // Ignore last sample
    if (&sample == &sampleHeaders.back())
      break;

    // Add a new sample
    samples.emplace_back();

    // Default values
    samples.back().name = "";
    samples.back().samples = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };
    samples.back().start = 2;
    samples.back().end = 6;
    samples.back().rate = 22050;
    samples.back().key = 60;
    samples.back().correction = 0;
    samples.back().type = Game::Midi::SoundFont::Sample::Link::Mono;
    samples.back().link = 0;

    // Only support stereo
    if (sample.sampleType != Game::Midi::SoundFont::Sf2Sample::Sf2SampleLink::Mono &&
      sample.sampleType != Game::Midi::SoundFont::Sf2Sample::Sf2SampleLink::Left &&
      sample.sampleType != Game::Midi::SoundFont::Sf2Sample::Sf2SampleLink::Right) {
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, unsupported sample type (" << (int)sample.sampleType << "), ignored." << std::endl;
      continue;
    }
      

    // Loop should be contained in samples and at least one sample long
    if (sample.loopStart < sample.sampleStart || sample.loopStart > sample.loopEnd ||
      sample.loopEnd < sample.sampleStart || sample.loopEnd > sample.loopEnd ||
      sample.loopStart >= sample.loopEnd ||
      sample.sampleStart >= sample.sampleEnd ||
      sample.sampleStart >= samplesFloat.size() || sample.sampleEnd >= samplesFloat.size()) {
      std::cerr << "[Game::Midi::SoundFont::load]: Warning, invalid sample points, ignored." << std::endl;
      continue;
    }

    // Copy attributes
    samples.back().name = (const char*)sample.name;
    samples.back().start = sample.loopStart - sample.sampleStart;
    samples.back().end = sample.loopEnd - sample.sampleStart;
    samples.back().rate = sample.sampleRate;
    samples.back().key = sample.pitchOriginal;
    samples.back().correction = sample.pitchCorrection;
    samples.back().type = (Game::Midi::SoundFont::Sample::Link)sample.sampleType;
    samples.back().link = sample.sampleLink;

    // Copy samples
    samples.back().samples.resize(sample.sampleEnd - sample.sampleStart, 0.f);
    for (auto index = sample.sampleStart; index < sample.sampleEnd; index++)
      samples.back().samples[index - sample.sampleStart] = samplesFloat[index];
  }
}

Game::Midi::SoundFont::Generator::Generator() :
  std::array<SoundFont::Sf2Amount, SoundFont::Sf2Generator::Count>()
{
  // Default values of generators
  (*this)[SoundFont::Sf2Generator::StartAddrsOffset].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::EndAddrsOffset].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::StartLoopAddrsOffset].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::EndLoopAddrsOffset].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::StartAddrsCoarseOffset].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::ModLfoToPitch].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::VibLfoToPitch].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::ModEnvToPitch].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::InitialFilterFc].u_amount = 13500;
  (*this)[SoundFont::Sf2Generator::InitialFilterQ].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::ModLfoToFilterFc].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::ModEnvToFilterFc].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::EndAddrsCoarseOffset].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::ModLfoToVolume].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::Unused1].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::ChorusEffectsSend].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::ReverbEffectsSend].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::Pan].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::Unused2].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::Unused3].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::Unused4].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::DelayModLfo].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::FreqModLfo].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::DelayVibLfo].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::FreqVibLfo].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::DelayModEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::AttackModEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::HoldModEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::DecayModEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::SustainModEnv].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::ReleaseModEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::KeynumToModEnvHold].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::KeynumToModEnvDecay].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::DelayVolEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::AttackVolEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::HoldVolEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::DecayVolEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::SustainVolEnv].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::ReleaseVolEnv].s_amount = -12000;
  (*this)[SoundFont::Sf2Generator::KeynumToVolEnvHold].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::KeynumToVolEnvDecay].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::Instrument].s_amount = -1;
  (*this)[SoundFont::Sf2Generator::Reserved1].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::KeyRange].range = { 0, 127 };
  (*this)[SoundFont::Sf2Generator::VelocityRange].range = { 0, 127 };
  (*this)[SoundFont::Sf2Generator::StartLoopAddrsCoarseOffset].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::Keynum].s_amount = -1;
  (*this)[SoundFont::Sf2Generator::Velocity].s_amount = -1;
  (*this)[SoundFont::Sf2Generator::InitialAttenuation].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::Reserved2].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::EndLoopAddrsCoarseOffset].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::CoarseTune].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::FineTune].s_amount = 0;
  (*this)[SoundFont::Sf2Generator::SampleId].u_amount = -1;
  (*this)[SoundFont::Sf2Generator::SampleMode].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::Reserved3].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::ScaleTuning].u_amount = 100;
  (*this)[SoundFont::Sf2Generator::ExclusiveClass].u_amount = 0;
  (*this)[SoundFont::Sf2Generator::OverridingRootKey].s_amount = -1;
  (*this)[SoundFont::Sf2Generator::Unused5].u_amount = 0;
}