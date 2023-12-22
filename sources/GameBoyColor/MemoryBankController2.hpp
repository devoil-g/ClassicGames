#pragma once

#include <string>

#include "GameBoyColor/MemoryBankController.hpp"

namespace GBC
{
  class MemoryBankController2 : public GBC::MemoryBankController
  {
  private:
    static const std::size_t  RamSize = 512;  // MBC2 always has 512 x 4 bits RAM

  public:
    MemoryBankController2(GBC::GameBoyColor& gbc, const std::vector<std::uint8_t>& rom, const std::filesystem::path& ramSave = "");
    virtual ~MemoryBankController2() override = default;

    virtual void  writeRom(std::uint16_t address, std::uint8_t value) override; // Write to MBC2 registers
  };
}