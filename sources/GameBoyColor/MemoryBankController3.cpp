#include <algorithm>
#include <array>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "GameBoyColor/MemoryBankController3.hpp"

GBC::MemoryBankController3::MemoryBankController3(const std::vector<std::uint8_t>& rom, std::size_t ramSize, const std::string& ramSave, const std::string& rtcSave) :
  GBC::MemoryBankController(rom, ramSize, ramSave),
  _rtcSave(rtcSave),
  _rtcSaveTime(0),
  _rtcSaveClock(0),
  _rtcTime(std::time(nullptr)),
  _rtcClock(0),
  _rtcRegister(0),
  _rtcLatch(0),
  _rtcHalt(0)
{
  // Load initial RTC
  loadRtc();

  // Set saved values
  _rtcSaveTime = _rtcTime;
  _rtcSaveClock = _rtcClock;
}

GBC::MemoryBankController3::~MemoryBankController3()
{
  // Save final RTC
  saveRtc();
}

std::uint8_t  GBC::MemoryBankController3::readRam(std::uint16_t address) const
{
  // Out of bound address
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

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
  // Out of bound address
  if (address >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // RAM enable
  if (address < 0x2000) {
    bool enable = (value & 0b00001111) == 0b00001010;

    // Save RTC
    if (getRamEnable() == true && enable == false)
      updateRtc();

    setRamEnable((value & 0b00001111) == 0b00001010);
  }

  // ROM bank number
  else if (address < 0x4000)
    setRomBanks(0, std::max(value & 0b01111111, 1));

  // RAM bank number / RTC register select
  else if (address < 0x6000)
    setRamBank(value);

  // Latch Clock Data
  else {
    if (_rtcLatch == 0x00 && value == 0x01)
      _rtcRegister = updateRtc();
    _rtcLatch = value;
  }
}

void  GBC::MemoryBankController3::writeRam(std::uint16_t address, std::uint8_t value)
{
  // Out of bound address
  if (address >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

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
    updateRtc();
    _rtcHalt = (value & 0b01000000) ? 1 : 0;
  }
}

std::uint64_t GBC::MemoryBankController3::updateRtc()
{
  std::uint64_t clock = std::time(nullptr);

  // At least a second passed
  if (_rtcHalt == 0 && _rtcTime < clock) {
    _rtcClock += clock - _rtcTime;
    _rtcSaveClock = _rtcClock;
  }

  // Save current time
  _rtcTime = clock;
  _rtcSaveTime = _rtcTime;

  return _rtcClock;
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

  std::array<std::uint64_t, 2>  data = { 0, 0 };

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

  // Update clock since last power-up
  updateRtc();
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

  std::array<std::uint64_t, 2>  data = { _rtcSaveTime, _rtcSaveClock };

  // Write External RAM to save file
  file.write((const char*)data.data(), sizeof(data));

  // Check for success
  if (file.good() == false) {
    std::cerr << "[GBC::MBC] Warning, failed to write '" << _rtcSave << "'." << std::endl;
    return;
  }
}