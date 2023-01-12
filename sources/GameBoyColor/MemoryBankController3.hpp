#pragma once

#include <string>

#include "GameBoyColor/MemoryBankController.hpp"

namespace GBC
{
  class MemoryBankController3 : public GBC::MemoryBankController
  {
  private:
    const std::string _rtcSave;       // RTC register save file
    std::uint64_t     _rtcSaveTime;   // Real time to save
    std::uint64_t     _rtcSaveClock;  // RTC clock to save
    std::uint64_t     _rtcTime;       // Last real time updated
    std::uint64_t     _rtcClock;      // Internal RTC clock
    std::uint64_t     _rtcRegister;   // Latched Real Time Clock registers
    std::uint8_t      _rtcLatch;      // Latch Clock Data
    std::uint8_t      _rtcHalt;       // Stop flag of clock (0=Active, 1=Stop Timer)
    
    std::uint64_t updateRtc();  // Update RTC
    void          loadRtc();    // Load RTC from save file
    void          saveRtc();    // Save RTC to save file

  public:
    MemoryBankController3(GBC::GameBoyColor& gbc, const std::vector<std::uint8_t>& rom, std::size_t ramSize = 0, const std::string& ramSave = "", const std::string& rtcSave = "");
    virtual ~MemoryBankController3() override;

    virtual std::uint8_t  readRam(std::uint16_t address) const override;  // Read RAM

    virtual void  writeRom(std::uint16_t address, std::uint8_t value) override; // Write to MBC3 registers
    virtual void  writeRam(std::uint16_t address, std::uint8_t value) override; // Write to MBC3 RAM or RTC register

    virtual void  save(std::ofstream& file) const override; // Save state to file
    virtual void  load(std::ifstream& file) override;       // Load state from file
  };
}