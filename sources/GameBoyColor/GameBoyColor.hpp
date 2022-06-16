#pragma once

#include <array>
#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include <SFML/Graphics/Image.hpp>

#include "GameBoyColor/CentralProcessingUnit.hpp"
#include "GameBoyColor/MemoryBankController.hpp"
#include "GameBoyColor/PixelProcessingUnit.hpp"

namespace GBC
{
  class GameBoyColor
  {
    friend GBC::CentralProcessingUnit;
    friend GBC::MemoryBankController;
    friend GBC::PixelProcessingUnit;

  public:
    static const std::size_t  SoundSampleRate = 44100;                                            // Sample rate of sound
    static const std::size_t  SoundFrameSize = SoundSampleRate * (456 * 154) / (4 * 1024 * 1024); // Number of sample in each frame of sound
    static const std::size_t  SoundChannelCount = 2;                                              // Number of sound channels
    static const std::size_t  SoundBufferSize = SoundFrameSize * SoundChannelCount;               // Size of a sound buffer

  private:
    struct Header
    {
      enum MBC
      {
        MBCNone,  // None (32KByte ROM only)
        MBC1,     // MBC1 (max 2MByte ROM and/or 32KByte RAM)
        MBC2,     // MBC2 (max 256KByte ROM and 512x4 bits RAM)
        MBC3,     // MBC3 (max 2MByte ROM and/or 64KByte RAM and Timer)
        MBC5,     // MBC5 (max 8MByte ROM and/or 128KByte RAM)
      };

      enum CGBFlag
      {
        CGBSupport, // Game supports CGB functions, but works on old gameboys also.
        CGBOnly,    // Game works on CGB only (physically the same as CGBSupport).
        CGBNone     // No flag (older cartridges)
      };

      enum SGBFlag
      {
        SGBSupport, // Game supports SGB functions
        SGBNone     // No SGB functions (Normal Gameboy or CGB only game)
      };

      enum Region
      {
        RegionJP,     // Japanese
        RegionUSEU,   // US and Europe
        RegionUnknow  // Unknow region
      };

      bool            logo;             // False if unofficial logo
      std::string     title;            // Name of the game
      std::string     manufacturer;     // Manufacturer code (newer cartridges only)
      Header::CGBFlag cgb;              // CGB support flag
      std::uint16_t   licensee;         // Licensee Code
      Header::SGBFlag sgb;              // Support of the SGB functions
      Header::MBC     mbc;              // Memory Bank Controller format
      std::size_t     rom_size;         // Size of ROM
      std::size_t     ram_size;         // Size of RAM
      Header::Region  region;           // Game region
      std::uint8_t    version;          // Specifies the version number of the game
      bool            header_checksum;  // Checksum of the ROM header
      bool            global_checksum;  // Global checksum of the ROM
    };

    enum Interrupt  // Used by 0xFFFF IE (Interrupt Enable) & 0xFF0F IF (Interrupt Flag)
    {
      InterruptVBlank = 0b00000001,
      InterruptLcdStat = 0b00000010,
      InterruptTimer = 0b00000100,
      InterruptSerial = 0b00001000,
      InterruptJoypad = 0b00010000
    };

    enum IO
    {
      JOYP = 0x00,  // Joypad, R/W, bits 7-6 always set
      SB = 0x01,
      SC = 0x02,
      DIVLo = 0x03, // Low byte of DIV, not accessible
      DIVHi = 0x04, // High byte of DIV, R/W (always set to zero when written)
      TIMA = 0x05,  // Timer Counter, R/W
      TMA = 0x06,   // Timer Modulo, R/W
      TAC = 0x07,   // Time Control, R/W of bit 2-1-0

      IF = 0x0F,    // Interrupt Flags, bits set when an event occured, bits 7-6-5 always set
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

      LCDC = 0x40,  // LCD Control, R/W (see enum)
      STAT = 0x41,  // LCD Status, R/W (see enum)
      SCY = 0x42,   // Scroll Y, R/W
      SCX = 0x43,   // Scroll X, R/W
      LY = 0x44,    // LCD Y Coordinate, R
      LYC = 0x45,   // LCD Y Coordinate Compare, R/W
      DMA = 0x46,   // DMA Transfer and Start Address, R/W
      BGP = 0x47,   // Background and Window Palette Data, R/W, non CGB mode only
      OBP0 = 0x48,  // OBJ 0 Palette Data, R/W, non CGB mode only
      OBP1 = 0x49,  // OBJ 1 Palette Data, R/W, non CGB mode only  
      WY = 0x4A,    // Window Y Position, R/W
      WX = 0x4B,    // Window X Position, R/W
      KEY0 = 0x4C,  // CPU Mode, R/W, see enum
      KEY1 = 0x4D,  // CPU Speed Switch, R/W, CGB mode only, bit 7: current speed (0: normal, 1: double), bit 0: prepare switch (0: no, 1: prepare)

      VBK = 0x4F,   // Video RAM Bank, R/W, CGB mode only
      BANK = 0x50,  // Boot Bank Controller, W, 0 to enable Boot mapping in ROM
      HDMA1 = 0x51, // New DMA Transfers source high byte, W, CGB mode only
      HDMA2 = 0x52, // New DMA Transfers source low byte, W, CGB mode only
      HDMA3 = 0x53, // New DMA Transfers destination high byte, W, CGB mode only
      HDMA4 = 0x54, // New DMA Transfers destination low byte, W, CGB mode only
      HDMA5 = 0x55, // Start New DMA Transfer, R/W, CGB mode only
      RP = 0x56,

      BCPI = 0x68,  // Background Color Palette Index, R/W, CGB mode only
      BCPD = 0x69,  // Background Color Palette Data, R/W, reference byte in Background Color RAM at index BCPI, CGB mode only
      OCPI = 0x6A,  // OBJ Color Palette Index, R/W, CGB mode only
      OCPD = 0x6B,  // OBJ Color Palette Data, R/W, reference byte in OBJ Color RAM at index OCPI, CGB mode only
      OPRI = 0x6C,  // OBJ Priority Mode, R/W, CGB mode only, bit 0: mode (0 :OAM, 1: Coordinate)

      SVBK = 0x70   // Work Ram Bank, R/W, CGB mode only
    };

    enum CpuMode
    {
      CpuModeDmg = 0b00000100,    // Disable all CGB functions (0: CGB, 1: DMG) RP, VBK, SVBK + ExtDmg functions
      CpuModeExtDmg = 0b00001000, // Disable some CGB functions (0: normal, 1: Extended DMG mode) HDMA, BCPD, OCPD, KEY1
    };

    enum IME
    {
      IMEDisabled,  // Interrupt disabled
      IMEEnabled,   // Interrupt enabled
      IMEScheduled  // Enable interrupt after next instruction
    };

    GBC::GameBoyColor::Header                   _header;  // Main info of the ROM
    std::string                                 _path;    // Path of the ROM
    std::vector<std::uint8_t>                   _boot;    // Bootstrap sequence memory
    std::size_t                                 _cycles;  // Number of CPU cycle since boot
    GBC::CentralProcessingUnit                  _cpu;     // Central Processing Unit
    GBC::PixelProcessingUnit                    _ppu;     // Pixel Processing Unit
    std::unique_ptr<GBC::MemoryBankController>  _mbc;     // Cartridge's Memory Bank Controller
    std::array<std::uint8_t, 32 * 1024>         _wRam;    // Raw Work RAM memory
    std::array<std::uint8_t, 128>               _io;      // IO registers
    std::array<std::uint8_t, 127>               _hRam;    // Raw High RAM memory
    std::uint8_t                                _ie;      // Interrupt Enable register
    
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

    std::array<std::int16_t, GBC::GameBoyColor::SoundBufferSize> _sound; // Sound buffer of current frame

    enum Key
    {
      KeyDown,
      KeyUp,
      KeyLeft,
      KeyRight,
      KeyStart,
      KeySelect,
      KeyB,
      KeyA,

      KeyCount
    };

    std::array<bool, Key::KeyCount> _keys;  // Currently pressed keys

    void  load(const std::string& filename);                                              // Load a new ROM in memory
    void  loadFile(const std::string& filename, std::vector<std::uint8_t>& destination);  // Load file to vector
    void  loadHeader(const std::vector<uint8_t>& rom);                                    // Get header data

    std::uint8_t  read(std::uint16_t addr);     // Read one byte from memory
    std::uint8_t  readWRam(std::uint16_t addr); // Read one byte from WRAM
    std::uint8_t  readIo(std::uint16_t addr);   // Read one byte from IO register
    std::uint8_t  readHRam(std::uint16_t addr); // Read one byte from HRAM
    
    void  write(std::uint16_t addr, std::uint8_t value);      // Write one byte to memory
    void  writeWRam(std::uint16_t addr, std::uint8_t value);  // Write one byte to WRAM
    void  writeIo(std::uint16_t addr, std::uint8_t value);    // Write one byte to IO register
    void  writeHRam(std::uint16_t addr, std::uint8_t value);  // Write one byte to HRAM

    void  simulateKeys();           // Handle keys
    void  simulateAudio();          // Update audio for one frame (4 * 1024 * 1024 cycles)
    void  simulateTimer();          // Update 4 CPU cycle of TIMA/TMA/DIV timer registers

  public:
    GameBoyColor(const std::string& filename);
    ~GameBoyColor() = default;

    void                                                                simulate();     // Simulate a frame
    const sf::Texture&                                                  lcd() const;    // Get rendering target
    const std::array<std::int16_t, GBC::GameBoyColor::SoundBufferSize>& sound() const;  // Get current sound frame
    const GBC::GameBoyColor::Header&                                    header() const; // Get game header
  };
}