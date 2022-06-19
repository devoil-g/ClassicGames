#include <cstring>
#include <stdexcept>
#include <string>

#include "GameBoyColor/GameBoyColor.hpp"
#include "GameBoyColor/PixelProcessingUnit.hpp"

GBC::PixelProcessingUnit::PixelProcessingUnit(GBC::GameBoyColor& gbc) :
  _gbc(gbc),
  _ram{ 0 },
  _bgc{ 0 },
  _obc{ 0 },
  _oam{ 0 },
  _mode(Mode::Mode2),
  _sprites(),
  _cycles(0),
  _enabled(false),
  _image(),
  _texture()
{
  // Allocate image memory buffers
  _image.create(GBC::PixelProcessingUnit::ScreenWidth, GBC::PixelProcessingUnit::ScreenHeight, sf::Color::White);
  _texture.create(_image.getSize().x, _image.getSize().y);

  // White image
  _texture.update(_image);

  // Texture is not filtered
  _texture.setSmooth(false);
}

void  GBC::PixelProcessingUnit::simulate()
{
  // Does nothing when PPU is disabled
  if (_enabled == false)
    return;

  constexpr std::array<void(GBC::PixelProcessingUnit::*)(), Mode::ModeCount> modes = {
    &GBC::PixelProcessingUnit::simulateMode0,
    &GBC::PixelProcessingUnit::simulateMode1,
    &GBC::PixelProcessingUnit::simulateMode2,
    &GBC::PixelProcessingUnit::simulateMode3
  };

  // Call PPU mode handler
  (*this.*(modes[_mode % Mode::ModeCount]))();
}

void  GBC::PixelProcessingUnit::simulateMode0()
{
  // Next cycle
  _cycles += 1;

  // Next mode
  if (_cycles % GBC::PixelProcessingUnit::ScanlineDuration == 0)
  {
    // Increment current line
    nextLine();

    // Draw next line
    if (_gbc._io[IO::LY] < GBC::PixelProcessingUnit::ScreenHeight)
      setMode(Mode::Mode2);

    // Start VBlank
    else {
      setMode(Mode::Mode1);
      _texture.update(_image);
    }
  }
}

void  GBC::PixelProcessingUnit::simulateMode1()
{
  // Next cycle
  _cycles += 1;

  // Next mode
  if (_cycles % GBC::PixelProcessingUnit::ScanlineDuration == 0)
  {
    // Increment current line
    nextLine();

    // Start drawing frame
    if (_gbc._io[IO::LY] == 0)
      setMode(Mode::Mode2);
  }
}

void  GBC::PixelProcessingUnit::simulateMode2()
{
  // Process all OBJ at once
  if (_cycles % GBC::PixelProcessingUnit::ScanlineDuration == 0)
  {
    // Reset OBJ list
    _sprites.clear();
    _sprites.reserve(GBC::PixelProcessingUnit::SpriteCount);

    // Push Y matching OBJ in order
    for (const auto& sprite : _oam.sprites)
      if (sprite.y <= _gbc._io[IO::LY] + 16 &&
        sprite.y + ((_gbc._io[IO::LCDC] & LcdControl::LcdControlObjSize) ? 16 : 8) > _gbc._io[IO::LY] + 16)
        _sprites.push_back(sprite);

    // Limit to 10 OBJ
    if (_sprites.size() > GBC::PixelProcessingUnit::SpriteLimit)
      _sprites.erase(std::next(_sprites.begin(), GBC::PixelProcessingUnit::SpriteLimit), _sprites.end());

    // In non-CGB mode, OBJ draw priority depends on its X position
    if ((_gbc._io[IO::OPRI] & 0b00000001) || (_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100))
      std::sort(_sprites.begin(), _sprites.end(), [](const auto& a, const auto& b) { return a.x < b.x; });
  }

  // Next cycle
  _cycles += 1;

  // Next mode
  if (_cycles % GBC::PixelProcessingUnit::ScanlineDuration == GBC::PixelProcessingUnit::Mode2Duration) {
    setMode(Mode::Mode3);
  }
}

void  GBC::PixelProcessingUnit::simulateMode3()
{
  // Render current line
  if (_cycles % GBC::PixelProcessingUnit::ScanlineDuration == GBC::PixelProcessingUnit::Mode2Duration)
  {
    std::uint8_t  sc_y = _gbc._io[IO::LY];
    std::uint8_t  bg_y = sc_y + _gbc._io[IO::SCY];
    std::uint8_t  wn_y = sc_y - _gbc._io[IO::WY];

    for (std::uint8_t sc_x = 0; sc_x < GBC::PixelProcessingUnit::ScreenWidth; sc_x++)
    {
      std::uint8_t                    bg_color_id = 255;
      std::uint8_t                    bg_attributes = 0;
      GBC::PixelProcessingUnit::Color bg_color = { 0 };
      std::uint8_t                    sp_color_id = 255;
      std::uint8_t                    sp_attributes = 0;
      GBC::PixelProcessingUnit::Color sp_color = { 0 };

      // Window
      if ((!(_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100) || (_gbc._io[IO::LCDC] & LcdControl::LcdControlWindowBackgroundEnable)) &&
        (_gbc._io[IO::LCDC] & LcdControl::LcdControlWindowEnable) &&
        (sc_x + 7 >= _gbc._io[IO::WX]) &&
        (sc_y >= _gbc._io[IO::WY]))
      {
        std::uint8_t  wn_x = sc_x - _gbc._io[IO::WX] + 7;
        std::uint16_t wn_tilemap = (_gbc._io[IO::LCDC] & LcdControl::LcdControlWindowTilemap) ? 0x1C00 : 0x1800;
        std::uint16_t wn_tilemap_id = (((std::uint16_t)wn_y / 8) * 32) + ((std::uint16_t)wn_x / 8);
        bg_attributes = _ram[1][wn_tilemap + wn_tilemap_id];
        std::uint8_t  wn_tile_id = _ram[0][wn_tilemap + wn_tilemap_id];
        std::uint16_t wn_tile_index = ((std::uint16_t)wn_tile_id * 16)
          + ((_gbc._io[IO::LCDC] & LcdControl::LcdControlData) ?
            0x0000 :
            ((wn_tile_id < 128) ? 0x1000 : 0x0000));

        // Flip tile coordinates
        std::uint8_t  wn_tile_x = (bg_attributes & BackgroundAttributes::BackgroundAttributesXFlip) ? ((8 - 1) - wn_x % 8) : (wn_x % 8);
        std::uint8_t  wn_tile_y = (bg_attributes & BackgroundAttributes::BackgroundAttributesYFlip) ? ((8 - 1) - wn_y % 8) : (wn_y % 8);

        bg_color_id =
          ((_ram[(bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ? 1 : 0][wn_tile_index + (wn_tile_y % 8) * 2 + 0] & (0b10000000 >> (wn_tile_x % 8))) ? 1 : 0) +
          ((_ram[(bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ? 1 : 0][wn_tile_index + (wn_tile_y % 8) * 2 + 1] & (0b10000000 >> (wn_tile_x % 8))) ? 2 : 0);

        // DMG color palette
        if (_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100)
          bg_color_id = (_gbc._io[IO::BGP] >> (bg_color_id * 2)) & 0b00000011;

        // Get color from palette
        bg_color = _bgc.colors[(bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id];
      }

      // Background
      else if (!(_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100) || (_gbc._io[IO::LCDC] & LcdControl::LcdControlWindowBackgroundEnable))
      {
        std::uint8_t  bg_x = sc_x + _gbc._io[IO::SCX];
        std::uint16_t bg_tilemap = (_gbc._io[IO::LCDC] & LcdControl::LcdControlBackgroundTilemap) ? 0x1C00 : 0x1800;
        std::uint16_t bg_tilemap_id = (((std::uint16_t)bg_y / 8) * 32) + ((std::uint16_t)bg_x / 8);
        bg_attributes = _ram[1][bg_tilemap + bg_tilemap_id];
        std::uint8_t  bg_tile_id = _ram[0][bg_tilemap + bg_tilemap_id];
        std::uint16_t bg_tile_index = ((std::uint16_t)bg_tile_id * 16)
          + ((_gbc._io[IO::LCDC] & LcdControl::LcdControlData) ?
            0x0000 :
            ((bg_tile_id < 128) ? 0x1000 : 0x0000));

        // Flip tile coordinates
        std::uint8_t  bg_tile_x = (bg_attributes & BackgroundAttributes::BackgroundAttributesXFlip) ? ((8 - 1) - bg_x % 8) : (bg_x % 8);
        std::uint8_t  bg_tile_y = (bg_attributes & BackgroundAttributes::BackgroundAttributesYFlip) ? ((8 - 1) - bg_y % 8) : (bg_y % 8);

        bg_color_id =
          ((_ram[(bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ? 1 : 0][bg_tile_index + (bg_tile_y % 8) * 2 + 0] & (0b10000000 >> (bg_tile_x % 8))) ? 1 : 0) +
          ((_ram[(bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ? 1 : 0][bg_tile_index + (bg_tile_y % 8) * 2 + 1] & (0b10000000 >> (bg_tile_x % 8))) ? 2 : 0);

        // DMG color palette
        if (_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100)
          bg_color_id = (_gbc._io[IO::BGP] >> (bg_color_id * 2)) & 0b00000011;

        // Get color from palette
        bg_color = _bgc.colors[(bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id];
      }

      // Sprite
      if (_gbc._io[IO::LCDC] & LcdControl::LcdControlObjEnable)
      {
        // Find first matching sprite
        for (const auto& sprite : _sprites) {
          std::uint8_t  sp_y = sc_y - sprite.y + 16;
          std::uint8_t  sp_x = sc_x - sprite.x + 8;

          // Sprite not on column
          if (sp_x >= 8)
            continue;

          std::uint8_t  sp_height = (_gbc._io[IO::LCDC] & LcdControl::LcdControlObjSize) ? 16 : 8;
          sp_attributes = sprite.attributes;

          // Flip sprite coordinates
          if (sp_attributes & SpriteAttributes::SpriteAttributesYFlip)
            sp_y = (sp_height - 1) - sp_y % sp_height;
          if (sp_attributes & SpriteAttributes::SpriteAttributesXFlip)
            sp_x = (8 - 1) - sp_x % 8;

          std::uint8_t  sp_tile_id = (sprite.tile & ((sp_height == 8) ? 0b11111111 : 0b11111110)) + ((sp_y < 8) ? 0 : 1);
          std::uint16_t sp_tile_index = ((std::uint16_t)sp_tile_id * 16);
          sp_color_id =
            ((_ram[(sp_attributes & SpriteAttributes::SpriteAttributesBank) ? 1 : 0][sp_tile_index + (sp_y % 8) * 2 + 0] & (0b10000000 >> (sp_x % 8))) ? 1 : 0) +
            ((_ram[(sp_attributes & SpriteAttributes::SpriteAttributesBank) ? 1 : 0][sp_tile_index + (sp_y % 8) * 2 + 1] & (0b10000000 >> (sp_x % 8))) ? 2 : 0);

          // ID 0 is transparency
          if (sp_color_id == 0) {
            sp_color_id = 255;
            continue;
          }

          // DMG color palette
          if (_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100) {
            if (sp_attributes & SpriteAttributes::SpriteAttributesPaletteNonCgb) {
              sp_color_id = (_gbc._io[IO::OBP1] >> (sp_color_id * 2)) & 0b00000011;
              sp_attributes = (sp_attributes & 0b11110000) | 0b00000001;
            }
            else {
              sp_color_id = (_gbc._io[IO::OBP0] >> (sp_color_id * 2)) & 0b00000011;
              sp_attributes = (sp_attributes & 0b11110000) | 0b00000000;
            }
          }

          // Get color from palette
          sp_color = _obc.colors[(sp_attributes & SpriteAttributes::SpriteAttributesPaletteCgb) * 4 + sp_color_id];
          break;
        }
      }

      // Background and Window Master Priority, CGB mode only
      if (bg_color_id != 255 &&
        sp_color_id != 255 &&
        !(_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100) &&
        !(_gbc._io[IO::LCDC] & LcdControl::LcdControlPriority))
        bg_color_id = 255;

      // Sprite priority over background and window
      else if (bg_color_id != 255 &&
        sp_color_id != 255 &&
        ((bg_attributes & BackgroundAttributes::BackgroundAttributesPriority) || (sp_attributes & SpriteAttributes::SpriteAttributesOverObj)) &&
        bg_color_id != 0)
        sp_color_id = 255;

      sf::Color color;

      // Select color to use
      if (sp_color_id != 255)
        color = sf::Color(sp_color.red(), sp_color.green(), sp_color.blue());
      else if (bg_color_id != 255)
        color = sf::Color(bg_color.red(), bg_color.green(), bg_color.blue());
      else
        color = sf::Color(255, 255, 255);

      // Draw pixel
      _image.setPixel(sc_x, sc_y, color);
    }
  }

  // End of drawing, go to HBlank
  else if (_cycles % GBC::PixelProcessingUnit::ScanlineDuration == GBC::PixelProcessingUnit::Mode2Duration + 172)
    setMode(Mode::Mode0);

  // Next cycle
  _cycles += 1;
}

const sf::Texture&  GBC::PixelProcessingUnit::lcd() const
{
  // Return rendering target
  return _texture;
}

void  GBC::PixelProcessingUnit::setMode(GBC::PixelProcessingUnit::Mode mode)
{
  // Change PPU mode
  _mode = mode;

  // Set LCD status mode
  _gbc._io[IO::STAT] = (_gbc._io[IO::STAT] & ~LcdStatus::LcdStatusMode) | (mode << 0);

  constexpr std::array<GBC::PixelProcessingUnit::LcdStatus, Mode::ModeCount> mask = {
    LcdStatus::LcdStatusMode0,
    LcdStatus::LcdStatusMode1,
    LcdStatus::LcdStatusMode2,
    (GBC::PixelProcessingUnit::LcdStatus)0
  };

  // Generate STAT interrupt
  if (_gbc._io[IO::STAT] & mask[mode])
    _gbc._io[GBC::GameBoyColor::IO::IF] |= GBC::GameBoyColor::Interrupt::InterruptLcdStat;

  // Generate VBlank interrupt
  if (mode == Mode::Mode1)
    _gbc._io[GBC::GameBoyColor::IO::IF] |= GBC::GameBoyColor::Interrupt::InterruptVBlank;
}

void  GBC::PixelProcessingUnit::nextLine()
{
  // Increment to next line
  _gbc._io[IO::LY] += 1;

  // Limit to 144 + 10 lines
  if (_gbc._io[IO::LY] >= GBC::PixelProcessingUnit::ScreenHeight + GBC::PixelProcessingUnit::ScreenBlank)
    _gbc._io[IO::LY] = 0;

  // LY / LCY register comparison
  if (_gbc._io[IO::LY] == _gbc._io[IO::LYC]) {
    _gbc._io[IO::STAT] |= LcdStatus::LcdStatusEqual;

    // STAT compare (LY/LYC) interrupt
    if (_gbc._io[IO::STAT] & LcdStatus::LcdStatusCompare)
      _gbc._io[GBC::GameBoyColor::IO::IF] |= GBC::GameBoyColor::Interrupt::InterruptLcdStat;
  }
  else
    _gbc._io[IO::STAT] &= ~LcdStatus::LcdStatusEqual;
}

std::uint8_t  GBC::PixelProcessingUnit::readRam(std::uint16_t address)
{
  // Out of bound address, should not happen
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Can't read during rendering
  if (_mode == Mode::Mode3)
    return 0xFF;

  // Read data from Video RAM
  return _ram[_gbc._io[IO::VBK] & 0b00000001][address];
}

std::uint8_t  GBC::PixelProcessingUnit::readOam(std::uint16_t address)
{
  // Out of bound address, should not happen
  if (address >= sizeof(_oam))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Can't read during sprite rendering
  if (_mode == Mode::Mode2 ||
    _mode == Mode::Mode3)
    return 0xFF;

  // Read data from OAM
  return _oam.raw[address];
}

std::uint8_t  GBC::PixelProcessingUnit::readIo(std::uint16_t address)
{
  switch (address) {
  case IO::VBK:   // Video RAM Bank, R/W, CGB mode only
    return _gbc._io[IO::VBK] | 0b11111110;

  case IO::BCPD:  // Background Color Palette Data, R/W, reference byte in Background Color RAM at index BCPI, CGB mode only
    // Can't read during rendering
    if (_mode == Mode::Mode3)
      return 0xFF;

    // Read data from Background Color palette
    else
      return _bgc.raw[_gbc._io[IO::BCPI] & 0b00111111];

  case IO::OCPD:  // OBJ Color Palette Data, R/W, reference byte in OBJ Color RAM at index OCPI, CGB mode only
    // Can't read during rendering
    if (_mode == Mode::Mode3)
      return 0xFF;

    // Read data from Sprite Color palette
    else
      return _obc.raw[_gbc._io[IO::OCPI] & 0b00111111];

  case IO::LCDC:  // LCD Control, R/W (see enum)
  case IO::STAT:  // LCD Status, R/W (see enum)
  case IO::SCY:   // Scroll Y, R/W
  case IO::SCX:   // Scroll X, R/W
  case IO::LY:    // LCD Y Coordinate, R
  case IO::LYC:   // LCD Y Coordinate Compare, R/W
  case IO::DMA:   // DMA Transfer and Start Address, R/W
  case IO::BGP:   // Background and Window Palette Data, R/W, non CGB mode only
  case IO::OBP0:  // OBJ 0 Palette Data, R/W, non CGB mode only
  case IO::OBP1:  // OBJ 1 Palette Data, R/W, non CGB mode only  
  case IO::WY:    // Window Y Position, R/W
  case IO::WX:    // Window X Position, R/W
  case IO::HDMA5: // Start New DMA Transfer, R/W, CGB mode only
  case IO::BCPI:  // Background Color Palette Index, R/W, CGB mode only
  case IO::OCPI:  // OBJ Color Palette Index, R/W, CGB mode only
  case IO::OPRI:  // OBJ Priority Mode, R/W, CGB mode only, bit 0: mode (0 :OAM, 1: Coordinate)
    // Basic read, just return stored value
    return _gbc._io[address];

  case IO::HDMA1: // New DMA Transfers source high byte, W, CGB mode only
  case IO::HDMA2: // New DMA Transfers source low byte, W, CGB mode only
  case IO::HDMA3: // New DMA Transfers destination high byte, W, CGB mode only
  case IO::HDMA4: // New DMA Transfers destination low byte, W, CGB mode only
    // Not accessible
    return 0xFF;

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

void  GBC::PixelProcessingUnit::writeRam(std::uint16_t address, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Can't write during rendering
  if (_mode == Mode::Mode3)
    return;

  // Write data to Video RAM
  _ram[_gbc._io[IO::VBK] & 0b00000001][address] = value;
}

void  GBC::PixelProcessingUnit::writeOam(std::uint16_t address, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (address >= sizeof(_oam))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Can't write during sprite rendering
  if (_mode == Mode::Mode2 ||
    _mode == Mode::Mode3)
    return;

  // Write data to OAM
  _oam.raw[address] = value;
}

void  GBC::PixelProcessingUnit::writeDma(std::uint16_t address, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (address >= sizeof(_oam))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Write data to OAM
  _oam.raw[address] = value;

  // Sprite of current scanline are not displayed during DMA transfer
  _sprites.clear();
}

void          GBC::PixelProcessingUnit::writeIo(std::uint16_t address, std::uint8_t value)
{
  switch (address)
  {
  case IO::LCDC:  // LCD Control, R/W (see enum)
    if (_enabled == true && !(value & LcdControl::LcdControlEnable)) {
      _enabled = false;
      _gbc._io[IO::LY] = 0;
      _cycles = 0;
      _mode = Mode::Mode0;

      // White image
      std::memset((std::uint8_t*)_image.getPixelsPtr(), 0xFF, _image.getSize().x * _image.getSize().y * 4);
      _texture.update(_image);
    }

    // PPU enabled, start
    else if (_enabled == false && (value & LcdControl::LcdControlEnable)) {
      _enabled = true;
      _mode = Mode::Mode2;
    }

    // Write to register
    _gbc._io[IO::LCDC] = value;
    break;

  case IO::STAT:  // LCD Status, R/W (see enum)
    // Only bits 6-5-4-3 are writable 
    _gbc._io[IO::STAT] = (_gbc._io[IO::STAT] & 0b00000111) | (value & 0b01111000);
    break;

  case IO::LYC:   // LCD Y Coordinate Compare, R/W
    // Write value to register
    _gbc._io[IO::LYC] = value;

    // LY / LCY register comparison
    if (_gbc._io[IO::LY] == _gbc._io[IO::LYC]) {
      _gbc._io[IO::STAT] |= LcdStatus::LcdStatusEqual;

      // STAT compare (LY/LYC) interrupt
      if (_gbc._io[IO::STAT] & LcdStatus::LcdStatusCompare)
        _gbc._io[GBC::GameBoyColor::IO::IF] |= GBC::GameBoyColor::Interrupt::InterruptLcdStat;
    }
    else
      _gbc._io[IO::STAT] &= ~LcdStatus::LcdStatusEqual;
    break;

  case IO::DMA:   // DMA Transfer and Start Address, R/W
    // Write value to register
    _gbc._io[IO::DMA] = value;

    // Transfer data to OAM
    for (std::uint16_t index = 0; index < 160; index++)
      writeDma(index, _gbc.read(((std::uint16_t)_gbc._io[IO::DMA] << 8) + index));

    // TODO: DMA transfer is 160 cycles long, lock memory access
    break;

  case IO::BCPD:  // Background Color Palette Data, R/W, reference byte in Background Color RAM at index BCPI, CGB mode only
    // Can't write during rendering
    if (_mode != Mode::Mode3)
      _bgc.raw[_gbc._io[IO::BCPI] & 0b00111111] = value;

    // Increment pointer
    if (_gbc._io[IO::BCPI] & 0b10000000)
      _gbc._io[IO::BCPI] = 0b10000000 | ((_gbc._io[IO::BCPI] + 1) & 0b00111111);
    break;

  case IO::OCPD:  // OBJ Color Palette Data, R/W, reference byte in OBJ Color RAM at index OCPI, CGB mode only
    // Can't write during rendering
    if (_mode != Mode::Mode3)
      _obc.raw[_gbc._io[IO::OCPI] & 0b00111111] = value;

    // Increment pointer
    if (_gbc._io[IO::OCPI] & 0b10000000)
      _gbc._io[IO::OCPI] = 0b10000000 | ((_gbc._io[IO::OCPI] + 1) & 0b00111111);
    break;

  case IO::OPRI:  // OBJ Priority Mode, R/W, CGB mode only, bit 0: mode (0 :OAM, 1: Coordinate)
    _gbc._io[IO::OPRI] = value & 0b00000001;
    break;

  case IO::LY:    // LCD Y Coordinate, R
    // Not accessible
    break;

  case IO::HDMA5: // Start New DMA Transfer, R/W, CGB mode only
  {
    std::uint16_t source = (((std::uint16_t)_gbc._io[IO::HDMA1] << 8) + (std::uint16_t)_gbc._io[IO::HDMA2]) & 0b1111111111110000;
    std::uint16_t destination = ((((std::uint16_t)_gbc._io[IO::HDMA3] << 8) + (std::uint16_t)_gbc._io[IO::HDMA4]) & 0b00011111111110000) | 0b1000000000000000;
    std::uint16_t length = ((std::uint16_t)(value & 0b01111111) + 1) * 0x10;

    // Transfer data to VRAM
    for (unsigned int count = 0; count < length; count++) {
      std::uint8_t  data;

      // Get data from source
      if ((source >= 0x0000 && source < 0x8000) ||
        (source >= 0xA000 && source < 0xC000) ||
        (source >= 0xC000 && source < 0xE000))
        data = _gbc.read(source);
      else
        break;

      // Write data to destination
      if (destination >= 0x8000 && destination < 0xA000)
        writeRam(destination - 0x8000, data);
      else
        break;

      // Increment addresses
      source += 1;
      destination += 1;
    }

    // Set register to completed transfer value
    _gbc._io[IO::HDMA5] = 0xFF;

    // Write end addresses to HDMA registers
    _gbc._io[IO::HDMA1] = (source >> 8) & 0b11111111;
    _gbc._io[IO::HDMA2] = (source >> 0) & 0b11111111;
    _gbc._io[IO::HDMA3] = (destination >> 8) & 0b11111111;
    _gbc._io[IO::HDMA4] = (destination >> 0) & 0b11111111;

    // TODO: respect HDMA transfer timing for mode 0 & 1
    break;
  }

  case IO::SCY:   // Scroll Y, R/W
  case IO::SCX:   // Scroll X, R/W
  case IO::BGP:   // Background and Window Palette Data, R/W, non CGB mode only
  case IO::OBP0:  // OBJ 0 Palette Data, R/W, non CGB mode only
  case IO::OBP1:  // OBJ 1 Palette Data, R/W, non CGB mode only
  case IO::WY:    // Window Y Position, R/W
  case IO::WX:    // Window X Position, R/W
  case IO::VBK:   // Video RAM Bank, R/W, CGB mode only
  case IO::HDMA1: // New DMA Transfers source high byte, W, CGB mode only
  case IO::HDMA2: // New DMA Transfers source low byte, W, CGB mode only
  case IO::HDMA3: // New DMA Transfers destination high byte, W, CGB mode only
  case IO::HDMA4: // New DMA Transfers destination low byte, W, CGB mode only
  case IO::BCPI:  // Background Color Palette Index, R/W, CGB mode only
  case IO::OCPI:  // OBJ Color Palette Index, R/W, CGB mode only
    _gbc._io[address] = value;
    break;

  default:
    printf("unknow register write %02X %02X\n", address, value);
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}
