#include <future>
#include <list>

#include "System/Audio/Synthesizer.hpp"

// TODO: remove this
#include <SFML/System.hpp>
#include <iostream>
#include "System/Config.hpp"

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

Game::Audio::Synthesizer::Synthesizer(const std::string& midi, const std::string& soundfont) :
  midi(midi),
  soundfont(soundfont)
{}

std::vector<float>  Game::Audio::Synthesizer::generate(std::size_t sequenceId, std::size_t sampleRate) const
{
  // TODO: remove this
  sf::Clock clock;

  Game::Audio::Midi::Sequence   sequence = *std::next(midi.sequences.begin(), sequenceId);
  std::vector<float>            buffer((std::size_t)((sequence.metadata.end * (float)sampleRate) + 1) * 2, 0.f);
  std::mutex                    buffer_lock;
  std::list<std::future<void>>  tasks;

  // Run a task for each channel of each track
  for (const auto& [track_id, track] : sequence.tracks) {
    for (auto& channel : track.channel)
    {
      // Skip empty channels
      if (channel.events.empty() == true)
        continue;

      // Execute task
      tasks.push_back(std::async(std::launch::async, [this, &sequence, &track, &channel, &buffer, &buffer_lock, sampleRate]
        {
          // Generate channel
          generateChannel(sequence, track, channel, buffer, buffer_lock, sampleRate);
        }));
    }
  }

  // Wait for rendering tasks to complete
  for (auto& task : tasks)
    task.wait();

  float max = 1.f;

  // Get max value of buffer
  for (float sample : buffer)
    max = std::max(max, std::abs(sample));

  // Normalize buffer
  for (float& sample : buffer)
    sample = sample / max;

  std::cout << "generation time: " << clock.getElapsedTime().asSeconds() << "s." << std::endl;

  puts("Generate WAVE file...");
  ::toWave(Game::Config::ExecutablePath + "/generated.wav", buffer, sampleRate);
  puts("Done.");

  return buffer;
}

void  Game::Audio::Synthesizer::generateChannel(const Game::Audio::Midi::Sequence& sequence, const Game::Audio::Midi::Sequence::Track& track, const Game::Audio::Midi::Sequence::Track::Channel& channel, std::vector<float>& buffer, std::mutex& buffer_lock, std::size_t sampleRate) const
{
  std::array<int8_t, 128>   keys = {};         // Velocity of channel keys
  std::array<uint8_t, 128>  polyphonics = {};  // Pressure on channel keys
  std::array<uint8_t, 128>  controllers = {};  // Controllers of channel
  uint8_t                   program = 0;      // Current program of channel
  uint16_t                  pitch = 0;        // Current pitch of channel

  struct Note {
    uint8_t key;
    // TODO
  };

  std::list<Note>           notes;  // Notes currently playing

  // Process each event of channel
  for (auto event = channel.events.begin(); true; event++) {



    if (event == channel.events.end())
      break;

    // Handle event
    switch (event->type) {
    case Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventController:    //
      break;

    case Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventKey:           //
      break;

    case Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventPolyphonicKey: //
      break;

    case Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventPitch:         // Pitch change
      pitch = event->data.pitch;
      break;

    case Game::Audio::Midi::Sequence::Track::Channel::Event::Type::EventProgram:       // Program change
      program = event->data.program;
      break;

    default:  // Error
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
  }

  /*
  std::mutex                    buffer_lock;
  std::list<std::future<void>>  tasks;

  // Process each "positive" note of channel
  for (auto event = channel.events.begin(); event != channel.events.end(); event++)
  {
    // Skip released notes
    if (event->type != Game::Midi::Sequence::Track::Channel::Event::Type::EventKey || event->data.key.velocity <= 0)
      continue;

    // Execute task
    tasks.push_back(std::async(std::launch::async, [this, &sequence, &track, &buffer, &buffer_lock, &channel, event, sampleRate]
      {
        // Find matching released key event
        auto event_end = std::find_if(std::next(event), channel.events.end(), [this, event](const auto& end) { return event->type == Game::Midi::Sequence::Track::Channel::Event::Type::EventKey && event->data.key.key == end.data.key.key; });

        auto    start = *event;
        auto    end = event_end != channel.events.end() ? *event_end : Game::Midi::Sequence::Track::Channel::Event{ .clock = sequence.metadata.end, .type = Game::Midi::Sequence::Track::Channel::Event::Type::EventKey, .data = { .key = { .key = start.data.key.key, .velocity = 0 } } };

        // Missing note release event
        if (end.data.key.velocity > 0) {
          std::cerr << "[Game::Midi::generate]: Warning, missing MIDI key release event (key: " << (int)start.second.key << ", time: " << duration(sequence, start.first) << "s)." << std::endl;
          end.data.key.velocity = 0;
        }

        // Hold pedal
        end.clock = generateChannelHold(sequence, track, channel, start, end);

        // Sustenuto pedal
        end.clock = generateChannelSustenuto(sequence, track, channel, start, end);

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
  */
}

/*
void  Game::Midi::generateNote(const Game::Midi::Sequence& sequence, const Game::Midi::Sequence::Track& track, const Game::Midi::Sequence::Track::Channel& channel, std::vector<float>& buffer, std::mutex& buffer_lock, std::size_t sampleRate, std::list<Game::Midi::Sequence::Track::Channel::Event>::const_iterator note_on, Game::Midi::Sequence::Track::Channel::Context context) const
{
  // Get current bank number and program
  uint8_t program = context.program;
  uint8_t bank = context.controllers[Game::Midi::Sequence::Track::Channel::Controller::ControllerBankSelectCoarse]
    + context.controllers[Game::Midi::Sequence::Track::Channel::Controller::ControllerBankSelectFine] * 128
    + (&channel == &track.channel[9] ? 128 : 0);

  // Check a preset exist for given bank/key
  if (_soundfont.presets.find(bank) == _soundfont.presets.end() || _soundfont.presets.find(bank)->second.find(program) == _soundfont.presets.find(bank)->second.end()) {
    std::cerr << "[Game::Midi::generate]: Warning, no preset for MIDI bank/program (bank: " << (int)bank << ", program: " << (int)program << ", time: " << note_on->clock << "s)." << std::endl;
    return;
  }

  // Get preset to be played
  const auto& preset = _soundfont.presets.find(bank)->second.find(program)->second;

  // Find instrument bags to generate
  for (const auto& instrument : preset.instruments) {
    for (const auto& bag : instrument.bags)
    {
      // Check bag's key range
      if (note_on->data.note.key < bag.generator[SoundFont::Sf2Generator::KeyRange].range.low || note_on->data.note.key > bag.generator[SoundFont::Sf2Generator::KeyRange].range.high)
        continue;

      // Check bag's velocity range
      if (note_on->data.note.velocity < bag.generator[SoundFont::Sf2Generator::VelocityRange].range.low || note_on->data.note.velocity > bag.generator[SoundFont::Sf2Generator::VelocityRange].range.high)
        continue;

      // Get sample to play
      const auto& sample = _soundfont.samples[bag.generator[SoundFont::Sf2Generator::SampleId].u_amount];

      // Generate note from events and sample
      for (auto event = note_on; event != channel.events.end(); event++) {
        // TODO
      }
    }
  }
}
*/
