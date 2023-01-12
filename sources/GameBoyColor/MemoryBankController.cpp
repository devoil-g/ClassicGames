#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "GameBoyColor/MemoryBankController.hpp"
#include "GameBoyColor/GameBoyColor.hpp"

GBC::MemoryBankController::MemoryBankController(GBC::GameBoyColor& gbc, const std::vector<std::uint8_t>& rom, std::size_t ramSize, const std::string& ramSave) :
  _gbc(gbc),
  _rom(rom),
  _romBank0(0),
  _romBank1(1),
  _ram(ramSize, 0),
  _ramEnable(false),
  _ramBank(0),
  _ramSave(ramSave),
  _ramSaved()
{
  // ROM cannot be empty
  if (_rom.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load RAM from memory
  loadRam();

  // Set saved RAM
  _ramSaved = _ram;
}

GBC::MemoryBankController::~MemoryBankController()
{
  // Save RAM to file
  saveRam();
}

std::uint8_t  GBC::MemoryBankController::readRom(std::uint16_t address) const
{
  // Out of bound address
  if (address >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Get bank from address range
  std::size_t bank = address < 0x4000 ? _romBank0 : _romBank1;

  // Truncate address to ROM size
  return _rom[(bank * 0x4000 + address % 0x4000) % _rom.size()];
}

std::uint8_t  GBC::MemoryBankController::readRam(std::uint16_t address) const
{
  // Out of bound address
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Cancel if no RAM or disabled
  if (_ram.empty() == true || _ramEnable == false)
    return 0xFF;

  // Get RAM, truncate address
  return _ram[(_ramBank * 0x2000 + address) % _ram.size()];
}

void  GBC::MemoryBankController::writeRom(std::uint16_t address, std::uint8_t value)
{
  // Out of bound address
  if (address >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Default MBC has no registers
  std::cerr << "[GBC::MBC] Warning, invalid write to MBC register." << std::endl;
}

void  GBC::MemoryBankController::writeRam(std::uint16_t address, std::uint8_t value)
{
  // Out of bound address
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Cancel if no RAM or disabled
  if (_ram.empty() == true || _ramEnable == false)
    return;

  // Write to RAM, truncate address
  _ram[(_ramBank * 0x2000 + address) % _ram.size()] = value;
}

std::pair<std::size_t, std::size_t> GBC::MemoryBankController::getRomBanks() const
{
  // Get ROM banks
  return { _romBank0, _romBank1 };
}

std::size_t GBC::MemoryBankController::getRamBank() const
{
  // Get RAM bank
  return _ramBank;
}

void  GBC::MemoryBankController::setRomBanks(std::size_t bank0, std::size_t bank1)
{
  // Set ROM banks of first and second half of address range
  _romBank0 = bank0;
  _romBank1 = bank1;
}

void  GBC::MemoryBankController::setRamBank(std::size_t bank)
{
  // Set RAM bank
  _ramBank = bank;
}

bool  GBC::MemoryBankController::getRamEnable() const
{
  // Get RAM enable flag
  return _ramEnable;
}

void  GBC::MemoryBankController::setRamEnable(bool enable)
{
  // Save RAM to buffer
  if (_ramEnable == true && enable == false)
    _ramSaved = _ram;

  // Enable/disable RAM
  _ramEnable = enable;
}

void  GBC::MemoryBankController::loadRam()
{
  // No save file
  if (_ramSave.empty() == true)
    return;

  std::ifstream file(_ramSave, std::ifstream::binary);

  // Check if file open properly
  if (file.good() == false) {
    std::cerr << "[GBC::MBC] Warning, failed to load '" << _ramSave << "'." << std::endl;
    return;
  }

  // Check compatible size
  if (std::filesystem::file_size(_ramSave) != _ram.size()) {
    std::cerr << "[GBC::MBC] Warning, invalid save size '" << _ramSave << "'." << std::endl;
    return;
  }

  std::vector<std::uint8_t> data(std::filesystem::file_size(_ramSave), 0);

  // Load ROM
  file.read((char*)data.data(), data.size());

  // Check for error
  if (file.gcount() != data.size()) {
    std::cerr << "[GBC::MBC] Warning, invalid read size of '" << _ramSave << "'." << std::endl;
    return;
  }

  // Copy RAM
  std::copy(data.begin(), data.end(), _ram.begin());
}

void  GBC::MemoryBankController::saveRam() const
{
  // No save file
  if (_ramSave.empty() == true)
    return;

  std::ofstream file(_ramSave, std::ofstream::binary | std::ofstream::trunc);

  // Check if file open properly
  if (file.good() == false) {
    std::cerr << "[GBC::MBC] Warning, failed to open '" << _ramSave << "'." << std::endl;
    return;
  }

  // Write External RAM to save file
  file.write((const char*)_ramSaved.data(), _ramSaved.size());

  // Check for success
  if (file.good() == false) {
    std::cerr << "[GBC::MBC] Warning, failed to write '" << _ramSave << "'." << std::endl;
    return;
  }
}

void    GBC::MemoryBankController::save(std::ofstream& file) const
{
  // Save MBC variables
  _gbc.save(file, "MBC_ROMBANK0", _romBank0);
  _gbc.save(file, "MBC_ROMBANK1", _romBank1);
  _gbc.save(file, "MBC_RAM", _ram);
  _gbc.save(file, "MBC_RAMENABLE", _ramEnable);
  _gbc.save(file, "MBC_RAMBANK", _ramBank);
  _gbc.save(file, "MBC_RAMSAVE", _ramSave);
  _gbc.save(file, "MBC_RAMSAVED", _ramSaved);
}

void    GBC::MemoryBankController::load(std::ifstream& file)
{
  // Load MBC variables
  _gbc.load(file, "MBC_ROMBANK0", _romBank0);
  _gbc.load(file, "MBC_ROMBANK1", _romBank1);
  _gbc.load(file, "MBC_RAM", _ram);
  _gbc.load(file, "MBC_RAMENABLE", _ramEnable);
  _gbc.load(file, "MBC_RAMBANK", _ramBank);
  _gbc.load(file, "MBC_RAMSAVE", _ramSave);
  _gbc.load(file, "MBC_RAMSAVED", _ramSaved);
}