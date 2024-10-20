#include <stdexcept>

#include "GameBoyColor/AudioProcessingUnit.hpp"
#include "GameBoyColor/GameBoyColor.hpp"
#include "Math/Math.hpp"

#include "System/Window.hpp"

GBC::AudioProcessingUnit::AudioProcessingUnit(GBC::GameBoyColor& gbc) :
  _gbc(gbc),
  _sound1{ 0 },
  _sound2{ 0 },
  _sound3{ 0 },
  _sound4{ 0 },
  _sound{ 0 }
{
  // Initialize audio
  _sound1.length = 0.f;
  _sound2.length = 0.f;
  _sound3.length = 0.f;
  _sound4.length = 0.f;

  // Channel 1 timers
  _sound1.frequencyTime = 999.f;
  _sound1.envelopeTime = 999.f;

  // Channel 2 timers
  _sound2.envelopeTime = 999.f;

  // Channel 4 timers
  _sound4.envelopeTime = 999.f;
  _sound4.counterTime = 1.f / 524288.f;
}

void  GBC::AudioProcessingUnit::simulate()
{
  // Time between two samples
  const float delta = (float)(456 * 154) / (float)(4 * 1024 * 1024) / (float)GBC::AudioProcessingUnit::FrameSize;

  std::array<float, GBC::AudioProcessingUnit::FrameSize>  channel1 = { 0 };
  std::array<float, GBC::AudioProcessingUnit::FrameSize>  channel2 = { 0 };
  std::array<float, GBC::AudioProcessingUnit::FrameSize>  channel3 = { 0 };
  std::array<float, GBC::AudioProcessingUnit::FrameSize>  channel4 = { 0 };

  // Channel 1
  for (std::size_t index = 0; index < channel1.size(); index++)
  {
    // No envelope
    if (_sound1.envelope == 0.f && _sound1.envelopeDirection == false)
      break;

    // Sound duration
    _sound1.length = std::max(0.f, _sound1.length - delta);
    if (_sound1.length <= 0.f)
      break;

    // Limit wave clock to one oscillation
    _sound1.clock = Math::Modulo(_sound1.clock, 1.f / _sound1.frequency);

    // Generate wave form
    channel1[index] = _sound1.envelope * ((_sound1.clock < 1.f / _sound1.frequency * _sound1.wave) ? -1.f : +1.f);

    // Tick wave clock
    _sound1.clock += delta;

    // Frequency sweep
    for (_sound1.frequencyElapsed += delta;
      _sound1.frequencyElapsed >= _sound1.frequencyTime;
      _sound1.frequencyElapsed -= _sound1.frequencyTime)
      _sound1.frequency += (_sound1.frequencyDirection ? -1.f : +1.f) * _sound1.frequency * _sound1.frequencyShift;

    // Envelope change
    for (_sound1.envelopeElapsed += delta;
      _sound1.envelopeElapsed >= _sound1.envelopeTime;
      _sound1.envelopeElapsed -= _sound1.envelopeTime)
      _sound1.envelope = std::clamp(_sound1.envelope + (_sound1.envelopeDirection ? +1.f / 16.f : -1.f / 16.f), 0.f, 1.f);
  }

  // Channel 2
  for (std::size_t index = 0; index < channel2.size(); index++)
  {
    // No envelope
    if (_sound2.envelope == 0.f && _sound2.envelopeDirection == false)
      break;

    // Sound duration
    _sound2.length = std::max(0.f, _sound2.length - delta);
    if (_sound2.length <= 0.f)
      break;

    // Limit wave clock to one oscillation
    _sound2.clock = Math::Modulo(_sound2.clock, 1.f / _sound2.frequency);

    // Generate wave form
    channel2[index] = _sound2.envelope * ((_sound2.clock < 1.f / _sound2.frequency * _sound2.wave) ? -1.f : +1.f);

    // Tick wave clock
    _sound2.clock += delta;

    // Envelope change
    for (_sound2.envelopeElapsed += delta;
      _sound2.envelopeElapsed >= _sound2.envelopeTime;
      _sound2.envelopeElapsed -= _sound2.envelopeTime)
      _sound2.envelope = std::clamp(_sound2.envelope + (_sound2.envelopeDirection ? +1.f / 16.f : -1.f / 16.f), 0.f, 1.f);
  }

  // Channel 3
  for (std::size_t index = 0; index < channel3.size(); index++)
  {
    // Sound stopped
    if (!(_gbc._io[IO::NR30] & 0b10000000))
      break;

    // No envelope
    if (_sound3.envelope == 0.f)
      break;

    // Sound duration
    _sound3.length = std::max(0.f, _sound3.length - delta);
    if (_sound3.length <= 0.f)
      break;

    // Limit wave clock to one oscillation
    _sound3.clock = Math::Modulo(_sound3.clock, 1.f / _sound3.frequency);

    // Generate wave form
    channel3[index] = _sound3.envelope * _sound3.wave[(std::size_t)(_sound3.clock * _sound3.frequency * 32.f)];

    // Tick wave clock
    _sound3.clock += delta;
  }

  // Channel 4
  for (std::size_t index = 0; index < channel4.size(); index++)
  {
    // No envelope
    if (_sound4.envelope == 0.f && _sound4.envelopeDirection == false)
      break;

    // Sound duration
    _sound4.length = std::max(0.f, _sound4.length - delta);
    if (_sound4.length <= 0.f)
      break;

    // Generate wave form
    channel4[index] = _sound4.counter * _sound4.envelope;

    // Envelope change
    for (_sound4.envelopeElapsed += delta;
      _sound4.envelopeElapsed >= _sound4.envelopeTime;
      _sound4.envelopeElapsed -= _sound4.envelopeTime)
      _sound4.envelope = std::clamp(_sound4.envelope + (_sound4.envelopeDirection ? +1.f / 16.f : -1.f / 16.f), 0.f, 1.f);

    // LFSR step
    for (_sound4.counterElapsed += delta;
      _sound4.counterElapsed >= _sound4.counterTime;
      _sound4.counterElapsed -= _sound4.counterTime) {
      bool  left = (_sound4.counterValue & (0b0000000000000001 << (_sound4.counterWidth - 1))) ? true : false;
      bool  right = (_sound4.counterValue & (0b0000000000000001 << (_sound4.counterWidth - 2))) ? true : false;

      _sound4.counterValue <<= 1;
      if (left ^ right) {
        _sound4.counter *= -1.f;
        _sound4.counterValue |= 0b0000000000000001;
      }
    }
  }
  
  // Mixer and amplifier
  float leftVolume = (((_gbc._io[IO::NR50] & 0b01110000) >> 4) + 1) / 8.f;
  float rightVolume = (((_gbc._io[IO::NR50] & 0b00000111) >> 0) + 1) / 8.f;

  for (std::size_t index = 0; index < _sound.size() / 2; index++) {
    float leftSample = 0.f;
    float rightSample = 0.f;

    // Accumulate right channel
    if (_gbc._io[IO::NR51] & 0b00000001)
      rightSample += channel1[index];
    if (_gbc._io[IO::NR51] & 0b00000010)
      rightSample += channel2[index];
    if (_gbc._io[IO::NR51] & 0b00000100)
      rightSample += channel3[index];
    if (_gbc._io[IO::NR51] & 0b00001000)
      rightSample += channel4[index];

    // Accumulate left channel
    if (_gbc._io[IO::NR51] & 0b00010000)
      leftSample += channel1[index];
    if (_gbc._io[IO::NR51] & 0b00100000)
      leftSample += channel2[index];
    if (_gbc._io[IO::NR51] & 0b01000000)
      leftSample += channel3[index];
    if (_gbc._io[IO::NR51] & 0b10000000)
      leftSample += channel4[index];

    // Normalize samples
    leftSample /= 4.f;
    rightSample /= 4.f;

    // Add to sound buffer
    _sound[index * 2 + 0] = (std::uint16_t)(leftSample * leftVolume * 32767.f);
    _sound[index * 2 + 1] = (std::uint16_t)(rightSample * rightVolume * 32767.f);
  }
}

std::uint8_t  GBC::AudioProcessingUnit::readIo(std::uint16_t address)
{
  // APU disabled
  if (!(_gbc._io[IO::NR52] & 0b10000000) && address != IO::NR52 && (address < IO::WAVE00 || address > IO::WAVE30))
    return 0xFF;

  switch (address)
  {
  case IO::NR11:    // Channel 1 Length/wave pattern, R/W
  case IO::NR21:    // Channel 2 Length/wave pattern, R/W
    // Only wave pattern is readdable
    return _gbc._io[address] & 0b11000000;

  case IO::NR14:    // Channel 1 Frequency higher 3 bits, limit flag, start sound, R/W
  case IO::NR24:    // Channel 2 Frequency higher 3 bits, limit flag, start sound, R/W
  case IO::NR34:    // Channel 3 Frequency higher 3 bits, limit flag, start sound, R/W
  case IO::NR44:    // Channel 4 Limit flag, start sound, R/W
    // Only limit flag is readable
    return _gbc._io[address] & 0b01000000;

  case IO::NR52:    // Sound enable, R/W, bit 7 W/R all sound on/off (0: stop), bits 0-1-2-3 R Sound 1-2-3-4 ON flag
    return (_gbc._io[IO::NR52] & 0b10000000)
      | (_sound1.length > 0.f ? 0b00000001 : 0b00000000)
      | (_sound2.length > 0.f ? 0b00000010 : 0b00000000)
      | (_sound3.length > 0.f && (_gbc._io[IO::NR30] & 0b10000000) ? 0b00000100 : 0b00000000)
      | (_sound4.length > 0.f ? 0b00001000 : 0b00000000);

  case IO::NR10:    // Channel 1 Sweep, R/W
  case IO::NR12:    // Channel 1 Envelope, R/W
  case IO::NR22:    // Channel 2 Envelope, R/W
  case IO::NR30:    // Channel 3 Sound on/off, R/W
  case IO::NR32:    // Channel 3 Envelope, R/W
  case IO::NR42:    // Channel 4 Envelope, R/W
  case IO::NR43:    // Channel 4 Polynomial counter, R/W
  case IO::NR50:    // Sound stereo left/right volume, R/W, 6-5-4 left volume, 2-1-0 right volume (bits 7&3 not implemented, Vin output)
  case IO::NR51:    // Sound stereo left/right enable, R/W, bits 7-6-5-4 output sound 4-3-2-1 to left, bits 3-2-1-0 output sound 4-3-2-1 to right
  case IO::WAVE00:  // Channel 3 Wave pattern 00 & 01, R/W
  case IO::WAVE02:  // Channel 3 Wave pattern 02 & 03, R/W
  case IO::WAVE04:  // Channel 3 Wave pattern 04 & 05, R/W
  case IO::WAVE06:  // Channel 3 Wave pattern 06 & 07, R/W
  case IO::WAVE08:  // Channel 3 Wave pattern 08 & 09, R/W
  case IO::WAVE10:  // Channel 3 Wave pattern 10 & 11, R/W
  case IO::WAVE12:  // Channel 3 Wave pattern 12 & 13, R/W
  case IO::WAVE14:  // Channel 3 Wave pattern 14 & 15, R/W
  case IO::WAVE16:  // Channel 3 Wave pattern 16 & 17, R/W
  case IO::WAVE18:  // Channel 3 Wave pattern 18 & 19, R/W
  case IO::WAVE20:  // Channel 3 Wave pattern 20 & 21, R/W
  case IO::WAVE22:  // Channel 3 Wave pattern 22 & 23, R/W
  case IO::WAVE24:  // Channel 3 Wave pattern 24 & 25, R/W
  case IO::WAVE26:  // Channel 3 Wave pattern 26 & 27, R/W
  case IO::WAVE28:  // Channel 3 Wave pattern 28 & 29, R/W
  case IO::WAVE30:  // Channel 3 Wave pattern 30 & 31, R/W
    // Basic read, just return stored value
    return _gbc._io[address];

  case IO::NR13:    // Channel 1 Frequency lower 8 bits, W
  case IO::NR23:    // Channel 2 Frequency lower 8 bits, W
  case IO::NR31:    // Channel 3 Length, W
  case IO::NR33:    // Channel 3 Frequency lower 8 bits, W
  case IO::NR41:    // Channel 4 Length, W
    // Default value in case of error
    return 0xFF;
  
  default:
#ifdef _DEBUG
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif
    return 0xFF;
  }
}

void  GBC::AudioProcessingUnit::writeIo(std::uint16_t address, std::uint8_t value)
{
  // APU disabled
  if (!(_gbc._io[IO::NR52] & 0b10000000) && address != IO::NR52 && (address < IO::WAVE00 || address > IO::WAVE30))
    return;

  switch (address)
  {
  case IO::NR10:    // Channel 1 Sweep, R/W
    // Write register
    _gbc._io[IO::NR10] = value;

    // Update channel sweep
    _sound1.frequencyTime = (_gbc._io[IO::NR10] & 0b01110000) ?
      ((_gbc._io[IO::NR10] & 0b01110000) >> 4) / 128.f :
      999.f;
    _sound1.frequencyDirection = (_gbc._io[IO::NR10] & 0b00001000) ? true : false;
    _sound1.frequencyShift = 1.f / (float)std::pow(2.f, _gbc._io[IO::NR10] & 0b00000111);
    break;

  case IO::NR11:    // Channel 1 Length/wave pattern, R/W
    // Write register
    _gbc._io[IO::NR11] = value;

    // Update channel wave
    {
      constexpr std::array<float, 4> wave = { 0.125f, 0.25f, 0.5f, 0.75f };

      _sound1.wave = wave[(_gbc._io[IO::NR11] & 0b11000000) >> 6];
    }
    break;

  case IO::NR13:    // Channel 1 Frequency lower 8 bits, W
    // Write register
    _gbc._io[IO::NR13] = value;

    // Update frequency & wave clock to avoid cracks
    {
      auto old = _sound1.frequency;

      _sound1.frequency = 131072.f / (2048.f - (((std::uint16_t)_gbc._io[IO::NR13]) + (((std::uint16_t)_gbc._io[IO::NR14] & 0b00000111) << 8)));
      _sound1.clock *= old / _sound1.frequency;
    }

    break;

  case IO::NR14:    // Channel 1 Frequency higher 3 bits, limit flag, start sound, R/W
    // Bits 7 is always off
    _gbc._io[IO::NR14] = value & 0b01111111;

    // Update frequency & wave clock to avoid cracks
    {
      auto old = _sound1.frequency;

      _sound1.frequency = 131072.f / (2048.f - (((std::uint16_t)_gbc._io[IO::NR13]) + (((std::uint16_t)_gbc._io[IO::NR14] & 0b00000111) << 8)));
      _sound1.clock *= old / _sound1.frequency;
    }

    // Start sound 1
    if (value & 0b10000000) {
      _sound1.frequencyElapsed = 0.f;      
      _sound1.length = (_gbc._io[IO::NR14] & 0b01000000) ?
        (64 - (_gbc._io[IO::NR11] & 0b00111111)) / 256.f :
        999.f;
      _sound1.envelope = ((_gbc._io[IO::NR12] & 0b11110000) >> 4) / 16.f;
      _sound1.envelopeDirection = (_gbc._io[IO::NR12] & 0b00001000) ? true : false;
      _sound1.envelopeTime = (_gbc._io[IO::NR12] & 0b00000111) ?
        (_gbc._io[IO::NR12] & 0b00000111) / 64.f :
        999.f;
      _sound1.envelopeElapsed = 0.f;
      _sound1.clock = 0.f;
    }
    break;

  case IO::NR21:    // Channel 2 Length/wave pattern, R/W
    // Write register
    _gbc._io[IO::NR21] = value;

    // Update channel wave
    {
      constexpr std::array<float, 4> wave = { 0.125f, 0.25f, 0.5f, 0.75f };

      _sound2.wave = wave[(_gbc._io[IO::NR21] & 0b11000000) >> 6];
    }

    break;

  case IO::NR23:    // Channel 2 Frequency lower 8 bits, W
    // Write register
    _gbc._io[IO::NR23] = value;

    // Update frequency & wave clock to avoid cracks
    {
      auto old = _sound2.frequency;

      _sound2.frequency = 131072.f / (2048.f - (((std::uint16_t)_gbc._io[IO::NR23]) + (((std::uint16_t)_gbc._io[IO::NR24] & 0b00000111) << 8)));
      _sound2.clock *= old / _sound2.frequency;
    }
    break;

  case IO::NR24:    // Channel 2 Frequency higher 3 bits, limit flag, start sound, R/W
    // Bits 7 is always off
    _gbc._io[IO::NR24] = value & 0b01111111;

    // Update frequency & wave clock to avoid cracks
    {
      auto old = _sound2.frequency;

      _sound2.frequency = 131072.f / (2048.f - (((std::uint16_t)_gbc._io[IO::NR23]) + (((std::uint16_t)_gbc._io[IO::NR24] & 0b00000111) << 8)));
      _sound2.clock *= old / _sound2.frequency;
    }

    // Start sound 2
    if (value & 0b10000000) {
      _sound2.length = (_gbc._io[IO::NR24] & 0b01000000) ?
        (64 - (_gbc._io[IO::NR21] & 0b00111111)) / 256.f :
        999.f;
      _sound2.envelope = ((_gbc._io[IO::NR22] & 0b11110000) >> 4) / 16.f;
      _sound2.envelopeDirection = (_gbc._io[IO::NR22] & 0b00001000) ? true : false;
      _sound2.envelopeTime = (_gbc._io[IO::NR22] & 0b00000111) ?
        (_gbc._io[IO::NR22] & 0b00000111) / 64.f :
        999.f;
      _sound2.envelopeElapsed = 0.f;
      _sound2.clock = 0.f;
    }
    break;
  
  case IO::NR32:    // Channel 3 Envelope, R/W
    // Write register
    _gbc._io[IO::NR32] = value;

    // Update channel envelope
    {
      constexpr std::array<float, 4> envelope = { 0.f, 1.f, 0.5f, 0.25f };

      _sound3.envelope = envelope[(_gbc._io[IO::NR32] & 0b01100000) >> 5];
    }
    break;

  case IO::NR33:    // Channel 3 Frequency lower 8 bits, W
    // Write register
    _gbc._io[IO::NR33] = value;

    // Update frequency & wave clock to avoid cracks
    {
      auto old = _sound3.frequency;

      _sound3.frequency = 65536.f / (2048.f - (((std::uint16_t)_gbc._io[IO::NR33]) + (((std::uint16_t)_gbc._io[IO::NR34] & 0b00000111) << 8)));
      _sound3.clock *= old / _sound3.frequency;
    }
    break;

  case IO::NR34:    // Channel 3 Frequency higher 3 bits, limit flag, start sound, R/W
    // Bits 7 is always off
    _gbc._io[IO::NR34] = value & 0b01111111;

    // Update frequency & wave clock to avoid cracks
    {
      auto old = _sound3.frequency;

      _sound3.frequency = 65536.f / (2048.f - (((std::uint16_t)_gbc._io[IO::NR33]) + (((std::uint16_t)_gbc._io[IO::NR34] & 0b00000111) << 8)));
      _sound3.clock *= old / _sound3.frequency;
    }

    // Start sound 3
    if (value & 0b10000000) {
      _sound3.length = (_gbc._io[IO::NR34] & 0b01000000) ?
        (256 - _gbc._io[IO::NR31]) / 256.f :
        999.f;
      _sound3.clock = 0.f;
    }
    break;

  case IO::NR43:    // Channel 4 Polynomial counter, R/W
    // Write register
    _gbc._io[IO::NR43] = value;

    // Update channel counter
    _sound4.counter = +1.f;
    _sound4.counterValue = 0b1111111111111111;
    _sound4.counterWidth = (_gbc._io[IO::NR43] & 0b00001000) ? 7 : 15;
    _sound4.counterTime = 1.f / (524288.f / std::clamp((float)(_gbc._io[IO::NR43] & 0b00000111), 0.5f, 7.f) / std::pow(2.f, ((_gbc._io[IO::NR43] & 0b11110000) >> 4) + 1.f));
    break;

  case IO::NR44:    // Channel 4 Limit flag, start sound, R/W
    // Only bit 6 is writable
    _gbc._io[IO::NR44] = value & 0b01000000;

    // Start sound 4
    if (value & 0b10000000) {
      _sound4.length = (_gbc._io[IO::NR44] & 0b01000000) ?
        (64 - (_gbc._io[IO::NR41] & 0b00111111)) / 256.f :
        999.f;
      _sound4.envelope = ((_gbc._io[IO::NR42] & 0b11110000) >> 4) / 16.f;
      _sound4.envelopeDirection = (_gbc._io[IO::NR42] & 0b00001000) ? true : false;
      _sound4.envelopeTime = (_gbc._io[IO::NR42] & 0b00000111) ?
        (_gbc._io[IO::NR42] & 0b00000111) / 64.f :
        999.f;
      _sound4.envelopeElapsed = 0.f;
      _sound4.counterElapsed = 0.f;
    }
    break;

  case IO::NR52:    // Sound enable, R/W, bit 7 W/R all sound on/off (0: stop), bits 0-1-2-3 R Sound 1-2-3-4 ON flag
    // Disable all sound channels
    if (!(value & 0b10000000)) {
      _sound1.length = 0.f;
      _sound2.length = 0.f;
      _sound3.length = 0.f;
      _sound4.length = 0.f;

      // Reset registers
      for (auto reg : {
        IO::NR10, IO::NR11, IO::NR12, IO::NR13, IO::NR14,
        IO::NR21, IO::NR22, IO::NR23, IO::NR24,
        IO::NR30, IO::NR31, IO::NR32, IO::NR33, IO::NR34,
        IO::NR41, IO::NR42, IO::NR43, IO::NR44,
        IO::NR50, IO::NR51
        })
        writeIo(reg, 0);
    }

    // Only write sound on/off bit
    _gbc._io[IO::NR52] = value & 0b10000000;

    break;

  case IO::WAVE00:  // Channel 3 Wave pattern 00 & 01, R/W
  case IO::WAVE02:  // Channel 3 Wave pattern 02 & 03, R/W
  case IO::WAVE04:  // Channel 3 Wave pattern 04 & 05, R/W
  case IO::WAVE06:  // Channel 3 Wave pattern 06 & 07, R/W
  case IO::WAVE08:  // Channel 3 Wave pattern 08 & 09, R/W
  case IO::WAVE10:  // Channel 3 Wave pattern 10 & 11, R/W
  case IO::WAVE12:  // Channel 3 Wave pattern 12 & 13, R/W
  case IO::WAVE14:  // Channel 3 Wave pattern 14 & 15, R/W
  case IO::WAVE16:  // Channel 3 Wave pattern 16 & 17, R/W
  case IO::WAVE18:  // Channel 3 Wave pattern 18 & 19, R/W
  case IO::WAVE20:  // Channel 3 Wave pattern 20 & 21, R/W
  case IO::WAVE22:  // Channel 3 Wave pattern 22 & 23, R/W
  case IO::WAVE24:  // Channel 3 Wave pattern 24 & 25, R/W
  case IO::WAVE26:  // Channel 3 Wave pattern 26 & 27, R/W
  case IO::WAVE28:  // Channel 3 Wave pattern 28 & 29, R/W
  case IO::WAVE30:  // Channel 3 Wave pattern 30 & 31, R/W
    // Write wave register
    _gbc._io[address] = value;

    // Update channel 3 wave
    _sound3.wave[(address - IO::WAVE00) * 2 + 0] = ((value & 0b11110000) >> 4) / 8.f - 1.f;
    _sound3.wave[(address - IO::WAVE00) * 2 + 1] = ((value & 0b00001111) >> 0) / 8.f - 1.f;
    break;

  case IO::NR12:    // Channel 1 Envelope, R/W
  case IO::NR22:    // Channel 2 Envelope, R/W
  case IO::NR30:    // Channel 3 Sound on/off, R/W
  case IO::NR31:    // Channel 3 Length, W
  case IO::NR41:    // Channel 4 Length, W
  case IO::NR42:    // Channel 4 Envelope, R/W
  case IO::NR50:    // Sound stereo left/right volume, R/W, 6-5-4 left volume, 2-1-0 right volume (bits 7&3 not implemented, Vin output)
  case IO::NR51:    // Sound stereo left/right enable, R/W, bits 7-6-5-4 output sound 4-3-2-1 to left, bits 3-2-1-0 output sound 4-3-2-1 to right
    // Basic write, just write value to register
    _gbc._io[address] = value;
    break;

  default:
#ifdef _DEBUG
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif
    break;
  }
}


const std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize>& GBC::AudioProcessingUnit::sound() const
{
  // Return current sound buffer
  return _sound;
}

void  GBC::AudioProcessingUnit::save(std::ofstream& file) const
{
  // Save APU variables
  _gbc.save(file, "APU_SOUND1", _sound1);
  _gbc.save(file, "APU_SOUND2", _sound2);
  _gbc.save(file, "APU_SOUND3", _sound3);
  _gbc.save(file, "APU_SOUND4", _sound4);
}

void  GBC::AudioProcessingUnit::load(std::ifstream& file)
{
  // Load APU variables
  _gbc.load(file, "APU_SOUND1", _sound1);
  _gbc.load(file, "APU_SOUND2", _sound2);
  _gbc.load(file, "APU_SOUND3", _sound3);
  _gbc.load(file, "APU_SOUND4", _sound4);
}