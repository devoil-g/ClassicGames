#include <stdexcept>

#include "GameBoyColor/CentralProcessingUnit.hpp"
#include "GameBoyColor/GameBoyColor.hpp"

constexpr const std::array<GBC::CentralProcessingUnit::Opcode, 256> GBC::CentralProcessingUnit::_opcodes = {
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },                     // 0x00, 0b00000000: NOP
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_rr_nn(cpu._rBC); },        // 0x01, 0b00000001: LD BC, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_prr_A(cpu._rBC); },        // 0x02, 0b00000010: LD (BC), A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_rr(cpu._rBC); },          // 0x03, 0b00000011: INC BC
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_r(cpu._rBC.u8.high); },   // 0x04, 0b00000100: INC B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_r(cpu._rBC.u8.high); },   // 0x05, 0b00000101: DEC B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_n(cpu._rBC.u8.high); },  // 0x06, 0b00000110: LD B, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLCA(); },                    // 0x07, 0b00000111: RLCA
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pnn_SP(); },               // 0x08, 0b00001000: LD (nn), SP
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_HL_rr(cpu._rBC); },       // 0x09, 0b00001001: ADD HL, BC
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_A_prr(cpu._rBC); },        // 0x0A, 0b00001010: LD A, (BC)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_rr(cpu._rBC); },          // 0x0B, 0b00001011: DEC BC
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_r(cpu._rBC.u8.low); },    // 0x0C, 0b00001100: INC C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_r(cpu._rBC.u8.low); },    // 0x0D, 0b00001101: DEC C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_n(cpu._rBC.u8.low); },   // 0x0E, 0b00001110: LD C, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRCA(); },                    // 0x0F, 0b00001111: RRCA

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_STOP(); },                    // 0x10, 0b00010000: STOP (0x10 & 0x00)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_rr_nn(cpu._rDE); },        // 0x11, 0b00010001: LD DE, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_prr_A(cpu._rDE); },        // 0x12, 0b00010010: LD (DE), A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_rr(cpu._rDE); },          // 0x13, 0b00010011: INC DE
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_r(cpu._rDE.u8.high); },   // 0x14, 0b00010100: INC D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_r(cpu._rDE.u8.high); },   // 0x15, 0b00010101: DEC D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_n(cpu._rDE.u8.high); },  // 0x16, 0b00010110: LD D, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLA(); },                     // 0x17, 0b00010111: RLA
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JR_n(); },                    // 0x18, 0b00011000: JR n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_HL_rr(cpu._rDE); },       // 0x19, 0b00011001: ADD HL, DE
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_A_prr(cpu._rDE); },        // 0x1A, 0b00011010: LD A, (DE)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_rr(cpu._rDE); },          // 0x1B, 0b00011011: DEC DE
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_r(cpu._rDE.u8.low); },    // 0x1C, 0b00011100: INC E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_r(cpu._rDE.u8.low); },    // 0x1D, 0b00011101: DEC E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_n(cpu._rDE.u8.low); },   // 0x1E, 0b00011110: LD E, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRA(); },                     // 0x1F, 0b00011111: RRA

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JR_c0_n(Register::Z); },      // 0x20, 0b00100000: JR NZ, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_rr_nn(cpu._rHL); },        // 0x21, 0b00100001: LD HL, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_HLi_A(); },                // 0x22, 0b00100010: LD (HL+), A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_rr(cpu._rHL); },          // 0x23, 0b00100011: INC HL
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_r(cpu._rHL.u8.high); },   // 0x24, 0b00100100: INC H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_r(cpu._rHL.u8.high); },   // 0x25, 0b00100101: DEC H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_n(cpu._rHL.u8.high); },  // 0x26, 0b00100110: LD H, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DAA(); },                     // 0x27, 0b00100111: DAA
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JR_c1_n(Register::Z); },      // 0x28, 0b00101000: JR Z, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_HL_rr(cpu._rHL); },       // 0x29, 0b00101001: ADD HL, HL
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_A_HLi(); },                // 0x2A, 0b00101010: LD A, (HL+)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_rr(cpu._rHL); },          // 0x2B, 0b00101011: DEC HL
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_r(cpu._rHL.u8.low); },    // 0x2C, 0b00101100: INC L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_r(cpu._rHL.u8.low); },    // 0x2D, 0b00101101: DEC L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_n(cpu._rHL.u8.low); },   // 0x2E, 0b00101110: LD L, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CPL(); },                     // 0x2F, 0b00101111: CPL

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JR_c0_n(Register::C); },      // 0x30, 0b00110000: JR NC, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_rr_nn(cpu._rSP); },        // 0x31, 0b00110001: LD SP, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_HLd_A(); },                // 0x32, 0b00110010: LD (HL-), A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_rr(cpu._rSP); },          // 0x33, 0b00110011: INC SP
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_pHL(); },                 // 0x34, 0b00110100: INC (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_pHL(); },                 // 0x35, 0b00110101: DEC (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pHL_n(); },                // 0x36, 0b00110110: LD (HL), n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SCF(); },                     // 0x37, 0b00110111: SCF
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JR_c1_n(Register::C); },      // 0x38, 0b00111000: JR C, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_HL_rr(cpu._rSP); },       // 0x39, 0b00111001: ADD HL, SP
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_A_HLd(); },                // 0x3A, 0b00111010: LD A, (HL-)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_rr(cpu._rSP); },          // 0x3B, 0b00111011: DEC SP
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INC_r(cpu._rAF.u8.high); },   // 0x3C, 0b00111100: INC A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DEC_r(cpu._rAF.u8.high); },   // 0x3D, 0b00111101: DEC A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_n(cpu._rAF.u8.high); },  // 0x3E, 0b00111110: LD A, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CCF(); },                     // 0x3F, 0b00111111: CCF

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.high, cpu._rBC.u8.high); },  // 0x40, 0b01000000: LD B, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.high, cpu._rBC.u8.low); },   // 0x41, 0b01000001: LD B, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.high, cpu._rDE.u8.high); },  // 0x42, 0b01000010: LD B, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.high, cpu._rDE.u8.low); },   // 0x43, 0b01000011: LD B, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.high, cpu._rHL.u8.high); },  // 0x44, 0b01000100: LD B, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.high, cpu._rHL.u8.low); },   // 0x45, 0b01000101: LD B, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_pHL(cpu._rBC.u8.high); },                  // 0x46, 0b01000110: LD B, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.high, cpu._rAF.u8.high); },  // 0x47, 0b01000111: LD B, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.low, cpu._rBC.u8.high); },   // 0x48, 0b01001000: LD C, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.low, cpu._rBC.u8.low); },    // 0x49, 0b01001001: LD C, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.low, cpu._rDE.u8.high); },   // 0x4A, 0b01001010: LD C, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.low, cpu._rDE.u8.low); },    // 0x4B, 0b01001011: LD C, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.low, cpu._rHL.u8.high); },   // 0x4C, 0b01001100: LD C, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.low, cpu._rHL.u8.low); },    // 0x4D, 0b01001101: LD C, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_pHL(cpu._rBC.u8.low); },                   // 0x4E, 0b01001110: LD C, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rBC.u8.low, cpu._rAF.u8.high); },   // 0x4F, 0b01001111: LD C, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.high, cpu._rBC.u8.high); },  // 0x50, 0b01010000: LD D, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.high, cpu._rBC.u8.low); },   // 0x51, 0b01010001: LD D, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.high, cpu._rDE.u8.high); },  // 0x52, 0b01010010: LD D, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.high, cpu._rDE.u8.low); },   // 0x53, 0b01010011: LD D, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.high, cpu._rHL.u8.high); },  // 0x54, 0b01010100: LD D, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.high, cpu._rHL.u8.low); },   // 0x55, 0b01010101: LD D, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_pHL(cpu._rDE.u8.high); },                  // 0x56, 0b01010110: LD D, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.high, cpu._rAF.u8.high); },  // 0x57, 0b01010111: LD D, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.low, cpu._rBC.u8.high); },   // 0x58, 0b01011000: LD E, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.low, cpu._rBC.u8.low); },    // 0x59, 0b01011001: LD E, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.low, cpu._rDE.u8.high); },   // 0x5A, 0b01011010: LD E, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.low, cpu._rDE.u8.low); },    // 0x5B, 0b01011011: LD E, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.low, cpu._rHL.u8.high); },   // 0x5C, 0b01011100: LD E, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.low, cpu._rHL.u8.low); },    // 0x5D, 0b01011101: LD E, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_pHL(cpu._rDE.u8.low); },                   // 0x5E, 0b01011110: LD E, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rDE.u8.low, cpu._rAF.u8.high); },   // 0x5F, 0b01011111: LD E, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.high, cpu._rBC.u8.high); },  // 0x60, 0b01100000: LD H, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.high, cpu._rBC.u8.low); },   // 0x61, 0b01100001: LD H, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.high, cpu._rDE.u8.high); },  // 0x62, 0b01100010: LD H, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.high, cpu._rDE.u8.low); },   // 0x63, 0b01100011: LD H, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.high, cpu._rHL.u8.high); },  // 0x64, 0b01100100: LD H, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.high, cpu._rHL.u8.low); },   // 0x65, 0b01100101: LD H, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_pHL(cpu._rHL.u8.high); },                  // 0x66, 0b01100110: LD H, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.high, cpu._rAF.u8.high); },  // 0x67, 0b01100111: LD H, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.low, cpu._rBC.u8.high); },   // 0x68, 0b01101000: LD L, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.low, cpu._rBC.u8.low); },    // 0x69, 0b01101001: LD L, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.low, cpu._rDE.u8.high); },   // 0x6A, 0b01101010: LD L, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.low, cpu._rDE.u8.low); },    // 0x6B, 0b01101011: LD L, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.low, cpu._rHL.u8.high); },   // 0x6C, 0b01101100: LD L, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.low, cpu._rHL.u8.low); },    // 0x6D, 0b01101101: LD L, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_pHL(cpu._rHL.u8.low); },                   // 0x6E, 0b01101110: LD L, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rHL.u8.low, cpu._rAF.u8.high); },   // 0x6F, 0b01101111: LD L, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pHL_r(cpu._rBC.u8.high); },                  // 0x70, 0b01110000: LD (HL), B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pHL_r(cpu._rBC.u8.low); },                   // 0x71, 0b01110001: LD (HL), C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pHL_r(cpu._rDE.u8.high); },                  // 0x72, 0b01110010: LD (HL), D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pHL_r(cpu._rDE.u8.low); },                   // 0x73, 0b01110011: LD (HL), E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pHL_r(cpu._rHL.u8.high); },                  // 0x74, 0b01110100: LD (HL), H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pHL_r(cpu._rHL.u8.low); },                   // 0x75, 0b01110101: LD (HL), L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_HALT(); },                                      // 0x76, 0b01110110: HALT
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pHL_r(cpu._rAF.u8.high); },                  // 0x77, 0b01110111: LD (HL), A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rAF.u8.high, cpu._rBC.u8.high); },  // 0x78, 0b01111000: LD A, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rAF.u8.high, cpu._rBC.u8.low); },   // 0x79, 0b01111001: LD A, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rAF.u8.high, cpu._rDE.u8.high); },  // 0x7A, 0b01111010: LD A, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rAF.u8.high, cpu._rDE.u8.low); },   // 0x7B, 0b01111011: LD A, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rAF.u8.high, cpu._rHL.u8.high); },  // 0x7C, 0b01111100: LD A, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rAF.u8.high, cpu._rHL.u8.low); },   // 0x7D, 0b01111101: LD A, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_pHL(cpu._rAF.u8.high); },                  // 0x7E, 0b01111110: LD A, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_r_r(cpu._rAF.u8.high, cpu._rAF.u8.high); },  // 0x7F, 0b01111111: LD A, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_r(cpu._rBC.u8.high); }, // 0x80, 0b10000000: ADD B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_r(cpu._rBC.u8.low); },  // 0x81, 0b10000001: ADD C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_r(cpu._rDE.u8.high); }, // 0x82, 0b10000010: ADD D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_r(cpu._rDE.u8.low); },  // 0x83, 0b10000011: ADD E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_r(cpu._rHL.u8.high); }, // 0x84, 0b10000100: ADD H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_r(cpu._rHL.u8.low); },  // 0x85, 0b10000101: ADD L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_pHL(); },               // 0x86, 0b10000110: ADD (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_r(cpu._rAF.u8.high); }, // 0x87, 0b10000111: ADD A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADC_r(cpu._rBC.u8.high); }, // 0x88, 0b10001000: ADC B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADC_r(cpu._rBC.u8.low); },  // 0x89, 0b10001001: ADC C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADC_r(cpu._rDE.u8.high); }, // 0x8A, 0b10001010: ADC D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADC_r(cpu._rDE.u8.low); },  // 0x8B, 0b10001011: ADC E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADC_r(cpu._rHL.u8.high); }, // 0x8C, 0b10001100: ADC H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADC_r(cpu._rHL.u8.low); },  // 0x8D, 0b10001101: ADC L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADC_pHL(); },               // 0x8E, 0b10001110: ADC (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADC_r(cpu._rAF.u8.high); }, // 0x8F, 0b10001111: ADC A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SUB_r(cpu._rBC.u8.high); }, // 0x90, 0b10010000: SUB B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SUB_r(cpu._rBC.u8.low); },  // 0x91, 0b10010001: SUB C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SUB_r(cpu._rDE.u8.high); }, // 0x92, 0b10010010: SUB D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SUB_r(cpu._rDE.u8.low); },  // 0x93, 0b10010011: SUB E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SUB_r(cpu._rHL.u8.high); }, // 0x94, 0b10010100: SUB H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SUB_r(cpu._rHL.u8.low); },  // 0x95, 0b10010101: SUB L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SUB_pHL(); },               // 0x96, 0b10010110: SUB (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SUB_r(cpu._rAF.u8.high); }, // 0x97, 0b10010111: SUB A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SBC_r(cpu._rBC.u8.high); }, // 0x98, 0b10011000: SBC B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SBC_r(cpu._rBC.u8.low); },  // 0x99, 0b10011001: SBC C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SBC_r(cpu._rDE.u8.high); }, // 0x9A, 0b10011010: SBC D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SBC_r(cpu._rDE.u8.low); },  // 0x9B, 0b10011011: SBC E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SBC_r(cpu._rHL.u8.high); }, // 0x9C, 0b10011100: SBC H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SBC_r(cpu._rHL.u8.low); },  // 0x9D, 0b10011101: SBC L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SBC_pHL(); },               // 0x9E, 0b10011110: SBC (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SBC_r(cpu._rAF.u8.high); }, // 0x9F, 0b10011111: SBC A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_AND_r(cpu._rBC.u8.high); }, // 0xA0, 0b10100000: AND B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_AND_r(cpu._rBC.u8.low); },  // 0xA1, 0b10100001: AND C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_AND_r(cpu._rDE.u8.high); }, // 0xA2, 0b10100010: AND D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_AND_r(cpu._rDE.u8.low); },  // 0xA3, 0b10100011: AND E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_AND_r(cpu._rHL.u8.high); }, // 0xA4, 0b10100100: AND H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_AND_r(cpu._rHL.u8.low); },  // 0xA5, 0b10100101: AND L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_AND_pHL(); },               // 0xA6, 0b10100110: AND (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_AND_r(cpu._rAF.u8.high); }, // 0xA7, 0b10100111: AND A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_XOR_r(cpu._rBC.u8.high); }, // 0xA8, 0b10101000: XOR B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_XOR_r(cpu._rBC.u8.low); },  // 0xA9, 0b10101001: XOR C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_XOR_r(cpu._rDE.u8.high); }, // 0xAA, 0b10101010: XOR D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_XOR_r(cpu._rDE.u8.low); },  // 0xAB, 0b10101011: XOR E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_XOR_r(cpu._rHL.u8.high); }, // 0xAC, 0b10101100: XOR H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_XOR_r(cpu._rHL.u8.low); },  // 0xAD, 0b10101101: XOR L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_XOR_pHL(); },               // 0xAE, 0b10101110: XOR (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_XOR_r(cpu._rAF.u8.high); }, // 0xAF, 0b10101111: XOR A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_OR_r(cpu._rBC.u8.high); },  // 0xB0, 0b10110000: OR B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_OR_r(cpu._rBC.u8.low); },   // 0xB1, 0b10110001: OR C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_OR_r(cpu._rDE.u8.high); },  // 0xB2, 0b10110010: OR D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_OR_r(cpu._rDE.u8.low); },   // 0xB3, 0b10110011: OR E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_OR_r(cpu._rHL.u8.high); },  // 0xB4, 0b10110100: OR H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_OR_r(cpu._rHL.u8.low); },   // 0xB5, 0b10110101: OR L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_OR_pHL(); },                // 0xB6, 0b10110110: OR (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_OR_r(cpu._rAF.u8.high); },  // 0xB7, 0b10110111: OR A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CP_r(cpu._rBC.u8.high); },  // 0xB8, 0b10111000: CP B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CP_r(cpu._rBC.u8.low); },   // 0xB9, 0b10111001: CP C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CP_r(cpu._rDE.u8.high); },  // 0xBA, 0b10111010: CP D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CP_r(cpu._rDE.u8.low); },   // 0xBB, 0b10111011: CP E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CP_r(cpu._rHL.u8.high); },  // 0xBC, 0b10111100: CP H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CP_r(cpu._rHL.u8.low); },   // 0xBD, 0b10111101: CP L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CP_pHL(); },                // 0xBE, 0b10111110: CP (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CP_r(cpu._rAF.u8.high); },  // 0xBF, 0b10111111: CP A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RET_c0(Register::Z); },     // 0xC0, 0b11000000: RET NZ
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_POP_rr(cpu._rBC); },        // 0xC1, 0b11000001: POP BC
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JP_c0_nn(Register::Z); },   // 0xC2, 0b11000010: JP NZ, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JP_nn(); },                 // 0xC3, 0b11000011: JP nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CALL_c0_nn(Register::Z); }, // 0xC4, 0b11000100: CALL NZ, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_PUSH_rr(cpu._rBC); },       // 0xC5, 0b11000101: PUSH BC
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_n(); },                 // 0xC6, 0b11000110: ADD n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RST(0x0000); },             // 0xC7, 0b11000111: RST 00
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RET_c1(Register::Z); },     // 0xC8, 0b11001000: RET Z
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RET(); },                   // 0xC9, 0b11001001: RET
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JP_c1_nn(Register::Z); },   // 0xCA, 0b11001010: JP Z, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CB(); },                    // 0xCB, 0b11001011: SWAP / RLC / RL / RRC / RR / SLA / SRA / SRL / BIT / SET / RES
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CALL_c1_nn(Register::Z); }, // 0xCC, 0b11001100: CALL Z, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CALL_nn(); },               // 0xCD, 0b11001101: CALL nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADC_n(); },                 // 0xCE, 0b11001110: ADC A, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RST(0x0008); },             // 0xCF, 0b11001111: RST 08

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RET_c0(Register::C); },     // 0xD0, 0b11010000: RET NC
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_POP_rr(cpu._rDE); },        // 0xD1, 0b11010001: POP DE
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JP_c0_nn(Register::C); },   // 0xD2, 0b11010010: JP NC, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },                   // 0xD3, 0b11010011: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CALL_c0_nn(Register::C); }, // 0xD4, 0b11010100: CALL NC, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_PUSH_rr(cpu._rDE); },       // 0xD5, 0b11010101: PUSH DE
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SUB_n(); },                 // 0xD6, 0b11010110: SUB n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RST(0x0010); },             // 0xD7, 0b11010111: RST 10
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RET_c1(Register::C); },     // 0xD8, 0b11011000: RET C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RETI(); },                  // 0xD9, 0b11011001: RETI
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JP_c1_nn(Register::C); },   // 0xDA, 0b11011010: JP C, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },                   // 0xDB, 0b11011011: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CALL_c1_nn(Register::C); }, // 0xDC, 0b11011100: CALL C, nn
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },                   // 0xDD, 0b11011101: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SBC_n(); },                 // 0xDE, 0b11011110: SBC A, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RST(0x0018); },             // 0xDF, 0b11011111: RST 18

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LDH_pn_A(); },        // 0xE0, 0b11100000: LDH (n), A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_POP_rr(cpu._rHL); },  // 0xE1, 0b11100001: POP HL
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LDH_pC_A(); },        // 0xE2, 0b11100010: LDH (C), A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },             // 0xE3, 0b11100011: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },             // 0xE4, 0b11100100: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_PUSH_rr(cpu._rHL); }, // 0xE5, 0b11100101: PUSH HL
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_AND_n(); },           // 0xE6, 0b11100110: AND n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RST(0x0020); },       // 0xE7, 0b11100111: RST 20
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_ADD_SP_n(); },        // 0xE8, 0b11101000: ADD SP, n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JP_HL(); },           // 0xE9, 0b11101001: JP HL
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_pnn_A(); },        // 0xEA, 0b11101010: LD (nn), A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },             // 0xEB, 0b11101011: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },             // 0xEC, 0b11101100: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },             // 0xED, 0b11101101: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_XOR_n(); },           // 0xEE, 0b11101110: XOR n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RST(0x0028); },       // 0xEF, 0b11101111: RST 28

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LDH_A_pn(); },        // 0xF0, 0b11110000: LD A, (n)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_POP_AF(); },          // 0xF1, 0b11110001: POP AF
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LDH_A_pC(); },        // 0xF2, 0b11110010: LD A, (C)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_DI(); },              // 0xF3, 0b11110011: DI
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },             // 0xF4, 0b11110100: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_PUSH_rr(cpu._rAF); }, // 0xF5, 0b11110101: PUSH AF
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_OR_n(); },            // 0xF6, 0b11110110: OR n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RST(0x0030); },       // 0xF7, 0b11110111: RST 30
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_HL_SPn(); },       // 0xF8, 0b11111000: LD HL, SP+n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_SP_HL(); },        // 0xF9, 0b11111001: LD SP, HL
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_LD_A_pnn(); },        // 0xFA, 0b11111010: LD A, (nn)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_EI(); },              // 0xFB, 0b11111011: EI
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },             // 0xFC, 0b11111100: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_NOP(); },             // 0xFD, 0b11111101: 
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CP_n(); },            // 0xFE, 0b11111110: CP n
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RST(0x0038); }        // 0xFF, 0b11111111: RST 38
};

constexpr const std::array<GBC::CentralProcessingUnit::Opcode, 256> GBC::CentralProcessingUnit::_opcodesCb = {
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLC(cpu._rBC.u8.high); }, // 0x00, 0b00000000: RLC B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLC(cpu._rBC.u8.low); },  // 0x01, 0b00000001: RLC C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLC(cpu._rDE.u8.high); }, // 0x02, 0b00000010: RLC D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLC(cpu._rDE.u8.low); },  // 0x03, 0b00000011: RLC E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLC(cpu._rHL.u8.high); }, // 0x04, 0b00000100: RLC H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLC(cpu._rHL.u8.low); },  // 0x05, 0b00000101: RLC L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLC_pHL(); },             // 0x06, 0b00000110: RLC (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RLC(cpu._rAF.u8.high); }, // 0x07, 0b00000111: RLC A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRC(cpu._rBC.u8.high); }, // 0x08, 0b00001000: RRC B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRC(cpu._rBC.u8.low); },  // 0x09, 0b00001001: RRC C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRC(cpu._rDE.u8.high); }, // 0x0A, 0b00001010: RRC D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRC(cpu._rDE.u8.low); },  // 0x0B, 0b00001011: RRC E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRC(cpu._rHL.u8.high); }, // 0x0C, 0b00001100: RRC H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRC(cpu._rHL.u8.low); },  // 0x0D, 0b00001101: RRC L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRC_pHL(); },             // 0x0E, 0b00001110: RRC (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RRC(cpu._rAF.u8.high); }, // 0x0F, 0b00001111: RRC A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RL(cpu._rBC.u8.high); },  // 0x10, 0b00010000: RL B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RL(cpu._rBC.u8.low); },   // 0x11, 0b00010001: RL C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RL(cpu._rDE.u8.high); },  // 0x12, 0b00010010: RL D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RL(cpu._rDE.u8.low); },   // 0x13, 0b00010011: RL E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RL(cpu._rHL.u8.high); },  // 0x14, 0b00010100: RL H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RL(cpu._rHL.u8.low); },   // 0x15, 0b00010101: RL L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RL_pHL(); },              // 0x16, 0b00010110: RL (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RL(cpu._rAF.u8.high); },  // 0x17, 0b00010111: RL A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RR(cpu._rBC.u8.high); },  // 0x18, 0b00011000: RR B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RR(cpu._rBC.u8.low); },   // 0x19, 0b00011001: RR C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RR(cpu._rDE.u8.high); },  // 0x1A, 0b00011010: RR D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RR(cpu._rDE.u8.low); },   // 0x1B, 0b00011011: RR E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RR(cpu._rHL.u8.high); },  // 0x1C, 0b00011100: RR H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RR(cpu._rHL.u8.low); },   // 0x1D, 0b00011101: RR L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RR_pHL(); },              // 0x1E, 0b00011110: RR (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RR(cpu._rAF.u8.high); },  // 0x1F, 0b00011111: RR A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SLA(cpu._rBC.u8.high); }, // 0x20, 0b00100000: SLA B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SLA(cpu._rBC.u8.low); },  // 0x21, 0b00100001: SLA C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SLA(cpu._rDE.u8.high); }, // 0x22, 0b00100010: SLA D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SLA(cpu._rDE.u8.low); },  // 0x23, 0b00100011: SLA E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SLA(cpu._rHL.u8.high); }, // 0x24, 0b00100100: SLA H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SLA(cpu._rHL.u8.low); },  // 0x25, 0b00100101: SLA L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SLA_pHL(); },             // 0x26, 0b00100110: SLA (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SLA(cpu._rAF.u8.high); }, // 0x27, 0b00100111: SLA A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRA(cpu._rBC.u8.high); }, // 0x28, 0b00101000: SRA B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRA(cpu._rBC.u8.low); },  // 0x29, 0b00101001: SRA C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRA(cpu._rDE.u8.high); }, // 0x2A, 0b00101010: SRA D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRA(cpu._rDE.u8.low); },  // 0x2B, 0b00101011: SRA E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRA(cpu._rHL.u8.high); }, // 0x2C, 0b00101100: SRA H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRA(cpu._rHL.u8.low); },  // 0x2D, 0b00101101: SRA L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRA_pHL(); },             // 0x2E, 0b00101110: SRA (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRA(cpu._rAF.u8.high); }, // 0x2F, 0b00101111: SRA A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SWAP(cpu._rBC.u8.high); },  // 0x30, 0b00110000: SWAP B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SWAP(cpu._rBC.u8.low); },   // 0x31, 0b00110001: SWAP C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SWAP(cpu._rDE.u8.high); },  // 0x32, 0b00110010: SWAP D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SWAP(cpu._rDE.u8.low); },   // 0x33, 0b00110011: SWAP E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SWAP(cpu._rHL.u8.high); },  // 0x34, 0b00110100: SWAP H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SWAP(cpu._rHL.u8.low); },   // 0x35, 0b00110101: SWAP L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SWAP_pHL(); },              // 0x36, 0b00110110: SWAP (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SWAP(cpu._rAF.u8.high); },  // 0x37, 0b00110111: SWAP A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRL(cpu._rBC.u8.high); },   // 0x38, 0b00111000: SRL B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRL(cpu._rBC.u8.low); },    // 0x39, 0b00111001: SRL C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRL(cpu._rDE.u8.high); },   // 0x3A, 0b00111010: SRL D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRL(cpu._rDE.u8.low); },    // 0x3B, 0b00111011: SRL E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRL(cpu._rHL.u8.high); },   // 0x3C, 0b00111100: SRL H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRL(cpu._rHL.u8.low); },    // 0x3D, 0b00111101: SRL L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRL_pHL(); },               // 0x3E, 0b00111110: SRL (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SRL(cpu._rAF.u8.high); },   // 0x3F, 0b00111111: SRL A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<0>(cpu._rBC.u8.high); },  // 0x40, 0b01000000: BIT 0, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<0>(cpu._rBC.u8.low); },   // 0x41, 0b01000001: BIT 0, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<0>(cpu._rDE.u8.high); },  // 0x42, 0b01000010: BIT 0, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<0>(cpu._rDE.u8.low); },   // 0x43, 0b01000011: BIT 0, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<0>(cpu._rHL.u8.high); },  // 0x44, 0b01000100: BIT 0, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<0>(cpu._rHL.u8.low); },   // 0x45, 0b01000101: BIT 0, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT_pHL<0>(); },              // 0x46, 0b01000110: BIT 0, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<0>(cpu._rAF.u8.high); },  // 0x47, 0b01000111: BIT 0, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<1>(cpu._rBC.u8.high); },  // 0x48, 0b01001000: BIT 1, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<1>(cpu._rBC.u8.low); },   // 0x49, 0b01001001: BIT 1, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<1>(cpu._rDE.u8.high); },  // 0x4A, 0b01001010: BIT 1, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<1>(cpu._rDE.u8.low); },   // 0x4B, 0b01001011: BIT 1, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<1>(cpu._rHL.u8.high); },  // 0x4C, 0b01001100: BIT 1, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<1>(cpu._rHL.u8.low); },   // 0x4D, 0b01001101: BIT 1, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT_pHL<1>(); },              // 0x4E, 0b01001110: BIT 1, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<1>(cpu._rAF.u8.high); },  // 0x4F, 0b01001111: BIT 1, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<2>(cpu._rBC.u8.high); },  // 0x50, 0b01010000: BIT 2, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<2>(cpu._rBC.u8.low); },   // 0x51, 0b01010001: BIT 2, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<2>(cpu._rDE.u8.high); },  // 0x52, 0b01010010: BIT 2, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<2>(cpu._rDE.u8.low); },   // 0x53, 0b01010011: BIT 2, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<2>(cpu._rHL.u8.high); },  // 0x54, 0b01010100: BIT 2, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<2>(cpu._rHL.u8.low); },   // 0x55, 0b01010101: BIT 2, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT_pHL<2>(); },              // 0x56, 0b01010110: BIT 2, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<2>(cpu._rAF.u8.high); },  // 0x57, 0b01010111: BIT 2, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<3>(cpu._rBC.u8.high); },  // 0x58, 0b01011000: BIT 3, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<3>(cpu._rBC.u8.low); },   // 0x59, 0b01011001: BIT 3, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<3>(cpu._rDE.u8.high); },  // 0x5A, 0b01011010: BIT 3, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<3>(cpu._rDE.u8.low); },   // 0x5B, 0b01011011: BIT 3, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<3>(cpu._rHL.u8.high); },  // 0x5C, 0b01011100: BIT 3, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<3>(cpu._rHL.u8.low); },   // 0x5D, 0b01011101: BIT 3, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT_pHL<3>(); },              // 0x5E, 0b01011110: BIT 3, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<3>(cpu._rAF.u8.high); },  // 0x5F, 0b01011111: BIT 3, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<4>(cpu._rBC.u8.high); },  // 0x60, 0b01100000: BIT 4, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<4>(cpu._rBC.u8.low); },   // 0x61, 0b01100001: BIT 4, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<4>(cpu._rDE.u8.high); },  // 0x62, 0b01100010: BIT 4, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<4>(cpu._rDE.u8.low); },   // 0x63, 0b01100011: BIT 4, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<4>(cpu._rHL.u8.high); },  // 0x64, 0b01100100: BIT 4, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<4>(cpu._rHL.u8.low); },   // 0x65, 0b01100101: BIT 4, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT_pHL<4>(); },              // 0x66, 0b01100110: BIT 4, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<4>(cpu._rAF.u8.high); },  // 0x67, 0b01100111: BIT 4, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<5>(cpu._rBC.u8.high); },  // 0x68, 0b01101000: BIT 5, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<5>(cpu._rBC.u8.low); },   // 0x69, 0b01101001: BIT 5, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<5>(cpu._rDE.u8.high); },  // 0x6A, 0b01101010: BIT 5, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<5>(cpu._rDE.u8.low); },   // 0x6B, 0b01101011: BIT 5, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<5>(cpu._rHL.u8.high); },  // 0x6C, 0b01101100: BIT 5, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<5>(cpu._rHL.u8.low); },   // 0x6D, 0b01101101: BIT 5, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT_pHL<5>(); },              // 0x6E, 0b01101110: BIT 5, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<5>(cpu._rAF.u8.high); },  // 0x6F, 0b01101111: BIT 5, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<6>(cpu._rBC.u8.high); },  // 0x70, 0b01110000: BIT 6, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<6>(cpu._rBC.u8.low); },   // 0x71, 0b01110001: BIT 6, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<6>(cpu._rDE.u8.high); },  // 0x72, 0b01110010: BIT 6, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<6>(cpu._rDE.u8.low); },   // 0x73, 0b01110011: BIT 6, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<6>(cpu._rHL.u8.high); },  // 0x74, 0b01110100: BIT 6, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<6>(cpu._rHL.u8.low); },   // 0x75, 0b01110101: BIT 6, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT_pHL<6>(); },              // 0x76, 0b01110110: BIT 6, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<6>(cpu._rAF.u8.high); },  // 0x77, 0b01110111: BIT 6, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<7>(cpu._rBC.u8.high); },  // 0x78, 0b01111000: BIT 7, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<7>(cpu._rBC.u8.low); },   // 0x79, 0b01111001: BIT 7, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<7>(cpu._rDE.u8.high); },  // 0x7A, 0b01111010: BIT 7, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<7>(cpu._rDE.u8.low); },   // 0x7B, 0b01111011: BIT 7, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<7>(cpu._rHL.u8.high); },  // 0x7C, 0b01111100: BIT 7, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<7>(cpu._rHL.u8.low); },   // 0x7D, 0b01111101: BIT 7, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT_pHL<7>(); },              // 0x7E, 0b01111110: BIT 7, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_BIT<7>(cpu._rAF.u8.high); },  // 0x7F, 0b01111111: BIT 7, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<0>(cpu._rBC.u8.high); },  // 0x80, 0b10000000: RES 0, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<0>(cpu._rBC.u8.low); },   // 0x81, 0b10000001: RES 0, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<0>(cpu._rDE.u8.high); },  // 0x82, 0b10000010: RES 0, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<0>(cpu._rDE.u8.low); },   // 0x83, 0b10000011: RES 0, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<0>(cpu._rHL.u8.high); },  // 0x84, 0b10000100: RES 0, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<0>(cpu._rHL.u8.low); },   // 0x85, 0b10000101: RES 0, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES_pHL<0>(); },              // 0x86, 0b10000110: RES 0, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<0>(cpu._rAF.u8.high); },  // 0x87, 0b10000111: RES 0, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<1>(cpu._rBC.u8.high); },  // 0x88, 0b10001000: RES 1, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<1>(cpu._rBC.u8.low); },   // 0x89, 0b10001001: RES 1, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<1>(cpu._rDE.u8.high); },  // 0x8A, 0b10001010: RES 1, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<1>(cpu._rDE.u8.low); },   // 0x8B, 0b10001011: RES 1, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<1>(cpu._rHL.u8.high); },  // 0x8C, 0b10001100: RES 1, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<1>(cpu._rHL.u8.low); },   // 0x8D, 0b10001101: RES 1, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES_pHL<1>(); },              // 0x8E, 0b10001110: RES 1, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<1>(cpu._rAF.u8.high); },  // 0x8F, 0b10001111: RES 1, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<2>(cpu._rBC.u8.high); },  // 0x90, 0b10010000: RES 2, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<2>(cpu._rBC.u8.low); },   // 0x91, 0b10010001: RES 2, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<2>(cpu._rDE.u8.high); },  // 0x92, 0b10010010: RES 2, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<2>(cpu._rDE.u8.low); },   // 0x93, 0b10010011: RES 2, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<2>(cpu._rHL.u8.high); },  // 0x94, 0b10010100: RES 2, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<2>(cpu._rHL.u8.low); },   // 0x95, 0b10010101: RES 2, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES_pHL<2>(); },              // 0x96, 0b10010110: RES 2, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<2>(cpu._rAF.u8.high); },  // 0x97, 0b10010111: RES 2, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<3>(cpu._rBC.u8.high); },  // 0x98, 0b10011000: RES 3, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<3>(cpu._rBC.u8.low); },   // 0x99, 0b10011001: RES 3, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<3>(cpu._rDE.u8.high); },  // 0x9A, 0b10011010: RES 3, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<3>(cpu._rDE.u8.low); },   // 0x9B, 0b10011011: RES 3, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<3>(cpu._rHL.u8.high); },  // 0x9C, 0b10011100: RES 3, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<3>(cpu._rHL.u8.low); },   // 0x9D, 0b10011101: RES 3, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES_pHL<3>(); },              // 0x9E, 0b10011110: RES 3, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<3>(cpu._rAF.u8.high); },  // 0x9F, 0b10011111: RES 3, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<4>(cpu._rBC.u8.high); },  // 0xA0, 0b10100000: RES 4, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<4>(cpu._rBC.u8.low); },   // 0xA1, 0b10100001: RES 4, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<4>(cpu._rDE.u8.high); },  // 0xA2, 0b10100010: RES 4, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<4>(cpu._rDE.u8.low); },   // 0xA3, 0b10100011: RES 4, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<4>(cpu._rHL.u8.high); },  // 0xA4, 0b10100100: RES 4, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<4>(cpu._rHL.u8.low); },   // 0xA5, 0b10100101: RES 4, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES_pHL<4>(); },              // 0xA6, 0b10100110: RES 4, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<4>(cpu._rAF.u8.high); },  // 0xA7, 0b10100111: RES 4, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<5>(cpu._rBC.u8.high); },  // 0xA8, 0b10101000: RES 5, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<5>(cpu._rBC.u8.low); },   // 0xA9, 0b10101001: RES 5, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<5>(cpu._rDE.u8.high); },  // 0xAA, 0b10101010: RES 5, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<5>(cpu._rDE.u8.low); },   // 0xAB, 0b10101011: RES 5, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<5>(cpu._rHL.u8.high); },  // 0xAC, 0b10101100: RES 5, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<5>(cpu._rHL.u8.low); },   // 0xAD, 0b10101101: RES 5, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES_pHL<5>(); },              // 0xAE, 0b10101110: RES 5, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<5>(cpu._rAF.u8.high); },  // 0xAF, 0b10101111: RES 5, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<6>(cpu._rBC.u8.high); },  // 0xB0, 0b10110000: RES 6, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<6>(cpu._rBC.u8.low); },   // 0xB1, 0b10110001: RES 6, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<6>(cpu._rDE.u8.high); },  // 0xB2, 0b10110010: RES 6, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<6>(cpu._rDE.u8.low); },   // 0xB3, 0b10110011: RES 6, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<6>(cpu._rHL.u8.high); },  // 0xB4, 0b10110100: RES 6, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<6>(cpu._rHL.u8.low); },   // 0xB5, 0b10110101: RES 6, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES_pHL<6>(); },              // 0xB6, 0b10110110: RES 6, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<6>(cpu._rAF.u8.high); },  // 0xB7, 0b10110111: RES 6, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<7>(cpu._rBC.u8.high); },  // 0xB8, 0b10111000: RES 7, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<7>(cpu._rBC.u8.low); },   // 0xB9, 0b10111001: RES 7, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<7>(cpu._rDE.u8.high); },  // 0xBA, 0b10111010: RES 7, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<7>(cpu._rDE.u8.low); },   // 0xBB, 0b10111011: RES 7, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<7>(cpu._rHL.u8.high); },  // 0xBC, 0b10111100: RES 7, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<7>(cpu._rHL.u8.low); },   // 0xBD, 0b10111101: RES 7, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES_pHL<7>(); },              // 0xBE, 0b10111110: RES 7, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RES<7>(cpu._rAF.u8.high); },  // 0xBF, 0b10111111: RES 7, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<0>(cpu._rBC.u8.high); },  // 0xC0, 0b11000000: SET 0, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<0>(cpu._rBC.u8.low); },   // 0xC1, 0b11000001: SET 0, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<0>(cpu._rDE.u8.high); },  // 0xC2, 0b11000010: SET 0, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<0>(cpu._rDE.u8.low); },   // 0xC3, 0b11000011: SET 0, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<0>(cpu._rHL.u8.high); },  // 0xC4, 0b11000100: SET 0, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<0>(cpu._rHL.u8.low); },   // 0xC5, 0b11000101: SET 0, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET_pHL<0>(); },              // 0xC6, 0b11000110: SET 0, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<0>(cpu._rAF.u8.high); },  // 0xC7, 0b11000111: SET 0, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<1>(cpu._rBC.u8.high); },  // 0xC8, 0b11001000: SET 1, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<1>(cpu._rBC.u8.low); },   // 0xC9, 0b11001001: SET 1, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<1>(cpu._rDE.u8.high); },  // 0xCA, 0b11001010: SET 1, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<1>(cpu._rDE.u8.low); },   // 0xCB, 0b11001011: SET 1, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<1>(cpu._rHL.u8.high); },  // 0xCC, 0b11001100: SET 1, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<1>(cpu._rHL.u8.low); },   // 0xCD, 0b11001101: SET 1, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET_pHL<1>(); },              // 0xCE, 0b11001110: SET 1, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<1>(cpu._rAF.u8.high); },  // 0xCF, 0b11001111: SET 1, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<2>(cpu._rBC.u8.high); },  // 0xD0, 0b11010000: SET 2, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<2>(cpu._rBC.u8.low); },   // 0xD1, 0b11010001: SET 2, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<2>(cpu._rDE.u8.high); },  // 0xD2, 0b11010010: SET 2, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<2>(cpu._rDE.u8.low); },   // 0xD3, 0b11010011: SET 2, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<2>(cpu._rHL.u8.high); },  // 0xD4, 0b11010100: SET 2, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<2>(cpu._rHL.u8.low); },   // 0xD5, 0b11010101: SET 2, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET_pHL<2>(); },              // 0xD6, 0b11010110: SET 2, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<2>(cpu._rAF.u8.high); },  // 0xD7, 0b11010111: SET 2, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<3>(cpu._rBC.u8.high); },  // 0xD8, 0b11011000: SET 3, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<3>(cpu._rBC.u8.low); },   // 0xD9, 0b11011001: SET 3, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<3>(cpu._rDE.u8.high); },  // 0xDA, 0b11011010: SET 3, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<3>(cpu._rDE.u8.low); },   // 0xDB, 0b11011011: SET 3, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<3>(cpu._rHL.u8.high); },  // 0xDC, 0b11011100: SET 3, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<3>(cpu._rHL.u8.low); },   // 0xDD, 0b11011101: SET 3, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET_pHL<3>(); },              // 0xDE, 0b11011110: SET 3, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<3>(cpu._rAF.u8.high); },  // 0xDF, 0b11011111: SET 3, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<4>(cpu._rBC.u8.high); },  // 0xE0, 0b11100000: SET 4, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<4>(cpu._rBC.u8.low); },   // 0xE1, 0b11100001: SET 4, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<4>(cpu._rDE.u8.high); },  // 0xE2, 0b11100010: SET 4, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<4>(cpu._rDE.u8.low); },   // 0xE3, 0b11100011: SET 4, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<4>(cpu._rHL.u8.high); },  // 0xE4, 0b11100100: SET 4, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<4>(cpu._rHL.u8.low); },   // 0xE5, 0b11100101: SET 4, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET_pHL<4>(); },              // 0xE6, 0b11100110: SET 4, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<4>(cpu._rAF.u8.high); },  // 0xE7, 0b11100111: SET 4, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<5>(cpu._rBC.u8.high); },  // 0xE8, 0b11101000: SET 5, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<5>(cpu._rBC.u8.low); },   // 0xE9, 0b11101001: SET 5, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<5>(cpu._rDE.u8.high); },  // 0xEA, 0b11101010: SET 5, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<5>(cpu._rDE.u8.low); },   // 0xEB, 0b11101011: SET 5, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<5>(cpu._rHL.u8.high); },  // 0xEC, 0b11101100: SET 5, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<5>(cpu._rHL.u8.low); },   // 0xED, 0b11101101: SET 5, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET_pHL<5>(); },              // 0xEE, 0b11101110: SET 5, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<5>(cpu._rAF.u8.high); },  // 0xEF, 0b11101111: SET 5, A

  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<6>(cpu._rBC.u8.high); },  // 0xF0, 0b11110000: SET 6, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<6>(cpu._rBC.u8.low); },   // 0xF1, 0b11110001: SET 6, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<6>(cpu._rDE.u8.high); },  // 0xF2, 0b11110010: SET 6, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<6>(cpu._rDE.u8.low); },   // 0xF3, 0b11110011: SET 6, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<6>(cpu._rHL.u8.high); },  // 0xF4, 0b11110100: SET 6, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<6>(cpu._rHL.u8.low); },   // 0xF5, 0b11110101: SET 6, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET_pHL<6>(); },              // 0xF6, 0b11110110: SET 6, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<6>(cpu._rAF.u8.high); },  // 0xF7, 0b11110111: SET 6, A
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<7>(cpu._rBC.u8.high); },  // 0xF8, 0b11111000: SET 7, B
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<7>(cpu._rBC.u8.low); },   // 0xF9, 0b11111001: SET 7, C
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<7>(cpu._rDE.u8.high); },  // 0xFA, 0b11111010: SET 7, D
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<7>(cpu._rDE.u8.low); },   // 0xFB, 0b11111011: SET 7, E
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<7>(cpu._rHL.u8.high); },  // 0xFC, 0b11111100: SET 7, H
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<7>(cpu._rHL.u8.low); },   // 0xFD, 0b11111101: SET 7, L
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET_pHL<7>(); },              // 0xFE, 0b11111110: SET 7, (HL)
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_SET<7>(cpu._rAF.u8.high); }   // 0xFF, 0b11111111: SET 7, A
};

constexpr const std::array<GBC::CentralProcessingUnit::Opcode, 5> GBC::CentralProcessingUnit::_opcodesSpecial = {
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_INTERRUPT(); },       // Call interrupt handler
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JR_c_execute(); },    // Execute relative jump
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_JP_c_execute(); },    // Execute absolute jump
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_CALL_c_execute(); },  // Execute function call
  [](GBC::CentralProcessingUnit& cpu) { cpu.instruction_RET_c_execute(); }    // Execute function return
};

constexpr static const std::array<std::string_view, 256> _opcodes_desc = {
  "0x00, 0b00000000: NOP",
  "0x01, 0b00000001: LD BC, nn",
  "0x02, 0b00000010: LD (BC), A",
  "0x03, 0b00000011: INC BC",
  "0x04, 0b00000100: INC B",
  "0x05, 0b00000101: DEC B",
  "0x06, 0b00000110: LD B, n",
  "0x07, 0b00000111: RLCA",
  "0x08, 0b00001000: LD (nn), SP",
  "0x09, 0b00001001: ADD HL, BC",
  "0x0A, 0b00001010: LD A, (BC)",
  "0x0B, 0b00001011: DEC BC",
  "0x0C, 0b00001100: INC C",
  "0x0D, 0b00001101: DEC C",
  "0x0E, 0b00001110: LD C, n",
  "0x0F, 0b00001111: RRCA",

  "0x10, 0b00010000: STOP (0x10 & 0x00)",
  "0x11, 0b00010001: LD DE, nn",
  "0x12, 0b00010010: LD (DE), A",
  "0x13, 0b00010011: INC DE",
  "0x14, 0b00010100: INC D",
  "0x15, 0b00010101: DEC D",
  "0x16, 0b00010110: LD D, n",
  "0x17, 0b00010111: RLA",
  "0x18, 0b00011000: JR n",
  "0x19, 0b00011001: ADD HL, DE",
  "0x1A, 0b00011010: LD A, (DE)",
  "0x1B, 0b00011011: DEC DE",
  "0x1C, 0b00011100: INC E",
  "0x1D, 0b00011101: DEC E",
  "0x1E, 0b00011110: LD E, n",
  "0x1F, 0b00011111: RRA",

  "0x20, 0b00100000: JR NZ, n",
  "0x21, 0b00100001: LD HL, nn",
  "0x22, 0b00100010: LD (HL+), A",
  "0x23, 0b00100011: INC HL",
  "0x24, 0b00100100: INC H",
  "0x25, 0b00100101: DEC H",
  "0x26, 0b00100110: LD H, n",
  "0x27, 0b00100111: DAA",
  "0x28, 0b00101000: JR Z, n",
  "0x29, 0b00101001: ADD HL, HL",
  "0x2A, 0b00101010: LD A, (HL+)",
  "0x2B, 0b00101011: DEC HL",
  "0x2C, 0b00101100: INC L",
  "0x2D, 0b00101101: DEC L",
  "0x2E, 0b00101110: LD L, n",
  "0x2F, 0b00101111: CPL",

  "0x30, 0b00110000: JR NC, n",
  "0x31, 0b00110001: LD SP, nn",
  "0x32, 0b00110010: LD (HL-), A",
  "0x33, 0b00110011: INC SP",
  "0x34, 0b00110100: INC (HL)",
  "0x35, 0b00110101: DEC (HL)",
  "0x36, 0b00110110: LD (HL), n",
  "0x37, 0b00110111: SCF",
  "0x38, 0b00111000: JR C, n",
  "0x39, 0b00111001: ADD HL, SP",
  "0x3A, 0b00111010: LD A, (HL-)",
  "0x3B, 0b00111011: DEC SP",
  "0x3C, 0b00111100: INC A",
  "0x3D, 0b00111101: DEC A",
  "0x3E, 0b00111110: LD A, n",
  "0x3F, 0b00111111: CCF",

  "0x40, 0b01000000: LD B, B",
  "0x41, 0b01000001: LD B, C",
  "0x42, 0b01000010: LD B, D",
  "0x43, 0b01000011: LD B, E",
  "0x44, 0b01000100: LD B, H",
  "0x45, 0b01000101: LD B, L",
  "0x46, 0b01000110: LD B, (HL)",
  "0x47, 0b01000111: LD B, A",
  "0x48, 0b01001000: LD C, B",
  "0x49, 0b01001001: LD C, C",
  "0x4A, 0b01001010: LD C, D",
  "0x4B, 0b01001011: LD C, E",
  "0x4C, 0b01001100: LD C, H",
  "0x4D, 0b01001101: LD C, L",
  "0x4E, 0b01001110: LD C, (HL)",
  "0x4F, 0b01001111: LD C, A",

  "0x50, 0b01010000: LD D, B",
  "0x51, 0b01010001: LD D, C",
  "0x52, 0b01010010: LD D, D",
  "0x53, 0b01010011: LD D, E",
  "0x54, 0b01010100: LD D, H",
  "0x55, 0b01010101: LD D, L",
  "0x56, 0b01010110: LD D, (HL)",
  "0x57, 0b01010111: LD D, A",
  "0x58, 0b01011000: LD E, B",
  "0x59, 0b01011001: LD E, C",
  "0x5A, 0b01011010: LD E, D",
  "0x5B, 0b01011011: LD E, E",
  "0x5C, 0b01011100: LD E, H",
  "0x5D, 0b01011101: LD E, L",
  "0x5E, 0b01011110: LD E, (HL)",
  "0x5F, 0b01011111: LD E, A",

  "0x60, 0b01100000: LD H, B",
  "0x61, 0b01100001: LD H, C",
  "0x62, 0b01100010: LD H, D",
  "0x63, 0b01100011: LD H, E",
  "0x64, 0b01100100: LD H, H",
  "0x65, 0b01100101: LD H, L",
  "0x66, 0b01100110: LD H, (HL)",
  "0x67, 0b01100111: LD H, A",
  "0x68, 0b01101000: LD L, B",
  "0x69, 0b01101001: LD L, C",
  "0x6A, 0b01101010: LD L, D",
  "0x6B, 0b01101011: LD L, E",
  "0x6C, 0b01101100: LD L, H",
  "0x6D, 0b01101101: LD L, L",
  "0x6E, 0b01101110: LD L, (HL)",
  "0x6F, 0b01101111: LD L, A",

  "0x70, 0b01110000: LD (HL), B",
  "0x71, 0b01110001: LD (HL), C",
  "0x72, 0b01110010: LD (HL), D",
  "0x73, 0b01110011: LD (HL), E",
  "0x74, 0b01110100: LD (HL), H",
  "0x75, 0b01110101: LD (HL), L",
  "0x76, 0b01110110: HALT",
  "0x77, 0b01110111: LD (HL), A",
  "0x78, 0b01111000: LD A, B",
  "0x79, 0b01111001: LD A, C",
  "0x7A, 0b01111010: LD A, D",
  "0x7B, 0b01111011: LD A, E",
  "0x7C, 0b01111100: LD A, H",
  "0x7D, 0b01111101: LD A, L",
  "0x7E, 0b01111110: LD A, (HL)",
  "0x7F, 0b01111111: LD A, A",

  "0x80, 0b10000000: ADD B",
  "0x81, 0b10000001: ADD C",
  "0x82, 0b10000010: ADD D",
  "0x83, 0b10000011: ADD E",
  "0x84, 0b10000100: ADD H",
  "0x85, 0b10000101: ADD L",
  "0x86, 0b10000110: ADD (HL)",
  "0x87, 0b10000111: ADD A",
  "0x88, 0b10001000: ADC B",
  "0x89, 0b10001001: ADC C",
  "0x8A, 0b10001010: ADC D",
  "0x8B, 0b10001011: ADC E",
  "0x8C, 0b10001100: ADC H",
  "0x8D, 0b10001101: ADC L",
  "0x8E, 0b10001110: ADC (HL)",
  "0x8F, 0b10001111: ADC A",

  "0x90, 0b10010000: SUB B",
  "0x91, 0b10010001: SUB C",
  "0x92, 0b10010010: SUB D",
  "0x93, 0b10010011: SUB E",
  "0x94, 0b10010100: SUB H",
  "0x95, 0b10010101: SUB L",
  "0x96, 0b10010110: SUB (HL)",
  "0x97, 0b10010111: SUB A",
  "0x98, 0b10011000: SBC B",
  "0x99, 0b10011001: SBC C",
  "0x9A, 0b10011010: SBC D",
  "0x9B, 0b10011011: SBC E",
  "0x9C, 0b10011100: SBC H",
  "0x9D, 0b10011101: SBC L",
  "0x9E, 0b10011110: SBC (HL)",
  "0x9F, 0b10011111: SBC A",

  "0xA0, 0b10100000: AND B",
  "0xA1, 0b10100001: AND C",
  "0xA2, 0b10100010: AND D",
  "0xA3, 0b10100011: AND E",
  "0xA4, 0b10100100: AND H",
  "0xA5, 0b10100101: AND L",
  "0xA6, 0b10100110: AND (HL)",
  "0xA7, 0b10100111: AND A",
  "0xA8, 0b10101000: XOR B",
  "0xA9, 0b10101001: XOR C",
  "0xAA, 0b10101010: XOR D",
  "0xAB, 0b10101011: XOR E",
  "0xAC, 0b10101100: XOR H",
  "0xAD, 0b10101101: XOR L",
  "0xAE, 0b10101110: XOR (HL)",
  "0xAF, 0b10101111: XOR A",

  "0xB0, 0b10110000: OR B",
  "0xB1, 0b10110001: OR C",
  "0xB2, 0b10110010: OR D",
  "0xB3, 0b10110011: OR E",
  "0xB4, 0b10110100: OR H",
  "0xB5, 0b10110101: OR L",
  "0xB6, 0b10110110: OR (HL)",
  "0xB7, 0b10110111: OR A",
  "0xB8, 0b10111000: CP B",
  "0xB9, 0b10111001: CP C",
  "0xBA, 0b10111010: CP D",
  "0xBB, 0b10111011: CP E",
  "0xBC, 0b10111100: CP H",
  "0xBD, 0b10111101: CP L",
  "0xBE, 0b10111110: CP (HL)",
  "0xBF, 0b10111111: CP A",

  "0xC0, 0b11000000: RET NZ",
  "0xC1, 0b11000001: POP BC",
  "0xC2, 0b11000010: JP NZ, nn",
  "0xC3, 0b11000011: JP nn",
  "0xC4, 0b11000100: CALL NZ, nn",
  "0xC5, 0b11000101: PUSH BC",
  "0xC6, 0b11000110: ADD n",
  "0xC7, 0b11000111: RST 00",
  "0xC8, 0b11001000: RET Z",
  "0xC9, 0b11001001: RET",
  "0xCA, 0b11001010: JP Z, nn",
  "0xCB, 0b11001011: SWAP / RLC / RL / RRC / RR / SLA / SRA / SRL / BIT / SET / RES",
  "0xCC, 0b11001100: CALL Z, nn",
  "0xCD, 0b11001101: CALL nn",
  "0xCE, 0b11001110: ADC A, n",
  "0xCF, 0b11001111: RST 08",

  "0xD0, 0b11010000: RET NC",
  "0xD1, 0b11010001: POP DE",
  "0xD2, 0b11010010: JP NC, nn",
  "0xD3, 0b11010011: ",
  "0xD4, 0b11010100: CALL NC, nn",
  "0xD5, 0b11010101: PUSH DE",
  "0xD6, 0b11010110: SUB n",
  "0xD7, 0b11010111: RST 10",
  "0xD8, 0b11011000: RET C",
  "0xD9, 0b11011001: RETI",
  "0xDA, 0b11011010: JP C, nn",
  "0xDB, 0b11011011: ",
  "0xDC, 0b11011100: CALL C, nn",
  "0xDD, 0b11011101: ",
  "0xDE, 0b11011110: SBC A, n",
  "0xDF, 0b11011111: RST 18",

  "0xE0, 0b11100000: LDH (n), A",
  "0xE1, 0b11100001: POP HL",
  "0xE2, 0b11100010: LDH (C), A",
  "0xE3, 0b11100011: ",
  "0xE4, 0b11100100: ",
  "0xE5, 0b11100101: PUSH HL",
  "0xE6, 0b11100110: AND n",
  "0xE7, 0b11100111: RST 20",
  "0xE8, 0b11101000: ADD SP, n",
  "0xE9, 0b11101001: JP HL",
  "0xEA, 0b11101010: LD (nn), A",
  "0xEB, 0b11101011: ",
  "0xEC, 0b11101100: ",
  "0xED, 0b11101101: ",
  "0xEE, 0b11101110: XOR n",
  "0xEF, 0b11101111: RST 28",

  "0xF0, 0b11110000: LD A, (n)",
  "0xF1, 0b11110001: POP AF",
  "0xF2, 0b11110010: LD A, (C)",
  "0xF3, 0b11110011: DI",
  "0xF4, 0b11110100: ",
  "0xF5, 0b11110101: PUSH AF",
  "0xF6, 0b11110110: OR n",
  "0xF7, 0b11110111: RST 30",
  "0xF8, 0b11111000: LD HL, SP+n",
  "0xF9, 0b11111001: LD SP, HL",
  "0xFA, 0b11111010: LD A, (nn)",
  "0xFB, 0b11111011: EI",
  "0xFC, 0b11111100: ",
  "0xFD, 0b11111101: ",
  "0xFE, 0b11111110: CP n",
  "0xFF, 0b11111111: RST 38"
};

constexpr static const std::array<std::string_view, 256> _opcodesCB_desc = {
  "0x00, 0b00000000: RLC B",
  "0x01, 0b00000001: RLC C",
  "0x02, 0b00000010: RLC D",
  "0x03, 0b00000011: RLC E",
  "0x04, 0b00000100: RLC H",
  "0x05, 0b00000101: RLC L",
  "0x06, 0b00000110: RLC (HL)",
  "0x07, 0b00000111: RLC A",
  "0x08, 0b00001000: RRC B",
  "0x09, 0b00001001: RRC C",
  "0x0A, 0b00001010: RRC D",
  "0x0B, 0b00001011: RRC E",
  "0x0C, 0b00001100: RRC H",
  "0x0D, 0b00001101: RRC L",
  "0x0E, 0b00001110: RRC (HL)",
  "0x0F, 0b00001111: RRC A",

  "0x10, 0b00010000: RL B",
  "0x11, 0b00010001: RL C",
  "0x12, 0b00010010: RL D",
  "0x13, 0b00010011: RL E",
  "0x14, 0b00010100: RL H",
  "0x15, 0b00010101: RL L",
  "0x16, 0b00010110: RL (HL)",
  "0x17, 0b00010111: RL A",
  "0x18, 0b00011000: RR B",
  "0x19, 0b00011001: RR C",
  "0x1A, 0b00011010: RR D",
  "0x1B, 0b00011011: RR E",
  "0x1C, 0b00011100: RR H",
  "0x1D, 0b00011101: RR L",
  "0x1E, 0b00011110: RR (HL)",
  "0x1F, 0b00011111: RR A",

  "0x20, 0b00100000: SLA B",
  "0x21, 0b00100001: SLA C",
  "0x22, 0b00100010: SLA D",
  "0x23, 0b00100011: SLA E",
  "0x24, 0b00100100: SLA H",
  "0x25, 0b00100101: SLA L",
  "0x26, 0b00100110: SLA (HL)",
  "0x27, 0b00100111: SLA A",
  "0x28, 0b00101000: SRA B",
  "0x29, 0b00101001: SRA C",
  "0x2A, 0b00101010: SRA D",
  "0x2B, 0b00101011: SRA E",
  "0x2C, 0b00101100: SRA H",
  "0x2D, 0b00101101: SRA L",
  "0x2E, 0b00101110: SRA (HL)",
  "0x2F, 0b00101111: SRA A",

  "0x30, 0b00110000: SWAP B",
  "0x31, 0b00110001: SWAP C",
  "0x32, 0b00110010: SWAP D",
  "0x33, 0b00110011: SWAP E",
  "0x34, 0b00110100: SWAP H",
  "0x35, 0b00110101: SWAP L",
  "0x36, 0b00110110: SWAP (HL)",
  "0x37, 0b00110111: SWAP A",
  "0x38, 0b00111000: SRL B",
  "0x39, 0b00111001: SRL C",
  "0x3A, 0b00111010: SRL D",
  "0x3B, 0b00111011: SRL E",
  "0x3C, 0b00111100: SRL H",
  "0x3D, 0b00111101: SRL L",
  "0x3E, 0b00111110: SRL (HL)",
  "0x3F, 0b00111111: SRL A",

  "0x40, 0b01000000: BIT 0, B",
  "0x41, 0b01000001: BIT 0, C",
  "0x42, 0b01000010: BIT 0, D",
  "0x43, 0b01000011: BIT 0, E",
  "0x44, 0b01000100: BIT 0, H",
  "0x45, 0b01000101: BIT 0, L",
  "0x46, 0b01000110: BIT 0, (HL)",
  "0x47, 0b01000111: BIT 0, A",
  "0x48, 0b01001000: BIT 1, B",
  "0x49, 0b01001001: BIT 1, C",
  "0x4A, 0b01001010: BIT 1, D",
  "0x4B, 0b01001011: BIT 1, E",
  "0x4C, 0b01001100: BIT 1, H",
  "0x4D, 0b01001101: BIT 1, L",
  "0x4E, 0b01001110: BIT 1, (HL)",
  "0x4F, 0b01001111: BIT 1, A",

  "0x50, 0b01010000: BIT 2, B",
  "0x51, 0b01010001: BIT 2, C",
  "0x52, 0b01010010: BIT 2, D",
  "0x53, 0b01010011: BIT 2, E",
  "0x54, 0b01010100: BIT 2, H",
  "0x55, 0b01010101: BIT 2, L",
  "0x56, 0b01010110: BIT 2, (HL)",
  "0x57, 0b01010111: BIT 2, A",
  "0x58, 0b01011000: BIT 3, B",
  "0x59, 0b01011001: BIT 3, C",
  "0x5A, 0b01011010: BIT 3, D",
  "0x5B, 0b01011011: BIT 3, E",
  "0x5C, 0b01011100: BIT 3, H",
  "0x5D, 0b01011101: BIT 3, L",
  "0x5E, 0b01011110: BIT 3, (HL)",
  "0x5F, 0b01011111: BIT 3, A",

  "0x60, 0b01100000: BIT 4, B",
  "0x61, 0b01100001: BIT 4, C",
  "0x62, 0b01100010: BIT 4, D",
  "0x63, 0b01100011: BIT 4, E",
  "0x64, 0b01100100: BIT 4, H",
  "0x65, 0b01100101: BIT 4, L",
  "0x66, 0b01100110: BIT 4, (HL)",
  "0x67, 0b01100111: BIT 4, A",
  "0x68, 0b01101000: BIT 5, B",
  "0x69, 0b01101001: BIT 5, C",
  "0x6A, 0b01101010: BIT 5, D",
  "0x6B, 0b01101011: BIT 5, E",
  "0x6C, 0b01101100: BIT 5, H",
  "0x6D, 0b01101101: BIT 5, L",
  "0x6E, 0b01101110: BIT 5, (HL)",
  "0x6F, 0b01101111: BIT 5, A",

  "0x70, 0b01110000: BIT 6, B",
  "0x71, 0b01110001: BIT 6, C",
  "0x72, 0b01110010: BIT 6, D",
  "0x73, 0b01110011: BIT 6, E",
  "0x74, 0b01110100: BIT 6, H",
  "0x75, 0b01110101: BIT 6, L",
  "0x76, 0b01110110: BIT 6, (HL)",
  "0x77, 0b01110111: BIT 6, A",
  "0x78, 0b01111000: BIT 7, B",
  "0x79, 0b01111001: BIT 7, C",
  "0x7A, 0b01111010: BIT 7, D",
  "0x7B, 0b01111011: BIT 7, E",
  "0x7C, 0b01111100: BIT 7, H",
  "0x7D, 0b01111101: BIT 7, L",
  "0x7E, 0b01111110: BIT 7, (HL)",
  "0x7F, 0b01111111: BIT 7, A",

  "0x80, 0b10000000: RES 0, B",
  "0x81, 0b10000001: RES 0, C",
  "0x82, 0b10000010: RES 0, D",
  "0x83, 0b10000011: RES 0, E",
  "0x84, 0b10000100: RES 0, H",
  "0x85, 0b10000101: RES 0, L",
  "0x86, 0b10000110: RES 0, (HL)",
  "0x87, 0b10000111: RES 0, A",
  "0x88, 0b10001000: RES 1, B",
  "0x89, 0b10001001: RES 1, C",
  "0x8A, 0b10001010: RES 1, D",
  "0x8B, 0b10001011: RES 1, E",
  "0x8C, 0b10001100: RES 1, H",
  "0x8D, 0b10001101: RES 1, L",
  "0x8E, 0b10001110: RES 1, (HL)",
  "0x8F, 0b10001111: RES 1, A",

  "0x90, 0b10010000: RES 2, B",
  "0x91, 0b10010001: RES 2, C",
  "0x92, 0b10010010: RES 2, D",
  "0x93, 0b10010011: RES 2, E",
  "0x94, 0b10010100: RES 2, H",
  "0x95, 0b10010101: RES 2, L",
  "0x96, 0b10010110: RES 2, (HL)",
  "0x97, 0b10010111: RES 2, A",
  "0x98, 0b10011000: RES 3, B",
  "0x99, 0b10011001: RES 3, C",
  "0x9A, 0b10011010: RES 3, D",
  "0x9B, 0b10011011: RES 3, E",
  "0x9C, 0b10011100: RES 3, H",
  "0x9D, 0b10011101: RES 3, L",
  "0x9E, 0b10011110: RES 3, (HL)",
  "0x9F, 0b10011111: RES 3, A",

  "0xA0, 0b10100000: RES 4, B",
  "0xA1, 0b10100001: RES 4, C",
  "0xA2, 0b10100010: RES 4, D",
  "0xA3, 0b10100011: RES 4, E",
  "0xA4, 0b10100100: RES 4, H",
  "0xA5, 0b10100101: RES 4, L",
  "0xA6, 0b10100110: RES 4, (HL)",
  "0xA7, 0b10100111: RES 4, A",
  "0xA8, 0b10101000: RES 5, B",
  "0xA9, 0b10101001: RES 5, C",
  "0xAA, 0b10101010: RES 5, D",
  "0xAB, 0b10101011: RES 5, E",
  "0xAC, 0b10101100: RES 5, H",
  "0xAD, 0b10101101: RES 5, L",
  "0xAE, 0b10101110: RES 5, (HL)",
  "0xAF, 0b10101111: RES 5, A",

  "0xB0, 0b10110000: RES 6, B",
  "0xB1, 0b10110001: RES 6, C",
  "0xB2, 0b10110010: RES 6, D",
  "0xB3, 0b10110011: RES 6, E",
  "0xB4, 0b10110100: RES 6, H",
  "0xB5, 0b10110101: RES 6, L",
  "0xB6, 0b10110110: RES 6, (HL)",
  "0xB7, 0b10110111: RES 6, A",
  "0xB8, 0b10111000: RES 7, B",
  "0xB9, 0b10111001: RES 7, C",
  "0xBA, 0b10111010: RES 7, D",
  "0xBB, 0b10111011: RES 7, E",
  "0xBC, 0b10111100: RES 7, H",
  "0xBD, 0b10111101: RES 7, L",
  "0xBE, 0b10111110: RES 7, (HL)",
  "0xBF, 0b10111111: RES 7, A",

  "0xC0, 0b11000000: SET 0, B",
  "0xC1, 0b11000001: SET 0, C",
  "0xC2, 0b11000010: SET 0, D",
  "0xC3, 0b11000011: SET 0, E",
  "0xC4, 0b11000100: SET 0, H",
  "0xC5, 0b11000101: SET 0, L",
  "0xC6, 0b11000110: SET 0, (HL)",
  "0xC7, 0b11000111: SET 0, A",
  "0xC8, 0b11001000: SET 1, B",
  "0xC9, 0b11001001: SET 1, C",
  "0xCA, 0b11001010: SET 1, D",
  "0xCB, 0b11001011: SET 1, E",
  "0xCC, 0b11001100: SET 1, H",
  "0xCD, 0b11001101: SET 1, L",
  "0xCE, 0b11001110: SET 1, (HL)",
  "0xCF, 0b11001111: SET 1, A",

  "0xD0, 0b11010000: SET 2, B",
  "0xD1, 0b11010001: SET 2, C",
  "0xD2, 0b11010010: SET 2, D",
  "0xD3, 0b11010011: SET 2, E",
  "0xD4, 0b11010100: SET 2, H",
  "0xD5, 0b11010101: SET 2, L",
  "0xD6, 0b11010110: SET 2, (HL)",
  "0xD7, 0b11010111: SET 2, A",
  "0xD8, 0b11011000: SET 3, B",
  "0xD9, 0b11011001: SET 3, C",
  "0xDA, 0b11011010: SET 3, D",
  "0xDB, 0b11011011: SET 3, E",
  "0xDC, 0b11011100: SET 3, H",
  "0xDD, 0b11011101: SET 3, L",
  "0xDE, 0b11011110: SET 3, (HL)",
  "0xDF, 0b11011111: SET 3, A",

  "0xE0, 0b11100000: SET 4, B",
  "0xE1, 0b11100001: SET 4, C",
  "0xE2, 0b11100010: SET 4, D",
  "0xE3, 0b11100011: SET 4, E",
  "0xE4, 0b11100100: SET 4, H",
  "0xE5, 0b11100101: SET 4, L",
  "0xE6, 0b11100110: SET 4, (HL)",
  "0xE7, 0b11100111: SET 4, A",
  "0xE8, 0b11101000: SET 5, B",
  "0xE9, 0b11101001: SET 5, C",
  "0xEA, 0b11101010: SET 5, D",
  "0xEB, 0b11101011: SET 5, E",
  "0xEC, 0b11101100: SET 5, H",
  "0xED, 0b11101101: SET 5, L",
  "0xEE, 0b11101110: SET 5, (HL)",
  "0xEF, 0b11101111: SET 5, A",

  "0xF0, 0b11110000: SET 6, B",
  "0xF1, 0b11110001: SET 6, C",
  "0xF2, 0b11110010: SET 6, D",
  "0xF3, 0b11110011: SET 6, E",
  "0xF4, 0b11110100: SET 6, H",
  "0xF5, 0b11110101: SET 6, L",
  "0xF6, 0b11110110: SET 6, (HL)",
  "0xF7, 0b11110111: SET 6, A",
  "0xF8, 0b11111000: SET 7, B",
  "0xF9, 0b11111001: SET 7, C",
  "0xFA, 0b11111010: SET 7, D",
  "0xFB, 0b11111011: SET 7, E",
  "0xFC, 0b11111100: SET 7, H",
  "0xFD, 0b11111101: SET 7, L",
  "0xFE, 0b11111110: SET 7, (HL)",
  "0xFF, 0b11111111: SET 7, A"
};

GBC::CentralProcessingUnit::CentralProcessingUnit(GBC::GameBoyColor& gbc) :
  _gbc(gbc),
  _status(Status::StatusRun),
  _ime(InterruptMasterEnable::IMEDisabled),
  _instructions(),
  _opcode(0),
  _set(InstructionSet::InstructionSetDefault),
  _parameters(),
  _rAF{ .u16 = 0x0000 },
  _rBC{ .u16 = 0x0000 },
  _rDE{ .u16 = 0x0000 },
  _rHL{ .u16 = 0x0000 },
  _rSP{ .u16 = 0x0000 },
  _rPC{ .u16 = 0x0000 },
  _rW{ .u16 = 0x0000 }
{}

void  GBC::CentralProcessingUnit::simulate()
{
  // CPU is running, execute instruction
  if (_status == Status::StatusRun)
  {
    // Execute current instruction
    if (_instructions.empty() == false) {
      _instructions.front()(*this);
      _instructions.pop();
    }

    // Fetch next instruction
    if (_instructions.empty() == true && _status == Status::StatusRun)
      simulateFetch();
  }

  // CPU is stopped, wait for interrupt
  else
    simulateInterrupt();
}

void  GBC::CentralProcessingUnit::simulateFetch()
{
  // Check for interrupt before fetching a new instruction
  simulateInterrupt();

  // Stop in case of interrupt
  if (_instructions.empty() == false)
    return;

  // Fetch opcode
  _opcode = _gbc.read(_rPC.u16);
  _set = InstructionSet::InstructionSetDefault;

  // Add instructions
  _rPC.u16 += 1;
  _opcodes[_opcode](*this);
}

void  GBC::CentralProcessingUnit::simulateInterrupt()
{
  // Enable scheduled interrupt flag for next tick
  if (_ime == InterruptMasterEnable::IMEScheduled) {
    _ime = InterruptMasterEnable::IMEEnabled;
    return;
  }

  // Check if interrupts are enabled
  if (_ime != InterruptMasterEnable::IMEEnabled && _status != Status::StatusHalt)
    return;

  // Define interrupt priority
  auto interrupts = {
    GBC::GameBoyColor::Interrupt::InterruptVBlank,
    GBC::GameBoyColor::Interrupt::InterruptLcdStat,
    GBC::GameBoyColor::Interrupt::InterruptTimer,
    GBC::GameBoyColor::Interrupt::InterruptSerial,
    GBC::GameBoyColor::Interrupt::InterruptJoypad
  };

  // Interrupt Enable register
  auto interruptEnable = _gbc.read(0xFFFF);

  // Get Interrupt Flag register
  auto interruptFlag = _gbc.readIo(GBC::GameBoyColor::IO::IF);

  // Check each interrupt type
  for (auto interrupt : interrupts)
  {
    // Check interrupt flag on IE
    if ((interruptEnable & interrupt) == 0)
      continue;

    // Check interrupt flag on IF
    if ((interruptFlag & interrupt) == 0)
      continue;

    // Call interrupt handler
    if (_ime == InterruptMasterEnable::IMEEnabled)
    {
      // Reset Interrupt Flag register
      _gbc.writeIo(GBC::GameBoyColor::IO::IF, (std::uint8_t)(interruptFlag & ~interrupt));

      // Disable interrupt
      _ime = InterruptMasterEnable::IMEDisabled;

      std::uint16_t handler = 0x0040;

      // Compute interrupt destination
      while ((interrupt = (GBC::GameBoyColor::Interrupt)(interrupt >> 1)) != 0)
        handler += 0x0008;

      // Call interrupt handler
      _rW.u16 = handler;
      instruction_INTERRUPT();
    }

    // Resume execution after an interrupt
    _status = Status::StatusRun;

    break;
  }
}

void  GBC::CentralProcessingUnit::save(std::ofstream& file) const
{
  // Save CPU variables
  _gbc.save(file, "CPU_STATUS", _status);
  _gbc.save(file, "CPU_OPCODE", _opcode);
  _gbc.save(file, "CPU_SET", _set);
  _gbc.save(file, "CPU_STEP", _instructions.size());
  _gbc.save(file, "CPU_IME", _status);
  _gbc.save(file, "CPU_RAF", _rAF);
  _gbc.save(file, "CPU_RBC", _rBC);
  _gbc.save(file, "CPU_RDE", _rDE);
  _gbc.save(file, "CPU_RHL", _rHL);
  _gbc.save(file, "CPU_RSP", _rSP);
  _gbc.save(file, "CPU_RPC", _rPC);
  _gbc.save(file, "CPU_RW", _rW);
}

void  GBC::CentralProcessingUnit::load(std::ifstream& file)
{
  std::size_t step = 0;

  // Load CPU variable
  _gbc.load(file, "CPU_STATUS", _status);
  _gbc.load(file, "CPU_OPCODE", _opcode);
  _gbc.load(file, "CPU_SET", _set);
  _gbc.load(file, "CPU_STEP", step);
  _gbc.load(file, "CPU_IME", _status);
  _gbc.load(file, "CPU_RAF", _rAF);
  _gbc.load(file, "CPU_RBC", _rBC);
  _gbc.load(file, "CPU_RDE", _rDE);
  _gbc.load(file, "CPU_RHL", _rHL);
  _gbc.load(file, "CPU_RSP", _rSP);
  _gbc.load(file, "CPU_RPC", _rPC);
  _gbc.load(file, "CPU_RW", _rW);

  // Clear instruction queue
  while (_instructions.empty() == false)
    _instructions.pop();

  // Restore instruction queue
  switch (_set) {
  case InstructionSet::InstructionSetDefault: _opcodes[_opcode](*this); break;
  case InstructionSet::InstructionSetBitwise: _opcodesCb[_opcode](*this); break;
  case InstructionSet::InstructionSetSpecial: _opcodesSpecial[_opcode](*this); break;
  default: throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // Pop already executed sub-instructions
  while (_instructions.size() > step)
    _instructions.pop();
}

GBC::CentralProcessingUnit::Instructions::Instructions() :
  _queue{ 0 },
  _size(0),
  _index(0)
{}

bool  GBC::CentralProcessingUnit::Instructions::empty() const
{
  // Return true if the queue is empty
  return _size == 0;
}

std::size_t GBC::CentralProcessingUnit::Instructions::size() const
{
  // Return number of elements in queue
  return _size;
}

GBC::CentralProcessingUnit::Instructions::Instruction GBC::CentralProcessingUnit::Instructions::front() const
{
#ifdef _DEBUG
  if (empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Get next element of queue
  return _queue[_index];
}

void  GBC::CentralProcessingUnit::Instructions::pop()
{
  // Increment index, reduce size
  _index = (_index + 1) % _queue.size();
  _size -= 1;
}


void  GBC::CentralProcessingUnit::Instructions::push(GBC::CentralProcessingUnit::Instructions::Instruction instruction)
{
#ifdef _DEBUG
  if (size() == _queue.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
#endif

  // Push element to queue
  _queue[(_index + _size) % _queue.size()] = instruction;
  _size += 1;
}

void  GBC::CentralProcessingUnit::instruction_NOP()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_STOP()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if (cpu._gbc._io[GBC::GameBoyColor::IO::KEY1] & 0b00000001)
      cpu._gbc._io[GBC::GameBoyColor::IO::KEY1] ^= 0b10000001;
    else
      cpu._status = Status::StatusStop;
    // TODO: 8200 cycle wait
    });
}

void  GBC::CentralProcessingUnit::instruction_HALT()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._status = Status::StatusHalt;
    });
}

void  GBC::CentralProcessingUnit::instruction_DI()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._ime = InterruptMasterEnable::IMEDisabled;
    });
}

void  GBC::CentralProcessingUnit::instruction_EI()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._ime = InterruptMasterEnable::IMEScheduled;
    });
}

void  GBC::CentralProcessingUnit::instruction_INTERRUPT()
{
  _opcode = 0;
  _set = InstructionSet::InstructionSetSpecial;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rSP.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._rPC.u8.high);
    cpu._rSP.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._rPC.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC.u16 = cpu._rW.u16;
    });
}

void  GBC::CentralProcessingUnit::instruction_JR_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC.u16 += cpu._rW.s8.low;
    });
}

void  GBC::CentralProcessingUnit::instruction_JR_c0_n(Register::Flag flag)
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _parameters[0].u8 = flag;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if (!(cpu._rAF.u8.low & cpu._parameters[0].u8))
      cpu.instruction_JR_c_execute();
    });
}

void  GBC::CentralProcessingUnit::instruction_JR_c1_n(Register::Flag flag)
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _parameters[0].u8 = flag;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if (cpu._rAF.u8.low & cpu._parameters[0].u8)
      cpu.instruction_JR_c_execute();
    });
}

void  GBC::CentralProcessingUnit::instruction_JR_c_execute()
{
  _opcode = 1;
  _set = GBC::CentralProcessingUnit::InstructionSet::InstructionSetSpecial;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC.u16 += cpu._rW.s8.low;
    });
}

void  GBC::CentralProcessingUnit::instruction_JP_nn()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC = cpu._rW;
    });
}

void  GBC::CentralProcessingUnit::instruction_JP_c0_nn(Register::Flag flag)
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _parameters[0].u8 = flag;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if (!(cpu._rAF.u8.low & cpu._parameters[0].u8))
      cpu.instruction_JP_c_execute();
    });
}

void  GBC::CentralProcessingUnit::instruction_JP_c1_nn(Register::Flag flag)
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _parameters[0].u8 = flag;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if (cpu._rAF.u8.low & cpu._parameters[0].u8)
      cpu.instruction_JP_c_execute();
    });
}

void  GBC::CentralProcessingUnit::instruction_JP_c_execute()
{
  _opcode = 2;
  _set = GBC::CentralProcessingUnit::InstructionSet::InstructionSetSpecial;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC = cpu._rW;
    });
}

void  GBC::CentralProcessingUnit::instruction_JP_HL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC = cpu._rHL;
    });
}

void  GBC::CentralProcessingUnit::instruction_CALL_nn()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rSP.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._rPC.u8.high);
    cpu._rSP.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._rPC.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC = cpu._rW;
    });
}

void  GBC::CentralProcessingUnit::instruction_CALL_c0_nn(Register::Flag flag)
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _parameters[0].u8 = flag;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if (!(cpu._rAF.u8.low & cpu._parameters[0].u8)) {
      cpu.instruction_CALL_c_execute();
      cpu._rSP.u16 -= 1;
    }
    });
}

void  GBC::CentralProcessingUnit::instruction_CALL_c1_nn(Register::Flag flag)
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _parameters[0].u8 = flag;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if (cpu._rAF.u8.low & cpu._parameters[0].u8) {
      cpu.instruction_CALL_c_execute();
      cpu._rSP.u16 -= 1;
    }
    });
}

void  GBC::CentralProcessingUnit::instruction_CALL_c_execute()
{
  _opcode = 3;
  _set = GBC::CentralProcessingUnit::InstructionSet::InstructionSetSpecial;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._rPC.u8.high);
    cpu._rSP.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._rPC.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC = cpu._rW;
    });
}

void  GBC::CentralProcessingUnit::instruction_RST(std::uint16_t address)
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rSP.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._rPC.u8.high);
    cpu._rSP.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._rPC.u8.low);
    });
  _parameters[0].u16 = address;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC.u16 = cpu._parameters[0].u16;
    });
}

void  GBC::CentralProcessingUnit::instruction_RET()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rSP.u16);
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rSP.u16);
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC = cpu._rW;
    });
}

void  GBC::CentralProcessingUnit::instruction_RET_c0(Register::Flag flag)
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
  _parameters[0].u8 = flag;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if (!(cpu._rAF.u8.low & cpu._parameters[0].u8))
      cpu.instruction_RET_c_execute();
    });
}

void  GBC::CentralProcessingUnit::instruction_RET_c1(Register::Flag flag)
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
  _parameters[0].u8 = flag;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
      if (cpu._rAF.u8.low & cpu._parameters[0].u8)
        cpu.instruction_RET_c_execute();
    });
}

void  GBC::CentralProcessingUnit::instruction_RET_c_execute()
{
  _opcode = 4;
  _set = GBC::CentralProcessingUnit::InstructionSet::InstructionSetSpecial;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rSP.u16);
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rSP.u16);
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC = cpu._rW;
    });
}

void  GBC::CentralProcessingUnit::instruction_RETI()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rSP.u16);
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rSP.u16);
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rPC = cpu._rW;
    cpu._ime = InterruptMasterEnable::IMEEnabled;
    });
}

void  GBC::CentralProcessingUnit::instruction_INC_r(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pu8) += 1;
    cpu.setFlag<Register::Z>((*cpu._parameters[0].pu8) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(((*cpu._parameters[0].pu8) & 0b00001111) == 0b00000000);
    });
}

void  GBC::CentralProcessingUnit::instruction_DEC_r(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pu8) -= 1;
    cpu.setFlag<Register::Z>((*cpu._parameters[0].pu8) == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>(((*cpu._parameters[0].pu8) & 0b00001111) == 0b00001111);
    });
}

void  GBC::CentralProcessingUnit::instruction_INC_rr(Register& reg16)
{
  _parameters[0].pr = &reg16;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pr).u8.low += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if ((*cpu._parameters[0].pr).u8.low == 0)
      (*cpu._parameters[0].pr).u8.high += 1;
    });
}

void  GBC::CentralProcessingUnit::instruction_DEC_rr(Register& reg16)
{
  _parameters[0].pr = &reg16;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pr).u8.low -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if ((*cpu._parameters[0].pr).u8.low == 0b11111111)
      (*cpu._parameters[0].pr).u8.high -= 1;
    });
}

void  GBC::CentralProcessingUnit::instruction_INC_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low += 1;
    cpu.setFlag<Register::Z>(cpu._rW.u8.low == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>((cpu._rW.u8.low & 0b00001111) == 0b00000000);
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_DEC_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low -= 1;
    cpu.setFlag<Register::Z>(cpu._rW.u8.low == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((cpu._rW.u8.low & 0b00001111) == 0b00001111);
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_CB()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._opcode = cpu._gbc.read(cpu._rPC.u16);
    cpu._set = InstructionSet::InstructionSetBitwise;
    cpu._rPC.u16 += 1;
    _opcodesCb[cpu._opcode](cpu);
    });
}

void  GBC::CentralProcessingUnit::instruction_RLC(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>((*cpu._parameters[0].pu8) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>((*cpu._parameters[0].pu8) & 0b10000000);
    (*cpu._parameters[0].pu8) = ((*cpu._parameters[0].pu8) << 1) | ((*cpu._parameters[0].pu8) >> 7);
    });
}

void  GBC::CentralProcessingUnit::instruction_RLC_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>(cpu._rW.u8.low == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rW.u8.low & 0b10000000);
    cpu._rW.u8.low = (cpu._rW.u8.low << 1) | (cpu._rW.u8.low >> 7);
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_RLCA()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>(false);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rAF.u8.high & 0b10000000);
    cpu._rAF.u8.high = (cpu._rAF.u8.high << 1) | (cpu._rAF.u8.high >> 7);
    });
}

void  GBC::CentralProcessingUnit::instruction_RRC(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>((*cpu._parameters[0].pu8) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>((*cpu._parameters[0].pu8) & 0b00000001);
    (*cpu._parameters[0].pu8) = ((*cpu._parameters[0].pu8) >> 1) | (((*cpu._parameters[0].pu8) & 0b00000001) ? 0b10000000 : 0b00000000);
    });
}

void  GBC::CentralProcessingUnit::instruction_RRC_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>(cpu._rW.u8.low == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rW.u8.low & 0b00000001);
    cpu._rW.u8.low = (cpu._rW.u8.low >> 1) | ((cpu._rW.u8.low & 0b00000001) ? 0b10000000 : 0b00000000);
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_RRCA()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>(false);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rAF.u8.high & 0b00000001);
    cpu._rAF.u8.high = (cpu._rAF.u8.high >> 1) | ((cpu._rAF.u8.high & 0b00000001) ? 0b10000000 : 0b00000000);
    });
}

void  GBC::CentralProcessingUnit::instruction_RL(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 0b00000001 : 0b00000000;

    cpu.setFlag<Register::Z>(((*cpu._parameters[0].pu8) & 0b01111111) == 0 && carry == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>((*cpu._parameters[0].pu8) & 0b10000000);
    (*cpu._parameters[0].pu8) = ((*cpu._parameters[0].pu8) << 1) | carry;
    });
}

void  GBC::CentralProcessingUnit::instruction_RL_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 0b00000001 : 0b00000000;

    cpu.setFlag<Register::Z>((cpu._rW.u8.low & 0b01111111) == 0 && carry == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rW.u8.low & 0b10000000);
    cpu._rW.u8.low = (cpu._rW.u8.low << 1) | carry;
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_RLA()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 0b00000001 : 0b00000000;

    cpu.setFlag<Register::Z>(false);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rAF.u8.high & 0b10000000);
    cpu._rAF.u8.high = (cpu._rAF.u8.high << 1) | carry;
    });
}

void  GBC::CentralProcessingUnit::instruction_RR(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 0b10000000 : 0b00000000;

    cpu.setFlag<Register::Z>(((*cpu._parameters[0].pu8) & 0b11111110) == 0 && carry == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>((*cpu._parameters[0].pu8) & 0b00000001);
    (*cpu._parameters[0].pu8) = ((*cpu._parameters[0].pu8) >> 1) | carry;
    });
}

void  GBC::CentralProcessingUnit::instruction_RR_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 0b10000000 : 0b00000000;

    cpu.setFlag<Register::Z>((cpu._rW.u8.low & 0b11111110) == 0 && carry == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rW.u8.low & 0b00000001);
    cpu._rW.u8.low = (cpu._rW.u8.low >> 1) | carry;
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_RRA()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 0b10000000 : 0b00000000;

    cpu.setFlag<Register::Z>(false);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rAF.u8.high & 0b00000001);
    cpu._rAF.u8.high = (cpu._rAF.u8.high >> 1) | carry;
    });
}

void  GBC::CentralProcessingUnit::instruction_SLA(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>(((*cpu._parameters[0].pu8) & 0b01111111) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>((*cpu._parameters[0].pu8) & 0b10000000);
    (*cpu._parameters[0].pu8) <<= 1;
    });
}

void  GBC::CentralProcessingUnit::instruction_SLA_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>((cpu._rW.u8.low & 0b01111111) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rW.u8.low & 0b10000000);
    cpu._rW.u8.low <<= 1;
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_SRA(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>(((*cpu._parameters[0].pu8) & 0b11111110) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>((*cpu._parameters[0].pu8) & 0b00000001);
    (*cpu._parameters[0].pu8) = ((*cpu._parameters[0].pu8) >> 1) | ((*cpu._parameters[0].pu8) & 0b10000000);
    });
}

void  GBC::CentralProcessingUnit::instruction_SRA_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>((cpu._rW.u8.low & 0b11111110) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rW.u8.low & 0b00000001);
    cpu._rW.u8.low = (cpu._rW.u8.low >> 1) | (cpu._rW.u8.low & 0b10000000);
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_SWAP(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>((*cpu._parameters[0].pu8) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(false);
    (*cpu._parameters[0].pu8) = ((*cpu._parameters[0].pu8) >> 4) | ((*cpu._parameters[0].pu8) << 4);
    });
}

void  GBC::CentralProcessingUnit::instruction_SWAP_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>(cpu._rW.u8.low == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(false);
    cpu._rW.u8.low = (cpu._rW.u8.low >> 4) | (cpu._rW.u8.low << 4);
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_SRL(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>(((*cpu._parameters[0].pu8) & 0b11111110) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>((*cpu._parameters[0].pu8) & 0b00000001);
    (*cpu._parameters[0].pu8) >>= 1;
    });
}

void  GBC::CentralProcessingUnit::instruction_SRL_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>((cpu._rW.u8.low & 0b11111110) == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(cpu._rW.u8.low & 0b00000001);
    cpu._rW.u8.low >>= 1;
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_DAA()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    if (cpu.getFlag<Register::N>() == true) {
      if (cpu.getFlag<Register::H>() == true)
        cpu._rAF.u8.high += 0xFA;
      if (cpu.getFlag<Register::C>() == true)
        cpu._rAF.u8.high += 0xA0;
    }
    else {
      std::uint16_t a = cpu._rAF.u8.high;

      if ((a & 0b0000000000001111) > 0b00001001 || cpu.getFlag<Register::H>() == true)
        a += 0b00000110;
      if ((a & 0b0000000111110000) > 0b10010000 || cpu.getFlag<Register::C>() == true) {
        a += 0b01100000;
        cpu.setFlag<Register::C>(true);
      }
      else
        cpu.setFlag<Register::C>(false);
      cpu._rAF.u8.high = (std::uint8_t)a;
    }
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    });
}

void  GBC::CentralProcessingUnit::instruction_CPL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high = ~cpu._rAF.u8.high;
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>(true);
    });
}

void  GBC::CentralProcessingUnit::instruction_SCF()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(true);
    });
}

void  GBC::CentralProcessingUnit::instruction_CCF()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(!cpu.getFlag<Register::C>());
    });
}

template <unsigned int Bit>
void  GBC::CentralProcessingUnit::instruction_BIT(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu.setFlag<Register::Z>(!(cpu._parameters[0].u8 & (0b00000001 << Bit)));
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(true);
    });
}

template <unsigned int Bit>
void  GBC::CentralProcessingUnit::instruction_BIT_pHL()
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
void  GBC::CentralProcessingUnit::instruction_RES(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pu8) &= ~(0b00000001 << Bit);
    });
}

template <unsigned int Bit>
void  GBC::CentralProcessingUnit::instruction_RES_pHL()
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
void  GBC::CentralProcessingUnit::instruction_SET(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pu8) |= 0b00000001 << Bit;
    });
}

template <unsigned int Bit>
void  GBC::CentralProcessingUnit::instruction_SET_pHL()
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

void  GBC::CentralProcessingUnit::instruction_ADD_r(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._parameters[0].u8;

    cpu._rAF.u8.high = left + right;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>((left & 0b00001111) + (right & 0b00001111) > 0b00001111);
    cpu.setFlag<Register::C>((std::uint16_t)left + (std::uint16_t)right > 0b11111111);
    });
}

void  GBC::CentralProcessingUnit::instruction_ADD_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;

    cpu._rAF.u8.high = left + right;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>((left & 0b00001111) + (right & 0b00001111) > 0b00001111);
    cpu.setFlag<Register::C>((std::uint16_t)left + (std::uint16_t)right > 0b11111111);
    });
}

void  GBC::CentralProcessingUnit::instruction_ADD_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;

    cpu._rAF.u8.high = left + right;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>((left & 0b00001111) + (right & 0b00001111) > 0b00001111);
    cpu.setFlag<Register::C>((std::uint16_t)left + (std::uint16_t)right > 0b11111111);
    });
}

void  GBC::CentralProcessingUnit::instruction_ADC_r(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._parameters[0].u8;
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 1 : 0;

    cpu._rAF.u8.high = left + right + carry;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>((left & 0b00001111) + (right & 0b00001111) + carry > 0b00001111);
    cpu.setFlag<Register::C>((std::uint16_t)left + (std::uint16_t)right + (std::uint16_t)carry > 0b11111111);
    });
}

void  GBC::CentralProcessingUnit::instruction_ADC_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 1 : 0;

    cpu._rAF.u8.high = left + right + carry;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>((left & 0b00001111) + (right & 0b00001111) + carry > 0b00001111);
    cpu.setFlag<Register::C>((std::uint16_t)left + (std::uint16_t)right + (std::uint16_t)carry > 0b11111111);
    });
}

void  GBC::CentralProcessingUnit::instruction_ADC_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 1 : 0;

    cpu._rAF.u8.high = left + right + carry;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>((left & 0b00001111) + (right & 0b00001111) + carry > 0b00001111);
    cpu.setFlag<Register::C>((std::uint16_t)left + (std::uint16_t)right + (std::uint16_t)carry > 0b11111111);
    });
}

void  GBC::CentralProcessingUnit::instruction_SUB_r(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._parameters[0].u8;

    cpu._rAF.u8.high = left - right;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((left & 0b00001111) < (right & 0b00001111));
    cpu.setFlag<Register::C>(left < right);
    });
}

void  GBC::CentralProcessingUnit::instruction_SUB_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;

    cpu._rAF.u8.high = left - right;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((left & 0b00001111) < (right & 0b00001111));
    cpu.setFlag<Register::C>(left < right);
    });
}

void  GBC::CentralProcessingUnit::instruction_SUB_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;

    cpu._rAF.u8.high = left - right;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((left & 0b00001111) < (right & 0b00001111));
    cpu.setFlag<Register::C>(left < right);
    });
}

void  GBC::CentralProcessingUnit::instruction_SBC_r(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._parameters[0].u8;
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 1 : 0;

    cpu._rAF.u8.high = left - right - carry;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((std::uint16_t)(left & 0b00001111) < (std::uint16_t)(right & 0b00001111) + (std::uint16_t)carry);
    cpu.setFlag<Register::C>((std::uint16_t)left < (std::uint16_t)right + (std::uint16_t)carry);
    });
}

void  GBC::CentralProcessingUnit::instruction_SBC_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 1 : 0;

    cpu._rAF.u8.high = left - right - carry;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((std::uint16_t)(left & 0b00001111) < (std::uint16_t)(right & 0b00001111) + (std::uint16_t)carry);
    cpu.setFlag<Register::C>((std::uint16_t)left < (std::uint16_t)right + (std::uint16_t)carry);
    });
}

void  GBC::CentralProcessingUnit::instruction_SBC_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;
    std::uint8_t  carry = cpu.getFlag<Register::C>() ? 1 : 0;

    cpu._rAF.u8.high = left - right - carry;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((std::uint16_t)(left & 0b00001111) < (std::uint16_t)(right & 0b00001111) + (std::uint16_t)carry);
    cpu.setFlag<Register::C>((std::uint16_t)left < (std::uint16_t)right + (std::uint16_t)carry);
    });
}

void  GBC::CentralProcessingUnit::instruction_AND_r(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high &= cpu._parameters[0].u8;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(true);
    cpu.setFlag<Register::C>(false);
    });
}

void  GBC::CentralProcessingUnit::instruction_AND_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high &= cpu._rW.u8.low;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(true);
    cpu.setFlag<Register::C>(false);
    });
}

void  GBC::CentralProcessingUnit::instruction_AND_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high &= cpu._rW.u8.low;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(true);
    cpu.setFlag<Register::C>(false);
    });
}

void  GBC::CentralProcessingUnit::instruction_XOR_r(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high ^= cpu._parameters[0].u8;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(false);
    });
}

void  GBC::CentralProcessingUnit::instruction_XOR_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high ^= cpu._rW.u8.low;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(false);
    });
}

void  GBC::CentralProcessingUnit::instruction_XOR_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high ^= cpu._rW.u8.low;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(false);
    });
}

void  GBC::CentralProcessingUnit::instruction_OR_r(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high |= cpu._parameters[0].u8;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(false);
    });
}

void  GBC::CentralProcessingUnit::instruction_OR_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high |= cpu._rW.u8.low;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(false);
    });
}

void  GBC::CentralProcessingUnit::instruction_OR_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high |= cpu._rW.u8.low;
    cpu.setFlag<Register::Z>(cpu._rAF.u8.high == 0);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(false);
    cpu.setFlag<Register::C>(false);
    });
}

void  GBC::CentralProcessingUnit::instruction_CP_r(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._parameters[0].u8;
    std::uint8_t  result = left - right;

    cpu.setFlag<Register::Z>(result == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((left & 0b00001111) < (right & 0b00001111));
    cpu.setFlag<Register::C>(left < right);
    });
}

void  GBC::CentralProcessingUnit::instruction_CP_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;
    std::uint8_t  result = left - right;

    cpu.setFlag<Register::Z>(result == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((left & 0b00001111) < (right & 0b00001111));
    cpu.setFlag<Register::C>(left < right);
    });
}

void  GBC::CentralProcessingUnit::instruction_CP_pHL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint8_t  left = cpu._rAF.u8.high;
    std::uint8_t  right = cpu._rW.u8.low;
    std::uint8_t  result = left - right;

    cpu.setFlag<Register::Z>(result == 0);
    cpu.setFlag<Register::N>(true);
    cpu.setFlag<Register::H>((std::uint16_t)(left & 0b00001111) < (std::uint16_t)(right & 0b00001111));
    cpu.setFlag<Register::C>((std::uint16_t)left < (std::uint16_t)right);
    });
}

void  GBC::CentralProcessingUnit::instruction_LD_r_r(std::uint8_t& reg8_destination, std::uint8_t reg8_source)
{
  _parameters[0].pu8 = &reg8_destination;
  _parameters[1].u8 = reg8_source;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pu8) = cpu._parameters[1].u8;
    });
}

void  GBC::CentralProcessingUnit::instruction_LD_r_n(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pu8) = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_r_pHL(std::uint8_t& reg8)
{
  _parameters[0].pu8 = &reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pu8) = cpu._gbc.read(cpu._rHL.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_pHL_r(std::uint8_t reg8)
{
  _parameters[0].u8 = reg8;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rHL.u16, cpu._parameters[0].u8);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_pHL_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rHL.u16, cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_A_prr(Register reg16)
{
  _parameters[0].u16 = reg16.u16;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high = cpu._gbc.read(cpu._parameters[0].u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_prr_A(Register reg16)
{
  _parameters[0].u16 = reg16.u16;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._parameters[0].u16, cpu._rAF.u8.high);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_A_pnn()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high = cpu._gbc.read(cpu._rW.u16);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_pnn_A()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rW.u16, cpu._rAF.u8.high);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LDH_A_pC()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high = cpu._gbc.read(0xFF00 + cpu._rBC.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LDH_pC_A()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(0xFF00 + cpu._rBC.u8.low, cpu._rAF.u8.high);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LDH_A_pn()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high = cpu._gbc.read(0xFF00 + cpu._rW.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LDH_pn_A()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(0xFF00 + cpu._rW.u8.low, cpu._rAF.u8.high);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_A_HLd()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high = cpu._gbc.read(cpu._rHL.u16);
    cpu._rHL.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_HLd_A()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rHL.u16, cpu._rAF.u8.high);
    cpu._rHL.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_A_HLi()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high = cpu._gbc.read(cpu._rHL.u16);
    cpu._rHL.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_HLi_A()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rHL.u16, cpu._rAF.u8.high);
    cpu._rHL.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_ADD_HL_rr(Register reg16)
{
  _parameters[0].u16 = reg16.u16;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::uint16_t left = cpu._rHL.u16;
    std::uint16_t right = cpu._parameters[0].u16;

    cpu._rHL.u16 = left + right;
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>((left & 0b0000111111111111) + (right & 0b0000111111111111) > 0b0000111111111111);
    cpu.setFlag<Register::C>((std::uint32_t)left + (std::uint32_t)right > 0b1111111111111111);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_ADD_SP_n()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    std::int32_t  n = cpu._rW.s8.low;
    std::int32_t  sp = cpu._rSP.u16;

    cpu._rSP.u16 = sp + n;
    cpu.setFlag<Register::Z>(false);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>((sp & 0b00001111) + (n & 0b00001111) > 0b00001111);
    cpu.setFlag<Register::C>((sp & 0b11111111) + (n & 0b11111111) > 0b11111111);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_rr_nn(Register& reg16)
{
  _parameters[0].pr = &reg16;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pr).u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pr).u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_pnn_SP()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.high = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rW.u16, cpu._rSP.u8.low);
    cpu._rW.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rW.u16, cpu._rSP.u8.high);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_LD_HL_SPn()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rW.u8.low = cpu._gbc.read(cpu._rPC.u16);
    cpu._rPC.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rHL.u16 = cpu._rSP.u16 + cpu._rW.s8.low;
    cpu.setFlag<Register::Z>(false);
    cpu.setFlag<Register::N>(false);
    cpu.setFlag<Register::H>(((std::uint16_t)cpu._rW.s8.low & 0b00001111) + (cpu._rSP.u16 & 0b00001111) > 0b00001111);
    cpu.setFlag<Register::C>(((std::uint16_t)cpu._rW.s8.low & 0b11111111) + (cpu._rSP.u16 & 0b11111111) > 0b11111111);
    });
}

void  GBC::CentralProcessingUnit::instruction_LD_SP_HL()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rSP.u8.low = cpu._rHL.u8.low;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rSP.u8.high = cpu._rHL.u8.high;
    });
}

void  GBC::CentralProcessingUnit::instruction_PUSH_rr(Register reg16)
{
  _parameters[0].r = reg16;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rSP.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._parameters[0].r.u8.high);
    cpu._rSP.u16 -= 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._gbc.write(cpu._rSP.u16, cpu._parameters[0].r.u8.low);
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_POP_rr(Register& reg16)
{
  _parameters[0].pr = &reg16;
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pr).u8.low = cpu._gbc.read(cpu._rSP.u16);
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    (*cpu._parameters[0].pr).u8.high = cpu._gbc.read(cpu._rSP.u16);
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}

void  GBC::CentralProcessingUnit::instruction_POP_AF()
{
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.low = cpu._gbc.read(cpu._rSP.u16) & 0b11110000;
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {
    cpu._rAF.u8.high = cpu._gbc.read(cpu._rSP.u16);
    cpu._rSP.u16 += 1;
    });
  _instructions.push([](GBC::CentralProcessingUnit& cpu) {});
}