#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <list>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <functional>
#include <fstream>
#include <iostream>

#include <SFML/Graphics/Texture.hpp>

#include "GameBoyColor/AudioProcessingUnit.hpp"
#include "GameBoyColor/CentralProcessingUnit.hpp"
#include "GameBoyColor/MemoryBankController.hpp"
#include "GameBoyColor/PixelProcessingUnit.hpp"
#include "Math/Vector.hpp"

namespace GBC
{
  class MemoryBankController1;
  class MemoryBankController3;

  class GameBoyColor
  {
    friend GBC::AudioProcessingUnit;
    friend GBC::CentralProcessingUnit;
    friend GBC::MemoryBankController;
    friend GBC::MemoryBankController1;
    friend GBC::MemoryBankController3;
    friend GBC::PixelProcessingUnit;

  public:
    enum Control
    {
      Control1, // First controller, or ZQSD + TFEA
      Control2, // Second controller, or UHJK + PLIY

      ControlsCount
    };

  private:
    static const std::string_view SaveStateBase;

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

    enum Interrupt : std::uint8_t // Used by 0xFFFF IE (Interrupt Enable) & 0xFF0F IF (Interrupt Flag)
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
      SB = 0x01,    // Serial transfer Data, R/W
      SC = 0x02,    // Serial transfer Control, R/W, bit 0: shift clock (0: external, 1: internal), bit 1: clock speed (0: normal, 1: fast), bit 7: transfer start flag
      DIVLo = 0x03, // Low byte of DIV, not accessible
      DIVHi = 0x04, // High byte of DIV, R/W (always set to zero when written)
      TIMA = 0x05,  // Timer Counter, R/W
      TMA = 0x06,   // Timer Modulo, R/W
      TAC = 0x07,   // Time Control, R/W of bit 2-1-0

      IF = 0x0F,    // Interrupt Flags, bits set when an event occured, bits 7-6-5 always set

      DMA = 0x46,   // DMA Transfer and Start Address, R/W

      KEY0 = 0x4C,  // CPU Mode, R/W, see enum
      KEY1 = 0x4D,  // CPU Speed Switch, R/W, CGB mode only, bit 7: current speed (0: normal, 1: double), bit 0: prepare switch (0: no, 1: prepare)

      BANK = 0x50,  // Boot Bank Controller, W, 0 to enable Boot mapping in ROM

      HDMA1 = 0x51, // New DMA Transfers source high byte, W, CGB mode only
      HDMA2 = 0x52, // New DMA Transfers source low byte, W, CGB mode only
      HDMA3 = 0x53, // New DMA Transfers destination high byte, W, CGB mode only
      HDMA4 = 0x54, // New DMA Transfers destination low byte, W, CGB mode only
      HDMA5 = 0x55, // Start New DMA Transfer, R/W, CGB mode only

      RP = 0x56,    // Infared communication, bit 0: LED on/off (R/W), bit 1: signal off/on (R), bit 7-6: read enable (3) / disable (0) (signal will stay off)

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

    GBC::GameBoyColor::Header                   _header;    // Main info of the ROM
    std::filesystem::path                       _path;      // Path of the ROM
    std::vector<std::uint8_t>                   _boot;      // Bootstrap sequence memory
    std::size_t                                 _cycles;    // Number of CPU cycle since boot
    GBC::CentralProcessingUnit                  _cpu;       // Central Processing Unit
    GBC::PixelProcessingUnit                    _ppu;       // Pixel Processing Unit
    GBC::AudioProcessingUnit                    _apu;       // Audio Processing Unit
    std::unique_ptr<GBC::MemoryBankController>  _mbc;       // Cartridge's Memory Bank Controller
    std::array<std::uint8_t, 32 * 1024>         _wRam;      // Raw Work RAM memory
    std::array<std::uint8_t, 128>               _io;        // IO registers
    std::array<std::uint8_t, 127>               _hRam;      // Raw High RAM memory
    std::uint8_t                                _ie;        // Interrupt Enable register
    
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

    std::array<bool, Key::KeyCount> _keys;    // Currently pressed keys
    GBC::GameBoyColor::Control      _control; // Controller used

    enum Transfer
    {
      TransferNone,   // No transfer
      TransferDma,    // DMA transfer
      TransferHdma0,  // HDMA 0 transfer
      TransferHdma1,  // HDMA 1 transfer
    };

    Transfer    _transferMode;    // Current transfer mode
    std::size_t _transferIndex;   // Index of running transfer
    bool        _transferTrigger; // Trigger for HDMA1 transfer

    void  load(const std::filesystem::path& filename);                                              // Load a new ROM in memory
    void  loadFile(const std::filesystem::path& filename, std::vector<std::uint8_t>& destination);  // Load file to vector
    void  loadHeader(const std::vector<uint8_t>& rom);                                              // Get header data

    std::uint8_t  read(std::uint16_t addr);     // Read one byte from memory
    std::uint8_t  readWRam(std::uint16_t addr); // Read one byte from WRAM
    std::uint8_t  readIo(std::uint16_t addr);   // Read one byte from IO register
    std::uint8_t  readHRam(std::uint16_t addr); // Read one byte from HRAM
    
    void  write(std::uint16_t addr, std::uint8_t value);      // Write one byte to memory
    void  writeWRam(std::uint16_t addr, std::uint8_t value);  // Write one byte to WRAM
    void  writeIo(std::uint16_t addr, std::uint8_t value);    // Write one byte to IO register
    void  writeHRam(std::uint16_t addr, std::uint8_t value);  // Write one byte to HRAM

    void  simulateKeys();   // Handle keys
    void  simulateTimer();  // Update 4 CPU cycle of TIMA/TMA/DIV timer registers

    void        save(std::ofstream& file, const std::string& name, const void* data, std::size_t size) const; // Save raw variable data to file
    void        save(std::ofstream& file, const std::string& name, const std::string& data) const;            // Save string variable to file

    template<typename Type>
    void        save(std::ofstream& file, const std::string& name, const std::vector<Type>& data) const       // Save vector variable to file
    {
      save(file, name, data.data(), data.size() * sizeof(Type));
    }

    template<typename Type>
    void        save(std::ofstream& file, const std::string& name, const Type& data) const                    // Save variable to file
    {
      save(file, name, &data, sizeof(Type));
    }

    std::string loadVariable(std::ifstream& file, const std::string& name);                                   // Load raw variable data from file
    void        loadValue(const std::string& value, void* data, std::size_t size);                            // Load raw variable data from file
    void        load(std::ifstream& file, const std::string& name, std::string& data);                        // Load string variable to file

    template<typename Type>
    void        load(std::ifstream& file, const std::string& name, std::vector<Type>& data)                   // Load vector variable to file
    {
      // Get value from file
      std::string value = loadVariable(file, name);

      // Resize target vector
      data.resize(value.length() / 2 / sizeof(Type));

      // Load value to vector
      loadValue(value, data.data(), data.size() * sizeof(Type));
    }

    template<typename Type>
    void        load(std::ifstream& file, const std::string& name, Type& data)                                // Load variable from file
    {
      // Get value from file
      std::string value = loadVariable(file, name);

      // Load value to variable
      loadValue(value, (void*)&data, sizeof(data));
    }

  public:
    GameBoyColor(const std::filesystem::path& filename, sf::Texture& texture, Math::Vector<2, unsigned int> origin, GBC::GameBoyColor::Control control = GBC::GameBoyColor::Control::Control1);
    ~GameBoyColor() = default;

    void  simulate();       // Simulate a frame
    void  simulatePre();    // Simulate a CPU tick
    void  simulateCycle();  // Simulate a CPU tick
    void  simulatePost();   // Simulate a CPU tick

    std::size_t                                                           cycles() const; // Get cycle count
    const sf::Texture&                                                    lcd() const;    // Get rendering target
    const std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize>& sound() const;  // Get current sound frame
    const GBC::GameBoyColor::Header&                                      header() const; // Get game header

    void  load(std::size_t id);       // Load saved state
    void  save(std::size_t id) const; // Save state
  };
}