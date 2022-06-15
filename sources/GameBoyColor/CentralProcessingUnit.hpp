#pragma once

#include <cstdint>
#include <functional>
#include <queue>

namespace GBC
{
  class GameBoyColor;

  class CentralProcessingUnit
  {
  private:
    union Register
    {
      enum Flag
      {
        Z = 0b10000000, // Zero, set when the result of a math operation is zero or two values match when using CP
        N = 0b01000000, // Subtract, set if a subtraction was performed in the last math instruction
        H = 0b00100000, // Half carry, set if a carry occurred from the lower nibble in the last math operation
        C = 0b00010000  // Carry, set if a carry occurred from the last math operation or if register A is the smaller value when executing the CP instruction
      };

      std::uint16_t u16;  // Unsigned 16 bits value
      std::int16_t  s16;  // Signed 16 bits value

      struct
      {
        std::uint8_t  low;  // Low unsigned 8 bits value
        std::uint8_t  high; // High unsigned 8 bits value
      } u8;

      struct
      {
        std::int8_t low;  // Low signed 8 bits value
        std::int8_t high; // High signed 8 bits value
      } s8;
    };

    enum Status
    {
      StatusRun,              // Running CPU instructions
      StatusHalt,             // Stop CPU waiting for events
      StatusStop = StatusHalt // Same as Halt, turn-off display (NOTE: handled as a simple HALT)
    };

    enum InterruptMasterEnable
    {
      IMEDisabled,  // Interrupt disabled
      IMEEnabled,   // Interrupt enabled
      IMEScheduled  // Enable interrupt after next instruction
    };

    GBC::GameBoyColor&    _gbc;     // Main GBC reference for memory bus
    Status                _status;  // CPU running status
    InterruptMasterEnable _ime;     // Interrupt Master Enable
    
    using Instruction = std::function<void(CentralProcessingUnit&)>;
    using Instructions = std::queue<Instruction>;

    Instructions  _instructions;  // Steps of currently executed instruction

    Register  _rAF; // AF register (accumulator + flags)
    Register  _rBC; // BC register
    Register  _rDE; // DE register
    Register  _rHL; // HL register
    Register  _rSP; // SP register (stack pointer)
    Register  _rPC; // PC register (program counter / pointer)
    Register  _rW;  // Internal Work register (not accessible)

    static const std::array<std::function<Instructions(CentralProcessingUnit&)>, 256> _opcodes;   // CPU instruction set
    static const std::array<std::function<Instructions(CentralProcessingUnit&)>, 256> _opcodesCb; // CPU CB sub-instruction set

    template<Register::Flag F>
    void  setFlag(bool value) // Set flag bit in register AF
    {
      // Set flag bit
      if (value)
        _rAF.u8.low |= F;

      // Unset flag bit
      else
        _rAF.u8.low &= ~F;
    }

    template<Register::Flag F>
    bool  getFlag() // Get flag bit from register AF
    {
      // Get flag bit
      return (_rAF.u8.low & F) ? true : false;
    }

    Instructions  instruction_NOP();
    Instructions  instruction_STOP();
    Instructions  instruction_HALT();
    Instructions  instruction_DI();
    Instructions  instruction_EI();
    
    Instructions  instruction_JR_n();
    Instructions  instruction_JR_c0_n(Register::Flag flag);
    Instructions  instruction_JR_c1_n(Register::Flag flag);
    Instructions  instruction_JP_nn();
    Instructions  instruction_JP_c0_nn(Register::Flag flag);
    Instructions  instruction_JP_c1_nn(Register::Flag flag);
    Instructions  instruction_JP_HL();
    Instructions  instruction_CALL_nn();
    Instructions  instruction_CALL_c0_nn(Register::Flag flag);
    Instructions  instruction_CALL_c1_nn(Register::Flag flag);
    Instructions  instruction_RST(std::uint16_t address);
    Instructions  instruction_RET();
    Instructions  instruction_RET_c0(Register::Flag flag);
    Instructions  instruction_RET_c1(Register::Flag flag);
    Instructions  instruction_RETI();

    Instructions  instruction_INC_r(std::uint8_t& reg8);
    Instructions  instruction_DEC_r(std::uint8_t& reg8);
    Instructions  instruction_INC_rr(Register& reg16);
    Instructions  instruction_DEC_rr(Register& reg16);
    Instructions  instruction_INC_pHL();
    Instructions  instruction_DEC_pHL();

    Instructions  instruction_CB();
    Instructions  instruction_RLC(std::uint8_t& reg8);
    Instructions  instruction_RLC_pHL();
    Instructions  instruction_RLCA();
    Instructions  instruction_RRC(std::uint8_t& reg8);
    Instructions  instruction_RRC_pHL();
    Instructions  instruction_RRCA();
    Instructions  instruction_RL(std::uint8_t& reg8);
    Instructions  instruction_RL_pHL();
    Instructions  instruction_RLA();
    Instructions  instruction_RR(std::uint8_t& reg8);
    Instructions  instruction_RR_pHL();
    Instructions  instruction_RRA();
    Instructions  instruction_SLA(std::uint8_t& reg8);
    Instructions  instruction_SLA_pHL();
    Instructions  instruction_SRA(std::uint8_t& reg8);
    Instructions  instruction_SRA_pHL();
    Instructions  instruction_SWAP(std::uint8_t& reg8);
    Instructions  instruction_SWAP_pHL();
    Instructions  instruction_SRL(std::uint8_t& reg8);
    Instructions  instruction_SRL_pHL();
    Instructions  instruction_DAA();
    Instructions  instruction_CPL();
    Instructions  instruction_SCF();
    Instructions  instruction_CCF();

    template <unsigned int Bit>
    Instructions  instruction_BIT(std::uint8_t reg8)
    {
      return GBC::CentralProcessingUnit::Instructions({
        [=](GBC::CentralProcessingUnit& cpu) {
          setFlag<Register::Z>(!(reg8 & (0b00000001 << Bit)));
          setFlag<Register::N>(false);
          setFlag<Register::H>(true);
        }
      });
    }

    template <unsigned int Bit>
    Instructions  instruction_BIT_pHL()
    {
      return GBC::CentralProcessingUnit::Instructions({
        [](GBC::CentralProcessingUnit& cpu) {
          cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
        },
        [=](GBC::CentralProcessingUnit& cpu) {
          setFlag<Register::Z>(!(cpu._rW.u8.low & (0b00000001 << Bit)));
          setFlag<Register::N>(false);
          setFlag<Register::H>(true);
        },
        [](GBC::CentralProcessingUnit& cpu) {}
      });
    }

    template <unsigned int Bit>
    Instructions  instruction_RES(std::uint8_t& reg8)
    {
      return GBC::CentralProcessingUnit::Instructions({
        [&](GBC::CentralProcessingUnit& cpu) {
          reg8 &= ~(0b00000001 << Bit);
        }
      });
    }

    template <unsigned int Bit>
    Instructions  instruction_RES_pHL()
    {
      return GBC::CentralProcessingUnit::Instructions({
        [](GBC::CentralProcessingUnit& cpu) {
          cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
        },
        [](GBC::CentralProcessingUnit& cpu) {
          cpu._rW.u8.low &= ~(0b00000001 << Bit);
          cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
        },
        [](GBC::CentralProcessingUnit& cpu) {}
      });
    }

    template <unsigned int Bit>
    Instructions  instruction_SET(std::uint8_t& reg8)
    {
      return GBC::CentralProcessingUnit::Instructions({
        [&](GBC::CentralProcessingUnit& cpu) {
          reg8 |= 0b00000001 << Bit;
        }
      });
    }

    template <unsigned int Bit>
    Instructions  instruction_SET_pHL()
    {
      return GBC::CentralProcessingUnit::Instructions({
        [](GBC::CentralProcessingUnit& cpu) {
          cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
        },
        [](GBC::CentralProcessingUnit& cpu) {
          cpu._rW.u8.low |= 0b00000001 << Bit;
          cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
        },
        [](GBC::CentralProcessingUnit& cpu) {}
      });
    }

    Instructions  instruction_ADD_r(std::uint8_t reg8);
    Instructions  instruction_ADD_n();
    Instructions  instruction_ADD_pHL();
    Instructions  instruction_ADC_r(std::uint8_t reg8);
    Instructions  instruction_ADC_n();
    Instructions  instruction_ADC_pHL();
    Instructions  instruction_SUB_r(std::uint8_t reg8);
    Instructions  instruction_SUB_n();
    Instructions  instruction_SUB_pHL();
    Instructions  instruction_SBC_r(std::uint8_t reg8);
    Instructions  instruction_SBC_n();
    Instructions  instruction_SBC_pHL();
    Instructions  instruction_AND_r(std::uint8_t reg8);
    Instructions  instruction_AND_n();
    Instructions  instruction_AND_pHL();
    Instructions  instruction_XOR_r(std::uint8_t reg8);
    Instructions  instruction_XOR_n();
    Instructions  instruction_XOR_pHL();
    Instructions  instruction_OR_r(std::uint8_t reg8);
    Instructions  instruction_OR_n();
    Instructions  instruction_OR_pHL();
    Instructions  instruction_CP_r(std::uint8_t reg8);
    Instructions  instruction_CP_n();
    Instructions  instruction_CP_pHL();

    Instructions  instruction_LD_r_r(std::uint8_t& reg8_destination, std::uint8_t reg8_source);
    Instructions  instruction_LD_r_n(std::uint8_t& reg8);
    Instructions  instruction_LD_r_pHL(std::uint8_t& reg8);
    Instructions  instruction_LD_pHL_r(std::uint8_t reg8);
    Instructions  instruction_LD_pHL_n();
    Instructions  instruction_LD_A_prr(Register reg16);
    Instructions  instruction_LD_prr_A(Register reg16);
    Instructions  instruction_LD_A_pnn();
    Instructions  instruction_LD_pnn_A();
    Instructions  instruction_LDH_A_pC();
    Instructions  instruction_LDH_pC_A();
    Instructions  instruction_LDH_A_pn();
    Instructions  instruction_LDH_pn_A();
    Instructions  instruction_LD_A_HLd();
    Instructions  instruction_LD_HLd_A();
    Instructions  instruction_LD_A_HLi();
    Instructions  instruction_LD_HLi_A();

    Instructions  instruction_ADD_HL_rr(Register reg16);
    Instructions  instruction_ADD_SP_n();
    Instructions  instruction_LD_rr_nn(Register& reg16);
    Instructions  instruction_LD_pnn_SP();
    Instructions  instruction_LD_HL_SPn();
    Instructions  instruction_LD_SP_HL();
    Instructions  instruction_PUSH_rr(Register reg16);
    Instructions  instruction_POP_rr(Register& reg16);
    Instructions  instruction_POP_AF();

    void  simulateFetch();      // Fetch next instruction or interrupt to execute
    void  simulateInterrupt();  // CPU interrupt handling

  public:
    CentralProcessingUnit(GBC::GameBoyColor& gbc);
    ~CentralProcessingUnit() = default;

    void  simulate(); // Simulate 4 clock ticks / 1 CPU tick of the CPU
  };
}