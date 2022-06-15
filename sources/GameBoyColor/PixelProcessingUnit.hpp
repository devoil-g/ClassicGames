#pragma once

#include <array>
#include <cstdint>
#include <list>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace GBC
{
  class GameBoyColor;

  class PixelProcessingUnit
  {
  public:
    static constexpr unsigned int SpriteLimit = 10;                                                 // Maximum number of OBJ per scanline
    static constexpr unsigned int SpriteCount = 40;                                                 // Number of OBJ in OAM
    static constexpr unsigned int ScreenWidth = 160;                                                // Game Boy screen width
    static constexpr unsigned int ScreenHeight = 144;                                               // Game Boy screen height
    static constexpr unsigned int ScreenBlank = 10;                                                 // Number of VBlank scanlines
    static constexpr unsigned int Mode2Duration = 80;                                               // Duration of OAM search
    static constexpr unsigned int ScanlineDuration = 456;                                           // Number of tick per scanline
    static constexpr unsigned int FrameDuration = (ScreenHeight + ScreenBlank) * ScanlineDuration;  // Number of tick per frame
    
  public:
    enum LcdControl : std::uint8_t
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

    enum LcdStatus : std::uint8_t
    {
      LcdStatusCompare = 0b01000000,  // LYC=LY STAT Interrupt source (1: Enable) (Read/Write)
      LcdStatusMode2 = 0b00100000,    // Mode 2 OAM STAT Interrupt source (1: Enable) (Read/Write)
      LcdStatusMode1 = 0b00010000,    // Mode 1 VBlank STAT Interrupt source (1: Enable) (Read/Write)
      LcdStatusMode0 = 0b00001000,    // Mode 0 HBlank STAT Interrupt source (1: Enable) (Read/Write)
      LcdStatusEqual = 0b00000100,    // LYC=LY Flag (0: Different, 1: Equal) (Read Only)
      LcdStatusMode = 0b00000011      // Mode Flag (0: HBlank, 1: VBlank, 2: Searching OAM, 3: Transferring Data to LCD Controller) (Read Only)
    };

    enum BackgroundAttributes : std::uint8_t
    {
      BackgroundAttributesPriority = 0b10000000,  // Background priority when set
      BackgroundAttributesYFlip = 0b01000000,     // Vertical flip (0=Normal, 1=Vertically mirrored)
      BackgroundAttributesXFlip = 0b00100000,     // Horitontal flip (0=Normal, 1=Horizontally mirrored)
      BackgroundAttributesBank = 0b00001000,      // VRAM bank in CBG mode (0=Bank 0, 1=Bank 1)
      BackgroundAttributesPalette = 0b00000111    // Palette in CGB mode (BGP0-7)
    };

    enum SpriteAttributes : std::uint8_t
    {
      SpriteAttributesOverObj = 0b10000000,       // BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
      SpriteAttributesYFlip = 0b01000000,         // Vertical flip (0=Normal, 1=Vertically mirrored)
      SpriteAttributesXFlip = 0b00100000,         // Horitontal flip (0=Normal, 1=Horizontally mirrored)
      SpriteAttributesPaletteNonCgb = 0b00010000, // Palette in non-CGB mode (0=OBP0, 1=OBP1)
      SpriteAttributesBank = 0b00001000,          // VRAM bank in CBG mode (0=Bank 0, 1=Bank 1)
      SpriteAttributesPaletteCgb = 0b00000111     // Palette in CGB mode (OBP0-7)
    };

  private:
    struct Sprite
    {
      std::uint8_t                                y;          // Y position
      std::uint8_t                                x;          // X position
      std::uint8_t                                tile;       // Tile index
      GBC::PixelProcessingUnit::SpriteAttributes  attributes; // Sprite attributes
    };

    struct Color
    {
      std::uint16_t raw;  // Raw color data

      inline std::uint8_t  red() const { return ((raw >> 0) & 0b00011111) << 3; };    // Convert red component to 0-255
      inline std::uint8_t  green() const { return ((raw >> 5) & 0b00011111) << 3; };  // Convert green component to 0-255
      inline std::uint8_t  blue() const { return ((raw >> 10) & 0b00011111) << 3; };  // Convert blue component to 0-255
    };

    union Palette
    {
      std::array<GBC::PixelProcessingUnit::Color, 32> colors; // Color of palettes
      std::array<std::uint8_t, 64>                    raw;    // Raw memory
    };

    enum Mode {
      Mode0,  // Horizontal blank
      Mode1,  // Vertical blank
      Mode2,  // Searching OAM
      Mode3,  // Transferring data to LCD Controller

      ModeCount // Number of PPU mode
    };

    GBC::GameBoyColor& _gbc;  // Main GBC reference for memory bus

    std::array<std::array<std::uint8_t, 8192>, 2> _ram; // Raw Video RAM memory
    GBC::PixelProcessingUnit::Palette             _bgc; // Background color RAM (CGB mode only)
    GBC::PixelProcessingUnit::Palette             _obc; // OBJ color RAM (CGB mode only)

    union
    {
      std::array<GBC::PixelProcessingUnit::Sprite, GBC::PixelProcessingUnit::SpriteCount>                         sprites; // Access OAM sprites
      std::array<std::uint8_t, sizeof(GBC::PixelProcessingUnit::Sprite) * GBC::PixelProcessingUnit::SpriteCount>  raw;     // Raw OAM
    } _oam; // Sprite (Obj) Attribute Table

    Mode  _mode;  // Current PPU mode

    std::vector<GBC::PixelProcessingUnit::Sprite> _sprites; // Sprites to be drawn on the current line
    std::size_t                                   _cycles;  // Number of PPU cycle since first scanline
    bool                                          _enabled; // PPU enabled flag

    sf::Image   _image;   // Rendering target on RAM
    sf::Texture _texture; // Rendering target in GPU
    
    void  simulateMode0(); // Simulate an horizontal blank tick
    void  simulateMode1(); // Simulate a vertical blank tick
    void  simulateMode2(); // Simulate a searching OAM tick
    void  simulateMode3(); // Simulate a LCD Controller data transfer tick

    void  setMode(GBC::PixelProcessingUnit::Mode mode); // Set PPU to new mode
    void  nextLine();                                   // Set IO::LY to next line

  public:
    PixelProcessingUnit(GBC::GameBoyColor& gbc);
    ~PixelProcessingUnit() = default;

    std::uint8_t  readRam(std::uint16_t address); // Read a byte from Video RAM
    std::uint8_t  readOam(std::uint16_t address); // Read a byte from OBJ Attribute Table
    std::uint8_t  readBgc(std::uint8_t address);  // Read a byte from Background Color palette
    std::uint8_t  readObc(std::uint8_t address);  // Read a byte from Sprite Color palette

    void  writeRam(std::uint16_t address, std::uint8_t value);  // Write a byte to Video RAM
    void  writeOam(std::uint16_t address, std::uint8_t value);  // Write a byte to OBJ Attribute Table
    void  writeDma(std::uint16_t address, std::uint8_t value);  // Write a byte to OBJ Attribute Table for DMA transfer
    void  writeBgc(std::uint8_t address, std::uint8_t value);   // Write a byte to Background Color palette
    void  writeObc(std::uint8_t address, std::uint8_t value);   // Write a byte to Sprite Color palette

    void  simulate(); // Simulate 1 tick of the PPU

    const sf::Texture&  lcd() const;  // Get rendering target
  };
}