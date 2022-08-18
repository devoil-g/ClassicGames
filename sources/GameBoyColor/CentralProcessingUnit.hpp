#pragma once

#include <array>
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

    class Instructions
    {
    public:
      using Instruction = void(*)(GBC::CentralProcessingUnit&);

    private:
      std::array<Instruction, 8>  _queue; // Pointer to instructions
      std::size_t                 _size;  // Size of the stack
      std::size_t                 _index; // Current position in stack

    public:
      Instructions();
      ~Instructions() = default;

      bool        empty() const;  // Return true if instruction stack is empty
      std::size_t size() const;   // Get number of instructions remaining
      Instruction front() const;    // Get next instruction

      void  pop();                          // Pop next instruction and returns it
      void  push(Instruction instruction);  // Push new instruction in the queue
    };

    union Parameter
    {
      std::uint8_t    u8;
      std::uint8_t*   pu8;
      std::uint16_t   u16;
      std::uint16_t*  pu16;
      Register        r;
      Register*       pr;
      
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
    
    Instructions              _instructions;  // Steps of currently executed instruction
    std::array<Parameter, 2>  _parameters;    // Parameters of instructions

    Register  _rAF; // AF register (accumulator + flags)
    Register  _rBC; // BC register
    Register  _rDE; // DE register
    Register  _rHL; // HL register
    Register  _rSP; // SP register (stack pointer)
    Register  _rPC; // PC register (program counter / pointer)
    Register  _rW;  // Internal Work register (not accessible)

    using Opcode = void(*)(GBC::CentralProcessingUnit&);

    static const std::array<Opcode, 256> _opcodes;    // CPU instruction set
    static const std::array<Opcode, 256> _opcodesCb;  // CPU CB sub-instruction set

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

    void  instruction_NOP();
    void  instruction_STOP();
    void  instruction_HALT();
    void  instruction_DI();
    void  instruction_EI();
    
    void  instruction_JR_n();
    void  instruction_JR_c0_n(Register::Flag flag);
    void  instruction_JR_c1_n(Register::Flag flag);
    void  instruction_JP_nn();
    void  instruction_JP_c0_nn(Register::Flag flag);
    void  instruction_JP_c1_nn(Register::Flag flag);
    void  instruction_JP_HL();
    void  instruction_CALL_nn();
    void  instruction_CALL_c0_nn(Register::Flag flag);
    void  instruction_CALL_c1_nn(Register::Flag flag);
    void  instruction_RST(std::uint16_t address);
    void  instruction_RET();
    void  instruction_RET_c0(Register::Flag flag);
    void  instruction_RET_c1(Register::Flag flag);
    void  instruction_RETI();

    void  instruction_INC_r(std::uint8_t& reg8);
    void  instruction_DEC_r(std::uint8_t& reg8);
    void  instruction_INC_rr(Register& reg16);
    void  instruction_DEC_rr(Register& reg16);
    void  instruction_INC_pHL();
    void  instruction_DEC_pHL();

    void  instruction_CB();
    void  instruction_RLC(std::uint8_t& reg8);
    void  instruction_RLC_pHL();
    void  instruction_RLCA();
    void  instruction_RRC(std::uint8_t& reg8);
    void  instruction_RRC_pHL();
    void  instruction_RRCA();
    void  instruction_RL(std::uint8_t& reg8);
    void  instruction_RL_pHL();
    void  instruction_RLA();
    void  instruction_RR(std::uint8_t& reg8);
    void  instruction_RR_pHL();
    void  instruction_RRA();
    void  instruction_SLA(std::uint8_t& reg8);
    void  instruction_SLA_pHL();
    void  instruction_SRA(std::uint8_t& reg8);
    void  instruction_SRA_pHL();
    void  instruction_SWAP(std::uint8_t& reg8);
    void  instruction_SWAP_pHL();
    void  instruction_SRL(std::uint8_t& reg8);
    void  instruction_SRL_pHL();
    void  instruction_DAA();
    void  instruction_CPL();
    void  instruction_SCF();
    void  instruction_CCF();

    template <unsigned int Bit>
    void  instruction_BIT(std::uint8_t reg8)
    {
      _parameters[0].u8 = reg8;
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {
        cpu.setFlag<Register::Z>(!(cpu._parameters[0].u8 & (0b00000001 << Bit)));
        cpu.setFlag<Register::N>(false);
        cpu.setFlag<Register::H>(true);
        });
    }

    template <unsigned int Bit>
    void  instruction_BIT_pHL()
    {
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {
        cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
        });
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {
        cpu.setFlag<Register::Z>(!(cpu._rW.u8.low & (0b00000001 << Bit)));
        cpu.setFlag<Register::N>(false);
        cpu.setFlag<Register::H>(true);
        });
    }

    template <unsigned int Bit>
    void  instruction_RES(std::uint8_t& reg8)
    {
      _parameters[0].pu8 = &reg8;
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {
        (*cpu._parameters[0].pu8) &= ~(0b00000001 << Bit);
        });
    }

    template <unsigned int Bit>
    void  instruction_RES_pHL()
    {
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {
        cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
        });
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {
        cpu._rW.u8.low &= ~(0b00000001 << Bit);
        cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
        });
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
    }

    template <unsigned int Bit>
    void  instruction_SET(std::uint8_t& reg8)
    {
      _parameters[0].pu8 = &reg8;
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {
        (*cpu._parameters[0].pu8) |= 0b00000001 << Bit;
        });
    }

    template <unsigned int Bit>
    void  instruction_SET_pHL()
    {
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {
        cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
        });
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {
        cpu._rW.u8.low |= 0b00000001 << Bit;
        cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
        });
      _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
    }

    void  instruction_ADD_r(std::uint8_t reg8);
    void  instruction_ADD_n();
    void  instruction_ADD_pHL();
    void  instruction_ADC_r(std::uint8_t reg8);
    void  instruction_ADC_n();
    void  instruction_ADC_pHL();
    void  instruction_SUB_r(std::uint8_t reg8);
    void  instruction_SUB_n();
    void  instruction_SUB_pHL();
    void  instruction_SBC_r(std::uint8_t reg8);
    void  instruction_SBC_n();
    void  instruction_SBC_pHL();
    void  instruction_AND_r(std::uint8_t reg8);
    void  instruction_AND_n();
    void  instruction_AND_pHL();
    void  instruction_XOR_r(std::uint8_t reg8);
    void  instruction_XOR_n();
    void  instruction_XOR_pHL();
    void  instruction_OR_r(std::uint8_t reg8);
    void  instruction_OR_n();
    void  instruction_OR_pHL();
    void  instruction_CP_r(std::uint8_t reg8);
    void  instruction_CP_n();
    void  instruction_CP_pHL();

    void  instruction_LD_r_r(std::uint8_t& reg8_destination, std::uint8_t reg8_source);
    void  instruction_LD_r_n(std::uint8_t& reg8);
    void  instruction_LD_r_pHL(std::uint8_t& reg8);
    void  instruction_LD_pHL_r(std::uint8_t reg8);
    void  instruction_LD_pHL_n();
    void  instruction_LD_A_prr(Register reg16);
    void  instruction_LD_prr_A(Register reg16);
    void  instruction_LD_A_pnn();
    void  instruction_LD_pnn_A();
    void  instruction_LDH_A_pC();
    void  instruction_LDH_pC_A();
    void  instruction_LDH_A_pn();
    void  instruction_LDH_pn_A();
    void  instruction_LD_A_HLd();
    void  instruction_LD_HLd_A();
    void  instruction_LD_A_HLi();
    void  instruction_LD_HLi_A();

    void  instruction_ADD_HL_rr(Register reg16);
    void  instruction_ADD_SP_n();
    void  instruction_LD_rr_nn(Register& reg16);
    void  instruction_LD_pnn_SP();
    void  instruction_LD_HL_SPn();
    void  instruction_LD_SP_HL();
    void  instruction_PUSH_rr(Register reg16);
    void  instruction_POP_rr(Register& reg16);
    void  instruction_POP_AF();

    void  simulateFetch();      // Fetch next instruction or interrupt to execute
    void  simulateInterrupt();  // CPU interrupt handling

  public:
    CentralProcessingUnit(GBC::GameBoyColor& gbc);
    ~CentralProcessingUnit() = default;

    void  simulate(); // Simulate 4 clock ticks / 1 CPU tick of the CPU
  };
}