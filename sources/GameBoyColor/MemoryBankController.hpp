#pragma once

#include <string>
#include <vector>

namespace GBC
{
  class GameBoyColor;

  class MemoryBankController
  {
  protected:
    GBC::GameBoyColor& _gbc;  // Main GBC reference for save states

  private:
    const std::vector<std::uint8_t> _rom;       // Raw ROM memory
    std::size_t                     _romBank0;  // ROM bank of first half of address range
    std::size_t                     _romBank1;  // ROM bank of second half of address range
    std::vector<std::uint8_t>       _ram;       // Raw RAM memory
    bool                            _ramEnable; // RAM enable flag
    std::size_t                     _ramBank;   // RAM bank
    const std::string               _ramSave;   // Path of RAM save files, empty if no battery
    std::vector<std::uint8_t>       _ramSaved;  // Raw RAM to save

    void  loadRam();        // Load MBC RAM from save file
    void  saveRam() const;  // Save MBC RAM to save file

  protected:
    std::pair<std::size_t, std::size_t> getRomBanks() const;  // Get ROM banks
    std::size_t                         getRamBank() const;   // Get RAM bank

    void  setRomBanks(std::size_t bank0, std::size_t bank1);  // Set ROM banks of first and second half of address range
    void  setRamBank(std::size_t bank);                       // Set RAM bank

    bool  getRamEnable() const;       // Get RAM enable flag
    void  setRamEnable(bool enable);  // Enable/disable RAM

  public:
    MemoryBankController(GBC::GameBoyColor& gbc, const std::vector<std::uint8_t>& rom, std::size_t ramSize = 0, const std::string& ramSave = "");
    virtual ~MemoryBankController();

    virtual std::uint8_t  readRom(std::uint16_t address) const; // Read ROM
    virtual std::uint8_t  readRam(std::uint16_t address) const; // Read RAM

    virtual void  writeRom(std::uint16_t address, std::uint8_t value);  // Write to MBC registers
    virtual void  writeRam(std::uint16_t address, std::uint8_t value);  // Write to RAM

    virtual void  save(std::ofstream& file) const;  // Save state to file
    virtual void  load(std::ifstream& file);        // Load state from file
  };
}