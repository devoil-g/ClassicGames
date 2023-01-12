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
  _mode(Mode::Mode0),
  _sprites(),
  _cycles(0),
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
  if (!(_gbc._io[IO::LCDC] & LcdControl::LcdControlEnable))
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
    // First line, reset window flag
    if (_gbc._io[IO::LY] == 0) {
      _wFlagY = false;
      _wY = 0;
    }

    // Check window Y coordinates
    if (_gbc._io[IO::LY] == _gbc._io[IO::WY])
      _wFlagY = true;

    // Reset OBJ list
    _sprites.clear();
    _sprites.reserve(GBC::PixelProcessingUnit::SpriteCount);

    // Push Y matching OBJ in order
    for (const auto& sprite : _oam.sprites) {
      if (sprite.y <= _gbc._io[IO::LY] + 16 &&
        sprite.y + ((_gbc._io[IO::LCDC] & LcdControl::LcdControlObjSize) ? 16 : 8) > _gbc._io[IO::LY] + 16)
        _sprites.push_back(sprite);
    }

    // Limit to 10 OBJ at selection
    if (_sprites.size() > GBC::PixelProcessingUnit::SpriteLimit)
      _sprites.erase(std::next(_sprites.begin(), GBC::PixelProcessingUnit::SpriteLimit), _sprites.end());

    // In non-CGB mode, OBJ draw priority depends on its X position
    if ((_gbc._io[IO::OPRI] & 0b00000001) || (_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100))
      std::sort(_sprites.begin(), _sprites.end(), [](const auto& a, const auto& b) { return a.x < b.x; });
  }

  // Next cycle
  _cycles += 1;

  // Next mode
  if (_cycles % GBC::PixelProcessingUnit::ScanlineDuration == GBC::PixelProcessingUnit::Mode2Duration)
    setMode(Mode::Mode3);
}

void  GBC::PixelProcessingUnit::simulateMode3()
{
  // Reset Mode3 datas at start of line
  if (_cycles % GBC::PixelProcessingUnit::ScanlineDuration == GBC::PixelProcessingUnit::Mode2Duration) {
    _lx = 0;
    _bwFifo.clear();
    _bwOffset = _gbc._io[IO::SCX] % 8;
    _bwTile = 0;
    _bwWait = 0;
    _wFlagX = false;
    _sFifo.clear();
    _sOffset = 0;
    _sWait = 0;
  }

  // Start to draw window
  if (((_lx == 0) ? (_lx + 7 >= _gbc._io[IO::WX]) : (_lx + 7 == _gbc._io[IO::WX])) &&
    _wFlagX == false && _wFlagY == true &&
    (_gbc._io[IO::LCDC] & LcdControl::LcdControlWindowEnable)) {
    _wFlagX = true;
    _bwFifo.clear();
    _bwOffset = _lx + 7 - _gbc._io[IO::WX];
    _bwTile = 0;
    _bwWait = 0;
  }

  // Fetch background/window pixels
  if (_bwFifo.size() <= 16 && _bwWait == 0)
    simulateMode3BackgroundWindow();

  // Fetch sprites pixels
  if (_sWait == 0)
    simulateMode3Sprites();

  // Pop a pixel from the FIFOs
  if (_bwFifo.size() > 16 && _sWait == 0)
    simulateMode3Draw();

  // Decrement fetchers timers
  if (_bwWait > 0)
    _bwWait -= 1;
  if (_sWait > 0)
    _sWait -= 1;

  // End of line drawing
  if (_lx == GBC::PixelProcessingUnit::ScreenWidth)
  {
    // Go to HBlank
    setMode(Mode::Mode0);

    // Increase window line counter
    if (_wFlagX == true)
      _wY += 1;
  }

  // Next cycle
  _cycles += 1;
}

void  GBC::PixelProcessingUnit::simulateMode3Draw()
{
  // Ignore offset pixels
  if (_bwOffset > 0 || _sOffset > 0)
  {
    // Discard Background/Window pixel
    if (_bwOffset > 0) {
      _bwOffset -= 1;
      _bwFifo.pop();
    }

    // Discard Sprites pixel
    if (_sOffset > 0) {
      _sOffset -= 1;
      _sFifo.pop();
    }
  }

  // Draw pixel
  else
  {
    auto bwPixel = _bwFifo.front();
    auto sPixel = _sFifo.empty() == false ?
      _sFifo.front() :
      GBC::PixelProcessingUnit::PixelFifo::Pixel{ .color = 0, .palette = 0, .attributes = 0, .priority = 0 };

    // Remove pixel from Background/Window pixel FIFO
    _bwFifo.pop();

    // Remove pixel from Sprites pixel FIFO
    if (_sFifo.empty() == false)
      _sFifo.pop();

    bool      bwEnable = !(_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100) || (_gbc._io[IO::LCDC] & LcdControl::LcdControlWindowBackgroundEnable);
    bool      sEnable = sPixel.color != 0;

    // Background and Window Master Priority, CGB mode only
    if (bwEnable == true &&
      sEnable == true &&
      !(_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100) &&
      !(_gbc._io[IO::LCDC] & LcdControl::LcdControlPriority))
      bwEnable = false;

    // Sprite priority over background and window
    else if (bwEnable == true &&
      sEnable == true &&
      ((bwPixel.attributes & BackgroundAttributes::BackgroundAttributesPriority) || (sPixel.attributes & SpriteAttributes::SpriteAttributesOverObj)) &&
      bwPixel.color != 0)
      sEnable = false;

    GBC::PixelProcessingUnit::Color color;
    
    // Select Sprite color
    if (sEnable == true)
    {
      // DMG color palette conversion
      if (_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100) {
        if (sPixel.attributes & SpriteAttributes::SpriteAttributesPaletteNonCgb) {
          sPixel.color = (_gbc._io[IO::OBP1] >> (sPixel.color * 2)) & 0b00000011;
          sPixel.palette = 1;
        }
        else {
          sPixel.color = (_gbc._io[IO::OBP0] >> (sPixel.color * 2)) & 0b00000011;
          sPixel.palette = 0;
        }
      }

      // Get color from Sprite color palettes
      color = _obc.colors[sPixel.palette * 4 + sPixel.color];
    }

    // Select Background/Window color
    else if (bwEnable == true)
    {
      // DMG color palette conversion
      if (_gbc._io[GBC::GameBoyColor::IO::KEY0] & 0b00001100)
        bwPixel.color = (_gbc._io[IO::BGP] >> (bwPixel.color * 2)) & 0b00000011;

      // Get color from Background/Window color palettes
      color = _bgc.colors[bwPixel.palette * 4 + bwPixel.color];
    }

    // Default to white
    else
      color.raw = 0b0111111111111111;

    // Set pixel color
    _image.setPixel(_lx, _gbc._io[IO::LY], sf::Color(color.red(), color.green(), color.blue()));

    // Next pixel
    _lx += 1;
  }
}

void  GBC::PixelProcessingUnit::simulateMode3BackgroundWindow()
{
  // Fetch a new pixel
  std::uint8_t  tile_y = 0;
  std::uint16_t tilemap = 0x0000;
  std::uint16_t tilemap_id = 0;

  // Window pixels data
  if (_wFlagX == true) {
    tile_y = _wY;
    tilemap = (_gbc._io[IO::LCDC] & LcdControl::LcdControlWindowTilemap) ? 0x1C00 : 0x1800;
    tilemap_id = ((std::uint16_t)tile_y / 8) * 32 + ((std::uint16_t)_bwTile) % 32;
  }

  // Background pixels data
  else {
    tile_y = _gbc._io[IO::LY] + _gbc._io[IO::SCY];
    tilemap = (_gbc._io[IO::LCDC] & LcdControl::LcdControlBackgroundTilemap) ? 0x1C00 : 0x1800;
    tilemap_id = ((std::uint16_t)tile_y / 8) * 32 + (((std::uint16_t)_gbc._io[IO::SCX] / 8) + _bwTile) % 32;
  }
  
  std::uint8_t  tile_id = _ram[0][tilemap + tilemap_id];
  std::uint8_t  tile_attributes = _ram[1][tilemap + tilemap_id];
  std::uint16_t tile_index = ((std::uint16_t)tile_id * 16)
    + (((_gbc._io[IO::LCDC] & LcdControl::LcdControlData)) ?
      0x0000 :
      ((tile_id < 128) ? 0x1000 : 0x0000));

  // Flip Y tile coordinates
  tile_y = (tile_attributes & BackgroundAttributes::BackgroundAttributesYFlip) ? (~tile_y & 0b00000111) : (tile_y & 0b00000111);

  std::uint8_t  tile_low = _ram[(tile_attributes & BackgroundAttributes::BackgroundAttributesBank) ? 1 : 0][tile_index + (tile_y % 8) * 2 + 0];
  std::uint8_t  tile_high = _ram[(tile_attributes & BackgroundAttributes::BackgroundAttributesBank) ? 1 : 0][tile_index + (tile_y % 8) * 2 + 1];

  // Register pixels according to X flip attribute
  for (std::uint8_t x = 0; x < 8; x += 1) {
    std::uint8_t  index = (tile_attributes & BackgroundAttributes::BackgroundAttributesXFlip) ? (7 - x) : (x);

    // Push pixel in FIFO
    _bwFifo.push({
      .color = (std::uint8_t)(((tile_low & (0b10000000 >> index)) ? 0b01 : 0b00) + ((tile_high & (0b10000000 >> index)) ? 0b10 : 0b00)),
      .palette = (std::uint8_t)(tile_attributes & BackgroundAttributes::BackgroundAttributesPalette),
      .attributes = tile_attributes,
      .priority = 0
      });
  }

  // Increment tile X index
  _bwTile += 1;

  // Wait 6 tick before next fetch
  _bwWait = 6;
}

void  GBC::PixelProcessingUnit::simulateMode3Sprites()
{
  // Search in OAM
  if (_gbc._io[IO::LCDC] & LcdControl::LcdControlObjEnable) {
    for (auto iterator = 0; iterator < _sprites.size(); iterator++)
    {
      auto& sprite = _sprites[iterator];

      // Draw sprite
      if (sprite.x <= _lx + 8)
      {
        std::uint8_t  tile_y = _gbc._io[IO::LY] - sprite.y + 16;
        std::uint8_t  tile_height = (_gbc._io[IO::LCDC] & LcdControl::LcdControlObjSize) ? 16 : 8;

        // Flip sprite Y coordinates
        if (sprite.attributes & SpriteAttributes::SpriteAttributesYFlip)
          tile_y = (tile_height - 1) - tile_y % tile_height;

        std::uint8_t  sp_tile_id = sprite.tile & ((tile_height == 8) ? 0b11111111 : 0b11111110);
        std::uint16_t sp_tile_index = ((std::uint16_t)sp_tile_id * 16);

        std::uint8_t  tile_low = _ram[(sprite.attributes & SpriteAttributes::SpriteAttributesBank) ? 1 : 0][sp_tile_index + tile_y * 2 + 0];
        std::uint8_t  tile_high = _ram[(sprite.attributes & SpriteAttributes::SpriteAttributesBank) ? 1 : 0][sp_tile_index + tile_y * 2 + 1];

        // Fill Sprites pixel FIFO
        while (_sFifo.size() < 8)
          _sFifo.push({ .color = 0, .palette = 0, .attributes = 0, .priority = 255 });

        // Discard passed pixels
        for (; _sOffset < _lx - sprite.x + 8; _sOffset += 1)
          _sFifo.insert({ .color = 0, .palette = 0, .attributes = 0, .priority = 255 });

        // Register pixels according to X flip attribute
        for (std::uint8_t index = 0; index < 8; index += 1) {
          std::uint8_t      x = (sprite.attributes & SpriteAttributes::SpriteAttributesXFlip) ? (7 - index) : (index);

          PixelFifo::Pixel  pixel = {
            .color = (std::uint8_t)(((tile_low & (0b10000000 >> x)) ? 0b01 : 0b00) + ((tile_high & (0b10000000 >> x)) ? 0b10 : 0b00)),
            .palette = (std::uint8_t)(sprite.attributes & SpriteAttributes::SpriteAttributesPaletteCgb),
            .attributes = sprite.attributes,
            .priority = (std::uint8_t)iterator
          };

          // Merge pixel in FIFO
          if (pixel.color != 0 && (pixel.priority < _sFifo[index].priority || _sFifo[index].color == 0))
            _sFifo[index] = pixel;
        }

        // Wait 6 tick before next fetch
        _sWait = 6;

        // Force sprite off screen
        sprite.x = 255;
        break;
      }
    }
  }

  // Ignore missed sprites
  else
    for (auto iterator = 0; iterator < _sprites.size(); iterator++)
      if (_sprites[iterator].x <= _lx)
        _sprites[iterator].x = 255;
}

const sf::Texture&  GBC::PixelProcessingUnit::lcd() const
{
  // Return rendering target
  return _texture;
}

void  GBC::PixelProcessingUnit::save(std::ofstream& file) const
{
  // Save PPU variables
  _gbc.save(file, "PPU_RAM", _ram);
  _gbc.save(file, "PPU_BGC", _bgc);
  _gbc.save(file, "PPU_OBC", _obc);
  _gbc.save(file, "PPU_OAM", _oam);
  _gbc.save(file, "PPU_MODE", _mode);
  _gbc.save(file, "PPU_SPRITES", _sprites);
  _gbc.save(file, "PPU_CYCLES", _cycles);
  _gbc.save(file, "PPU_LX", _lx);
  _gbc.save(file, "PPU_BWFIFO", _bwFifo);
  _gbc.save(file, "PPU_BWOFFSET", _bwOffset);
  _gbc.save(file, "PPU_BWTILE", _bwTile);
  _gbc.save(file, "PPU_BWWAIT", _bwWait);
  _gbc.save(file, "PPU_WFLAGX", _wFlagX);
  _gbc.save(file, "PPU_WFLAGY", _wFlagY);
  _gbc.save(file, "PPU_WY", _wY);
  _gbc.save(file, "PPU_SFIFO", _sFifo);
  _gbc.save(file, "PPU_SOFFSET", _sOffset);
  _gbc.save(file, "PPU_SWAIT", _sWait);
}

void  GBC::PixelProcessingUnit::load(std::ifstream& file)
{
  // Load PPU variables
  _gbc.load(file, "PPU_RAM", _ram);
  _gbc.load(file, "PPU_BGC", _bgc);
  _gbc.load(file, "PPU_OBC", _obc);
  _gbc.load(file, "PPU_OAM", _oam);
  _gbc.load(file, "PPU_MODE", _mode);
  _gbc.load(file, "PPU_SPRITES", _sprites);
  _gbc.load(file, "PPU_CYCLES", _cycles);
  _gbc.load(file, "PPU_LX", _lx);
  _gbc.load(file, "PPU_BWFIFO", _bwFifo);
  _gbc.load(file, "PPU_BWOFFSET", _bwOffset);
  _gbc.load(file, "PPU_BWTILE", _bwTile);
  _gbc.load(file, "PPU_BWWAIT", _bwWait);
  _gbc.load(file, "PPU_WFLAGX", _wFlagX);
  _gbc.load(file, "PPU_WFLAGY", _wFlagY);
  _gbc.load(file, "PPU_WY", _wY);
  _gbc.load(file, "PPU_SFIFO", _sFifo);
  _gbc.load(file, "PPU_SOFFSET", _sOffset);
  _gbc.load(file, "PPU_SWAIT", _sWait);
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

GBC::PixelProcessingUnit::PixelFifo::PixelFifo() :
  _fifo(),
  _size(0),
  _index(0)
{}

GBC::PixelProcessingUnit::PixelFifo::Pixel  GBC::PixelProcessingUnit::PixelFifo::front() const
{
#ifdef _DEBUG
  if (empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Get next pixel in FIFO
  return _fifo[_index];
}

bool  GBC::PixelProcessingUnit::PixelFifo::empty() const
{
  // Check if FIFO is empty
  return _size == 0;
}

std::size_t GBC::PixelProcessingUnit::PixelFifo::size() const
{
  // Get the number of pixels in FIFO
  return _size;
}

void  GBC::PixelProcessingUnit::PixelFifo::clear()
{
  // Reset number of pixels in FIFO
  _size = 0;
}

void  GBC::PixelProcessingUnit::PixelFifo::push(Pixel pixel)
{
#ifdef _DEBUG
  if (size() == _fifo.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Push pixel at back of FIFO
  _fifo[(_index + _size) % _fifo.size()] = pixel;
  _size += 1;
}

void  GBC::PixelProcessingUnit::PixelFifo::insert(Pixel pixel)
{
#ifdef _DEBUG
  if (size() == _fifo.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Push pixel at front of FIFO
  _index = (_index + _fifo.size() - 1) % _fifo.size();
  _fifo[_index % _fifo.size()] = pixel;
  _size += 1;
}

void  GBC::PixelProcessingUnit::PixelFifo::pop()
{
#ifdef _DEBUG
  if (empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Push pixel in FIFO
  _index = (_index + 1) % _fifo.size();
  _size -= 1;
}

GBC::PixelProcessingUnit::PixelFifo::Pixel& GBC::PixelProcessingUnit::PixelFifo::operator[](std::size_t position)
{
#ifdef _DEBUG
  if (position >= size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Get element in FIFO
  return _fifo[(_index + position) % _fifo.size()];
}

const GBC::PixelProcessingUnit::PixelFifo::Pixel& GBC::PixelProcessingUnit::PixelFifo::operator[](std::size_t position) const
{
#ifdef _DEBUG
  if (position >= size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Get element in FIFO
  return _fifo[(_index + position) % _fifo.size()];
}

std::uint8_t  GBC::PixelProcessingUnit::readRam(std::uint16_t address)
{
#ifdef _DEBUG
  // Out of bound address, should not happen
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Can't read during rendering
  if (_mode == Mode::Mode3)
    return 0xFF;

  // Read data from Video RAM
  return _ram[_gbc._io[IO::VBK] & 0b00000001][address];
}

std::uint8_t  GBC::PixelProcessingUnit::readOam(std::uint16_t address)
{
#ifdef _DEBUG
  // Out of bound address, should not happen
  if (address >= sizeof(_oam))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

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
  case IO::BGP:   // Background and Window Palette Data, R/W, non CGB mode only
  case IO::OBP0:  // OBJ 0 Palette Data, R/W, non CGB mode only
  case IO::OBP1:  // OBJ 1 Palette Data, R/W, non CGB mode only  
  case IO::WY:    // Window Y Position, R/W
  case IO::WX:    // Window X Position, R/W
  case IO::BCPI:  // Background Color Palette Index, R/W, CGB mode only
  case IO::OCPI:  // OBJ Color Palette Index, R/W, CGB mode only
  case IO::OPRI:  // OBJ Priority Mode, R/W, CGB mode only, bit 0: mode (0 :OAM, 1: Coordinate)
    // Basic read, just return stored value
    return _gbc._io[address];

  default:
#ifdef _DEBUG
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif
    return 0xFF;
  }
}

void  GBC::PixelProcessingUnit::writeRam(std::uint16_t address, std::uint8_t value)
{
#ifdef _DEBUG
  // Out of bound address, should not happen
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Can't write during rendering
  if (_mode == Mode::Mode3)
    return;

  // Write data to Video RAM
  _ram[_gbc._io[IO::VBK] & 0b00000001][address] = value;
}

void  GBC::PixelProcessingUnit::writeOam(std::uint16_t address, std::uint8_t value)
{
#ifdef _DEBUG
  // Out of bound address, should not happen
  if (address >= sizeof(_oam))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Can't write during sprite rendering
  if (_mode == Mode::Mode2 ||
    _mode == Mode::Mode3)
    return;

  // Write data to OAM
  _oam.raw[address] = value;
}

void  GBC::PixelProcessingUnit::writeDma(std::uint16_t address, std::uint8_t value)
{
#ifdef _DEBUG
  // Out of bound address, should not happen
  if (address >= sizeof(_oam))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Write data to OAM
  _oam.raw[address] = value;

  // Sprite of current scanline are not displayed during DMA transfer
  _sprites.clear();
}

void  GBC::PixelProcessingUnit::writeIo(std::uint16_t address, std::uint8_t value)
{
  switch (address)
  {
  case IO::LCDC:  // LCD Control, R/W (see enum)
    if ((_gbc._io[IO::LCDC] & LcdControl::LcdControlEnable) && !(value & LcdControl::LcdControlEnable)) {
      _gbc._io[IO::LY] = 0;
      _cycles = 0;
      _mode = Mode::Mode0;

      // Update STAT register
      if (_gbc._io[IO::LY] == _gbc._io[IO::LYC])
        _gbc._io[IO::STAT] |= LcdStatus::LcdStatusEqual;
      else
        _gbc._io[IO::STAT] &= ~LcdStatus::LcdStatusEqual;
      _gbc._io[IO::STAT] = (_gbc._io[IO::STAT] & ~LcdStatus::LcdStatusMode) | (Mode::Mode0);

      // White image
      std::memset((std::uint8_t*)_image.getPixelsPtr(), 0xFF, _image.getSize().x * _image.getSize().y * 4);
      _texture.update(_image);
    }

    // PPU enabled, start
    else if (!(_gbc._io[IO::LCDC] & LcdControl::LcdControlEnable) && (value & LcdControl::LcdControlEnable)) {
      _mode = Mode::Mode2;
      _gbc._io[IO::STAT] = (_gbc._io[IO::STAT] & ~LcdStatus::LcdStatusMode) | (Mode::Mode2);
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

  case IO::SCY:   // Scroll Y, R/W
  case IO::SCX:   // Scroll X, R/W
  case IO::BGP:   // Background and Window Palette Data, R/W, non CGB mode only
  case IO::OBP0:  // OBJ 0 Palette Data, R/W, non CGB mode only
  case IO::OBP1:  // OBJ 1 Palette Data, R/W, non CGB mode only
  case IO::WY:    // Window Y Position, R/W
  case IO::WX:    // Window X Position, R/W
  case IO::VBK:   // Video RAM Bank, R/W, CGB mode only
  case IO::BCPI:  // Background Color Palette Index, R/W, CGB mode only
  case IO::OCPI:  // OBJ Color Palette Index, R/W, CGB mode only
    _gbc._io[address] = value;
    break;

  default:
#ifdef _DEBUG
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif
    break;
  }
}
