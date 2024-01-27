#include <algorithm>
#include <array>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "GameBoyColor/MemoryBankController3.hpp"
#include "GameBoyColor/CentralProcessingUnit.hpp"
#include "GameBoyColor/GameBoyColor.hpp"

GBC::MemoryBankController3::MemoryBankController3(GBC::GameBoyColor& gbc, const std::vector<std::uint8_t>& rom, std::size_t ramSize, const std::filesystem::path& ramSave, const std::filesystem::path& rtcSave) :
  GBC::MemoryBankController(gbc, rom, ramSize, ramSave),
  _rtcSave(rtcSave),
  _rtcTime(0),
  _rtcClock(0),
  _rtcRegister(0),
  _rtcLatch(0),
  _rtcHalt(0)
{
  // Load initial RTC
  loadRtc();
}

GBC::MemoryBankController3::~MemoryBankController3()
{
  // Save final RTC
  saveRtc();
}

std::uint8_t  GBC::MemoryBankController3::readRam(std::uint16_t address) const
{
#ifdef _DEBUG
  // Out of bound address
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // RAM / RTC registers not enabled
  if (getRamEnable() == false)
    return 0xFF;

  // RAM bank
  else if (getRamBank() < 0x04)
    return GBC::MemoryBankController::readRam(address);

  // RTC seconds
  else if (getRamBank() == 0x08)
    return _rtcRegister / (1) % 60;

  // RTC minutes
  else if (getRamBank() == 0x09)
    return _rtcRegister / (60) % 60;

  // RTC hours
  else if (getRamBank() == 0x0A)
    return _rtcRegister / (60 * 60) % 60;

  // RTC days (lower 8 bits)
  else if (getRamBank() == 0x0B)
    return (_rtcRegister / (60 * 60 * 24)) & 0b11111111;

  // RTC days (9th bits, halt flag, day carry bit)
  else if (getRamBank() == 0x0C)
    return (((_rtcRegister / (60 * 60 * 24)) >> 9) & 0b00000001)
    | ((_rtcHalt == 1) ? 0b01000000 : 0b00000000)
    | ((_rtcRegister / (60 * 60 * 24 * 512) > 0) ? 0b10000000 : 0b00000000);

  // Unknow bank
  else
    return 0xFF;
}

void  GBC::MemoryBankController3::writeRom(std::uint16_t address, std::uint8_t value)
{
#ifdef _DEBUG
  // Out of bound address
  if (address >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // RAM enable
  if (address < 0x2000)
    setRamEnable((value & 0b00001111) == 0b00001010);

  // ROM bank number
  else if (address < 0x4000)
    setRomBanks(0, std::max(value & 0b01111111, 1));

  // RAM bank number / RTC register select
  else if (address < 0x6000)
    setRamBank(value);

  // Latch Clock Data
  else {
    if (_rtcLatch == 0x00 && value == 0x01)
      _rtcRegister = _rtcClock;
    _rtcLatch = value;
  }
}

void  GBC::MemoryBankController3::writeRam(std::uint16_t address, std::uint8_t value)
{
#ifdef _DEBUG
  // Out of bound address
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Cancel if disabled
  if (getRamEnable() == false)
    return;

  // Write to RAM
  if (getRamBank() < 0x04)
    GBC::MemoryBankController::writeRam(address, value);

  // RTC seconds
  else if (getRamBank() == 0x08) {
    if (_rtcHalt == 1) {
      _rtcClock = _rtcClock - (_rtcClock / (1) % 60 * (1)) + value * (1);
      _rtcRegister = _rtcClock;
    }
  }
  // RTC minutes
  else if (getRamBank() == 0x09) {
    if (_rtcHalt == 1) {
      _rtcClock = _rtcClock - (_rtcClock / (60) % 60 * (60)) + value * (60);
      _rtcRegister = _rtcClock;
    }
  }
  // RTC hours
  else if (getRamBank() == 0x0A) {
    if (_rtcHalt == 1) {
      _rtcClock = _rtcClock - (_rtcClock / (60 * 60) % 24 * (60 * 60)) + value * (60 * 60);
      _rtcRegister = _rtcClock;
    }
  }
  // RTC days
  else if (getRamBank() == 0x0B) {
    if (_rtcHalt == 1) {
      _rtcClock = _rtcClock - (_rtcClock / (60 * 60 * 24) % 256 * (60 * 60 * 24)) + value * (60 * 60 * 24);
      _rtcRegister = _rtcClock;
    }
  }
  // RTC days, halt flag and day carry
  else if (getRamBank() == 0x0C) {
    if (_rtcHalt == 1) {
      _rtcClock = _rtcClock - (_rtcClock / (60 * 60 * 24 * 256) % 512 * (60 * 60 * 24 * 256)) + (value & 0b00000001) * (60 * 60 * 24 * 256);
      _rtcClock = _rtcClock % (60 * 60 * 24 * 512) + ((value & 0b10000000) ? 1 : 0) * (60 * 60 * 24 * 512);
      _rtcRegister = _rtcClock;
    }
    _rtcHalt = (value & 0b01000000) ? 1 : 0;
  }
}

void  GBC::MemoryBankController3::loadRtc()
{
  // No save file
  if (_rtcSave.empty() == true)
    return;

  std::ifstream file(_rtcSave, std::ifstream::binary);

  // Check if file open properly
  if (file.good() == false) {
    std::cerr << "[GBC::MBC3] Warning, failed to load '" << _rtcSave << "'." << std::endl;
    return;
  }

  std::array<std::uint64_t, 3>  data = { 0, 0, 0  };
  
  // Check compatible size
  if (std::filesystem::file_size(_rtcSave) != sizeof(data)) {
    std::cerr << "[GBC::MBC3] Warning, invalid save size '" << _rtcSave << "'." << std::endl;
    return;
  }

  // Load ROM
  file.read((char*)data.data(), sizeof(data));

  // Check for error
  if (file.gcount() != sizeof(data)) {
    std::cerr << "[GBC::MBC3] Warning, invalid read size of '" << _rtcSave << "'." << std::endl;
    return;
  }

  // Copy RTC
  _rtcTime = data[0];
  _rtcClock = data[1];

  std::size_t saved_clock = data[2];
  std::size_t real_clock = std::time(nullptr);

  // Update internal clock from real time
  if (real_clock >= saved_clock)
    update((real_clock - saved_clock) * GBC::CentralProcessingUnit::Frequency);
  else
    std::cerr << "[GBC::MBC3] Warning, clock skew detected." << std::endl;
}

void  GBC::MemoryBankController3::saveRtc()
{
  // No save file
  if (_rtcSave.empty() == true)
    return;

  std::ofstream file(_rtcSave, std::ofstream::binary | std::ofstream::trunc);

  // Check if file open properly
  if (file.good() == false) {
    std::cerr << "[GBC::MBC3] Warning, failed to open '" << _rtcSave << "'." << std::endl;
    return;
  }

  std::array<std::uint64_t, 3>  data = { _rtcTime, _rtcClock, (std::size_t)std::time(nullptr) };

  // Write External RAM to save file
  file.write((const char*)data.data(), sizeof(data));

  // Check for success
  if (file.good() == false) {
    std::cerr << "[GBC::MBC] Warning, failed to write '" << _rtcSave << "'." << std::endl;
    return;
  }
}

void    GBC::MemoryBankController3::save(std::ofstream& file) const
{
  // Save MBC variables
  GBC::MemoryBankController::save(file);

  // Save MBC3 variables
  _gbc.save(file, "MBC3_RTCTIME", _rtcTime);
  _gbc.save(file, "MBC3_RTCCLOCK", _rtcClock);
  _gbc.save(file, "MBC3_RTCREGISTER", _rtcRegister);
  _gbc.save(file, "MBC3_RTCLATCH", _rtcLatch);
  _gbc.save(file, "MBC3_RTCHALT", _rtcHalt);
}

void    GBC::MemoryBankController3::load(std::ifstream& file)
{
  // Load MBC variables
  GBC::MemoryBankController::load(file);

  // Load MBC3 variables
  _gbc.load(file, "MBC3_RTCTIME", _rtcTime);
  _gbc.load(file, "MBC3_RTCCLOCK", _rtcClock);
  _gbc.load(file, "MBC3_RTCREGISTER", _rtcRegister);
  _gbc.load(file, "MBC3_RTCLATCH", _rtcLatch);
  _gbc.load(file, "MBC3_RTCHALT", _rtcHalt);
}

void    GBC::MemoryBankController3::update(std::size_t ticks)
{
  std::uint64_t clock = _rtcTime + ticks;

  // At least a second passed
  if (_rtcHalt == 0 && clock / GBC::CentralProcessingUnit::Frequency > _rtcTime / GBC::CentralProcessingUnit::Frequency)
    _rtcClock += clock / GBC::CentralProcessingUnit::Frequency - _rtcTime / GBC::CentralProcessingUnit::Frequency;

  // Save current time
  _rtcTime = clock;
}