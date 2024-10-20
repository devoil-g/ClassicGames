#pragma once

#include <string>

#include "GameBoyColor/MemoryBankController.hpp"

namespace GBC
{
  class GameBoyColor;

  class MemoryBankController1 : public GBC::MemoryBankController
  {
  private:
    std::uint8_t      _bank;  // RAM & ROM bank number, ROM bits 4-3-2-1-0, RAM/ROM upper bits 5-6, Mode bits 7

  public:
    MemoryBankController1(GBC::GameBoyColor& gbc, const std::vector<std::uint8_t>& rom, std::size_t ramSize = 0, const std::filesystem::path& ramSave = "");
    virtual ~MemoryBankController1() override = default;

    virtual void  writeRom(std::uint16_t address, std::uint8_t value) override; // Write to MBC1 registers

    virtual void  save(std::ofstream& file) const override; // Save state to file
    virtual void  load(std::ifstream& file) override;       // Load state from file
  };
}