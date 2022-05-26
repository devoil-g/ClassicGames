#pragma once

#include <string>

#include "GameBoyColor/MemoryBankController.hpp"

namespace GBC
{
  class MemoryBankController5 : public GBC::MemoryBankController
  {
  public:
    MemoryBankController5(const std::vector<std::uint8_t>& rom, std::size_t ramSize = 0, const std::string& ramSave = "");
    virtual ~MemoryBankController5() override = default;

    virtual void  writeRom(std::uint16_t address, std::uint8_t value) override; // Write to MBC2 registers
  };
}