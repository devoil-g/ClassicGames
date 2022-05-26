#include <algorithm>
#include <stdexcept>

#include "GameBoyColor/MemoryBankController1.hpp"

GBC::MemoryBankController1::MemoryBankController1(const std::vector<std::uint8_t>& rom, std::size_t ramSize, const std::string& ramSave) :
  GBC::MemoryBankController(rom, ramSize, ramSave),
  _bank(0)
{}

void  GBC::MemoryBankController1::writeRom(std::uint16_t address, std::uint8_t value)
{
  // Out of bound address
  if (address >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // RAM enable
  if (address < 0x2000)
    setRamEnable((value & 0b00001111) == 0b00001010);

  // ROM/RAM bank register
  else
  {
    // ROM bank number
    if (address < 0x4000)
      _bank = (_bank & 0b11100000) | ((value & 0b00011111) << 0);

    // RAM/ROM upper bits 5-6
    else if (address < 0x6000)
      _bank = (_bank & 0b10011111) | ((value & 0b00000011) << 5);

    // Banking mode
    else
      _bank = (_bank & 0b01111111) | ((value & 0b00000001) << 7);

    // Set ROM banks
    setRomBanks(
      (_bank & 0b10000000) ? (_bank & 0b01100000) : (0),
      (_bank & 0b01100000) + std::max(_bank & 0b00011111, 1)
    );

    // Set RAM bank
    setRamBank((_bank & 0b10000000) ? ((_bank & 0b01100000) >> 5) : (0));
  }
}