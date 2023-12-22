#include <stdexcept>

#include "GameBoyColor/MemoryBankController2.hpp"

GBC::MemoryBankController2::MemoryBankController2(GBC::GameBoyColor& gbc, const std::vector<std::uint8_t>& rom, const std::filesystem::path& ramSave) :
  GBC::MemoryBankController(gbc, rom, GBC::MemoryBankController2::RamSize, ramSave)
{}

void  GBC::MemoryBankController2::writeRom(std::uint16_t address, std::uint8_t value)
{
  // Out of bound address
  if (address >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // ROM bank number
  if (address & 0b0000000100000000)
    setRomBanks(0, std::max(value & 0b00001111, 1));

  // RAM enable
  else
    setRamEnable((value & 0b00001111) == 0b00001010);
}