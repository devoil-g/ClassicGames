#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <functional>

#include <SFML/Graphics/Image.hpp>

#include "Scenes/AbstractScene.hpp"

namespace GBC
{
  class GameBoyColor
  {
  public:
    static const std::size_t  SoundSampleRate = 22050;                                            // Sample rate of sound
    static const std::size_t  SoundFrameSize = SoundSampleRate * (456 * 154) / (4 * 1024 * 1024); // Number of sample in each frame of sound
    static const std::size_t  SoundChannelCount = 2;                                              // Number of sound channels
    static const std::size_t  SoundBufferSize = SoundFrameSize * SoundChannelCount;               // Size of a sound buffer

  private:
    union MemoryBankController
    {
      enum Type
      {
        None,   // None (32KByte ROM only)
        MBC1,   // MBC1 (max 2MByte ROM and/or 32KByte RAM)
        MBC2,   // MBC2 (max 256KByte ROM and 512x4 bits RAM)
        MBC3,   // MBC3 (max 2MByte ROM and/or 64KByte RAM and Timer)
        MBC5,   // MBC5 (max 8MByte ROM and/or 128KByte RAM)
        Unknow  // Unknow MBC
      };

      struct {
      } none; // No MBC

      struct {
        std::uint8_t  enable; // RAM enable flag (0x0A to enable)
        std::uint8_t  bank;   // RAM & ROM bank number, ROM bits 4-3-2-1-0, RAM/ROM upper bits 5-6, Mode bits 7
      } mbc1; // MBC1

      struct {
        std::uint8_t  enable; // RAM enable flag (0x0A to enable)
        std::uint8_t  bank;   // ROM bank number, bits 3-2-1-0
      } mbc2; // MBC2

      struct {
        std::uint8_t  enable; // RAM and RTC Register enable flag (0x0A to enable)
        std::uint8_t  rom;    // ROM bank number, bits 6-5-4-3-2-1-0
        std::uint8_t  ram;    // RAM bank number / RTC Register select
        std::uint8_t  latch;  // Latch Clock Data
        std::uint8_t  halt;   // Stop flag of clock (0=Active, 1=Stop Timer)
        std::uint64_t rtc;    // Latched Real Time Clock registers
        std::uint64_t time;   // Current time
      } mbc3; // MBC3

      struct {
        std::uint8_t  enable; // RAM enable flag (0x0A to enable)
        std::uint16_t rom;    // ROM bank number (9 bits)
        std::uint8_t  ram;    // RAM bank number, rumble on bit 3
      } mbc5; // MBC5

      // TODO: other MBC
    };

    struct Header
    {
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

      bool                        logo;             // False if unofficial logo
      std::string                 title;            // Name of the game
      std::string                 manufacturer;     // Manufacturer code (newer cartridges only)
      Header::CGBFlag             cgb;              // CGB support flag
      std::uint16_t               licensee;         // Licensee Code
      Header::SGBFlag             sgb;              // Support of the SGB functions
      MemoryBankController::Type  mbc;              // Memory Bank Controller format
      std::size_t                 rom_size;         // Size of ROM
      std::size_t                 ram_size;         // Size of RAM
      Header::Region              region;           // Game region
      std::uint8_t                version;          // Specifies the version number of the game
      bool                        header_checksum;  // Checksum of the ROM header
      bool                        global_checksum;  // Global checksum of the ROM
    };

    union Register
    {
      enum
      {
        Lo = 0, // Access lower 8 bits of register
        Hi = 1  // Access higher 8 bits of register
      };

      enum
      {
        Z = 0b10000000, // Zero, set when the result of a math operation is zero or two values match when using CP
        N = 0b01000000, // Subtract, set if a subtraction was performed in the last math instruction
        H = 0b00100000, // Half carry, set if a carry occurred from the lower nibble in the last math operation
        C = 0b00010000  // Carry, set if a carry occurred from the last math operation or if register A is the smaller value when executing the CP instruction
      };

      std::uint16_t               u16;  // Unsigned 16 bits value
      std::int16_t                s16;  // Signed 16 bits value
      std::array<std::uint8_t, 2> u8;   // Low/High unsigned 8 bits value
      std::array<std::int8_t, 2>  s8;   // Low/High signed 8 bits value
    };

    enum Interrupt  // Used by 0xFFFF IE (Interrupt Enable) & 0xFF0F IF (Interrupt Flag)
    {
      InterruptVBlank = 0b00000001,
      InterruptLcdStat = 0b00000010,
      InterruptTimer = 0b00000100,
      InterruptSerial = 0b00001000,
      InterruptJoypad = 0b00010000
    };

    enum Registers
    {
      RegisterJOYP = 0x00,  // Joypad, R/W, bits 7-6 always set
      RegisterSB = 0x01,
      RegisterSC = 0x02,
      RegisterDIVLo = 0x03, // Low byte of DIV, not accessible
      RegisterDIVHi = 0x04, // High byte of DIV, R/W (always set to zero when written)
      RegisterTIMA = 0x05,  // Timer Counter, R/W
      RegisterTMA = 0x06,   // Timer Modulo, R/W
      RegisterTAC = 0x07,   // Time Control, R/W of bit 2-1-0

      RegisterIF = 0x0F,    // Interrupt Flags, bits set when an event occured, bits 7-6-5 always set
      RegisterNR10 = 0x10,  // Channel 1 Sweep, R/W
      RegisterNR11 = 0x11,  // Channel 1 Length/wave pattern, R/W
      RegisterNR12 = 0x12,  // Channel 1 Envelope, R/W
      RegisterNR13 = 0x13,  // Channel 1 Frequency lower 8 bits, W
      RegisterNR14 = 0x14,  // Channel 1 Frequency higher 3 bits, limit flag, start sound, R/W

      RegisterNR21 = 0x16,  // Channel 2 Length/wave pattern, R/W
      RegisterNR22 = 0x17,  // Channel 2 Envelope, R/W
      RegisterNR23 = 0x18,  // Channel 2 Frequency lower 8 bits, W
      RegisterNR24 = 0x19,  // Channel 2 Frequency higher 3 bits, limit flag, start sound, R/W
      RegisterNR30 = 0x1A,
      RegisterNR31 = 0x1B,
      RegisterNR32 = 0x1C,
      RegisterNR33 = 0x1D,
      RegisterNR34 = 0x1E,

      RegisterNR41 = 0x20,
      RegisterNR42 = 0x21,
      RegisterNR43 = 0x22,
      RegisterNR44 = 0x23,
      RegisterNR50 = 0x24,  // Sound stereo left/right volume, R/W, 6-5-4 left volume, 2-1-0 right volume (bits 7&3 not implemented, Vin output)
      RegisterNR51 = 0x25,  // Sound stereo left/right enable, R/W, bits 7-6-5-4 output sound 4-3-2-1 to left, bits 3-2-1-0 output sound 4-3-2-1 to right
      RegisterNR52 = 0x26,  // Sound enable, R/W, bit 7 W/R all sound on/off (0: stop), bits 0-1-2-3 R Sound 1-2-3-4 ON flag

      RegisterLCDC = 0x40,  // LCD Control, R/W (see enum)
      RegisterSTAT = 0x41,  // LCD Status, R/W (see enum)
      RegisterSCY = 0x42,   // Scroll Y, R/W
      RegisterSCX = 0x43,   // Scroll X, R/W
      RegisterLY = 0x44,    // LCD Y Coordinate, R
      RegisterLYC = 0x45,   // LCD Y Coordinate Compare, R/W
      RegisterDMA = 0x46,   // DMA Transfer and Start Address, R/W
      RegisterBGP = 0x47,   // Background and Window Palette Data, R/W, non CGB mode only
      RegisterOBP0 = 0x48,  // OBJ 0 Palette Data, R/W, non CGB mode only
      RegisterOBP1 = 0x49,  // OBJ 1 Palette Data, R/W, non CGB mode only  
      RegisterWY = 0x4A,    // Window Y Position, R/W
      RegisterWX = 0x4B,    // Window X Position, R/W
      RegisterKEY0 = 0x4C,  // CPU Mode, R/W, see enum
      RegisterKEY1 = 0x4D,  // CPU Speed Switch, R/W, CGB mode only, bit 7: current speed (0: normal, 1: double), bit 0: prepare switch (0: no, 1: prepare)

      RegisterVBK = 0x4F,   // Video RAM Bank, R/W, CGB mode only
      RegisterBANK = 0x50,  // Boot Bank Controller, W, 0 to enable Boot mapping in ROM
      RegisterHDMA1 = 0x51, // New DMA Transfers source high byte, W, CGB mode only
      RegisterHDMA2 = 0x52, // New DMA Transfers source low byte, W, CGB mode only
      RegisterHDMA3 = 0x53, // New DMA Transfers destination high byte, W, CGB mode only
      RegisterHDMA4 = 0x54, // New DMA Transfers destination low byte, W, CGB mode only
      RegisterHDMA5 = 0x55, // Start New DMA Transfer, R/W, CGB mode only
      RegisterRP = 0x56,

      RegisterBCPI = 0x68,  // Background Color Palette Index, R/W, CGB mode only
      RegisterBCPD = 0x69,  // Background Color Palette Data, R/W, reference byte in Background Color RAM at index BCPI, CGB mode only
      RegisterOCPI = 0x6A,  // OBJ Color Palette Index, R/W, CGB mode only
      RegisterOCPD = 0x6B,  // OBJ Color Palette Data, R/W, reference byte in OBJ Color RAM at index OCPI, CGB mode only
      RegisterOPRI = 0x6C,  // OBJ Priority Mode, R/W, CGB mode only, bit 0: mode (0 :OAM, 1: Coordinate)

      RegisterSVBK = 0x70   // Work Ram Bank, R/W, CGB mode only
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

    enum LcdControl
    {
      LcdControlEnable = 0b10000000,                  // LCD and PPU enable (0=Off, 1=On)
      LcdControlWindowTilemap = 0b01000000,           // Window tilemap area (0=9800-9BFF, 1=9C00-9FFF)
      LcdControlWindowEnable = 0b00100000,            // Window enable (0=Off, 1=On)
      LcdControlData = 0b00010000,                    // Background and Window tile data area (0=8800-97FF, 1=8000-8FFF)
      LcdControlBackgroundTilemap = 0b00001000,       // Background tilemap area (0=9800-9BFF, 1=9C00-9FFF)
      LcdControlObjSize = 0b00000100,                 // OBJ size (0=8x8, 1=8x16)
      LcdControlObjEnable = 0b00000010,               // OBJ enable (0=Off, 1=On)
      LcdControlWindowBackgroundEnable = 0b00000001,  // Background and Window enable (0=Off, 1=On), non-CGB mode only
      LcdControlPriority = 0b00000001,                // Background and Window Master Priority (0=Off, 1=On), CGB mode only
    };

    enum LcdStatus
    {
      LcdStatusCompare = 0b01000000,  // LYC=LY STAT Interrupt source (1: Enable) (Read/Write)
      LcdStatusMode2 = 0b00100000,    // Mode 2 OAM STAT Interrupt source (1: Enable) (Read/Write)
      LcdStatusMode1 = 0b00010000,    // Mode 1 VBlank STAT Interrupt source (1: Enable) (Read/Write)
      LcdStatusMode0 = 0b00001000,    // Mode 0 HBlank STAT Interrupt source (1: Enable) (Read/Write)
      LcdStatusEqual = 0b00000100,    // LYC=LY Flag (0: Different, 1: Equal) (Read Only)
      LcdStatusMode = 0b00000011      // Mode Flag (0: HBlank, 1: VBlank, 2: Searching OAM, 3: Transferring Data to LCD Controller) (Read Only)
    };

    enum BackgroundAttributes
    {
      BackgroundAttributesPriority = 0b10000000,  // Background priority when set
      BackgroundAttributesYFlip = 0b01000000,     // Vertical flip (0=Normal, 1=Vertically mirrored)
      BackgroundAttributesXFlip = 0b00100000,     // Horitontal flip (0=Normal, 1=Horizontally mirrored)
      BackgroundAttributesBank = 0b00001000,      // VRAM bank in CBG mode (0=Bank 0, 1=Bank 1)
      BackgroundAttributesPalette = 0b00000111    // Palette in CGB mode (BGP0-7)
    };

    enum SpriteAttributes
    {
      SpriteAttributesOverObj = 0b10000000,       // BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
      SpriteAttributesYFlip = 0b01000000,         // Vertical flip (0=Normal, 1=Vertically mirrored)
      SpriteAttributesXFlip = 0b00100000,         // Horitontal flip (0=Normal, 1=Horizontally mirrored)
      SpriteAttributesPaletteNonCgb = 0b00010000, // Palette in non-CGB mode (0=OBP0, 1=OBP1)
      SpriteAttributesBank = 0b00001000,          // VRAM bank in CBG mode (0=Bank 0, 1=Bank 1)
      SpriteAttributesPaletteCgb = 0b00000111     // Palette in CGB mode (OBP0-7)
    };

    struct Instruction
    {
      std::string_view                            description;  // Description of the instruction
      std::function<void(GBC::GameBoyColor& gbc)> instruction;  // Code to execute
    };

    static const std::array<GBC::GameBoyColor::Instruction, 256>  _instructions;    // Processor instructions by OP code (8 bits)
    static const std::array<GBC::GameBoyColor::Instruction, 256>  _instructionsCB;  // Processor 2 bytes instructions (CB) by OP code (8 bits)


    void  instructionAdd(std::uint8_t left, std::uint8_t right, std::uint8_t carry, std::uint8_t& result);
    void  instructionAdd(std::uint16_t left, std::uint16_t right, std::uint16_t& result);
    void  instructionSub(std::uint8_t left, std::uint8_t right, std::uint8_t carry, std::uint8_t& result);
    void  instructionCp(std::uint8_t left, std::uint8_t right);
    void  instructionInc(std::uint8_t& value);
    void  instructionDec(std::uint8_t& value);
    void  instructionAnd(std::uint8_t left, std::uint8_t right, std::uint8_t& result);
    void  instructionOr(std::uint8_t left, std::uint8_t right, std::uint8_t& result);
    void  instructionXor(std::uint8_t left, std::uint8_t right, std::uint8_t& result);
    void  instructionRlc(std::uint8_t& value);
    void  instructionRrc(std::uint8_t& value);
    void  instructionRl(std::uint8_t& value);
    void  instructionRr(std::uint8_t& value);
    void  instructionSla(std::uint8_t& value);
    void  instructionSra(std::uint8_t& value);
    void  instructionSwap(std::uint8_t& value);
    void  instructionSrl(std::uint8_t& value);
    void  instructionPush(std::uint16_t value);
    void  instructionPop(std::uint16_t& result);
    void  instructionCall(std::uint16_t addr, std::uint16_t ret);
    void  instructionRet();

    template <unsigned int Bit>
    void  instructionBit(std::uint8_t value)
    {
      if (value & (0b00000001 << Bit))
        _rAF.u8[Register::Lo] &= ~Register::Z;
      else
        _rAF.u8[Register::Lo] |= Register::Z;
      _rAF.u8[Register::Lo] &= ~Register::N;
      _rAF.u8[Register::Lo] |= Register::H;
    }

    enum CPU
    {
      CPURun,             // Running CPU instructions
      CPUHalt,            // Stop CPU waiting for events
      CPUStop = CPUHalt   // Same as Halt, turn-off display (NOTE: handled as a simple HALT)
    };

    std::string                         _path;      // Path of the ROM
    std::vector<std::uint8_t>           _boot;      // Bootstrap sequence memory
    std::vector<std::uint8_t>           _rom;       // Raw ROM memory
    std::array<std::uint8_t, 16 * 1024> _vRam;      // Raw Video RAM memory
    std::vector<std::uint8_t>           _eRam;      // Raw External RAM memory
    std::array<std::uint8_t, 32 * 1024> _wRam;      // Raw Work RAM memory
    std::array<std::uint8_t, 160>       _oam;       // Sprite (Obj) Attribute Table
    std::array<std::uint8_t, 128>       _io;        // IO registers
    std::array<std::uint8_t, 127>       _hRam;      // Raw High RAM memory
    std::uint8_t                        _ie;        // Interrupt Enable register
    std::array<std::uint8_t, 64>        _bgcRam;    // Background color RAM (CGB mode only)
    std::array<std::uint8_t, 64>        _obcRam;    // OBJ color RAM (CGB mode only)
    
    GameBoyColor::CPU _cpu; // Current CPU state
    GameBoyColor::IME _ime; // Interrupt Master Enable

    std::uint64_t           _cpuCycle;  // Number of CPU cycle since boot
    std::uint64_t           _ppuCycle;  // Number of PPU cycle since last frame
    std::list<std::uint8_t> _ppuObj;    // OBJ index of sprite to be drawn on the current line
    sf::Image               _ppuLcd;    // Image representing the LCD Screen

    GameBoyColor::Header  _header;  // Main info of the RAM

    GameBoyColor::Register  _rAF; // AF register (accumulator + flags)
    GameBoyColor::Register  _rBC; // BC register
    GameBoyColor::Register  _rDE; // DE register
    GameBoyColor::Register  _rHL; // HL register
    GameBoyColor::Register  _rSP; // SP register (stack pointer)
    GameBoyColor::Register  _rPC; // PC register (program counter / pointer)

    GameBoyColor::MemoryBankController  _mbc;

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

    std::array<bool, Key::KeyCount> _keys;

    void  load(const std::string& filename);                                              // Load a new ROM in memory
    void  loadFile(const std::string& filename, std::vector<std::uint8_t>& destination);  // Load file to vector
    void  loadHeader();                                                                   // Get header data
    
    std::uint8_t  read(std::uint16_t addr);     // Read one byte from memory
    std::uint8_t  readRom(std::uint16_t addr);  // Read one byte from ROM
    std::uint8_t  readVRam(std::uint16_t addr); // Read one byte from VRAM
    std::uint8_t  readERam(std::uint16_t addr); // Read one byte from ERAM
    std::uint8_t  readWRam(std::uint16_t addr); // Read one byte from WRAM
    std::uint8_t  readOam(std::uint16_t addr);  // Read one byte from Obj Attribute Table
    std::uint8_t  readIo(std::uint16_t addr);   // Read one byte from IO register
    std::uint8_t  readHRam(std::uint16_t addr); // Read one byte from HRAM
    
    template<typename Type>
    Type  read(std::uint16_t addr)
    {
      union {
        Type                                    value;
        std::array<std::uint8_t, sizeof(Type)>  data;
      } output = { .value = 0 };
      
      // Extract each byte individually
      for (unsigned int index = 0; index < output.data.size(); index++)
        output.data[index] = read(addr + index);
      
      return output.value;
    }

    void  write(std::uint16_t addr, std::uint8_t value);      // Write one byte to memory
    void  writeRom(std::uint16_t addr, std::uint8_t value);   // Write one byte to ROM
    void  writeVRam(std::uint16_t addr, std::uint8_t value);  // Write one byte to VRAM
    void  writeERam(std::uint16_t addr, std::uint8_t value);  // Write one byte to ERAM
    void  writeWRam(std::uint16_t addr, std::uint8_t value);  // Write one byte to WRAM
    void  writeOam(std::uint16_t addr, std::uint8_t value);   // Write one byte to Obj Attribute Table
    void  writeIo(std::uint16_t addr, std::uint8_t value);    // Write one byte to IO register
    void  writeHRam(std::uint16_t addr, std::uint8_t value);  // Write one byte to HRAM

    template<typename Type>
    void  write(std::uint16_t addr, Type value)
    {
      union {
        Type                                    value;
        std::array<std::uint8_t, sizeof(Type)>  data;
      } input = { .value = value };
      
      // Write each byte individually
      for (unsigned int index = 0; index < input.data.size(); index++)
        write(addr + index, input.data[index]);
    }

    void  simulateKeys();         // Handle keys
    void  simulateInterrupt();      // Detect interrupt
    void  simulateInstruction();    // Run an instruction
    void  simulateGraphics();       // Update 4 CPU cycles of graphics
    void  simulateGraphicsMode2();  // Search OAM for OBJs that overlap current line
    void  simulateGraphicsMode3();  // Generate the picture
    void  simulateAudio();          // Update audio for one frame (4 * 1024 * 1024 cycles)
    void  simulateTimer();          // Update 4 CPU cycle of TIMA/TMA/DIV timer registers

    void  loadEram(); // Load External RAM from path+.gbs file
    void  saveEram(); // Save External RAM to path+.gbs file

  public:
    GameBoyColor(const std::string& filename);
    ~GameBoyColor();

    void                                                                simulate();     // Simulate a frame
    const sf::Image&                                                    screen() const; // Get current LCD frame
    const std::array<std::int16_t, GBC::GameBoyColor::SoundBufferSize>& sound() const;  // Get current sound frame
  };
}