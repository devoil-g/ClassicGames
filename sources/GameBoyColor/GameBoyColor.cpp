#include <cstring>
#include <fstream>
#include <filesystem>

#include "GameBoyColor/GameBoyColor.hpp"
#include "GameBoyColor/MemoryBankController1.hpp"
#include "GameBoyColor/MemoryBankController2.hpp"
#include "GameBoyColor/MemoryBankController3.hpp"
#include "GameBoyColor/MemoryBankController5.hpp"
#include "Math/Math.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

#include <iostream>
#include <thread>
#include <chrono>

GBC::GameBoyColor::GameBoyColor(const std::string& filename) :
  _path(filename),
  _boot(),
  _cpu(*this),
  _mbc(),
  _vRam(),
  _wRam(),
  _oam(),
  _io(),
  _hRam(),
  _ie(0),
  _bgcRam(),
  _obcRam(),
  _cpuCycle(0),
  _ppuCycle(0),
  _ppuObj(),
  _ppuLcd(),
  _header()
{
  // Set screen size and initial color
  _ppuLcd.create(160, 144, sf::Color::White);

  // Load ROM
  load(filename);

  // Initialize memory banks
  _vRam.fill(0);
  _wRam.fill(0);
  _oam.fill(0);
  _io.fill(0);
  _hRam.fill(0);
  _bgcRam.fill(0);
  _obcRam.fill(0);

  // Initialize Joypad
  _io[IO::JOYP] = 0b11111111;

  // Initialize audio
  _sound1.length = 0.f;
  _sound2.length = 0.f;
  _sound3.length = 0.f;
  _sound4.length = 0.f;
}

void  GBC::GameBoyColor::load(const std::string& filename)
{
  std::vector<uint8_t>  rom;

  // Load ROM to memory
  loadFile(filename, rom);

  // Parse ROM header
  loadHeader(rom);

  // Load bootstrap bios to memory
  loadFile(Game::Config::ExecutablePath + "/assets/gbc/cgb_boot.bin", _boot);
}

void  GBC::GameBoyColor::loadFile(const std::string& filename, std::vector<std::uint8_t>& destination)
{
  std::ifstream file(filename, std::ifstream::binary);

  // Check if file open properly
  if (file.good() == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Reset ROM memory
  destination.clear();
  destination.resize(std::filesystem::file_size(filename), 0);

  // Load ROM
  file.read((char*)destination.data(), destination.size());

  // Check for error
  if (file.gcount() != destination.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  GBC::GameBoyColor::loadHeader(const std::vector<uint8_t>& rom)
{
  // Too small game
  if (rom.size() < 0x0150)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Original Nintendo Logo Bitmap
  std::uint8_t  nintendo[] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
  };

  // Logo should be identical
  _header.logo = (std::memcmp(nintendo, rom.data() + 0x0104, sizeof(nintendo)) == 0);

  // Game title
  _header.title.clear();
  for (unsigned int index = 0x0134; index <= 0x0143 && rom[index] != '\0'; index++)
    _header.title += rom[index];

  // CGB format (we don't handle intermediary format between GB & CGB)
  if (rom[0x0143] & 0b10000000)
  {
    // 11 characters title
    _header.title = _header.title.substr(0, 11);

    // 4 characters manufacturer
    _header.manufacturer.clear();
    for (unsigned int index = 0x013F; index <= 0x0142 && rom[index] != '\0'; index++)
      _header.manufacturer += rom[index];

    // CGB flag
    switch (rom[0x0143] & 0b11000000) {
    case 0x80:
      _header.cgb = Header::CGBFlag::CGBSupport;
      break;
    case 0xC0:
      _header.cgb = Header::CGBFlag::CGBOnly;
      break;
    default:
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
  }

  // GB format
  else {
    _header.manufacturer = "";
    _header.cgb = Header::CGBFlag::CGBNone;
  }

  // Old licensee code
  if (rom[0x0144] == 0x01 && rom[0x0145] == 0x4B)
    _header.licensee = rom[0x014B];
  // New Lecensee code
  else {
    ((uint8_t*)&_header.licensee)[0] = rom[0x0144];
    ((uint8_t*)&_header.licensee)[1] = rom[0x0145];
  }

  // SGB flag
  switch (rom[0x0146]) {
  case 0x00:
    _header.sgb = Header::SGBFlag::SGBNone;
    break;
  case 0x03:
    _header.sgb = Header::SGBFlag::SGBSupport;
    break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // ROM size
  switch (rom[0x0148])
  {
  case 0x52:
    _header.rom_size = 72 * 16384;
    break;
  case 0x53:
    _header.rom_size = 80 * 16384;
    break;
  case 0x54:
    _header.rom_size = 96 * 16384;
    break;
  default:
    _header.rom_size = (0b00000010 << rom[0x0148]) * 16384;
    break;
  }

  // RAM size
  switch (rom[0x0149])
  {
  case 0x00:
    _header.ram_size = 0;
    break;
  case 0x01:
    _header.ram_size = 2 * 1024;
    break;
  case 0x02:
    _header.ram_size = 8 * 1024;
    break;
  case 0x03:
    _header.ram_size = 32 * 1024;
    break;
  case 0x04:
    _header.ram_size = 128 * 1024;
    break;
  case 0x05:
    _header.ram_size = 64 * 1024;
    break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // Cartridge Type
  switch (rom[0x0147]) {
  case 0x00:  // ROM only
    _mbc = std::make_unique<GBC::MemoryBankController>(rom);
    _header.mbc = Header::MBC::MBCNone;
    break;
  case 0x08:  // ROM+RAM, never used
    _mbc = std::make_unique<GBC::MemoryBankController>(rom, _header.ram_size);
    _header.mbc = Header::MBC::MBCNone;
    break;
  case 0x09:  // ROM+RAM+BATTERY, never used
    _mbc = std::make_unique<GBC::MemoryBankController>(rom, _header.ram_size, _path + ".gbs");
    _header.mbc = Header::MBC::MBCNone;
    break;

  case 0x01:  // MBC1
    _mbc = std::make_unique<GBC::MemoryBankController1>(rom);
    _header.mbc = Header::MBC::MBC1;
    break;
  case 0x02:  // MBC1+RAM
    _mbc = std::make_unique<GBC::MemoryBankController1>(rom, _header.ram_size);
    _header.mbc = Header::MBC::MBC1;
    break;
  case 0x03:  // MBC1+RAM+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController1>(rom, _header.ram_size, _path + ".gbs");
    _header.mbc = Header::MBC::MBC1;
    break;

  case 0x05:  // MBC2
    _mbc = std::make_unique<GBC::MemoryBankController2>(rom);
    _header.mbc = Header::MBC::MBC2;
    break;
  case 0x06:  // MBC2+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController2>(rom, _path + ".gbs");
    _header.mbc = Header::MBC::MBC2;
    break;

  case 0x0F:  // MBC3+TIMER+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController3>(rom, 0, "", _path + ".rtc");
    _header.mbc = Header::MBC::MBC3;
    break;
  case 0x10:  // MBC3+TIMER+RAM+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController3>(rom, _header.ram_size, _path + ".gbs", _path + ".rtc");
    _header.mbc = Header::MBC::MBC3;
    break;
  case 0x11:  // MBC3
    _mbc = std::make_unique<GBC::MemoryBankController3>(rom);
    _header.mbc = Header::MBC::MBC3;
    break;
  case 0x12:  // MBC3+RAM
    _mbc = std::make_unique<GBC::MemoryBankController3>(rom, _header.ram_size);
    _header.mbc = Header::MBC::MBC3;
    break;
  case 0x13:  // MBC3+RAM+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController3>(rom, _header.ram_size, _path + ".gbs");
    _header.mbc = Header::MBC::MBC3;
    break;

  case 0x19:  // MBC5
  case 0x1C:  // MBC5+RUMBLE
    _mbc = std::make_unique<GBC::MemoryBankController5>(rom);
    _header.mbc = Header::MBC::MBC5;
    break;
  case 0x1A:  // MBC5+RAM
  case 0x1D:  // MBC5+RUMBLE+RAM
    _mbc = std::make_unique<GBC::MemoryBankController5>(rom, _header.ram_size);
    _header.mbc = Header::MBC::MBC5;
    break;
  case 0x1B:  // MBC5+RAM+BATTERY
  case 0x1E:  // MBC5+RUMBLE+RAM+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController5>(rom, _header.ram_size, _path + ".gbs");
    _header.mbc = Header::MBC::MBC5;
    break;

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // Region
  switch (rom[0x014A]) {
  case 0x00:
    _header.region = Header::Region::RegionJP;
    break;
  case 0x01:
    _header.region = Header::Region::RegionUSEU;
    break;
  default:
    _header.region = Header::Region::RegionUnknow;
    break;
  }

  // Game ROM version
  _header.version = rom[0x014C];

  std::uint8_t  header_checksum = 0;

  // Compute header checksum
  for (unsigned int index = 0x0134; index <= 0x014C; index++)
    header_checksum = header_checksum - rom[index] - 1;

  // Check header checksum
  _header.header_checksum = (header_checksum == rom[0x014D]);

  std::uint16_t global_checksum = 0;

  // Compute global checksum
  for (unsigned int index = 0; index < rom.size(); index++)
    if (index < 0x014E || index > 0x014F)
      global_checksum += rom[index];

  // Check global checksum
  _header.global_checksum = (global_checksum == rom[0x014E] * 256 + rom[0x014F]);
}

void  GBC::GameBoyColor::simulate()
{
  std::uint64_t frame = _ppuCycle / (456 * 154);
  std::uint64_t cycle = _cpuCycle;
  std::uint64_t second = _cpuCycle / (4 * 1024 * 1024);

  // Simulate Joypad
  simulateKeys();

  // Execution loop
  while (frame == _ppuCycle / (456 * 154))
  {
    // Simulate a tick of the CPU
    _cpu.simulate();
    _cpuCycle += 4;

    // Update graphics and timer for the number of cycle executed
    for (; cycle < _cpuCycle; cycle += 4) {
      simulateTimer();
      if (cycle % ((_io[IO::KEY1] & 0b10000000) ? 8 : 4) == 0)
        simulateGraphics();
    }
  }

  // Update audio for one frame
  simulateAudio();
}

void  GBC::GameBoyColor::simulateKeys()
{
  std::array<bool, Key::KeyCount> keys = {
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) < -0.5f || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Down),
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) > +0.5f || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Up),
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovX) < -0.5f || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Left),
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovX) > +0.5f || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Right),
    Game::Window::Instance().joystick().buttonDown(0, 7) || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Numpad0),
    Game::Window::Instance().joystick().buttonDown(0, 6) || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Enter),
    Game::Window::Instance().joystick().buttonDown(0, 1) || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Numpad2),
    Game::Window::Instance().joystick().buttonDown(0, 0) || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Numpad6)
  };

  // Joypad interrupt when a selected key is pressed
  if ((!(_io[IO::JOYP] & 0b00010000) &&
    (_keys[Key::KeyDown] == false && keys[Key::KeyDown] == true ||
      _keys[Key::KeyUp] == false && keys[Key::KeyUp] == true ||
      _keys[Key::KeyLeft] == false && keys[Key::KeyLeft] == true ||
      _keys[Key::KeyRight] == false && keys[Key::KeyRight] == true))
    ||
    (!(_io[IO::JOYP] & 0b00100000) &&
      (_keys[Key::KeyStart] == false && keys[Key::KeyStart] == true ||
        _keys[Key::KeySelect] == false && keys[Key::KeySelect] == true ||
        _keys[Key::KeyB] == false && keys[Key::KeyB] == true ||
        _keys[Key::KeyA] == false && keys[Key::KeyA] == true)))
    _io[IO::IF] |= Interrupt::InterruptJoypad;

  // Save new key map
  _keys = keys;
}

void  GBC::GameBoyColor::simulateGraphics()
{
  // Only render lines 0 to 143
  if (_io[IO::LY] < 144)
  {
    // Search OAM for OBJs that overlap current line
    if (_ppuCycle % 456 == 0) {
      simulateGraphicsMode2();

      // STAT mode 2 (OAM) interrupt
      if (_io[IO::STAT] & LcdStatus::LcdStatusMode2)
        _io[IO::IF] |= Interrupt::InterruptLcdStat;
    }

    // Generate the picture
    else if (_ppuCycle % 456 == 80)
      simulateGraphicsMode3();

    // Trigger HBlank
    else if (_ppuCycle % 456 == 252)
    {
      // Set LCD status mode 0
      _io[IO::STAT] = (_io[IO::STAT] & ~LcdStatus::LcdStatusMode) | 0b00000000;

      // STAT mode 0 (HBlank) interrupt
      if (_io[IO::STAT] & LcdStatus::LcdStatusMode0)
        _io[IO::IF] |= Interrupt::InterruptLcdStat;
    }
  }

  // Trigger VBlank
  else if (_io[IO::LY] == 144 && _ppuCycle % 456 == 0) {
    // Set LCD status mode 1
    _io[IO::STAT] = (_io[IO::STAT] & ~LcdStatus::LcdStatusMode) | 0b00000001;

    // VBlank interrupt
    _io[IO::IF] |= Interrupt::InterruptVBlank;

    // STAT mode 2 (VBlank) interrupt
    if (_io[IO::STAT] & LcdStatus::LcdStatusMode1)
      _io[IO::IF] |= Interrupt::InterruptLcdStat;
  }
  
  // Next line
  if (_ppuCycle % 456 == 452)
  {
    // Increment to next line
    _io[IO::LY] += 1;
    
    // Limit to 144 + 10 lines
    if (_io[IO::LY] >= 144 + 10)
      _io[IO::LY] = 0;

    // LY / LCY register comparison
    if (_io[IO::LY] == _io[IO::LYC]) {
      _io[IO::STAT] |= LcdStatus::LcdStatusEqual;

      // STAT compare (LY/LYC) interrupt
      if (_io[IO::STAT] & LcdStatus::LcdStatusCompare)
        _io[IO::IF] |= Interrupt::InterruptLcdStat;
    }
    else
      _io[IO::STAT] &= ~LcdStatus::LcdStatusEqual;
  }

  // Advance in PPU simulation
  _ppuCycle += 4;
}

void  GBC::GameBoyColor::simulateGraphicsMode2()
{
  // Set LCD status mode 2
  _io[IO::STAT] = (_io[IO::STAT] & ~LcdStatus::LcdStatusMode) | 0b00000010;

  // Clear OBJ list
  _ppuObj.clear();

  // Push Y matching OBJ in order
  for (std::uint8_t index = 0; index < _oam.size() / 4; index++)
    if (_oam[index * 4 + 0] <= _io[IO::LY] + 16 && _oam[index * 4 + 0] + ((_io[IO::LCDC] & LcdControl::LcdControlObjSize) ? 16 : 8) > _io[IO::LY] + 16)
      _ppuObj.push_back(index);

  // Limit to 10 OBJ
  if (_ppuObj.size() > 10)
    _ppuObj.erase(std::next(_ppuObj.begin(), 10), _ppuObj.end());

  // In non-CGB mode, OBJ draw priority depends on its X position
  if ((_io[IO::OPRI] & 0b00000001) || (_io[IO::KEY0] & 0b00001100))
    _ppuObj.sort([this](const auto& a, const auto& b) { return _oam[a * 4 + 1] < _oam[b * 4 + 1]; });
}

void  GBC::GameBoyColor::simulateGraphicsMode3()
{
  // Set LCD status mode 3
  _io[IO::STAT] = (_io[IO::STAT] & ~LcdStatus::LcdStatusMode) | 0b00000011;

  // White screen if LCD disabled
  if (!(_io[IO::LCDC] & LcdControl::LcdControlEnable)) {
    std::memset((std::uint8_t*)_ppuLcd.getPixelsPtr(), 0xFF, _ppuLcd.getSize().x * _ppuLcd.getSize().y * 4);
    return;
  }

  std::uint8_t  sc_y = _io[IO::LY];
  std::uint8_t  bg_y = sc_y + _io[IO::SCY];
  std::uint8_t  wn_y = sc_y - _io[IO::WY];

  for (std::uint8_t sc_x = 0; sc_x < _ppuLcd.getSize().x; sc_x++)
  {
    std::uint8_t  bg_color_id = 255, bg_attributes = 0, bg_color_r, bg_color_g, bg_color_b;
    std::uint8_t  sp_color_id = 255, sp_attributes = 0, sp_color_r, sp_color_g, sp_color_b;

    // Window
    if ((!(_io[IO::KEY0] & 0b00001100) || (_io[IO::LCDC] & LcdControl::LcdControlWindowBackgroundEnable)) &&
      (_io[IO::LCDC] & LcdControl::LcdControlWindowEnable) &&
      (sc_x + 7 >= _io[IO::WX]) &&
      (sc_y >= _io[IO::WY]))
    {
      std::uint8_t  wn_x = sc_x - _io[IO::WX] + 7;
      std::uint16_t wn_tilemap = (_io[IO::LCDC] & LcdControl::LcdControlWindowTilemap) ? 0x1C00 : 0x1800;
      std::uint16_t wn_tilemap_id = (((std::uint16_t)wn_y / 8) * 32) + ((std::uint16_t)wn_x / 8);
      bg_attributes = _vRam[wn_tilemap + wn_tilemap_id + 0x2000];
      std::uint8_t  wn_tile_id = _vRam[wn_tilemap + wn_tilemap_id];
      std::uint16_t wn_tile_index = ((std::uint16_t)wn_tile_id * 16)
        + ((_io[IO::LCDC] & LcdControl::LcdControlData) ?
          0x0000 :
          ((wn_tile_id < 128) ? 0x1000 : 0x0000))
        + ((bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ?
          0x2000 :
          0x0000);

      // Flip tile coordinates
      std::uint8_t  wn_tile_x = (bg_attributes & BackgroundAttributes::BackgroundAttributesXFlip) ? ((8 - 1) - wn_x % 8) : (wn_x % 8);
      std::uint8_t  wn_tile_y = (bg_attributes & BackgroundAttributes::BackgroundAttributesYFlip) ? ((8 - 1) - wn_y % 8) : (wn_y % 8);

      bg_color_id =
        ((_vRam[wn_tile_index + (wn_tile_y % 8) * 2 + 0] & (0b10000000 >> (wn_tile_x % 8))) ? 1 : 0) +
        ((_vRam[wn_tile_index + (wn_tile_y % 8) * 2 + 1] & (0b10000000 >> (wn_tile_x % 8))) ? 2 : 0);

      // DMG color palette
      if (_io[IO::KEY0] & 0b00001100)
        bg_color_id = (_io[IO::BGP] >> (bg_color_id * 2)) & 0b00000011;

      // Get color from palette
      std::uint16_t wn_color =
        ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 0] << 0) +
        ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 1] << 8);

      bg_color_r = ((wn_color >> 0) & 0b00011111) * 8;
      bg_color_g = ((wn_color >> 5) & 0b00011111) * 8;
      bg_color_b = ((wn_color >> 10) & 0b00011111) * 8;
    }

    // Background
    else if (!(_io[IO::KEY0] & 0b00001100) || (_io[IO::LCDC] & LcdControl::LcdControlWindowBackgroundEnable))
    {
      std::uint8_t  bg_x = sc_x + _io[IO::SCX];
      std::uint16_t bg_tilemap = (_io[IO::LCDC] & LcdControl::LcdControlBackgroundTilemap) ? 0x1C00 : 0x1800;
      std::uint16_t bg_tilemap_id = (((std::uint16_t)bg_y / 8) * 32) + ((std::uint16_t)bg_x / 8);
      bg_attributes = _vRam[bg_tilemap + bg_tilemap_id + 0x2000];
      std::uint8_t  bg_tile_id = _vRam[bg_tilemap + bg_tilemap_id];
      std::uint16_t bg_tile_index = ((std::uint16_t)bg_tile_id * 16)
        + ((_io[IO::LCDC] & LcdControl::LcdControlData) ?
          0x0000 :
          ((bg_tile_id < 128) ? 0x1000 : 0x0000))
        + ((bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ?
          0x2000 :
          0x0000);

      // Flip tile coordinates
      std::uint8_t  bg_tile_x = (bg_attributes & BackgroundAttributes::BackgroundAttributesXFlip) ? ((8 - 1) - bg_x % 8) : (bg_x % 8);
      std::uint8_t  bg_tile_y = (bg_attributes & BackgroundAttributes::BackgroundAttributesYFlip) ? ((8 - 1) - bg_y % 8) : (bg_y % 8);
      
      bg_color_id =
        ((_vRam[bg_tile_index + (bg_tile_y % 8) * 2 + 0] & (0b10000000 >> (bg_tile_x % 8))) ? 1 : 0) +
        ((_vRam[bg_tile_index + (bg_tile_y % 8) * 2 + 1] & (0b10000000 >> (bg_tile_x % 8))) ? 2 : 0);

      // DMG color palette
      if (_io[IO::KEY0] & 0b00001100)
        bg_color_id = (_io[IO::BGP] >> (bg_color_id * 2)) & 0b00000011;

      // Get color from palette
      std::uint16_t bg_color =
        ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 0] << 0) +
        ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 1] << 8);

      bg_color_r = ((bg_color >> 0) & 0b00011111) * 8;
      bg_color_g = ((bg_color >> 5) & 0b00011111) * 8;
      bg_color_b = ((bg_color >> 10) & 0b00011111) * 8;
    }

    // Sprite
    if (_io[IO::LCDC] & LcdControl::LcdControlObjEnable)
    {
      // Find first matching sprite
      for (const auto& sp_index : _ppuObj) {
        std::uint8_t  sp_y = sc_y - _oam[sp_index * 4 + 0] + 16;
        std::uint8_t  sp_x = sc_x - _oam[sp_index * 4 + 1] + 8;

        // Sprite not on column
        if (sp_x >= 8)
          continue;

        std::uint8_t  sp_height = (_io[IO::LCDC] & LcdControl::LcdControlObjSize) ? 16 : 8;
        sp_attributes = _oam[sp_index * 4 + 3];

        // Flip sprite coordinates
        if (sp_attributes & SpriteAttributes::SpriteAttributesYFlip)
          sp_y = (sp_height - 1) - sp_y % sp_height;
        if (sp_attributes & SpriteAttributes::SpriteAttributesXFlip)
          sp_x = (8 - 1) - sp_x % 8;

        std::uint8_t  sp_tile_id = (_oam[sp_index * 4 + 2] & ((sp_height == 8) ? 0b11111111 : 0b11111110)) + ((sp_y < 8) ? 0 : 1);
        std::uint16_t sp_tile_index = ((std::uint16_t)sp_tile_id * 16)
          + (!(sp_attributes & SpriteAttributes::SpriteAttributesBank) ?
            0x0000 :
            0x2000);
        sp_color_id =
          ((_vRam[sp_tile_index + (sp_y % 8) * 2 + 0] & (0b10000000 >> (sp_x % 8))) ? 1 : 0) +
          ((_vRam[sp_tile_index + (sp_y % 8) * 2 + 1] & (0b10000000 >> (sp_x % 8))) ? 2 : 0);

        // ID 0 is transparency
        if (sp_color_id == 0) {
          sp_color_id = 255;
          continue;
        }

        // DMG color palette
        if (_io[IO::KEY0] & 0b00001100) {
          if (sp_attributes & SpriteAttributes::SpriteAttributesPaletteNonCgb) {
            sp_color_id = (_io[IO::OBP1] >> (sp_color_id * 2)) & 0b00000011;
            sp_attributes = (sp_attributes & 0b11110000) | 0b00000001;
          }
          else {
            sp_color_id = (_io[IO::OBP0] >> (sp_color_id * 2)) & 0b00000011;
            sp_attributes = (sp_attributes & 0b11110000) | 0b00000000;
          }
        }

        // Get color from palette
        std::uint16_t sp_color =
          ((std::uint16_t)_obcRam[((sp_attributes & SpriteAttributes::SpriteAttributesPaletteCgb) * 4 + sp_color_id) * 2 + 0] << 0) +
          ((std::uint16_t)_obcRam[((sp_attributes & SpriteAttributes::SpriteAttributesPaletteCgb) * 4 + sp_color_id) * 2 + 1] << 8);

        sp_color_r = ((sp_color >> 0) & 0b00011111) * 8;
        sp_color_g = ((sp_color >> 5) & 0b00011111) * 8;
        sp_color_b = ((sp_color >> 10) & 0b00011111) * 8;

        break;
      }
    }

    // Background and Window Master Priority, CGB mode only
    if (bg_color_id != 255 &&
      sp_color_id != 255 &&
      !(_io[IO::KEY0] & 0b00001100) &&
      !(_io[IO::LCDC] & LcdControl::LcdControlPriority))
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
      color = sf::Color(sp_color_r, sp_color_g, sp_color_b);
    else if (bg_color_id != 255)
      color = sf::Color(bg_color_r, bg_color_g, bg_color_b);
    else
      color = sf::Color(255, 255, 255);

    // Draw pixel
    _ppuLcd.setPixel(sc_x, sc_y, color);
  }
}

void  GBC::GameBoyColor::simulateAudio()
{
  // Time between two samples
  const float delta = (float)(456 * 154) / (float)(4 * 1024 * 1024) / (float)GBC::GameBoyColor::SoundFrameSize;

  std::array<float, GBC::GameBoyColor::SoundFrameSize>  channel1 = { 0 };
  std::array<float, GBC::GameBoyColor::SoundFrameSize>  channel2 = { 0 };
  std::array<float, GBC::GameBoyColor::SoundFrameSize>  channel3 = { 0 };
  std::array<float, GBC::GameBoyColor::SoundFrameSize>  channel4 = { 0 };

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
    if (!(_io[IO::NR30] & 0b10000000))
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

  // TODO: DAC 1
  // TODO: DAC 2
  // TODO: DAC 3
  // TODO: DAC 4

  // Mixer and amplifier
  float leftVolume = (((_io[IO::NR50] & 0b01110000) >> 4) + 1) / 8.f;
  float rightVolume = (((_io[IO::NR50] & 0b00000111) >> 0) + 1) / 8.f;

  for (std::size_t index = 0; index < _sound.size() / 2; index++) {
    float leftSample = 0.f;
    float rightSample = 0.f;

    // Accumulate right channel
    if (_io[IO::NR51] & 0b00000001)
      rightSample += channel1[index];
    if (_io[IO::NR51] & 0b00000010)
      rightSample += channel2[index];
    if (_io[IO::NR51] & 0b00000100)
      rightSample += channel3[index];
    if (_io[IO::NR51] & 0b00001000)
      rightSample += channel4[index];

    // Accumulate left channel
    if (_io[IO::NR51] & 0b00010000)
      leftSample += channel1[index];
    if (_io[IO::NR51] & 0b00100000)
      leftSample += channel2[index];
    if (_io[IO::NR51] & 0b01000000)
      leftSample += channel3[index];
    if (_io[IO::NR51] & 0b10000000)
      leftSample += channel4[index];

    // Normalize samples
    leftSample /= 4.f;
    rightSample /= 4.f;

    // Add to sound buffer
    _sound[index * 2 + 0] = (std::uint16_t)(leftSample * leftVolume * 32767.f);
    _sound[index * 2 + 1] = (std::uint16_t)(rightSample * rightVolume * 32767.f);
  }
}

void  GBC::GameBoyColor::simulateTimer()
{
  std::uint16_t div = ((std::uint16_t)_io[IO::DIVHi] << 8) + (std::uint16_t)_io[IO::DIVLo] + 4;

  // Update DIV timer
  _io[IO::DIVHi] = (div >> 8) & 0b11111111;
  _io[IO::DIVLo] = div & 0b11111111;

  // Update Timer only when enabled
  if (_io[IO::TAC] & 0b00000100) {
    const std::array<int, 4>  modulo = { 1024, 16, 64, 256 };

    // Tick the timer
    if (div % modulo[_io[IO::TAC] & 0b11] == 0) {
      _io[IO::TIMA] += 1;

      // Interrupt when overflow
      if (_io[IO::TIMA] == 0) {
        // Set interrupt flags
        _io[IO::IF] |= Interrupt::InterruptTimer;

        // Reset timer
        _io[IO::TIMA] = _io[IO::TMA];
      }
    }
  }
}

const sf::Image& GBC::GameBoyColor::screen() const
{

  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Num1)) {
    static sf::Image  back;

    back.create(256, 256);

    std::uint16_t bg_tilemap = 0x1800;

    for (unsigned int grid_y = 0; grid_y < 32; grid_y++)
      for (unsigned int grid_x = 0; grid_x < 32; grid_x++) {
        std::uint16_t bg_tilemap_id = grid_y * 32 + grid_x;

        std::uint8_t  bg_attributes = _vRam[bg_tilemap + bg_tilemap_id + 0x2000];
        std::uint8_t  bg_tile_id = _vRam[bg_tilemap + bg_tilemap_id];
        std::uint16_t bg_control = ((Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Num3)) ?
          0x0000 :
          ((bg_tile_id < 128) ? 0x1000 : 0x0000));
        std::uint16_t bg_tile_index = ((std::uint16_t)bg_tile_id * 16)
          + bg_control
          + ((bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ?
            0x2000 :
            0x0000);

        for (unsigned int tile_y = 0; tile_y < 8; tile_y++)
          for (unsigned int tile_x = 0; tile_x < 8; tile_x++) {
            unsigned int bg_x = tile_x;
            unsigned int bg_y = tile_y;

            if (bg_attributes & BackgroundAttributes::BackgroundAttributesYFlip)
              bg_y = (8 - 1) - bg_y % 8;
            if (bg_attributes & BackgroundAttributes::BackgroundAttributesXFlip)
              bg_x = (8 - 1) - bg_x % 8;

            std::uint8_t  bg_color_id =
              ((_vRam[bg_tile_index + (bg_y % 8) * 2 + 0] & (0b10000000 >> (bg_x % 8))) ? 1 : 0) +
              ((_vRam[bg_tile_index + (bg_y % 8) * 2 + 1] & (0b10000000 >> (bg_x % 8))) ? 2 : 0);

            if (_io[IO::KEY0] & 0b00001100)
              bg_color_id = (_io[IO::BGP] >> (bg_color_id * 2)) & 0b00000011;

            std::uint16_t bg_color =
              ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 0] << 0) +
              ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 1] << 8);

            back.setPixel(grid_x * 8 + tile_x, grid_y * 8 + tile_y,
              sf::Color(
                ((bg_color >> 0) & 0b00011111) * 8,
                ((bg_color >> 5) & 0b00011111) * 8,
                ((bg_color >> 10) & 0b00011111) * 8
              ));
          }

      }

    return back;
  }

  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Num2)) {
    static sf::Image  back;

    back.create(256, 256);

    std::uint16_t bg_tilemap = 0x1C00;

    for (unsigned int grid_y = 0; grid_y < 32; grid_y++)
      for (unsigned int grid_x = 0; grid_x < 32; grid_x++) {
        std::uint16_t bg_tilemap_id = grid_y * 32 + grid_x;
        std::uint8_t  bg_attributes = _vRam[bg_tilemap + bg_tilemap_id + 0x2000];
        std::uint8_t  bg_tile_id = _vRam[bg_tilemap + bg_tilemap_id];
        std::uint16_t bg_control = ((Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Num3)) ?
          0x0000 :
          ((bg_tile_id < 128) ? 0x1000 : 0x0000));
        std::uint16_t bg_tile_index = ((std::uint16_t)bg_tile_id * 16)
          + bg_control
          + ((bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ?
            0x2000 :
            0x0000);

        for (unsigned int tile_y = 0; tile_y < 8; tile_y++)
          for (unsigned int tile_x = 0; tile_x < 8; tile_x++) {
            unsigned int bg_x = tile_x;
            unsigned int bg_y = tile_y;

            if (bg_attributes & BackgroundAttributes::BackgroundAttributesYFlip)
              bg_y = (8 - 1) - bg_y % 8;
            if (bg_attributes & BackgroundAttributes::BackgroundAttributesXFlip)
              bg_x = (8 - 1) - bg_x % 8;

            std::uint8_t  bg_color_id =
              ((_vRam[bg_tile_index + (bg_y % 8) * 2 + 0] & (0b10000000 >> (bg_x % 8))) ? 1 : 0) +
              ((_vRam[bg_tile_index + (bg_y % 8) * 2 + 1] & (0b10000000 >> (bg_x % 8))) ? 2 : 0);

            if (_io[IO::KEY0] & 0b00001100)
              bg_color_id = (_io[IO::BGP] >> (bg_color_id * 2)) & 0b00000011;

            std::uint16_t bg_color =
              ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 0] << 0) +
              ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 1] << 8);

            back.setPixel(grid_x * 8 + tile_x, grid_y * 8 + tile_y,
              sf::Color(
                ((bg_color >> 0) & 0b00011111) * 8,
                ((bg_color >> 5) & 0b00011111) * 8,
                ((bg_color >> 10) & 0b00011111) * 8
              ));
          }

      }

    return back;
  }

  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Num4)) {
    static sf::Image  back;

    back.create(8, 9);

    for (unsigned int index = 0; index < 32; index++) {
      std::uint16_t bg =
        ((std::uint16_t)_bgcRam[index * 2 + 0] << 0) +
        ((std::uint16_t)_bgcRam[index * 2 + 1] << 8);

      back.setPixel(index % 8, index / 8 + 0,
        sf::Color(
          ((bg >> 0) & 0b00011111) * 8,
          ((bg >> 5) & 0b00011111) * 8,
          ((bg >> 10) & 0b00011111) * 8
        ));

      std::uint16_t ob =
        ((std::uint16_t)_obcRam[index * 2 + 0] << 0) +
        ((std::uint16_t)_obcRam[index * 2 + 1] << 8);

      back.setPixel(index % 8, index / 8 + 5,
        sf::Color(
          ((ob >> 0) & 0b00011111) * 8,
          ((ob >> 5) & 0b00011111) * 8,
          ((ob >> 10) & 0b00011111) * 8
        ));
    }
    return back;
  }

  return _ppuLcd;
}

const std::array<std::int16_t, GBC::GameBoyColor::SoundBufferSize>& GBC::GameBoyColor::sound() const
{
  return _sound;
}

const GBC::GameBoyColor::Header& GBC::GameBoyColor::header() const
{
  return _header;
}

std::uint8_t  GBC::GameBoyColor::read(std::uint16_t addr)
{
  // 16 KiB ROM bank 00 from cartridge, usually a fixed bank
  // 16 KiB ROM Bank 01~NN Ffom cartridge, switchable bank via mapper (if any)
  // OR
  // Bootstrap sequence
  if (addr < 0x8000)
    return readRom(addr - 0x0000);

  // 8 KiB Video RAM (VRAM)
  // In CGB mode, switchable bank 0/1
  else if (addr < 0xA000)
    return readVRam(addr - 0x8000);

  // 8 KiB External RAM
  // From cartridge, switchable bank if any
  else if (addr < 0xC000)
    return _mbc->readRam(addr - 0xA000);

  // 8 KiB Work RAM (WRAM)
  // In CGB mode, second half is switchable bank 1~7 
  else if (addr < 0xE000)
    return readWRam(addr - 0xC000);

  // Mirror of C000~DDFF (ECHO RAM)
  // Nintendo says use of this area is prohibited
  else if (addr < 0xFE00)
    return readWRam(addr - 0xE000);

  // Sprite attribute table (OAM)	
  else if (addr < 0xFEA0)
    return readOam(addr - 0xFE00);

  // Not Usable
  // Nintendo says use of this area is prohibited
  else if (addr < 0xFF00)
    return 0x00;

  // I/O Registers
  else if (addr < 0xFF80)
    return readIo(addr - 0xFF00);

  // High RAM (HRAM)
  else if (addr < 0xFFFF)
    return readHRam(addr - 0xFF80);

  // Interrupt Enable register (IE)
  else
    return _ie;

  // Invalid address
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

std::uint8_t  GBC::GameBoyColor::readRom(std::uint16_t addr)
{
  
  // Out of bound address, should not happen
  if (addr >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Read from boot if not disabled
  if (_io[IO::BANK] == 0 && addr < _boot.size() && (addr <= 0x00FF || addr >= 0x0150))
    return _boot[addr];

  // Read from MBC ROM
  return _mbc->readRom(addr);
}

std::uint8_t  GBC::GameBoyColor::readVRam(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // VRAM locked when drawing
  // if ((_io[IO::STAT] & LcdStatus::LcdStatusMode) >= 3)
  //   return 0xFF;

  // Game Boy Color has 2 VRAM banks
  return _vRam[(std::uint16_t)(_io[IO::VBK] & 0b00000001) * 0x2000 + addr];
}

std::uint8_t  GBC::GameBoyColor::readWRam(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // First half is alway bank 0
  if (addr < 0x1000)
    return _wRam[addr];

  // Second half is a WRAM bank
  else
    return _wRam[(std::clamp(_io[IO::SVBK] & 0b00000111, 1, 7) * 0x1000) + (addr - 0x1000)];
}

std::uint8_t  GBC::GameBoyColor::readOam(std::uint16_t addr)
{
  // TODO: handle OAM lock at rendering

  // Out of bound address, should not happen
  if (addr >= 0x00A0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // OAM locked when drawing
  // if ((_io[IO::STAT] & LcdStatus::LcdStatusMode) >= 2)
  //   return 0xFF;

  // Get data from OAM
  return _oam[addr];
}

std::uint8_t  GBC::GameBoyColor::readIo(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= 0x0080)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  switch (addr) {
  case IO::JOYP:    // Joypad, R/W
    // Bits 7-6 are always set
    return _io[IO::JOYP] | 0b11000000;

  case IO::NR11:    // Channel 1 Length/wave pattern, R/W
  case IO::NR21:    // Channel 2 Length/wave pattern, R/W
    // Only wave pattern is readdable
    return _io[addr] & 0b11000000;

  case IO::NR14:    // Channel 1 Frequency higher 3 bits, limit flag, start sound, R/W
  case IO::NR24:    // Channel 2 Frequency higher 3 bits, limit flag, start sound, R/W
  case IO::NR34:    // Channel 3 Frequency higher 3 bits, limit flag, start sound, R/W
  case IO::NR44:    // Channel 4 Limit flag, start sound, R/W
    // Only limit flag is readable
    return _io[addr] & 0b01000000;

  case IO::NR52:    // Sound enable, R/W
    return (_io[IO::NR52] & 0b10000000)
      | (_sound1.length > 0.f ? 0b00000001 : 0b00000000)
      | (_sound2.length > 0.f ? 0b00000010 : 0b00000000)
      | (_sound3.length > 0.f && (_io[IO::NR30] & 0b10000000) ? 0b00000100 : 0b00000000)
      | (_sound4.length > 0.f ? 0b00000100 : 0b00000000);

  case IO::VBK:     // Video RAM Bank, R/W, CGB mode only
    return _io[IO::VBK] | 0b11111110;

  case IO::BCPD:    // Background Color Palette Data, R/W, reference byte in Background Color RAM at index BCPI, CGB mode only
    if ((_io[IO::STAT] & LcdStatus::LcdStatusMode) >= 3)
      return 0xFF;
    return _bgcRam[_io[IO::BCPI] & 0b00111111];

  case IO::OCPD:    // OBJ Color Palette Data, R/W, reference byte in OBJ Color RAM at index OCPI, CGB mode only
    if ((_io[IO::STAT] & LcdStatus::LcdStatusMode) >= 3)
      return 0xFF;
    return _obcRam[_io[IO::OCPI] & 0b00111111];

  case IO::SVBK:    // Work Ram Bank, R/W, CGB mode only
    return _io[IO::SVBK] & 0b00000111;

  case IO::DIVHi:   // High byte of DIV
  case IO::TIMA:    // Timer Counter, R/W
  case IO::TMA:     // Timer Modulo, R/W
  case IO::TAC:     // Time Control, R/W of bits 2-1-0
  case IO::IF:      // Interrupt Flags, R/W
  case IO::NR10:    // Channel 1 Sweep, R/W
  case IO::NR12:    // Channel 1 Envelope, R/W
  case IO::NR22:    // Channel 2 Envelope, R/W
  case IO::NR30:    // Channel 3 Sound on/off, R/W
  case IO::NR32:    // Channel 3 Envelope, R/W
  case IO::NR42:    // Channel 4 Envelope, R/W
  case IO::NR43:    // Channel 4 Polynomial counter, R/W
  case IO::NR50:    // Sound stereo left/right volume, R/W
  case IO::NR51:    // Sound stereo left/right enable, R/W
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
  case IO::LCDC:    // LCD Control, R/W (see enum)
  case IO::STAT:    // LCD Status, R/W (see enum)
  case IO::SCY:     // Scroll Y, R/W
  case IO::SCX:     // Scroll X, R/W
  case IO::LY:      // LCD Y Coordinate, R
  case IO::LYC:     // LCD Y Coordinate Compare, R/W
  case IO::DMA:     // DMA Transfer and Start Address, R/W
  case IO::BGP:     // Background Palette Data, R/W, non CGB mode only
  case IO::OBP0:    // OBJ 0 Palette Data, R/W, non CGB mode only
  case IO::OBP1:    // OBJ 1 Palette Data, R/W, non CGB mode only
  case IO::WY:      // Window Y Position, R/W
  case IO::WX:      // Window X Position, R/W
  case IO::KEY0:    // CPU Mode, R/W, see enum
  case IO::KEY1:    // CPU Speed Switch, R/W, CGB mode only
  case IO::HDMA1:   // New DMA Transfers source high byte, W, CGB mode only
  case IO::HDMA2:   // New DMA Transfers source low byte, W, CGB mode only
  case IO::HDMA3:   // New DMA Transfers destination high byte, W, CGB mode only
  case IO::HDMA4:   // New DMA Transfers destination low byte, W, CGB mode only
  case IO::HDMA5:   // Start New DMA Transfer
  case IO::BCPI:    // Background Color Palette Index, R/W, CGB mode only
  case IO::OCPI:    // OBJ Color Palette Index, R/W, CGB mode only
  case IO::OPRI:    // OBJ Priority Mode, R/W, CGB mode only
  case 0x72:        // Undocumented, R/W
  case 0x73:        // Undocumented, R/W
  case 0x75:        // Undocumented, R/W (bit 4-6)
    // Basic read, just return stored value
    return _io[addr];

  case IO::DIVLo:   // Low byte of DIV, not accessible
  case IO::NR13:    // Channel 1 Frequency lower 8 bits, W
  case IO::NR23:    // Channel 2 Frequency lower 8 bits, W
  case IO::NR31:    // Channel 3 Length, W
  case IO::NR41:    // Channel 4 Length, W
  case IO::NR33:    // Channel 3 Frequency lower 8 bits, W
  case IO::BANK:    // Boot Bank Controller, W, 0 to enable Boot mapping in ROM
  default:          // Invalid register
    // Default value in case of error
    return 0xFF;
  }
}

std::uint8_t  GBC::GameBoyColor::readHRam(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= _hRam.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Get data from HRAM
  return _hRam[addr];
}

void  GBC::GameBoyColor::write(std::uint16_t addr, std::uint8_t value)
{
  // 16 KiB ROM bank 00 from cartridge, usually a fixed bank
  // 16 KiB ROM Bank 01~NN Ffom cartridge, switchable bank via mapper (if any)
  // OR
  // Bootstrap sequence
  if (addr < 0x8000)
    _mbc->writeRom(addr - 0x0000, value);

  // 8 KiB Video RAM (VRAM)
  // In CGB mode, switchable bank 0/1
  else if (addr < 0xA000)
    writeVRam(addr - 0x8000, value);

  // 8 KiB External RAM
  // From cartridge, switchable bank if any
  else if (addr < 0xC000)
    _mbc->writeRam(addr - 0xA000, value);

  // 8 KiB Work RAM (WRAM)
  // In CGB mode, second half is switchable bank 1~7 
  else if (addr < 0xE000)
    writeWRam(addr - 0xC000, value);

  // Mirror of C000~DDFF (ECHO RAM)
  // Nintendo says use of this area is prohibited
  else if (addr < 0xFE00)
    write(addr - 0x2000, value);

  // Sprite attribute table (OAM)	
  else if (addr < 0xFEA0)
    writeOam(addr - 0xFE00, value);

  // Not Usable
  // Nintendo says use of this area is prohibited
  else if (addr < 0xFF00)
    return;

  // I/O Registers
  else if (addr < 0xFF80)
    writeIo(addr - 0xFF00, value);

  // High RAM (HRAM)
  else if (addr < 0xFFFF)
    writeHRam(addr - 0xFF80, value);

  // Interrupt Enable register (IE)
  else
    _ie = value;
}

void  GBC::GameBoyColor::writeVRam(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  
  // VRAM locked when drawing
  // if ((_io[IO::STAT] & LcdStatus::LcdStatusMode) >= 3)
  //   return;

  // Game Boy Color has 2 VRAM banks
  _vRam[(_io[IO::VBK] & 0b00000001) * 0x2000 + addr] = value;
}

void  GBC::GameBoyColor::writeWRam(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // First half is alway bank 0
  if (addr < 0x1000)
    _wRam[addr] = value;

  // Second half is a WRAM bank
  else
    _wRam[(std::clamp(_io[IO::SVBK] & 0b00000111, 1, 7) * 0x1000) + (addr - 0x1000)] = value;
}

void  GBC::GameBoyColor::writeOam(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x00A0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // OAM locked when drawing
  // if ((_io[IO::STAT] & LcdStatus::LcdStatusMode) >= 2)
  //   return;

  // Write data to OAM
  _oam[addr] = value;
}

void  GBC::GameBoyColor::writeIo(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x0080)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  switch (addr) {
  case IO::JOYP:    // Joypad, R/W
    // Reset pressed keys and force bits 7-6 to true
    _io[IO::JOYP] = value | 0b11001111;

    // Direction buttons
    if (!(_io[IO::JOYP] & 0b00010000)) {
      if (_keys[Key::KeyDown] == true)
        _io[IO::JOYP] &= 0b11110111;
      if (_keys[Key::KeyUp] == true)
        _io[IO::JOYP] &= 0b11111011;
      if (_keys[Key::KeyLeft] == true)
        _io[IO::JOYP] &= 0b11111101;
      if (_keys[Key::KeyRight] == true)
        _io[IO::JOYP] &= 0b11111110;
    }

    // Action buttons
    if (!(_io[IO::JOYP] & 0b00100000)) {
      if (_keys[Key::KeyStart] == true)
        _io[IO::JOYP] &= 0b11110111;
      if (_keys[Key::KeySelect] == true)
        _io[IO::JOYP] &= 0b11111011;
      if (_keys[Key::KeyB] == true)
        _io[IO::JOYP] &= 0b11111101;
      if (_keys[Key::KeyA] == true)
        _io[IO::JOYP] &= 0b11111110;
    }
    break;

  case IO::DIVHi:   // High byte of DIV, R/W (always set to zero when written)
    // Always set to 0
    _io[IO::DIVHi] = 0;
    break;

  case IO::TAC:     // Time Control, R/W of bit 2-1-0
    _io[IO::TAC] = value & 0b00000111;

  case IO::IF:      // Interrupt Flags, R/W
    // Bits 7-6-5 are always set
    _io[IO::IF] = value | 0b11100000;
    break;

  case IO::NR14:    // Channel 1 Frequency higher 3 bits, limit flag, start sound, R/W
    // Bits 7-6-5 are always set
    _io[IO::NR14] = value & 0b01111111;

    // Start sound 1
    if (value & 0b10000000) {
      _sound1.frequencyTime = (_io[IO::NR10] & 0b01110000) ?
        ((_io[IO::NR10] & 0b01110000) >> 4) / 128.f :
        999.f;
      _sound1.frequencyDirection = (_io[IO::NR10] & 0b00001000) ? true : false;
      _sound1.frequencyShift = 1.f / (float)std::pow(2.f, _io[IO::NR10] & 0b00000111);
      _sound1.frequencyElapsed = 0.f;
      switch ((_io[IO::NR11] & 0b11000000) >> 6) {
      case 0b00: _sound1.wave = 0.125f; break;
      case 0b01: _sound1.wave = 0.25f; break;
      case 0b10: _sound1.wave = 0.5f; break;
      case 0b11: _sound1.wave = 0.75f; break;
      default:
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }
      _sound1.length = (_io[IO::NR14] & 0b01000000) ?
        (64 - (_io[IO::NR11] & 0b00111111)) / 256.f :
        999.f;
      _sound1.envelope = ((_io[IO::NR12] & 0b11110000) >> 4) / 16.f;
      _sound1.envelopeDirection = (_io[IO::NR12] & 0b00001000) ? true : false;
      _sound1.envelopeTime = (_io[IO::NR12] & 0b00000111) ?
        (_io[IO::NR12] & 0b00000111) / 64.f :
        999.f;
      _sound1.envelopeElapsed = 0.f;
      _sound1.frequency = 131072.f / (2048.f - (((std::uint16_t)_io[IO::NR13]) + (((std::uint16_t)_io[IO::NR14] & 0b00000111) << 8)));
      _sound1.clock = 0.f;
    }
    break;

  case IO::NR24:    // Channel 2 Frequency higher 3 bits, limit flag, start sound, R/W
    // Bits 7-6-5 are always set
    _io[IO::NR24] = value & 0b01111111;

    // Start sound 2
    if (value & 0b10000000) {
      switch ((_io[IO::NR21] & 0b11000000) >> 6) {
      case 0b00: _sound2.wave = 0.125f; break;
      case 0b01: _sound2.wave = 0.25f; break;
      case 0b10: _sound2.wave = 0.5f; break;
      case 0b11: _sound2.wave = 0.75f; break;
      default:
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }
      _sound2.length = (_io[IO::NR24] & 0b01000000) ?
        (64 - (_io[IO::NR21] & 0b00111111)) / 256.f :
        999.f;
      _sound2.envelope = ((_io[IO::NR22] & 0b11110000) >> 4) / 16.f;
      _sound2.envelopeDirection = (_io[IO::NR22] & 0b00001000) ? true : false;
      _sound2.envelopeTime = (_io[IO::NR22] & 0b00000111) ?
        (_io[IO::NR22] & 0b00000111) / 64.f :
        999.f;
      _sound2.envelopeElapsed = 0.f;
      _sound2.frequency = 131072.f / (2048.f - (((std::uint16_t)_io[IO::NR23]) + (((std::uint16_t)_io[IO::NR24] & 0b00000111) << 8)));
      _sound2.clock = 0.f;
    }
    break;

  case IO::NR34:    // Channel 3 Frequency higher 3 bits, limit flag, start sound, R/W
    // Bits 7-6-5 are always set
    _io[IO::NR34] = value & 0b01111111;

    // Start sound 3
    if (value & 0b10000000) {
      for (std::uint8_t index = IO::WAVE00; index <= IO::WAVE30; index++) {
        _sound3.wave[(index - IO::WAVE00) * 2 + 0] = ((_io[index] & 0b11110000) >> 4) / 8.f - 1.f;
        _sound3.wave[(index - IO::WAVE00) * 2 + 1] = ((_io[index] & 0b00001111) >> 0) / 8.f - 1.f;
      }
      _sound3.length = (_io[IO::NR34] & 0b01000000) ?
        (256 - _io[IO::NR31]) / 256.f :
        999.f;
      switch ((_io[IO::NR32] & 0b01100000) >> 5) {
      case 0b00: _sound3.envelope = 0.f; break;
      case 0b01: _sound3.envelope = 1.f; break;
      case 0b10: _sound3.envelope = 0.5f; break;
      case 0b11: _sound3.envelope = 0.25f; break;
      default:
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }
      _sound3.frequency = 65536.f / (2048.f - (((std::uint16_t)_io[IO::NR33]) + (((std::uint16_t)_io[IO::NR34] & 0b00000111) << 8)));
      _sound3.clock = 0.f;
    }
    break;

  case IO::NR44:    // Channel 1 Frequency higher 3 bits, limit flag, start sound, R/W
    // Bits 7-6-5 are always set
    _io[IO::NR44] = value & 0b01000000;

    // Start sound 4
    if (value & 0b10000000) {
      _sound4.length = (_io[IO::NR44] & 0b01000000) ?
        (64 - (_io[IO::NR41] & 0b00111111)) / 256.f :
        999.f;
      _sound4.envelope = ((_io[IO::NR42] & 0b11110000) >> 4) / 16.f;
      _sound4.envelopeDirection = (_io[IO::NR42] & 0b00001000) ? true : false;
      _sound4.envelopeTime = (_io[IO::NR42] & 0b00000111) ?
        (_io[IO::NR42] & 0b00000111) / 64.f :
        999.f;
      _sound4.envelopeElapsed = 0.f;
      _sound4.counter = +1.f;
      _sound4.counterValue = 0b1111111111111111;
      _sound4.counterWidth = (_io[IO::NR43] & 0b00001000) ? 7 : 15;
      _sound4.counterTime = 1.f / (524288.f / std::clamp((float)(_io[IO::NR43] & 0b00000111), 0.5f, 7.f) / std::pow(2.f, ((_io[IO::NR43] & 0b11110000) >> 4) + 1.f));
      _sound4.counterElapsed = 0.f;
    }
    break;

  case IO::NR52:    // Sound enable, R/W
    // Only write sound on/off bit
    _io[IO::NR52] = value & 0b10000000;

    // Disable all sound channels
    if (!(_io[IO::NR52] & 0b10000000)) {
      _sound1.length = 0.f;
      _sound2.length = 0.f;
      _sound3.length = 0.f;
      _sound4.length = 0.f;
    }
    break;

  case IO::STAT:    // LCD Status, R/W (see enum)
    // Only bits 6-5-4-3 are writable 
    _io[IO::STAT] = (_io[IO::STAT] & 0b00000111) | (value & 0b01111000);
    break;

  case IO::LYC:     // LCD Y Coordinate Compare, R/W
    // Write value to register
    _io[IO::LYC] = value;

    // LY / LCY register comparison
    if (_io[IO::LY] == _io[IO::LYC]) {
      _io[IO::STAT] |= LcdStatus::LcdStatusEqual;

      // STAT compare (LY/LYC) interrupt
      if (_io[IO::STAT] & LcdStatus::LcdStatusCompare)
        _io[IO::IF] |= Interrupt::InterruptLcdStat;
    }
    else
      _io[IO::STAT] &= ~LcdStatus::LcdStatusEqual;
    break;

  case IO::DMA:     // DMA Transfer and Start Address, R/W
    // Write value to register
    _io[IO::DMA] = value;

    // Only work durong HBlank and VBlank
    // if ((_io[IO::STAT] & LcdStatus::LcdStatusMode) >= 2)
    //   break;

    // Transfer data to OAM
    for (std::uint16_t index = 0; index < _oam.size(); index++)
      writeOam(index, read<std::uint8_t>(((std::uint16_t)_io[IO::DMA] << 8) + index));

    // TODO: DMA transfer is 160 cycles long, lock memory access
    break;

  case IO::KEY0:    // CPU Mode, R/W, see enum
    // Active only during boot mapping
    if (_io[IO::BANK] == 0)
      _io[IO::KEY0] = value;
    break;

  case IO::BANK:    // Boot Bank Controller, W, 0 to enable Boot mapping in ROM
    // Active only during boot mapping
    if (_io[IO::BANK] == 0)
      _io[IO::BANK] = value;
    break;

  case IO::HDMA5:   // Start New DMA Transfer, R/W, CGB mode only
  {
    std::uint16_t source = (((std::uint16_t)_io[IO::HDMA1] << 8) + (std::uint16_t)_io[IO::HDMA2]) & 0b1111111111110000;
    std::uint16_t destination = ((((std::uint16_t)_io[IO::HDMA3] << 8) + (std::uint16_t)_io[IO::HDMA4]) & 0b00011111111110000) | 0b1000000000000000;
    std::uint16_t length = ((std::uint16_t)(value & 0b01111111) + 1) * 0x10;
    
    // Transfer data to VRAM
    for (unsigned int count = 0; count < length; count++) {
      std::uint8_t  data;

      // Get data from source
      if ((source >= 0x0000 && source < 0x8000) ||
        (source >= 0xA000 && source < 0xC000) ||
        (source >= 0xC000 && source < 0xE000))
        data = read(source);
      else
        break;

      // Write data to destination
      if (destination >= 0x8000 && destination < 0xA000)
        writeVRam(destination - 0x8000, data);
      else
        break;

      // Increment addresses
      source += 1;
      destination += 1;
    }

    // Set register to completed transfer value
    _io[IO::HDMA5] = 0xFF;

    // Write end addresses to HDMA registers
    _io[IO::HDMA1] = (source >> 8) & 0b11111111;
    _io[IO::HDMA2] = (source >> 0) & 0b11111111;
    _io[IO::HDMA3] = (destination >> 8) & 0b11111111;
    _io[IO::HDMA4] = (destination >> 0) & 0b11111111;

    // TODO: respect HDMA transfer timing for mode 0 & 1
    if (value & 0b10000000)
      _cpuCycle += length * ((_io[IO::KEY1] & 0b10000000) ? 4 : 2);

    break;
  }

  case IO::BCPD:    // Background Color Palette Data, R/W, reference byte in BG Color RAM at index BCPI, CGB mode only
    // Store data in Background Color RAM
    //if ((_io[IO::STAT] & LcdStatus::LcdStatusMode) != 3)
    _bgcRam[_io[IO::BCPI] & 0b00111111] = value;

    // Increment pointer
    if (_io[IO::BCPI] & 0b10000000)
      _io[IO::BCPI] = 0b10000000 | ((_io[IO::BCPI] + 1) & 0b00111111);
    break;

  case IO::OCPD:    // OBJ Color Palette Data, R/W, reference byte in OBJ Color RAM at index OCPI, CGB mode only
    // Store data in OBJ Color RAM
    //if ((_io[IO::STAT] & LcdStatus::LcdStatusMode) != 3)
    _obcRam[_io[IO::OCPI] & 0b00111111] = value;

    // Increment pointer
    if (_io[IO::OCPI] & 0b10000000)
      _io[IO::OCPI] = 0b10000000 | ((_io[IO::OCPI] + 1) & 0b00111111);
    break;

  case IO::OPRI:    // OBJ Priority Mode, R/W, CGB mode only
    _io[IO::OPRI] = value & 0b00000001;
    break;

  case IO::SVBK:    // Work Ram Bank, R/W, CGB mode only
    if ((_io[IO::KEY0] & 0b00001100) != CpuMode::CpuModeDmg)
      _io[IO::SVBK] = value & 0b00000111;
    break;

  case IO::TIMA:    // Timer Modulo, R/W
  case IO::TMA:     // Timer Modulo, R/W
  case IO::NR10:    // Channel 1 Sweep, R/W
  case IO::NR11:    // Channel 1 Length/wave pattern, R/W
  case IO::NR12:    // Channel 1 Envelope, R/W
  case IO::NR13:    // Channel 1 Frequency lower 8 bits, W
  case IO::NR21:    // Channel 2 Length/wave pattern, R/W
  case IO::NR22:    // Channel 2 Envelope, R/W
  case IO::NR23:    // Channel 2 Frequency lower 8 bits, W
  case IO::NR30:    // Channel 3 Sound on/off, R/W
  case IO::NR31:    // Channel 3 Length, R/W
  case IO::NR32:    // Channel 3 Envelope, R/W
  case IO::NR33:    // Channel 3 Frequency lower 8 bits, W
  case IO::NR41:    // Channel 4 Length, W
  case IO::NR42:    // Channel 4 Envelope, R/W
  case IO::NR43:    // Channel 4 Polynomial counter, R/W
  case IO::NR50:    // Sound stereo left/right volume, R/W
  case IO::NR51:    // Sound stereo left/right enable, R/W
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
  case IO::LCDC:    // LCD Control, R/W (see enum)
  case IO::SCY:     // Scroll Y, R/W
  case IO::SCX:     // Scroll X, R/W
  case IO::BGP:     // Background Palette Data, R/W, non CGB mode only
  case IO::OBP0:    // OBJ 0 Palette Data, R/W, non CGB mode only
  case IO::OBP1:    // OBJ 1 Palette Data, R/W, non CGB mode only
  case IO::WY:      // Window Y Position, R/W
  case IO::WX:      // Window X Position, R/W
  case IO::KEY1:    // CPU Speed Switch, R/W, CGB mode only
  case IO::VBK:     // Video RAM Bank, R/W, CGB mode only
  case IO::HDMA1:   // New DMA Transfers source high byte, W, CGB mode only
  case IO::HDMA2:   // New DMA Transfers source low byte, W, CGB mode only
  case IO::HDMA3:   // New DMA Transfers destination high byte, W, CGB mode only
  case IO::HDMA4:   // New DMA Transfers destination low byte, W, CGB mode only
  case IO::BCPI:    // Background Color Palette Index, R/W, CGB mode only
  case IO::OCPI:    // OBJ Color Palette Index, R/W, CGB mode only
  case 0x72:        // Undocumented, R/W
  case 0x73:        // Undocumented, R/W
    _io[addr] = value;
    break;

  case 0x75:        // Undocumented, R/W (bit 4-6)
    _io[addr] = value & 0b01110000;
    break;

  case IO::DIVLo:   // Low byte of DIV, not accessible
  case IO::LY:      // LCD Y Coordinate, R
    break;

  default:
    break;
    //throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

void  GBC::GameBoyColor::writeHRam(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= _hRam.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Write data to HRAM
  _hRam[addr] = value;
}