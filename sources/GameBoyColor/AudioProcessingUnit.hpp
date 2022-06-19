#pragma once

#include <cstdint>
#include <functional>
#include <queue>

#include "GameBoyColor/PixelProcessingUnit.hpp"

namespace GBC
{
  class GameBoyColor;

  class AudioProcessingUnit
  {
  public:
    static const std::size_t  SampleRate = 44100;                                                                                                                                                           // Sample rate of sound
    static const std::size_t  ChannelCount = 2;                                                                                                                                                             // Number of sound channels
    static const std::size_t  FrameSize = SampleRate * (GBC::PixelProcessingUnit::ScanlineDuration * (GBC::PixelProcessingUnit::ScreenHeight + GBC::PixelProcessingUnit::ScreenBlank)) / (4 * 1024 * 1024); // Number of sample in each frame of sound
    static const std::size_t  BufferSize = FrameSize * ChannelCount;                                                                                                                                        // Size of a sound buffer

    enum IO : std::uint8_t
    {
      NR10 = 0x10,  // Channel 1 Sweep, R/W
      NR11 = 0x11,  // Channel 1 Length/wave pattern, R/W
      NR12 = 0x12,  // Channel 1 Envelope, R/W
      NR13 = 0x13,  // Channel 1 Frequency lower 8 bits, W
      NR14 = 0x14,  // Channel 1 Frequency higher 3 bits, limit flag, start sound, R/W

      NR21 = 0x16,  // Channel 2 Length/wave pattern, R/W
      NR22 = 0x17,  // Channel 2 Envelope, R/W
      NR23 = 0x18,  // Channel 2 Frequency lower 8 bits, W
      NR24 = 0x19,  // Channel 2 Frequency higher 3 bits, limit flag, start sound, R/W
      NR30 = 0x1A,  // Channel 3 Sound on/off, R/W
      NR31 = 0x1B,  // Channel 3 Length, W
      NR32 = 0x1C,  // Channel 3 Envelope, R/W
      NR33 = 0x1D,  // Channel 3 Frequency lower 8 bits, W
      NR34 = 0x1E,  // Channel 3 Frequency higher 3 bits, limit flag, start sound, R/W

      NR41 = 0x20,  // Channel 4 Length, W
      NR42 = 0x21,  // Channel 4 Envelope, R/W
      NR43 = 0x22,  // Channel 4 Polynomial counter, R/W
      NR44 = 0x23,  // Channel 4 Limit flag, start sound, R/W
      NR50 = 0x24,  // Sound stereo left/right volume, R/W, 6-5-4 left volume, 2-1-0 right volume (bits 7&3 not implemented, Vin output)
      NR51 = 0x25,  // Sound stereo left/right enable, R/W, bits 7-6-5-4 output sound 4-3-2-1 to left, bits 3-2-1-0 output sound 4-3-2-1 to right
      NR52 = 0x26,  // Sound enable, R/W, bit 7 W/R all sound on/off (0: stop), bits 0-1-2-3 R Sound 1-2-3-4 ON flag

      WAVE00 = 0x30,  // Channel 3 Wave pattern 00 & 01, R/W
      WAVE02 = 0x31,  // Channel 3 Wave pattern 02 & 03, R/W
      WAVE04 = 0x32,  // Channel 3 Wave pattern 04 & 05, R/W
      WAVE06 = 0x33,  // Channel 3 Wave pattern 06 & 07, R/W
      WAVE08 = 0x34,  // Channel 3 Wave pattern 08 & 09, R/W
      WAVE10 = 0x35,  // Channel 3 Wave pattern 10 & 11, R/W
      WAVE12 = 0x36,  // Channel 3 Wave pattern 12 & 13, R/W
      WAVE14 = 0x37,  // Channel 3 Wave pattern 14 & 15, R/W
      WAVE16 = 0x38,  // Channel 3 Wave pattern 16 & 17, R/W
      WAVE18 = 0x39,  // Channel 3 Wave pattern 18 & 19, R/W
      WAVE20 = 0x3A,  // Channel 3 Wave pattern 20 & 21, R/W
      WAVE22 = 0x3B,  // Channel 3 Wave pattern 22 & 23, R/W
      WAVE24 = 0x3C,  // Channel 3 Wave pattern 24 & 25, R/W
      WAVE26 = 0x3D,  // Channel 3 Wave pattern 26 & 27, R/W
      WAVE28 = 0x3E,  // Channel 3 Wave pattern 28 & 29, R/W
      WAVE30 = 0x3F,  // Channel 3 Wave pattern 30 & 31, R/W
    };

  private:
    GBC::GameBoyColor& _gbc;  // Main GBC reference for memory bus

    struct {
      float frequencyTime;      // NR10 - Time between frequency steps (seconds)
      bool  frequencyDirection; // NR10 - Direction of frequency sweep (false: addition, true: subtraction)
      float frequencyShift;     // NR10 - Intensity of frequency sweep shift (F(t) = F(t-1) +/- F(t-1)*shift)
      float frequencyElapsed;   // Elasped time since last frequency sweep
      float wave;               // NR11 - Square wave pattern duty (percentage of time at 0)
      float length;             // NR11/14 - Sound length (seconds)
      float envelope;           // NR12 - Initial envelope volume
      bool  envelopeDirection;  // NR12 - Direction of envelope (false: decrease, true: increase)
      float envelopeTime;       // NR12 - Length of 1 step envelope (seconds)
      float envelopeElapsed;    // Elapsed time since last envelope change
      float frequency;          // NR13/14 - Frequency
      float clock;              // Wave clock
    } _sound1;  // Data of sound channel 1

    struct {
      float wave;               // NR21 - Square wave pattern duty (percentage of time at 0)
      float length;             // NR21 - Sound length (seconds)
      float envelope;           // NR22 - Initial envelope volume
      bool  envelopeDirection;  // NR22 - Direction of envelope (false: decrease, true: increase)
      float envelopeTime;       // NR22 - Length of 1 step envelope (seconds)
      float envelopeElapsed;    // Elapsed time since last envelope change
      float frequency;          // NR23/24 - Frequency
      float clock;              // Wave clock
    } _sound2;  // Data of sound channel 2

    struct {
      std::array<float, 32> wave;       // FF30-FF3F - Wave pattern
      float                 length;     // NR31 - Sound length (seconds)
      float                 envelope;   // NR32 - Envelope volume
      float                 frequency;  // NR33/34 - Frequency
      float                 clock;      // Wave clock
    } _sound3;  // Data of sound channel 3

    struct {
      float         length;             // NR41 - Sound length (seconds)
      float         envelope;           // NR42 - Initial envelope volume
      bool          envelopeDirection;  // NR42 - Direction of envelope (false: decrease, true: increase)
      float         envelopeTime;       // NR42 - Length of 1 step envelope (seconds)
      float         envelopeElapsed;    // Elapsed time since last envelope change
      float         counter;            // Counter wave
      std::uint16_t counterValue;       // LFSR counter
      std::uint8_t  counterWidth;       // NR43 - LFSR counter width
      float         counterTime;        // NR43 - LFSR counter length of step (seconds)
      float         counterElapsed;     // Elapsed time since last LFSR change
    } _sound4;  // Data of sound channel 4

    std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize> _sound; // Sound buffer of current frame

  public:
    AudioProcessingUnit(GBC::GameBoyColor& gbc);
    ~AudioProcessingUnit() = default;

    void  simulate(); // Simulate 70224 clock ticks (1 PPU frame) of the APU

    std::uint8_t  readIo(std::uint16_t address);                      // Read an audio IO
    void          writeIo(std::uint16_t address, std::uint8_t value); // Write an audio IO

    const std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize>& sound() const;  // Get sound buffer
  };
}