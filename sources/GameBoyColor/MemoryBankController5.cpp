#include <iostream>
#include <stdexcept>

#include "GameBoyColor/MemoryBankController5.hpp"

GBC::MemoryBankController5::MemoryBankController5(const std::vector<std::uint8_t>& rom, std::size_t ramSize, const std::string& ramSave) :
  GBC::MemoryBankController(rom, ramSize, ramSave)
{}

void  GBC::MemoryBankController5::writeRom(std::uint16_t address, std::uint8_t value)
{
  // Out of bound address
  if (address >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // RAM enable
  if (address < 0x2000)
    setRamEnable((value & 0b00001111) == 0b00001010);

  // 8 least significant bits of ROM bank number
  else if (address < 0x3000)
    setRomBanks(0, (getRomBanks().second & 0b0000000100000000) | ((std::size_t)(value & 0b11111111) << 0));

  // 9th bit of ROM bank number
  else if (address < 0x4000)
    setRomBanks(0, (getRomBanks().second & 0b0000000011111111) | ((std::size_t)(value & 0b00000001) << 8));

  // RAM bank number
  else if (address < 0x6000)
    setRamBank(value & 0b00001111);

  // Invalid address
  else
    std::cerr << "[GBC::MBC5] Warning, invalid write to MBC5 register." << std::endl;
}