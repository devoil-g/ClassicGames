#pragma once

#include <array>
#include <cstdint>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include <SFML/Graphics/Image.hpp>

#include "GameBoyColor/AudioProcessingUnit.hpp"
#include "GameBoyColor/CentralProcessingUnit.hpp"
#include "GameBoyColor/MemoryBankController.hpp"
#include "GameBoyColor/PixelProcessingUnit.hpp"

namespace GBC
{
  class GameBoyColor
  {
    friend GBC::AudioProcessingUnit;
    friend GBC::CentralProcessingUnit;
    friend GBC::MemoryBankController;
    friend GBC::PixelProcessingUnit;

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

    enum IO : std::uint8_t
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
      
      KEY0 = 0x4C,  // CPU Mode, R/W, see enum
      KEY1 = 0x4D,  // CPU Speed Switch, R/W, CGB mode only, bit 7: current speed (0: normal, 1: double), bit 0: prepare switch (0: no, 1: prepare)

      BANK = 0x50,  // Boot Bank Controller, W, 0 to enable Boot mapping in ROM
      
      RP = 0x56,

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
    GBC::AudioProcessingUnit                    _apu;     // Audio Processing Unit
    std::unique_ptr<GBC::MemoryBankController>  _mbc;     // Cartridge's Memory Bank Controller
    std::array<std::uint8_t, 32 * 1024>         _wRam;    // Raw Work RAM memory
    std::array<std::uint8_t, 128>               _io;      // IO registers
    std::array<std::uint8_t, 127>               _hRam;    // Raw High RAM memory
    std::uint8_t                                _ie;      // Interrupt Enable register
    
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
    void  simulateTimer();          // Update 4 CPU cycle of TIMA/TMA/DIV timer registers

  public:
    GameBoyColor(const std::string& filename);
    ~GameBoyColor() = default;

    void                                                                  simulate();     // Simulate a frame
    const sf::Texture&                                                    lcd() const;    // Get rendering target
    const std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize>& sound() const;  // Get current sound frame
    const GBC::GameBoyColor::Header&                                      header() const; // Get game header
  };
}