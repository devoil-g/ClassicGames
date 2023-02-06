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

const std::string_view GBC::GameBoyColor::SaveStateBase = "0123456789ABCDEF";

GBC::GameBoyColor::GameBoyColor(const std::string& filename) :
  _header(),
  _path(filename),
  _boot(),
  _cycles(0),
  _cpu(*this),
  _ppu(*this),
  _apu(*this),
  _mbc(),
  _wRam(),
  _io(),
  _hRam(),
  _ie(0),
  _keys{0},
  _transferMode(Transfer::TransferNone)
{
  // Load ROM
  load(filename);

  // Initialize memory banks
  _wRam.fill(0);
  _io.fill(0);
  _hRam.fill(0);

  // Initialize registers
  _io[IO::JOYP] = 0b11111111;
  _io[IO::HDMA5] = 0b11111111;
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
    _mbc = std::make_unique<GBC::MemoryBankController>(*this, rom);
    _header.mbc = Header::MBC::MBCNone;
    break;
  case 0x08:  // ROM+RAM, never used
    _mbc = std::make_unique<GBC::MemoryBankController>(*this, rom, _header.ram_size);
    _header.mbc = Header::MBC::MBCNone;
    break;
  case 0x09:  // ROM+RAM+BATTERY, never used
    _mbc = std::make_unique<GBC::MemoryBankController>(*this, rom, _header.ram_size, _path + ".gbs");
    _header.mbc = Header::MBC::MBCNone;
    break;

  case 0x01:  // MBC1
    _mbc = std::make_unique<GBC::MemoryBankController1>(*this, rom);
    _header.mbc = Header::MBC::MBC1;
    break;
  case 0x02:  // MBC1+RAM
    _mbc = std::make_unique<GBC::MemoryBankController1>(*this, rom, _header.ram_size);
    _header.mbc = Header::MBC::MBC1;
    break;
  case 0x03:  // MBC1+RAM+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController1>(*this, rom, _header.ram_size, _path + ".gbs");
    _header.mbc = Header::MBC::MBC1;
    break;

  case 0x05:  // MBC2
    _mbc = std::make_unique<GBC::MemoryBankController2>(*this, rom);
    _header.mbc = Header::MBC::MBC2;
    break;
  case 0x06:  // MBC2+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController2>(*this, rom, _path + ".gbs");
    _header.mbc = Header::MBC::MBC2;
    break;

  case 0x0F:  // MBC3+TIMER+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController3>(*this, rom, 0, "", _path + ".rtc");
    _header.mbc = Header::MBC::MBC3;
    break;
  case 0x10:  // MBC3+TIMER+RAM+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController3>(*this, rom, _header.ram_size, _path + ".gbs", _path + ".rtc");
    _header.mbc = Header::MBC::MBC3;
    break;
  case 0x11:  // MBC3
    _mbc = std::make_unique<GBC::MemoryBankController3>(*this, rom);
    _header.mbc = Header::MBC::MBC3;
    break;
  case 0x12:  // MBC3+RAM
    _mbc = std::make_unique<GBC::MemoryBankController3>(*this, rom, _header.ram_size);
    _header.mbc = Header::MBC::MBC3;
    break;
  case 0x13:  // MBC3+RAM+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController3>(*this, rom, _header.ram_size, _path + ".gbs");
    _header.mbc = Header::MBC::MBC3;
    break;

  case 0x19:  // MBC5
  case 0x1C:  // MBC5+RUMBLE
    _mbc = std::make_unique<GBC::MemoryBankController5>(*this, rom);
    _header.mbc = Header::MBC::MBC5;
    break;
  case 0x1A:  // MBC5+RAM
  case 0x1D:  // MBC5+RUMBLE+RAM
    _mbc = std::make_unique<GBC::MemoryBankController5>(*this, rom, _header.ram_size);
    _header.mbc = Header::MBC::MBC5;
    break;
  case 0x1B:  // MBC5+RAM+BATTERY
  case 0x1E:  // MBC5+RUMBLE+RAM+BATTERY
    _mbc = std::make_unique<GBC::MemoryBankController5>(*this, rom, _header.ram_size, _path + ".gbs");
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
  std::uint64_t frame = _cycles / (GBC::PixelProcessingUnit::ScanlineDuration * (GBC::PixelProcessingUnit::ScreenHeight + GBC::PixelProcessingUnit::ScreenBlank));
  std::uint64_t cycles = _cycles;

  // Simulate Joypad
  simulateKeys();

  // Execution loop
  while (frame == _cycles / (GBC::PixelProcessingUnit::ScanlineDuration * (GBC::PixelProcessingUnit::ScreenHeight + GBC::PixelProcessingUnit::ScreenBlank)))
  {
    switch (_transferMode)
    {
      // Simulate a tick of the CPU
    case Transfer::TransferNone:
      _cpu.simulate();
      _cycles += (_io[IO::KEY1] & 0b10000000) ? 2 : 4;
      break;

      // DMA transfer
    case Transfer::TransferDma:
      // Transfer 4 bytes to OAM
      for (std::uint16_t index = 0; index < 4; index++)
        _ppu.writeDma((std::uint16_t)_transferIndex + index, read(((std::uint16_t)_io[IO::DMA] << 8) + (std::uint16_t)_transferIndex + index));
      _transferIndex += 4;

      // Simulate a tick of the CPU
      _cpu.simulate();
      _cycles += (_io[IO::KEY1] & 0b10000000) ? 2 : 4;

      // End of DMA transfer
      if (_transferIndex == 160)
        _transferMode = Transfer::TransferNone;

      // TODO: restrict memory access to HRAM during DMA transfer
      break;

      // General purpose DMA
    case Transfer::TransferHdma0:
    {
      std::uint16_t source = ((((std::uint16_t)_io[IO::HDMA1] << 8) + (std::uint16_t)_io[IO::HDMA2]) & 0b1111111111110000) + (std::uint16_t)_transferIndex;
      std::uint16_t destination = (((((std::uint16_t)_io[IO::HDMA3] << 8) + (std::uint16_t)_io[IO::HDMA4]) & 0b00011111111110000) | 0b1000000000000000) + (std::uint16_t)_transferIndex;
      bool          end = false;

      // Transfer 8 bytes per tick
      for (std::size_t index = 0; index < 8 && end == false; index++)
      {
        // Check transfer adress
        if (((source >= 0x0000 && source < 0x8000) ||
          (source >= 0xA000 && source < 0xC000) ||
          (source >= 0xC000 && source < 0xE000)) &&
          (destination >= 0x8000 && destination < 0xA000))
          _ppu.writeRam(destination - 0x8000, read(source));
        else {
          end = true;
          break;
        }

        source += 1;
        destination += 1;
      }

      // Increment transfer index
      _transferIndex += 8;

      // End of HDMA transfer
      if (_transferIndex == ((std::size_t)_io[IO::HDMA5] + 1) * 0x10 || end == true) {
        _transferMode = Transfer::TransferNone;
        _io[IO::HDMA5] = 0xFF;

        // Increment source and destination
        _io[IO::HDMA1] = (source >> 8) & 0b11111111;
        _io[IO::HDMA2] = (source >> 0) & 0b11111111;
        _io[IO::HDMA3] = (destination >> 8) & 0b11111111;
        _io[IO::HDMA4] = (destination >> 0) & 0b11111111;
      }

      // CPU is not executed
      _cycles += 4;
      break;
    }
    
    case Transfer::TransferHdma1:
    {
      bool trigger = (_io[GBC::PixelProcessingUnit::IO::STAT] & GBC::PixelProcessingUnit::LcdStatus::LcdStatusMode) == GBC::PixelProcessingUnit::Mode::Mode0;

      // Trigger when entering mode 0
      if (_transferTrigger == false && trigger == true)
        _transferIndex = 16;

      // Save current mode
      _transferTrigger = trigger;

      // HDMA Transfer at start of HBlank
      if (_transferIndex > 0)
      {
        std::uint16_t source = ((((std::uint16_t)_io[IO::HDMA1] << 8) + (std::uint16_t)_io[IO::HDMA2]) & 0b1111111111110000) + (std::uint16_t)(16 - _transferIndex);
        std::uint16_t destination = (((((std::uint16_t)_io[IO::HDMA3] << 8) + (std::uint16_t)_io[IO::HDMA4]) & 0b00011111111110000) | 0b1000000000000000) + (std::uint16_t)(16 - _transferIndex);
        bool          end = false;

        // Transfer 8 bytes per tick
        for (std::size_t index = 0; index < 8 && end == false; index++)
        {
          // Check transfer adress
          if (((source >= 0x0000 && source < 0x8000) ||
            (source >= 0xA000 && source < 0xC000) ||
            (source >= 0xC000 && source < 0xE000)) &&
            (destination >= 0x8000 && destination < 0xA000))
            _ppu.writeRam(destination - 0x8000, read(source));
          else
            end = true;

          source += 1;
          destination += 1;
        }

        // Update transfer index
        _transferIndex -= 8;
        
        // End of data chunk transfer
        if (_transferIndex == 0) {
          _io[IO::HDMA5] -= 1;

          // Increment source and destination
          _io[IO::HDMA1] = (source >> 8) & 0b11111111;
          _io[IO::HDMA2] = (source >> 0) & 0b11111111;
          _io[IO::HDMA3] = (destination >> 8) & 0b11111111;
          _io[IO::HDMA4] = (destination >> 0) & 0b11111111;
        }

        // End of HDMA transfer
        if (_io[IO::HDMA5] == 0xFF || end == true) {
          _transferMode = Transfer::TransferNone;
          _io[IO::HDMA5] = 0xFF;
        }

        // CPU is not executed
        _cycles += 4;
      }

      // Simulate CPU when no transfer
      else {
        _cpu.simulate();
        _cycles += (_io[IO::KEY1] & 0b10000000) ? 2 : 4;
      }

      break;
    }

    default:
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }

    // Update timer
    simulateTimer();

    // Update Pixel Processing Unit
    for (; cycles < _cycles; cycles += 1)
      _ppu.simulate();
  }

  // Update audio for one frame
  _apu.simulate();

  // Update MBC clock
  // NOTE: we could do this every CPU cycles,
  // but we don't need that much precision
  _mbc->update(GBC::PixelProcessingUnit::ScanlineDuration * (GBC::PixelProcessingUnit::ScreenHeight + GBC::PixelProcessingUnit::ScreenBlank));
}

void  GBC::GameBoyColor::simulateKeys()
{
  std::array<bool, Key::KeyCount> keys = {
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) < -0.5f || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::S),  // Down
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) > +0.5f || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Z),  // Up
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovX) < -0.5f || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Q),  // Left
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovX) > +0.5f || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::D),  // Right
    Game::Window::Instance().joystick().buttonDown(0, 7) || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::E),                               // Start
    Game::Window::Instance().joystick().buttonDown(0, 6) || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::A),                               // Select
    Game::Window::Instance().joystick().buttonDown(0, 1) || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::L),                               // B
    Game::Window::Instance().joystick().buttonDown(0, 0) || Game::Window::Instance().keyboard().keyDown(sf::Keyboard::P)                                // A
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

const sf::Texture& GBC::GameBoyColor::lcd() const
{
  // Get rendering target from PPU
  return _ppu.lcd();
}

const std::array<std::int16_t, GBC::AudioProcessingUnit::BufferSize>& GBC::GameBoyColor::sound() const
{
  // Get current sound buffer
  return _apu.sound();
}

const GBC::GameBoyColor::Header& GBC::GameBoyColor::header() const
{
  // Get cartridge header
  return _header;
}

void  GBC::GameBoyColor::load(std::size_t id)
{
  std::string   path(_path + ".gbs." + std::to_string(id));
  std::ifstream file(path);

  // Check valid file
  if (file.good() == false) {
    std::cerr << "[GBC::GameBoyColor::load] Warning, failed to load '" << path << "' save state." << std::endl;
    return;
  }

  // Load GBC variables
  load(file, "GBC_CYCLES", _cycles);
  load(file, "GBC_WRAM", _wRam);
  load(file, "GBC_IO", _io);
  load(file, "GBC_HRAM", _hRam);
  load(file, "GBC_IE", _ie);

  // Load hardware state
  _cpu.load(file);
  _apu.load(file);
  _ppu.load(file);
  _mbc->load(file);
}

void  GBC::GameBoyColor::save(std::size_t id) const
{
  std::string   path(_path + ".gbs." + std::to_string(id));
  std::ofstream file(path);

  // Check valid file
  if (file.good() == false) {
    std::cerr << "[GBC::GameBoyColor::save] Warning, failed to save state to '" << path << "'." << std::endl;
    return;
  }

  // Save GBC variables
  save(file, "GBC_CYCLES", _cycles);
  save(file, "GBC_WRAM", _wRam);
  save(file, "GBC_IO", _io);
  save(file, "GBC_HRAM", _hRam);
  save(file, "GBC_IE", _ie);

  // Save hardware state
  _cpu.save(file);
  _apu.save(file);
  _ppu.save(file);
  _mbc->save(file);
}

void  GBC::GameBoyColor::save(std::ofstream& file, const std::string& name, const void* data, std::size_t size) const
{
  // Variable name
  file << name << "=";

  // Byte array
  for (std::size_t index = 0; index < size; index++)
    file << SaveStateBase[((const std::uint8_t*)data)[index] / SaveStateBase.length()] << SaveStateBase[((const std::uint8_t*)data)[index] % SaveStateBase.length()];

  // End of line
  file << std::endl;
}

std::string GBC::GameBoyColor::loadVariable(std::ifstream& file, const std::string& name)
{
  // Go back to start of file
  file.seekg(0, std::ios_base::beg);

  std::string line;

  // Search for variable in file
  while (std::getline(file, line))
  {
    auto varSeparator = line.find('=');

    // Invalid format
    if (varSeparator == std::string::npos)
      continue;

    std::string varName = line.substr(0, varSeparator);
    std::string varValue = line.substr(varSeparator + 1);

    // Wrong variable
    if (varName == name)
      return varValue;
  }

  // Variable not found
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  GBC::GameBoyColor::loadValue(const std::string& value, void* data, std::size_t size)
{
  // Invalid variable size
  if (value.length() % 2 != 0 || value.length() / 2 != size)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load variable to data
  for (std::size_t index = 0; index < value.size(); index += 2) {
    auto value0 = SaveStateBase.find(value[index + 0]);
    auto value1 = SaveStateBase.find(value[index + 1]);

    // Invalid character
    if (value0 == std::string_view::npos || value1 == std::string_view::npos)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Load value in data
    ((std::uint8_t*)data)[index / 2] = (std::uint8_t)value0 * 16 + (std::uint8_t)value1;
  }
}

std::uint8_t  GBC::GameBoyColor::read(std::uint16_t addr)
{
  // 16 KiB ROM bank 00 from cartridge, usually a fixed bank
  // 16 KiB ROM Bank 01~NN Ffom cartridge, switchable bank via mapper (if any)
  // OR
  // Bootstrap sequence
  if (addr < 0x8000)
  {
    // Read from boot if not disabled
    if (_io[IO::BANK] == 0 && addr - 0x0000 < _boot.size() && (addr - 0x0000 <= 0x00FF || addr - 0x0000 >= 0x0150))
      return _boot[addr];

    // Read from MBC ROM
    return _mbc->readRom(addr - 0x0000);
  }

  // 8 KiB Video RAM (VRAM)
  // In CGB mode, switchable bank 0/1
  else if (addr < 0xA000)
    return _ppu.readRam(addr - 0x8000);

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
    return _ppu.readOam(addr - 0xFE00);

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
    return _wRam[(std::max(_io[IO::SVBK] & 0b00000111, 1) * 0x1000) + (addr - 0x1000)];
}

std::uint8_t  GBC::GameBoyColor::readIo(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= 0x0080)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  switch (addr)
  {
    // Audio registers
  case GBC::AudioProcessingUnit::IO::NR10:
  case GBC::AudioProcessingUnit::IO::NR11:
  case GBC::AudioProcessingUnit::IO::NR12:
  case GBC::AudioProcessingUnit::IO::NR13:
  case GBC::AudioProcessingUnit::IO::NR14:
  case GBC::AudioProcessingUnit::IO::NR21:
  case GBC::AudioProcessingUnit::IO::NR22:
  case GBC::AudioProcessingUnit::IO::NR23:
  case GBC::AudioProcessingUnit::IO::NR24:
  case GBC::AudioProcessingUnit::IO::NR30:
  case GBC::AudioProcessingUnit::IO::NR31:
  case GBC::AudioProcessingUnit::IO::NR32:
  case GBC::AudioProcessingUnit::IO::NR33:
  case GBC::AudioProcessingUnit::IO::NR34:
  case GBC::AudioProcessingUnit::IO::NR41:
  case GBC::AudioProcessingUnit::IO::NR42:
  case GBC::AudioProcessingUnit::IO::NR43:
  case GBC::AudioProcessingUnit::IO::NR44:
  case GBC::AudioProcessingUnit::IO::NR50:
  case GBC::AudioProcessingUnit::IO::NR51:
  case GBC::AudioProcessingUnit::IO::NR52:
  case GBC::AudioProcessingUnit::IO::WAVE00:
  case GBC::AudioProcessingUnit::IO::WAVE02:
  case GBC::AudioProcessingUnit::IO::WAVE04:
  case GBC::AudioProcessingUnit::IO::WAVE06:
  case GBC::AudioProcessingUnit::IO::WAVE08:
  case GBC::AudioProcessingUnit::IO::WAVE10:
  case GBC::AudioProcessingUnit::IO::WAVE12:
  case GBC::AudioProcessingUnit::IO::WAVE14:
  case GBC::AudioProcessingUnit::IO::WAVE16:
  case GBC::AudioProcessingUnit::IO::WAVE18:
  case GBC::AudioProcessingUnit::IO::WAVE20:
  case GBC::AudioProcessingUnit::IO::WAVE22:
  case GBC::AudioProcessingUnit::IO::WAVE24:
  case GBC::AudioProcessingUnit::IO::WAVE26:
  case GBC::AudioProcessingUnit::IO::WAVE28:
  case GBC::AudioProcessingUnit::IO::WAVE30:
    return _apu.readIo(addr);

    // Graphics registers
  case GBC::PixelProcessingUnit::IO::LCDC:
  case GBC::PixelProcessingUnit::IO::STAT:
  case GBC::PixelProcessingUnit::IO::SCY:
  case GBC::PixelProcessingUnit::IO::SCX:
  case GBC::PixelProcessingUnit::IO::LY:
  case GBC::PixelProcessingUnit::IO::LYC:
  case GBC::PixelProcessingUnit::IO::BGP:
  case GBC::PixelProcessingUnit::IO::OBP0:
  case GBC::PixelProcessingUnit::IO::OBP1:
  case GBC::PixelProcessingUnit::IO::WY:
  case GBC::PixelProcessingUnit::IO::WX:
  case GBC::PixelProcessingUnit::IO::VBK:
  case GBC::PixelProcessingUnit::IO::BCPI:
  case GBC::PixelProcessingUnit::IO::BCPD:
  case GBC::PixelProcessingUnit::IO::OCPI:
  case GBC::PixelProcessingUnit::IO::OCPD:
  case GBC::PixelProcessingUnit::IO::OPRI:
    return _ppu.readIo(addr);

  case IO::JOYP:  // Joypad, R/W
    // Bits 7-6 are always set
    return _io[IO::JOYP] | 0b11000000;

  case IO::SC:    // Serial transfer Control
    return _io[IO::SC] | 0b10000011;

  case IO::RP:    // Infared communication
    return _io[IO::RP] & ((_io[IO::RP] & 0b11000000) == 0b11000000 ? 0b11000011 : 0b11000001);
    break;

  case IO::SVBK:  // Work Ram Bank, R/W, CGB mode only
    return _io[IO::SVBK] & 0b00000111;

  case IO::DIVHi: // High byte of DIV
  case IO::TIMA:  // Timer Counter, R/W
  case IO::TMA:   // Timer Modulo, R/W
  case IO::TAC:   // Time Control, R/W of bits 2-1-0
  case IO::IF:    // Interrupt Flags, R/W
  case IO::DMA:   // DMA Transfer and Start Address, R/W
  case IO::KEY0:  // CPU Mode, R/W, see enum
  case IO::KEY1:  // CPU Speed Switch, R/W, CGB mode only
  case IO::HDMA5: // Start New DMA Transfer, R/W, CGB mode only
  case 0x72:      // Undocumented, R/W
  case 0x73:      // Undocumented, R/W
  case 0x75:      // Undocumented, R/W (bit 4-6)
    // Basic read, just return stored value
    return _io[addr];



  case IO::SB:    // Serial transfer Data, R/W
  case IO::DIVLo: // Low byte of DIV, not accessible
  case IO::BANK:  // Boot Bank Controller, W, 0 to enable Boot mapping in ROM
  case IO::HDMA1: // New DMA Transfers source high byte, W, CGB mode only
  case IO::HDMA2: // New DMA Transfers source low byte, W, CGB mode only
  case IO::HDMA3: // New DMA Transfers destination high byte, W, CGB mode only
  case IO::HDMA4: // New DMA Transfers destination low byte, W, CGB mode only
  default:        // Invalid register
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
    _ppu.writeRam(addr - 0x8000, value);

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
    writeWRam(addr - 0xE000, value);

  // Sprite attribute table (OAM)	
  else if (addr < 0xFEA0)
    _ppu.writeOam(addr - 0xFE00, value);

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

void  GBC::GameBoyColor::writeIo(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x0080)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  switch (addr)
  {
    // Audio registers
  case GBC::AudioProcessingUnit::IO::NR10:
  case GBC::AudioProcessingUnit::IO::NR11:
  case GBC::AudioProcessingUnit::IO::NR12:
  case GBC::AudioProcessingUnit::IO::NR13:
  case GBC::AudioProcessingUnit::IO::NR14:
  case GBC::AudioProcessingUnit::IO::NR21:
  case GBC::AudioProcessingUnit::IO::NR22:
  case GBC::AudioProcessingUnit::IO::NR23:
  case GBC::AudioProcessingUnit::IO::NR24:
  case GBC::AudioProcessingUnit::IO::NR30:
  case GBC::AudioProcessingUnit::IO::NR31:
  case GBC::AudioProcessingUnit::IO::NR32:
  case GBC::AudioProcessingUnit::IO::NR33:
  case GBC::AudioProcessingUnit::IO::NR34:
  case GBC::AudioProcessingUnit::IO::NR41:
  case GBC::AudioProcessingUnit::IO::NR42:
  case GBC::AudioProcessingUnit::IO::NR43:
  case GBC::AudioProcessingUnit::IO::NR44:
  case GBC::AudioProcessingUnit::IO::NR50:
  case GBC::AudioProcessingUnit::IO::NR51:
  case GBC::AudioProcessingUnit::IO::NR52:
  case GBC::AudioProcessingUnit::IO::WAVE00:
  case GBC::AudioProcessingUnit::IO::WAVE02:
  case GBC::AudioProcessingUnit::IO::WAVE04:
  case GBC::AudioProcessingUnit::IO::WAVE06:
  case GBC::AudioProcessingUnit::IO::WAVE08:
  case GBC::AudioProcessingUnit::IO::WAVE10:
  case GBC::AudioProcessingUnit::IO::WAVE12:
  case GBC::AudioProcessingUnit::IO::WAVE14:
  case GBC::AudioProcessingUnit::IO::WAVE16:
  case GBC::AudioProcessingUnit::IO::WAVE18:
  case GBC::AudioProcessingUnit::IO::WAVE20:
  case GBC::AudioProcessingUnit::IO::WAVE22:
  case GBC::AudioProcessingUnit::IO::WAVE24:
  case GBC::AudioProcessingUnit::IO::WAVE26:
  case GBC::AudioProcessingUnit::IO::WAVE28:
  case GBC::AudioProcessingUnit::IO::WAVE30:
    _apu.writeIo(addr, value);
    break;

    // Graphics registers
  case GBC::PixelProcessingUnit::IO::LCDC:
  case GBC::PixelProcessingUnit::IO::STAT:
  case GBC::PixelProcessingUnit::IO::SCY:
  case GBC::PixelProcessingUnit::IO::SCX:
  case GBC::PixelProcessingUnit::IO::LY:
  case GBC::PixelProcessingUnit::IO::LYC:
  case GBC::PixelProcessingUnit::IO::BGP:
  case GBC::PixelProcessingUnit::IO::OBP0:
  case GBC::PixelProcessingUnit::IO::OBP1:
  case GBC::PixelProcessingUnit::IO::WY:
  case GBC::PixelProcessingUnit::IO::WX:
  case GBC::PixelProcessingUnit::IO::VBK:
  case GBC::PixelProcessingUnit::IO::BCPI:
  case GBC::PixelProcessingUnit::IO::BCPD:
  case GBC::PixelProcessingUnit::IO::OCPI:
  case GBC::PixelProcessingUnit::IO::OCPD:
  case GBC::PixelProcessingUnit::IO::OPRI:
    _ppu.writeIo(addr, value);
    break;

  case IO::JOYP:  // Joypad, R/W
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

  case IO::SC:    // Serial transfer Control
    _io[IO::SC] = value & 0b10000011;

    // Handle transfer
    // NOTE: transfer not supported, received bit set to 0b11111111
    if ((_io[IO::SC] & 0b10000001) == 0b10000001) {
      _io[IO::IF] |= Interrupt::InterruptSerial;
      _io[IO::SB] = 0b11111111;
      _io[IO::SC] &= 0b00000011;
    }
    break;

  case IO::DIVHi: // High byte of DIV, R/W (always set to zero when written)
    // Always set to 0
    _io[IO::DIVHi] = 0;
    break;

  case IO::TAC:     // Time Control, R/W of bit 2-1-0
    _io[IO::TAC] = value & 0b00000111;
    break;

  case IO::IF:    // Interrupt Flags, R/W
    // Bits 7-6-5 are always set
    _io[IO::IF] = value | 0b11100000;
    break;

  case IO::DMA:   // DMA Transfer and Start Address, R/W
    // Write value to register
    _io[IO::DMA] = value;

    // Start transfer
    _transferMode = Transfer::TransferDma;
    _transferIndex = 0;
    break;

  case IO::KEY0:  // CPU Mode, R/W, see enum
    // Active only during boot mapping
    if (_io[IO::BANK] == 0)
      _io[IO::KEY0] = value;
    break;

  case IO::KEY1:  // CPU Speed Switch, R/W, CGB mode only, bit 7: current speed (0: normal, 1: double), bit 0: prepare switch (0: no, 1: prepare)
    // Set CPU speed switch bit
    _io[IO::KEY1] = (_io[IO::KEY1] & 0b10000000) | (value & 0b00000001);
    break;

  case IO::BANK:  // Boot Bank Controller, W, 0 to enable Boot mapping in ROM
    // Active only during boot mapping
    if (_io[IO::BANK] == 0)
      _io[IO::BANK] = value;
    break;

  case IO::HDMA5: // Start New DMA Transfer, R/W, CGB mode only
  {
    // HBlank DMA
    if (value & 0b10000000)
    {
      _transferMode = Transfer::TransferHdma1;
      _transferIndex = 0;
      _transferTrigger = (_io[GBC::PixelProcessingUnit::IO::STAT] & GBC::PixelProcessingUnit::LcdStatus::LcdStatusMode) == GBC::PixelProcessingUnit::Mode::Mode0;
      _io[IO::HDMA5] = value & 0b01111111;
    }

    // Interrupt HBlank DMA
    else if (!(_io[IO::HDMA5] & 0b10000000)) {
      _transferMode = Transfer::TransferNone;
      _io[IO::HDMA5] |= 0b10000000;
    }

    // General Purpose DMA
    else {
      _transferMode = Transfer::TransferHdma0;
      _transferIndex = 0;
      _io[IO::HDMA5] = value & 0b01111111;
    }

    break;
  }

  case IO::RP:    // Infared communication, bit 0: LED on/off (R/W), bit 1: signal off/on (R), bit 7-6: read enable (3) / disable (0)
    _io[IO::RP] = (_io[IO::RP] & 0b00111110) | (value & 0b11000001);
    break;

  case IO::SVBK:  // Work Ram Bank, R/W, CGB mode only
    if ((_io[IO::KEY0] & 0b00001100) != CpuMode::CpuModeDmg)
      _io[IO::SVBK] = value & 0b00000111;
    break;

  case IO::SB:    // Serial transfer Data, R/W
  case IO::TIMA:  // Timer Modulo, R/W
  case IO::TMA:   // Timer Modulo, R/W
  case IO::HDMA1: // New DMA Transfers source high byte, W, CGB mode only
  case IO::HDMA2: // New DMA Transfers source low byte, W, CGB mode only
  case IO::HDMA3: // New DMA Transfers destination high byte, W, CGB mode only
  case IO::HDMA4: // New DMA Transfers destination low byte, W, CGB mode only
  case 0x72:      // Undocumented, R/W
  case 0x73:      // Undocumented, R/W
    _io[addr] = value;
    break;

  case 0x75:      // Undocumented, R/W (bit 4-6)
    _io[addr] = value & 0b01110000;
    break;

  case IO::DIVLo: // Low byte of DIV, not accessible
    break;

  default:
    break;
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