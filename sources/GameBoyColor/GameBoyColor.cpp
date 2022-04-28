#include <fstream>
#include <filesystem>

#include "GameBoyColor/GameBoyColor.hpp"
#include "Math/Math.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

#include <iostream>
#include <thread>
#include <chrono>

const std::array<GBC::GameBoyColor::Instruction, 256> GBC::GameBoyColor::_instructions = {
  GBC::GameBoyColor::Instruction {
    .description = "NOP",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x00, 0b00000000: NOP
  GBC::GameBoyColor::Instruction {
    .description = "LD BC, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u16 = gbc.read<std::uint16_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 3;
      gbc._cpuCycle += 12;
    }
  },  // 0x01, 0b00000001: LD BC, nn
  GBC::GameBoyColor::Instruction {
    .description = "LD (BC), A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rBC.u16, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x02, 0b00000010: LD (BC), A
  GBC::GameBoyColor::Instruction {
    .description = "INC BC",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u16 += 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x03, 0b00000011: INC BC
  GBC::GameBoyColor::Instruction {
    .description = "INC B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionInc(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x04, 0b00000100: INC B
  GBC::GameBoyColor::Instruction {
    .description = "DEC B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionDec(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x05, 0b00000101: DEC B
  GBC::GameBoyColor::Instruction {
    .description = "LD B, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0x06, 0b00000110: LD B, n
  GBC::GameBoyColor::Instruction {
    .description = "RLCA",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRlc(gbc._rAF.u8[Register::Hi]);
      gbc._rAF.u8[Register::Lo] &= ~Register::Z;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x07, 0b00000111: RLCA
  GBC::GameBoyColor::Instruction {
    .description = "LD (nn), SP",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint16_t>(gbc.read<std::uint16_t>(gbc._rPC.u16 + 1), gbc._rSP.u16);
      gbc._rPC.u16 += 3;
      gbc._cpuCycle += 20;
    }
  },  // 0x08, 0b00001000: LD (nn), SP
  GBC::GameBoyColor::Instruction {
    .description = "ADD HL, BC",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rHL.u16, gbc._rBC.u16, gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x09, 0b00001001: ADD HL, BC
  GBC::GameBoyColor::Instruction {
    .description = "LD A, (BC)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rBC.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x0A, 0b00001010: LD A, (BC)
  GBC::GameBoyColor::Instruction {
    .description = "DEC BC",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u16 -= 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x0B, 0b00001011: DEC BC
  GBC::GameBoyColor::Instruction {
    .description = "INC C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionInc(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x0C, 0b00001100: INC C
  GBC::GameBoyColor::Instruction {
    .description = "DEC C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionDec(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x0D, 0b00001101: DEC C
  GBC::GameBoyColor::Instruction {
    .description = "LD C, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] = gbc.read<std::uint8_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0x0E, 0b00001110: LD C, n
  GBC::GameBoyColor::Instruction {
    .description = "RRCA",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRrc(gbc._rAF.u8[Register::Hi]);
      gbc._rAF.u8[Register::Lo] &= ~Register::Z;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x0F, 0b00001111: RRCA
  GBC::GameBoyColor::Instruction {
    .description = "STOP (0x10 & 0x00)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (gbc._io[Registers::RegisterKEY1] & 0b00000001)
        gbc._io[Registers::RegisterKEY1] ^= 0b10000001;
      else
        gbc._cpu = GBC::GameBoyColor::CPU::CPUStop;
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8200;
    }
  },  // 0x10, 0b00010000: STOP (0x10 & 0x00)
  GBC::GameBoyColor::Instruction {
    .description = "LD DE, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u16 = gbc.read<std::uint16_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 3;
      gbc._cpuCycle += 12;
    }
  },  // 0x11, 0b00010001: LD DE, nn
  GBC::GameBoyColor::Instruction {
    .description = "LD (DE), A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rDE.u16, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x12, 0b00010010: LD (DE), A
  GBC::GameBoyColor::Instruction {
    .description = "INC DE",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u16 += 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x13, 0b00010011: INC DE
  GBC::GameBoyColor::Instruction {
    .description = "INC D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionInc(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x14, 0b00010100: INC D
  GBC::GameBoyColor::Instruction {
    .description = "DEC D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionDec(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x15, 0b00010101: DEC D
  GBC::GameBoyColor::Instruction {
    .description = "LD D, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0x16, 0b00010110: LD D, n
  GBC::GameBoyColor::Instruction {
    .description = "RLA",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRl(gbc._rAF.u8[Register::Hi]);
      gbc._rAF.u8[Register::Lo] &= ~Register::Z;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x17, 0b00010111: RLA
  GBC::GameBoyColor::Instruction {
    .description = "JR n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += gbc.read<std::int8_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 12;
    }
  },  // 0x18, 0b00011000: JR n
  GBC::GameBoyColor::Instruction {
    .description = "ADD HL, DE",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rHL.u16, gbc._rDE.u16, gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x19, 0b00011001: ADD HL, DE
  GBC::GameBoyColor::Instruction {
    .description = "LD A, (DE)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rDE.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x1A, 0b00011010: LD A, (DE)
  GBC::GameBoyColor::Instruction {
    .description = "DEC DE",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u16 -= 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x1B, 0b00011011: DEC DE
  GBC::GameBoyColor::Instruction {
    .description = "INC E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionInc(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x1C, 0b00011100: INC E
  GBC::GameBoyColor::Instruction {
    .description = "DEC E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionDec(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x1D, 0b00011101: DEC E
  GBC::GameBoyColor::Instruction {
    .description = "LD E, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] = gbc.read<std::uint8_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0x1E, 0b00011110: LD E, n
  GBC::GameBoyColor::Instruction {
    .description = "RRA",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRr(gbc._rAF.u8[Register::Hi]);
      gbc._rAF.u8[Register::Lo] &= ~Register::Z;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x1F, 0b00011111: RRA
  GBC::GameBoyColor::Instruction {
    .description = "JR NZ, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (!(gbc._rAF.u8[Register::Lo] & Register::Z)) {
        gbc._rPC.u16 += gbc.read<std::int8_t>(gbc._rPC.u16 + 1);
        gbc._cpuCycle += 12;
      }
      else
        gbc._cpuCycle += 8;
      gbc._rPC.u16 += 2;
    }
  },  // 0x20, 0b00100000: JR NZ, n
  GBC::GameBoyColor::Instruction {
    .description = "LD HL, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u16 = gbc.read<std::uint16_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 3;
      gbc._cpuCycle += 12;
    }
  },  // 0x21, 0b00100001: LD HL, nn
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL+), A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc._rAF.u8[Register::Hi]);
      gbc._rHL.u16 += 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x22, 0b00100010: LD (HL+), A
  GBC::GameBoyColor::Instruction {
    .description = "INC HL",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u16 += 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x23, 0b00100011: INC HL
  GBC::GameBoyColor::Instruction {
    .description = "INC H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionInc(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x24, 0b00100100: INC H
  GBC::GameBoyColor::Instruction {
    .description = "DEC H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionDec(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x25, 0b00100101: DEC H
  GBC::GameBoyColor::Instruction {
    .description = "LD H, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0x26, 0b00100110: LD H, n
  GBC::GameBoyColor::Instruction {
    .description = "DAA",
    .instruction = [](GBC::GameBoyColor& gbc) {
      //puts("in");
      if (gbc._rAF.u8[Register::Lo] & Register::N) {
        if (gbc._rAF.u8[Register::Lo] & Register::H)
          gbc._rAF.u8[Register::Hi] += 0xFA;
        if (gbc._rAF.u8[Register::Lo] & Register::C)
          gbc._rAF.u8[Register::Hi] += 0xA0;
      }
      else {
        std::uint16_t a = gbc._rAF.u8[Register::Hi];

        if ((a & 0b0000000000001111) > 0b00001001 || (gbc._rAF.u8[Register::Lo] & Register::H))
          a += 0b00000110;
        if ((a & 0b0000000111110000) > 0b10010000 || (gbc._rAF.u8[Register::Lo] & Register::C)) {
          a += 0b01100000;
          gbc._rAF.u8[Register::Lo] |= Register::C;
        }
        else
          gbc._rAF.u8[Register::Lo] &= ~Register::C;
        gbc._rAF.u8[Register::Hi] = (std::uint8_t)a;
      }
      gbc._rAF.u8[Register::Lo] &= ~Register::H;
      if (gbc._rAF.u8[Register::Hi] == 0)
        gbc._rAF.u8[Register::Lo] |= Register::Z;
      else
        gbc._rAF.u8[Register::Lo] &= ~Register::Z;

      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
      //puts("out");
      return;

      std::uint16_t r = gbc._rAF.u8[Register::Hi];

      if ((r & 0b00001111) > 0b00001001)
        r += 0b00000110;
      if ((r & 0b11110000) > 0b10010000)
        r += 0b01100000;
      gbc._rAF.u8[Register::Hi] = (r & 0b11111111);
      if (gbc._rAF.u8[Register::Hi] == 0)
        gbc._rAF.u8[Register::Lo] |= Register::Z;
      else
        gbc._rAF.u8[Register::Lo] &= ~Register::Z;
      gbc._rAF.u8[Register::Lo] &= ~Register::H;
      if (r & 0b1111111100000000)
        gbc._rAF.u8[Register::Lo] |= Register::C;
      else
        gbc._rAF.u8[Register::Lo] &= ~Register::C;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x27, 0b00100111: DAA
  GBC::GameBoyColor::Instruction {
    .description = "JR Z, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (gbc._rAF.u8[Register::Lo] & Register::Z) {
        gbc._rPC.u16 += gbc.read<std::int8_t>(gbc._rPC.u16 + 1);
        gbc._cpuCycle += 12;
      }
      else
        gbc._cpuCycle += 8;
      gbc._rPC.u16 += 2;
    }
  },  // 0x28, 0b00101000: JR Z, n
  GBC::GameBoyColor::Instruction {
    .description = "ADD HL, HL",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rHL.u16, gbc._rHL.u16, gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x29, 0b00101001: ADD HL, HL
  GBC::GameBoyColor::Instruction {
    .description = "LD A, (HL+)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rHL.u16);
      gbc._rHL.u16 += 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x2A, 0b00101010: LD A, (HL+)
  GBC::GameBoyColor::Instruction {
    .description = "DEC HL",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u16 -= 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x2B, 0b00101011: DEC HL
  GBC::GameBoyColor::Instruction {
    .description = "INC L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionInc(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x2C, 0b00101100: INC L
  GBC::GameBoyColor::Instruction {
    .description = "DEC L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionDec(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x2D, 0b00101101: DEC L
  GBC::GameBoyColor::Instruction {
    .description = "LD L, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] = gbc.read<std::uint8_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0x2E, 0b00101110: LD L, n
  GBC::GameBoyColor::Instruction {
    .description = "CPL",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = ~gbc._rAF.u8[Register::Hi];
      gbc._rAF.u8[Register::Lo] |= Register::N;
      gbc._rAF.u8[Register::Lo] |= Register::H;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x2F, 0b00101111: CPL
  GBC::GameBoyColor::Instruction {
    .description = "JR NC, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (!(gbc._rAF.u8[Register::Lo] & Register::C)) {
        gbc._rPC.u16 += gbc.read<std::int8_t>(gbc._rPC.u16 + 1);
        gbc._cpuCycle += 12;
      }
      else
        gbc._cpuCycle += 8;
      gbc._rPC.u16 += 2;
    }
  },  // 0x30, 0b00110000: JR NC, n
  GBC::GameBoyColor::Instruction {
    .description = "LD SP, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rSP.u16 = gbc.read<std::uint16_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 3;
      gbc._cpuCycle += 12;
    }
  },  // 0x31, 0b00110001: LD SP, nn
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL-), A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc._rAF.u8[Register::Hi]);
      gbc._rHL.u16 -= 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x32, 0b00110010: LD (HL-), A
  GBC::GameBoyColor::Instruction {
    .description = "INC SP",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rSP.u16 += 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x33, 0b00110011: INC SP
  GBC::GameBoyColor::Instruction {
    .description = "INC (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionInc(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 12;
    }
  },  // 0x34, 0b00110100: INC (HL)
  GBC::GameBoyColor::Instruction {
    .description = "DEC (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionDec(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 12;
    }
  },  // 0x35, 0b00110101: DEC (HL)
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL), n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc.read<std::uint8_t>(gbc._rPC.u16 + 1));
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 12;
    }
  },  // 0x36, 0b00110110: LD (HL), n
  GBC::GameBoyColor::Instruction {
    .description = "SCF",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Lo] &= ~Register::N;
      gbc._rAF.u8[Register::Lo] &= ~Register::H;
      gbc._rAF.u8[Register::Lo] |= Register::C;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x37, 0b00110111: SCF
  GBC::GameBoyColor::Instruction {
    .description = "JR C, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (gbc._rAF.u8[Register::Lo] & Register::C) {
        gbc._rPC.u16 += gbc.read<std::int8_t>(gbc._rPC.u16 + 1);
        gbc._cpuCycle += 12;
      }
      else
        gbc._cpuCycle += 8;
      gbc._rPC.u16 += 2;
    }
  },  // 0x38, 0b00111000: JR C, n
  GBC::GameBoyColor::Instruction {
    .description = "ADD HL, SP",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rHL.u16, gbc._rSP.u16, gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x39, 0b00111001: ADD HL, SP
  GBC::GameBoyColor::Instruction {
    .description = "LD A, (HL-)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rHL.u16);
      gbc._rHL.u16 -= 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x3A, 0b00111010: LD A, (HL-)
  GBC::GameBoyColor::Instruction {
    .description = "DEC SP",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rSP.u16 -= 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x3B, 0b00111011: DEC SP
  GBC::GameBoyColor::Instruction {
    .description = "INC A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionInc(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x3C, 0b00111100: INC A
  GBC::GameBoyColor::Instruction {
    .description = "DEC A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionDec(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x3D, 0b00111101: DEC A
  GBC::GameBoyColor::Instruction {
    .description = "LD A, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rPC.u16 + 1);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0x3E, 0b00111110: LD A, n
  GBC::GameBoyColor::Instruction {
    .description = "CCF",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Lo] &= ~Register::N;
      gbc._rAF.u8[Register::Lo] &= ~Register::H;
      gbc._rAF.u8[Register::Lo] ^= Register::C;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x3F, 0b00111111: CCF
  GBC::GameBoyColor::Instruction {
    .description = "LD B, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] = gbc._rBC.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x40, 0b01000000: LD B, B
  GBC::GameBoyColor::Instruction {
    .description = "LD B, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] = gbc._rBC.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x41, 0b01000001: LD B, C
  GBC::GameBoyColor::Instruction {
    .description = "LD B, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] = gbc._rDE.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x42, 0b01000010: LD B, D
  GBC::GameBoyColor::Instruction {
    .description = "LD B, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] = gbc._rDE.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x43, 0b01000011: LD B, E
  GBC::GameBoyColor::Instruction {
    .description = "LD B, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] = gbc._rHL.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x44, 0b01000100: LD B, H
  GBC::GameBoyColor::Instruction {
    .description = "LD B, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] = gbc._rHL.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x45, 0b01000101: LD B, L
  GBC::GameBoyColor::Instruction {
    .description = "LD B, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x46, 0b01000110: LD B, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "LD B, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] = gbc._rAF.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x47, 0b01000111: LD B, A
  GBC::GameBoyColor::Instruction {
    .description = "LD C, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] = gbc._rBC.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x48, 0b01001000: LD C, B
  GBC::GameBoyColor::Instruction {
    .description = "LD C, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] = gbc._rBC.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x49, 0b01001001: LD C, C
  GBC::GameBoyColor::Instruction {
    .description = "LD C, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] = gbc._rDE.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x4A, 0b01001010: LD C, D
  GBC::GameBoyColor::Instruction {
    .description = "LD C, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] = gbc._rDE.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x4B, 0b01001011: LD C, E
  GBC::GameBoyColor::Instruction {
    .description = "LD C, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] = gbc._rHL.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x4C, 0b01001100: LD C, H
  GBC::GameBoyColor::Instruction {
    .description = "LD C, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] = gbc._rHL.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x4D, 0b01001101: LD C, L
  GBC::GameBoyColor::Instruction {
    .description = "LD C, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] = gbc.read<std::uint8_t>(gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x4E, 0b01001110: LD C, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "LD C, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] = gbc._rAF.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x4F, 0b01001111: LD C, A
  GBC::GameBoyColor::Instruction {
    .description = "LD D, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] = gbc._rBC.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x50, 0b01010000: LD D, B
  GBC::GameBoyColor::Instruction {
    .description = "LD D, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] = gbc._rBC.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x51, 0b01010001: LD D, C
  GBC::GameBoyColor::Instruction {
    .description = "LD D, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] = gbc._rDE.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x52, 0b01010010: LD D, D
  GBC::GameBoyColor::Instruction {
    .description = "LD D, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] = gbc._rDE.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x53, 0b01010011: LD D, E
  GBC::GameBoyColor::Instruction {
    .description = "LD D, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] = gbc._rHL.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x54, 0b01010100: LD D, H
  GBC::GameBoyColor::Instruction {
    .description = "LD D, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] = gbc._rHL.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x55, 0b01010101: LD D, L
  GBC::GameBoyColor::Instruction {
    .description = "LD D, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x56, 0b01010110: LD D, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "LD D, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] = gbc._rAF.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x57, 0b01010111: LD D, A
  GBC::GameBoyColor::Instruction {
    .description = "LD E, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] = gbc._rBC.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x58, 0b01011000: LD E, B
  GBC::GameBoyColor::Instruction {
    .description = "LD E, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] = gbc._rBC.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x59, 0b01011001: LD E, C
  GBC::GameBoyColor::Instruction {
    .description = "LD E, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] = gbc._rDE.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x5A, 0b01011010: LD E, D
  GBC::GameBoyColor::Instruction {
    .description = "LD E, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] = gbc._rDE.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x5B, 0b01011011: LD E, E
  GBC::GameBoyColor::Instruction {
    .description = "LD E, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] = gbc._rHL.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x5C, 0b01011100: LD E, H
  GBC::GameBoyColor::Instruction {
    .description = "LD E, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] = gbc._rHL.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x5D, 0b01011101: LD E, L
  GBC::GameBoyColor::Instruction {
    .description = "LD E, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] = gbc.read<std::uint8_t>(gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x5E, 0b01011110: LD E, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "LD E, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] = gbc._rAF.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x5F, 0b01011111: LD E, A
  GBC::GameBoyColor::Instruction {
    .description = "LD H, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] = gbc._rBC.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x60, 0b01100000: LD H, B
  GBC::GameBoyColor::Instruction {
    .description = "LD H, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] = gbc._rBC.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x61, 0b01100001: LD H, C
  GBC::GameBoyColor::Instruction {
    .description = "LD H, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] = gbc._rDE.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x62, 0b01100010: LD H, D
  GBC::GameBoyColor::Instruction {
    .description = "LD H, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] = gbc._rDE.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x63, 0b01100011: LD H, E
  GBC::GameBoyColor::Instruction {
    .description = "LD H, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] = gbc._rHL.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x64, 0b01100100: LD H, H
  GBC::GameBoyColor::Instruction {
    .description = "LD H, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] = gbc._rHL.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x65, 0b01100101: LD H, L
  GBC::GameBoyColor::Instruction {
    .description = "LD H, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x66, 0b01100110: LD H, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "LD H, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] = gbc._rAF.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x67, 0b01100111: LD H, A
  GBC::GameBoyColor::Instruction {
    .description = "LD L, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] = gbc._rBC.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x68, 0b01101000: LD L, B
  GBC::GameBoyColor::Instruction {
    .description = "LD L, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] = gbc._rBC.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x69, 0b01101001: LD L, C
  GBC::GameBoyColor::Instruction {
    .description = "LD L, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] = gbc._rDE.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x6A, 0b01101010: LD L, D
  GBC::GameBoyColor::Instruction {
    .description = "LD L, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] = gbc._rDE.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x6B, 0b01101011: LD L, E
  GBC::GameBoyColor::Instruction {
    .description = "LD L, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] = gbc._rHL.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x6C, 0b01101100: LD L, H
  GBC::GameBoyColor::Instruction {
    .description = "LD L, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] = gbc._rHL.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x6D, 0b01101101: LD L, L
  GBC::GameBoyColor::Instruction {
    .description = "LD L, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] = gbc.read<std::uint8_t>(gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x6E, 0b01101110: LD L, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "LD L, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] = gbc._rAF.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x6F, 0b01101111: LD L, A
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL), B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x70, 0b01110000: LD (HL), B
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL), C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x71, 0b01110001: LD (HL), C
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL), D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x72, 0b01110010: LD (HL), D
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL), E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x73, 0b01110011: LD (HL), E
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL), H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x74, 0b01110100: LD (HL), H
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL), L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x75, 0b01110101: LD (HL), L
  GBC::GameBoyColor::Instruction {
    .description = "HALT",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._cpu = GBC::GameBoyColor::CPU::CPUHalt;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 0;
    }
  },  // 0x76, 0b01110110: HALT
  GBC::GameBoyColor::Instruction {
    .description = "LD (HL), A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc._rHL.u16, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x77, 0b01110111: LD (HL), A
  GBC::GameBoyColor::Instruction {
    .description = "LD A, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc._rBC.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x78, 0b01111000: LD A, B
  GBC::GameBoyColor::Instruction {
    .description = "LD A, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc._rBC.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x79, 0b01111001: LD A, C
  GBC::GameBoyColor::Instruction {
    .description = "LD A, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc._rDE.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x7A, 0b01111010: LD A, D
  GBC::GameBoyColor::Instruction {
    .description = "LD A, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc._rDE.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x7B, 0b01111011: LD A, E
  GBC::GameBoyColor::Instruction {
    .description = "LD A, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc._rHL.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x7C, 0b01111100: LD A, H
  GBC::GameBoyColor::Instruction {
    .description = "LD A, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc._rHL.u8[Register::Lo];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x7D, 0b01111101: LD A, L
  GBC::GameBoyColor::Instruction {
    .description = "LD A, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x7E, 0b01111110: LD A, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "LD A, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc._rAF.u8[Register::Hi];
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x7F, 0b01111111: LD A, A
  GBC::GameBoyColor::Instruction {
    .description = "ADD A, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Hi], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x80, 0b10000000: ADD A, B
  GBC::GameBoyColor::Instruction {
    .description = "ADD A, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Lo], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x81, 0b10000001: ADD A, C
  GBC::GameBoyColor::Instruction {
    .description = "ADD A, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Hi], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x82, 0b10000010: ADD A, D
  GBC::GameBoyColor::Instruction {
    .description = "ADD A, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Lo], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x83, 0b10000011: ADD A, E
  GBC::GameBoyColor::Instruction {
    .description = "ADD A, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Hi], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x84, 0b10000100: ADD A, H
  GBC::GameBoyColor::Instruction {
    .description = "ADD A, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Lo], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x85, 0b10000101: ADD A, L
  GBC::GameBoyColor::Instruction {
    .description = "ADD A, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rHL.u16), 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x86, 0b10000110: ADD A, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "ADD A, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x87, 0b10000111: ADD A, A
  GBC::GameBoyColor::Instruction {
    .description = "ADC A, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Hi], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x88, 0b10001000: ADC A, B
  GBC::GameBoyColor::Instruction {
    .description = "ADC A, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Lo], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x89, 0b10001001: ADC A, C
  GBC::GameBoyColor::Instruction {
    .description = "ADC A, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Hi], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x8A, 0b10001010: ADC A, D
  GBC::GameBoyColor::Instruction {
    .description = "ADC A, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Lo], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x8B, 0b10001011: ADC A, E
  GBC::GameBoyColor::Instruction {
    .description = "ADC A, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Hi], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x8C, 0b10001100: ADC A, H
  GBC::GameBoyColor::Instruction {
    .description = "ADC A, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Lo], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x8D, 0b10001101: ADC A, L
  GBC::GameBoyColor::Instruction {
    .description = "ADC A, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rHL.u16), (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x8E, 0b10001110: ADC A, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "ADC A, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x8F, 0b10001111: ADC A, A
  GBC::GameBoyColor::Instruction {
    .description = "SUB B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Hi], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x90, 0b10010000: SUB B
  GBC::GameBoyColor::Instruction {
    .description = "SUB C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Lo], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x91, 0b10010001: SUB C
  GBC::GameBoyColor::Instruction {
    .description = "SUB D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Hi], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x92, 0b10010010: SUB D
  GBC::GameBoyColor::Instruction {
    .description = "SUB E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Lo], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x93, 0b10010011: SUB E
  GBC::GameBoyColor::Instruction {
    .description = "SUB H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Hi], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x94, 0b10010100: SUB H
  GBC::GameBoyColor::Instruction {
    .description = "SUB L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Lo], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x95, 0b10010101: SUB L
  GBC::GameBoyColor::Instruction {
    .description = "SUB (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rHL.u16), 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x96, 0b10010110: SUB (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SUB A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi], 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x97, 0b10010111: SUB A
  GBC::GameBoyColor::Instruction {
    .description = "SBC A, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Hi], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x98, 0b10011000: SBC A, B
  GBC::GameBoyColor::Instruction {
    .description = "SBC A, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Lo], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x99, 0b10011001: SBC A, C
  GBC::GameBoyColor::Instruction {
    .description = "SBC A, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Hi], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x9A, 0b10011010: SBC A, D
  GBC::GameBoyColor::Instruction {
    .description = "SBC A, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Lo], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x9B, 0b10011011: SBC A, E
  GBC::GameBoyColor::Instruction {
    .description = "SBC A, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Hi], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x9C, 0b10011100: SBC A, H
  GBC::GameBoyColor::Instruction {
    .description = "SBC A, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Lo], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x9D, 0b10011101: SBC A, L
  GBC::GameBoyColor::Instruction {
    .description = "SBC A, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rHL.u16), (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x9E, 0b10011110: SBC A, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SBC A, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi], (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0x9F, 0b10011111: SBC A, A
  GBC::GameBoyColor::Instruction {
    .description = "AND B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAnd(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xA0, 0b10100000: AND B
  GBC::GameBoyColor::Instruction {
    .description = "AND C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAnd(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xA1, 0b10100001: AND C
  GBC::GameBoyColor::Instruction {
    .description = "AND D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAnd(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xA2, 0b10100010: AND D
  GBC::GameBoyColor::Instruction {
    .description = "AND E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAnd(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xA3, 0b10100011: AND E
  GBC::GameBoyColor::Instruction {
    .description = "AND H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAnd(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xA4, 0b10100100: AND H
  GBC::GameBoyColor::Instruction {
    .description = "AND L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAnd(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xA5, 0b10100101: AND L
  GBC::GameBoyColor::Instruction {
    .description = "AND (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAnd(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rHL.u16), gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xA6, 0b10100110: AND (HL)
  GBC::GameBoyColor::Instruction {
    .description = "AND A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAnd(gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xA7, 0b10100111: AND A
  GBC::GameBoyColor::Instruction {
    .description = "XOR B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionXor(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xA8, 0b10101000: XOR B
  GBC::GameBoyColor::Instruction {
    .description = "XOR C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionXor(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xA9, 0b10101001: XOR C
  GBC::GameBoyColor::Instruction {
    .description = "XOR D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionXor(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xAA, 0b10101010: XOR D
  GBC::GameBoyColor::Instruction {
    .description = "XOR E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionXor(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xAB, 0b10101011: XOR E
  GBC::GameBoyColor::Instruction {
    .description = "XOR H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionXor(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xAC, 0b10101100: XOR H
  GBC::GameBoyColor::Instruction {
    .description = "XOR L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionXor(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xAD, 0b10101101: XOR L
  GBC::GameBoyColor::Instruction {
    .description = "XOR (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionXor(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rHL.u16), gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xAE, 0b10101110: XOR (HL)
  GBC::GameBoyColor::Instruction {
    .description = "XOR A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionXor(gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xAF, 0b10101111: XOR A
  GBC::GameBoyColor::Instruction {
    .description = "OR B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionOr(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xB0, 0b10110000: OR B
  GBC::GameBoyColor::Instruction {
    .description = "OR C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionOr(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xB1, 0b10110001: OR C
  GBC::GameBoyColor::Instruction {
    .description = "OR D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionOr(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xB2, 0b10110010: OR D
  GBC::GameBoyColor::Instruction {
    .description = "OR E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionOr(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xB3, 0b10110011: OR E
  GBC::GameBoyColor::Instruction {
    .description = "OR H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionOr(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xB4, 0b10110100: OR H
  GBC::GameBoyColor::Instruction {
    .description = "OR L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionOr(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xB5, 0b10110101: OR L
  GBC::GameBoyColor::Instruction {
    .description = "OR (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionOr(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rHL.u16), gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xB6, 0b10110110: OR (HL)
  GBC::GameBoyColor::Instruction {
    .description = "OR A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionOr(gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xB7, 0b10110111: OR A
  GBC::GameBoyColor::Instruction {
    .description = "CP B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCp(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xB8, 0b10111000: CP B
  GBC::GameBoyColor::Instruction {
    .description = "CP C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCp(gbc._rAF.u8[Register::Hi], gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xB9, 0b10111001: CP C
  GBC::GameBoyColor::Instruction {
    .description = "CP D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCp(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xBA, 0b10111010: CP D
  GBC::GameBoyColor::Instruction {
    .description = "CP E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCp(gbc._rAF.u8[Register::Hi], gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xBB, 0b10111011: CP E
  GBC::GameBoyColor::Instruction {
    .description = "CP H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCp(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xBC, 0b10111100: CP H
  GBC::GameBoyColor::Instruction {
    .description = "CP L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCp(gbc._rAF.u8[Register::Hi], gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xBD, 0b10111101: CP L
  GBC::GameBoyColor::Instruction {
    .description = "CP (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCp(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rHL.u16));
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xBE, 0b10111110: CP (HL)
  GBC::GameBoyColor::Instruction {
    .description = "CP A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCp(gbc._rAF.u8[Register::Hi], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xBF, 0b10111111: CP A
  GBC::GameBoyColor::Instruction {
    .description = "RET NZ",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (!(gbc._rAF.u8[Register::Lo] & Register::Z)) {
        gbc.instructionRet();
        gbc._cpuCycle += 20;
      }
      else {
        gbc._rPC.u16 += 1;
        gbc._cpuCycle += 8;
      }
    }
  },  // 0xC0, 0b11000000: RET NZ
  GBC::GameBoyColor::Instruction {
    .description = "POP BC",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionPop(gbc._rBC.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 12;
    }
  },  // 0xC1, 0b11000001: POP BC
  GBC::GameBoyColor::Instruction {
    .description = "JP NZ, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (!(gbc._rAF.u8[Register::Lo] & Register::Z)) {
        gbc._rPC.u16 = gbc.read<std::uint16_t>(gbc._rPC.u16 + 1);
        gbc._cpuCycle += 16;
      }
      else {
        gbc._rPC.u16 += 3;
        gbc._cpuCycle += 12;
      }
    }
  },  // 0xC2, 0b11000010: JP NZ, nn
  GBC::GameBoyColor::Instruction {
    .description = "JP nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 = gbc.read<std::uint16_t>(gbc._rPC.u16 + 1);
      gbc._cpuCycle += 16;
    }
  },  // 0xC3, 0b11000011: JP nn
  GBC::GameBoyColor::Instruction {
    .description = "CALL NZ, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (!(gbc._rAF.u8[Register::Lo] & Register::Z)) {
        gbc.instructionCall(gbc.read<std::uint16_t>(gbc._rPC.u16 + 1), gbc._rPC.u16 + 3);
        gbc._cpuCycle += 24;
      }
      else {
        gbc._rPC.u16 += 3;
        gbc._cpuCycle += 12;
      }
    }
  },  // 0xC4, 0b11000100: CALL NZ, nn
  GBC::GameBoyColor::Instruction {
    .description = "PUSH BC",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionPush(gbc._rBC.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xC5, 0b11000101: PUSH BC
  GBC::GameBoyColor::Instruction {
    .description = "ADD A, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rPC.u16 + 1), 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0xC6, 0b11000110: ADD A, n
  GBC::GameBoyColor::Instruction {
    .description = "RST 00",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCall(0x0000, gbc._rPC.u16 + 1);
      gbc._cpuCycle += 16;
    }
  },  // 0xC7, 0b11000111: RST 00
  GBC::GameBoyColor::Instruction {
    .description = "RET Z",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (gbc._rAF.u8[Register::Lo] & Register::Z) {
        gbc.instructionRet();
        gbc._cpuCycle += 20;
      }
      else {
        gbc._rPC.u16 += 1;
        gbc._cpuCycle += 8;
      }
    }
  },  // 0xC8, 0b11001000: RET Z
  GBC::GameBoyColor::Instruction {
    .description = "RET",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRet();
      gbc._cpuCycle += 16;
    }
  },  // 0xC9, 0b11001001: RET
  GBC::GameBoyColor::Instruction {
    .description = "JP Z, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (gbc._rAF.u8[Register::Lo] & Register::Z) {
        gbc._rPC.u16 = gbc.read<std::uint16_t>(gbc._rPC.u16 + 1);
        gbc._cpuCycle += 16;
      }
      else {
        gbc._rPC.u16 += 3;
        gbc._cpuCycle += 12;
      }
    }
  },  // 0xCA, 0b11001010: JP Z, nn
  GBC::GameBoyColor::Instruction {
    .description = "SWAP / RLC / RL / RRC / RR / SLA / SRA / SRL / BIT / SET / RES",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      _instructionsCB[gbc.read<std::uint8_t>(gbc._rPC.u16)].instruction(gbc);
    }
  },  // 0xCB, 0b11001011: SWAP / RLC / RL / RRC / RR / SLA / SRA / SRL / BIT / SET / RES
  GBC::GameBoyColor::Instruction {
    .description = "CALL Z, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (gbc._rAF.u8[Register::Lo] & Register::Z) {
        gbc.instructionCall(gbc.read<std::uint16_t>(gbc._rPC.u16 + 1), gbc._rPC.u16 + 3);
        gbc._cpuCycle += 24;
      }
      else {
        gbc._rPC.u16 += 3;
        gbc._cpuCycle += 12;
      }
    }
  },  // 0xCC, 0b11001100: CALL Z, nn
  GBC::GameBoyColor::Instruction {
    .description = "CALL nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCall(gbc.read<std::uint16_t>(gbc._rPC.u16 + 1), gbc._rPC.u16 + 3);
      gbc._cpuCycle += 24;
    }
  },  // 0xCD, 0b11001101: CALL nn
  GBC::GameBoyColor::Instruction {
    .description = "ADC A, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAdd(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rPC.u16 + 1), (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xCE, 0b11001110: ADC A, n
  GBC::GameBoyColor::Instruction {
    .description = "RST 08",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCall(0x0008, gbc._rPC.u16 + 1);
      gbc._cpuCycle += 16;
    }
  },  // 0xCF, 0b11001111: RST 08
  GBC::GameBoyColor::Instruction {
    .description = "RET NC",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (!(gbc._rAF.u8[Register::Lo] & Register::C)) {
        gbc.instructionRet();
        gbc._cpuCycle += 20;
      }
      else {
        gbc._rPC.u16 += 1;
        gbc._cpuCycle += 8;
      }
    }
  },  // 0xD0, 0b11010000: RET NC
  GBC::GameBoyColor::Instruction {
    .description = "POP DE",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionPop(gbc._rDE.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 12;
    }
  },  // 0xD1, 0b11010001: POP DE
  GBC::GameBoyColor::Instruction {
    .description = "JP NC, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (!(gbc._rAF.u8[Register::Lo] & Register::C)) {
        gbc._rPC.u16 = gbc.read<std::uint16_t>(gbc._rPC.u16 + 1);
        gbc._cpuCycle += 16;
      }
      else {
        gbc._rPC.u16 += 3;
        gbc._cpuCycle += 12;
      }
    }
  },  // 0xD2, 0b11010010: JP NC, nn
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xD3, 0b11010011: 
  GBC::GameBoyColor::Instruction {
    .description = "CALL NC, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (!(gbc._rAF.u8[Register::Lo] & Register::C)) {
        gbc.instructionCall(gbc.read<std::uint16_t>(gbc._rPC.u16 + 1), gbc._rPC.u16 + 3);
        gbc._cpuCycle += 24;
      }
      else {
        gbc._rPC.u16 += 3;
        gbc._cpuCycle += 12;
      }
    }
  },  // 0xD4, 0b11010100: CALL NC, nn
  GBC::GameBoyColor::Instruction {
    .description = "PUSH DE",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionPush(gbc._rDE.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xD5, 0b11010101: PUSH DE
  GBC::GameBoyColor::Instruction {
    .description = "SUB n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rPC.u16 + 1), 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0xD6, 0b11010110: SUB n
  GBC::GameBoyColor::Instruction {
    .description = "RST 10",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCall(0x0010, gbc._rPC.u16 + 1);
      gbc._cpuCycle += 16;
    }
  },  // 0xD7, 0b11010111: RST 10
  GBC::GameBoyColor::Instruction {
    .description = "RET C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (gbc._rAF.u8[Register::Lo] & Register::C) {
        gbc.instructionRet();
        gbc._cpuCycle += 20;
      }
      else {
        gbc._rPC.u16 += 1;
        gbc._cpuCycle += 8;
      }
    }
  },  // 0xD8, 0b11011000: RET C
  GBC::GameBoyColor::Instruction {
    .description = "RETI",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRet();
      gbc._ime = GBC::GameBoyColor::IME::IMEEnabled;
      gbc._cpuCycle += 16;
    }
  },  // 0xD9, 0b11011001: RETI
  GBC::GameBoyColor::Instruction {
    .description = "JP C, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (gbc._rAF.u8[Register::Lo] & Register::C) {
        gbc._rPC.u16 = gbc.read<std::uint16_t>(gbc._rPC.u16 + 1);
        gbc._cpuCycle += 16;
      }
      else {
        gbc._rPC.u16 += 3;
        gbc._cpuCycle += 12;
      }
    }
  },  // 0xDA, 0b11011010: JP C, nn
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xDB, 0b11011011: 
  GBC::GameBoyColor::Instruction {
    .description = "CALL C, nn",
    .instruction = [](GBC::GameBoyColor& gbc) {
      if (gbc._rAF.u8[Register::Lo] & Register::C) {
        gbc.instructionCall(gbc.read<std::uint16_t>(gbc._rPC.u16 + 1), gbc._rPC.u16 + 3);
        gbc._cpuCycle += 24;
      }
      else {
        gbc._rPC.u16 += 3;
        gbc._cpuCycle += 12;
      }
    }
  },  // 0xDC, 0b11011100: CALL C, nn
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xDD, 0b11011101: 
  GBC::GameBoyColor::Instruction {
    .description = "SBC A, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSub(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rPC.u16 + 1), (gbc._rAF.u8[Register::Lo] & Register::C) ? 1 : 0, gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0xDE, 0b11011110: SBC A, n
  GBC::GameBoyColor::Instruction {
    .description = "RST 18",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCall(0x0018, gbc._rPC.u16 + 1);
      gbc._cpuCycle += 16;
    }
  },  // 0xDF, 0b11011111: RST 18
  GBC::GameBoyColor::Instruction {
    .description = "LD (n), A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(0xFF00 + gbc.read<std::uint8_t>(gbc._rPC.u16 + 1), gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 12;
    }
  },  // 0xE0, 0b11100000: LD (n), A
  GBC::GameBoyColor::Instruction {
    .description = "POP HL",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionPop(gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 12;
    }
  },  // 0xE1, 0b11100001: POP HL
  GBC::GameBoyColor::Instruction {
    .description = "LD (C), A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(0xFF00 + gbc._rBC.u8[Register::Lo], gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xE2, 0b11100010: LD (C), A
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xE3, 0b11100011: 
  GBC::GameBoyColor::Instruction { 
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xE4, 0b11100100:
  GBC::GameBoyColor::Instruction {
    .description = "PUSH HL",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionPush(gbc._rHL.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xE5, 0b11100101: PUSH HL
  GBC::GameBoyColor::Instruction {
    .description = "AND n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionAnd(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rPC.u16 + 1), gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0xE6, 0b11100110: AND n
  GBC::GameBoyColor::Instruction {
    .description = "RST 20",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCall(0x0020, gbc._rPC.u16 + 1);
      gbc._cpuCycle += 16;
    }
  },  // 0xE7, 0b11100111: RST 20
  GBC::GameBoyColor::Instruction {
    .description = "ADD SP, n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::int32_t  r16 = (std::int32_t)(gbc._rSP.u16 & 0b1111111111111111) + (std::int32_t)(gbc.read<std::int8_t>(gbc._rPC.u16 + 1) & 0b1111111111111111);
      std::int32_t  r12 = (std::int32_t)(gbc._rSP.u16 & 0b0000111111111111) + (std::int32_t)(gbc.read<std::int8_t>(gbc._rPC.u16 + 1) & 0b0000111111111111);

      gbc._rSP.u16 = r16;
      gbc._rAF.u8[Register::Lo] &= ~Register::Z;
      gbc._rAF.u8[Register::Lo] &= ~Register::N;
      if (r12 & 0b0001000000000000)
        gbc._rAF.u8[Register::Lo] |= Register::H;
      else
        gbc._rAF.u8[Register::Lo] &= ~Register::H;
      if (r16 & 0b10000000000000000)
        gbc._rAF.u8[Register::Lo] |= Register::C;
      else
        gbc._rAF.u8[Register::Lo] &= ~Register::C;
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 16;
    }
  },  // 0xE8, 0b11101000: ADD SP, n
  GBC::GameBoyColor::Instruction {
    .description = "JP HL",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 = gbc._rHL.u16;
      gbc._cpuCycle += 4;
    }
  },  // 0xE9, 0b11101001: JP HL
  GBC::GameBoyColor::Instruction {
    .description = "LD (nn), A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.write<std::uint8_t>(gbc.read<std::uint16_t>(gbc._rPC.u16 + 1), gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 3;
      gbc._cpuCycle += 16;
    }
  },  // 0xEA, 0b11101010: LD (nn), A
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xEB, 0b11101011: 
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xEC, 0b11101100: 
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xED, 0b11101101:
  GBC::GameBoyColor::Instruction {
    .description = "XOR n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionXor(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rPC.u16 + 1), gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0xEE, 0b11101110: XOR n
  GBC::GameBoyColor::Instruction {
    .description = "RST 28",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCall(0x0028, gbc._rPC.u16 + 1);
      gbc._cpuCycle += 16;
    }
  },  // 0xEF, 0b11101111: RST 28
  GBC::GameBoyColor::Instruction {
    .description = "LD A, (n)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc.read<std::uint8_t>(0xFF00 + gbc.read<std::uint8_t>(gbc._rPC.u16 + 1));
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 12;
    }
  },  // 0xF0, 0b11110000: LD A, (n)
  GBC::GameBoyColor::Instruction {
    .description = "POP AF",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionPop(gbc._rAF.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 12;
    }
  },  // 0xF1, 0b11110001: POP AF
  GBC::GameBoyColor::Instruction {
    .description = "LD A, (C)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc.read<std::uint8_t>(0xFF00 + gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xF2, 0b11110010: LD A, (C)
  GBC::GameBoyColor::Instruction {
    .description = "DI",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._ime = GBC::GameBoyColor::IME::IMEDisabled;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xF3, 0b11110011: DI
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xF4, 0b11110100: 
  GBC::GameBoyColor::Instruction {
    .description = "PUSH AF",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionPush(gbc._rAF.u16);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xF5, 0b11110101: PUSH AF
  GBC::GameBoyColor::Instruction {
    .description = "OR n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionOr(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rPC.u16 + 1), gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0xF6, 0b11110110: OR n
  GBC::GameBoyColor::Instruction {
    .description = "RST 30",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCall(0x0030, gbc._rPC.u16 + 1);
      gbc._cpuCycle += 16;
    }
  },  // 0xF7, 0b11110111: RST 30
  GBC::GameBoyColor::Instruction {
    .description = "LD HL, SP+n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::int8_t n = gbc.read<std::int8_t>(gbc._rPC.u16 + 1);

      gbc._rHL.u16 = gbc._rSP.u16 + n;
      if (((std::uint16_t)n & 0b11111111) + (gbc._rSP.u16 & 0b11111111) >= 0b0000000100000000)
        gbc._rAF.u8[Register::Lo] |= Register::C;
      else
        gbc._rAF.u8[Register::Lo] &= ~Register::C;
      if (((std::uint16_t)n & 0b00001111) + (gbc._rSP.u16 & 0b00001111) >= 0b0000000000010000)
        gbc._rAF.u8[Register::Lo] |= Register::H;
      else
        gbc._rAF.u8[Register::Lo] &= ~Register::H;
      gbc._rAF.u8[Register::Lo] &= ~Register::Z;
      gbc._rAF.u8[Register::Lo] &= ~Register::N;
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 12;
    }
  },  // 0xF8, 0b11111000: LD HL, (SP+n)
  GBC::GameBoyColor::Instruction {
    .description = "LD SP, HL",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rSP.u16 = gbc._rHL.u16;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xF9, 0b11111001: LD SP, HL
  GBC::GameBoyColor::Instruction {
    .description = "LD A, (nn)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] = gbc.read<std::uint8_t>(gbc.read<std::uint16_t>(gbc._rPC.u16 + 1));
      gbc._rPC.u16 += 3;
      gbc._cpuCycle += 16;
    }
  },  // 0xFA, 0b11111010: LD A, (nn)
  GBC::GameBoyColor::Instruction {
    .description = "EI",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._ime = GBC::GameBoyColor::IME::IMEScheduled;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xFB, 0b11111011: EI
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xFC, 0b11111100: 
  GBC::GameBoyColor::Instruction {
    .description = "???",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 4;
    }
  },  // 0xFD, 0b11111101: 
  GBC::GameBoyColor::Instruction {
    .description = "CP n",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCp(gbc._rAF.u8[Register::Hi], gbc.read<std::uint8_t>(gbc._rPC.u16 + 1));
      gbc._rPC.u16 += 2;
      gbc._cpuCycle += 8;
    }
  },  // 0xFE, 0b11111110: CP n
  GBC::GameBoyColor::Instruction {
    .description = "RST 38",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionCall(0x0038, gbc._rPC.u16 + 1);
      gbc._cpuCycle += 4;
    }
  }   // 0xFF, 0b11111111: RST 38
};

const std::array<GBC::GameBoyColor::Instruction, 256> GBC::GameBoyColor::_instructionsCB = {
  GBC::GameBoyColor::Instruction {
    .description = "RLC B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRlc(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x00, 0b00000000: RLC B
  GBC::GameBoyColor::Instruction {
    .description = "RLC C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRlc(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x01, 0b00000001: RLC C
  GBC::GameBoyColor::Instruction {
    .description = "RLC D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRlc(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x02, 0b00000010: RLC D
  GBC::GameBoyColor::Instruction {
    .description = "RLC E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRlc(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x03, 0b00000011: RLC E
  GBC::GameBoyColor::Instruction {
    .description = "RLC H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRlc(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x04, 0b00000100: RLC H
  GBC::GameBoyColor::Instruction {
    .description = "RLC L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRlc(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x05, 0b00000101: RLC L
  GBC::GameBoyColor::Instruction {
    .description = "RLC (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionRlc(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x06, 0b00000110: RLC (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RLC A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRlc(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x07, 0b00000111: RLC A
  GBC::GameBoyColor::Instruction {
    .description = "RRC B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRrc(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x08, 0b00001000: RRC B
  GBC::GameBoyColor::Instruction {
    .description = "RRC C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRrc(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x09, 0b00001001: RRC C
  GBC::GameBoyColor::Instruction {
    .description = "RRC D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRrc(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x0a, 0b00001010: RRC D
  GBC::GameBoyColor::Instruction {
    .description = "RRC E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRrc(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x0b, 0b00001011: RRC E
  GBC::GameBoyColor::Instruction {
    .description = "RRC H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRrc(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x0c, 0b00001100: RRC H
  GBC::GameBoyColor::Instruction {
    .description = "RRC L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRrc(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x0d, 0b00001101: RRC L
  GBC::GameBoyColor::Instruction {
    .description = "RRC (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionRrc(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x0e, 0b00001110: RRC (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RRC A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRrc(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x0f, 0b00001111: RRC A
  GBC::GameBoyColor::Instruction {
    .description = "RL B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRl(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x10, 0b00010000: RL B
  GBC::GameBoyColor::Instruction {
    .description = "RL C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRl(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x11, 0b00010001: RL C
  GBC::GameBoyColor::Instruction {
    .description = "RL D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRl(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x12, 0b00010010: RL D
  GBC::GameBoyColor::Instruction {
    .description = "RL E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRl(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x13, 0b00010011: RL E
  GBC::GameBoyColor::Instruction {
    .description = "RL H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRl(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x14, 0b00010100: RL H
  GBC::GameBoyColor::Instruction {
    .description = "RL L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRl(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x15, 0b00010101: RL L
  GBC::GameBoyColor::Instruction {
    .description = "RL (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionRl(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x16, 0b00010110: RL (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RL A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRl(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x17, 0b00010111: RL A
  GBC::GameBoyColor::Instruction {
    .description = "RR B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRr(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x18, 0b00011000: RR B
  GBC::GameBoyColor::Instruction {
    .description = "RR C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRr(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x19, 0b00011001: RR C
  GBC::GameBoyColor::Instruction {
    .description = "RR D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRr(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x1a, 0b00011010: RR D
  GBC::GameBoyColor::Instruction {
    .description = "RR E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRr(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x1b, 0b00011011: RR E
  GBC::GameBoyColor::Instruction {
    .description = "RR H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRr(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x1c, 0b00011100: RR H
  GBC::GameBoyColor::Instruction {
    .description = "RR L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRr(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x1d, 0b00011101: RR L
  GBC::GameBoyColor::Instruction {
    .description = "RR (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionRr(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x1e, 0b00011110: RR (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RR A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionRr(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x1f, 0b00011111: RR A
  GBC::GameBoyColor::Instruction {
    .description = "SLA B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSla(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x20, 0b00100000: SLA B
  GBC::GameBoyColor::Instruction {
    .description = "SLA C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSla(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x21, 0b00100001: SLA C
  GBC::GameBoyColor::Instruction {
    .description = "SLA D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSla(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x22, 0b00100010: SLA D
  GBC::GameBoyColor::Instruction {
    .description = "SLA E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSla(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x23, 0b00100011: SLA E
  GBC::GameBoyColor::Instruction {
    .description = "SLA H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSla(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x24, 0b00100100: SLA H
  GBC::GameBoyColor::Instruction {
    .description = "SLA L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSla(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x25, 0b00100101: SLA L
  GBC::GameBoyColor::Instruction {
    .description = "SLA (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionSla(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x26, 0b00100110: SLA (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SLA A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSla(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x27, 0b00100111: SLA A
  GBC::GameBoyColor::Instruction {
    .description = "SRA B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSra(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x28, 0b00101000: SRA B
  GBC::GameBoyColor::Instruction {
    .description = "SRA C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSra(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x29, 0b00101001: SRA C
  GBC::GameBoyColor::Instruction {
    .description = "SRA D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSra(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x2a, 0b00101010: SRA D
  GBC::GameBoyColor::Instruction {
    .description = "SRA E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSra(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x2b, 0b00101011: SRA E
  GBC::GameBoyColor::Instruction {
    .description = "SRA H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSra(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x2c, 0b00101100: SRA H
  GBC::GameBoyColor::Instruction {
    .description = "SRA L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSra(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x2d, 0b00101101: SRA L
  GBC::GameBoyColor::Instruction {
    .description = "SRA (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionSra(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x2e, 0b00101110: SRA (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SRA A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSra(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x2f, 0b00101111: SRA A
  GBC::GameBoyColor::Instruction {
    .description = "SWAP B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSwap(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x30, 0b00110000: SWAP B
  GBC::GameBoyColor::Instruction {
    .description = "SWAP C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSwap(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x31, 0b00110001: SWAP C
  GBC::GameBoyColor::Instruction {
    .description = "SWAP D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSwap(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x32, 0b00110010: SWAP D
  GBC::GameBoyColor::Instruction {
    .description = "SWAP E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSwap(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x33, 0b00110011: SWAP E
  GBC::GameBoyColor::Instruction {
    .description = "SWAP H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSwap(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x34, 0b00110100: SWAP H
  GBC::GameBoyColor::Instruction {
    .description = "SWAP L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSwap(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x35, 0b00110101: SWAP L
  GBC::GameBoyColor::Instruction {
    .description = "SWAP (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionSwap(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x36, 0b00110110: SWAP (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SWAP A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSwap(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x37, 0b00110111: SWAP A
  GBC::GameBoyColor::Instruction {
    .description = "SRL B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSrl(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x38, 0b00111000: SRL B
  GBC::GameBoyColor::Instruction {
    .description = "SRL C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSrl(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x39, 0b00111001: SRL C
  GBC::GameBoyColor::Instruction {
    .description = "SRL D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSrl(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x3a, 0b00111010: SRL D
  GBC::GameBoyColor::Instruction {
    .description = "SRL E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSrl(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x3b, 0b00111011: SRL E
  GBC::GameBoyColor::Instruction {
    .description = "SRL H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSrl(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x3c, 0b00111100: SRL H
  GBC::GameBoyColor::Instruction {
    .description = "SRL L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSrl(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x3d, 0b00111101: SRL L
  GBC::GameBoyColor::Instruction {
    .description = "SRL (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      gbc.instructionSrl(r);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x3e, 0b00111110: SRL (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SRL A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionSrl(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x3f, 0b00111111: SRL A
  GBC::GameBoyColor::Instruction {
    .description = "BIT 0, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<0>(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x40, 0b01000000: BIT 0, B
  GBC::GameBoyColor::Instruction {
    .description = "BIT 0, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<0>(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x41, 0b01000001: BIT 0, C
  GBC::GameBoyColor::Instruction {
    .description = "BIT 0, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<0>(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x42, 0b01000010: BIT 0, D
  GBC::GameBoyColor::Instruction {
    .description = "BIT 0, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<0>(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x43, 0b01000011: BIT 0, E
  GBC::GameBoyColor::Instruction {
    .description = "BIT 0, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<0>(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x44, 0b01000100: BIT 0, H
  GBC::GameBoyColor::Instruction {
    .description = "BIT 0, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<0>(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x45, 0b01000101: BIT 0, L
  GBC::GameBoyColor::Instruction {
    .description = "BIT 0, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<0>(gbc.read<std::uint8_t>(gbc._rHL.u16));
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x46, 0b01000110: BIT 0, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "BIT 0, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<0>(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x47, 0b01000111: BIT 0, A
  GBC::GameBoyColor::Instruction {
    .description = "BIT 1, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<1>(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x48, 0b01001000: BIT 1, B
  GBC::GameBoyColor::Instruction {
    .description = "BIT 1, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<1>(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x49, 0b01001001: BIT 1, C
  GBC::GameBoyColor::Instruction {
    .description = "BIT 1, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<1>(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x4a, 0b01001010: BIT 1, D
  GBC::GameBoyColor::Instruction {
    .description = "BIT 1, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<1>(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x4b, 0b01001011: BIT 1, E
  GBC::GameBoyColor::Instruction {
    .description = "BIT 1, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<1>(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x4c, 0b01001100: BIT 1, H
  GBC::GameBoyColor::Instruction {
    .description = "BIT 1, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<1>(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x4d, 0b01001101: BIT 1, L
  GBC::GameBoyColor::Instruction {
    .description = "BIT 1, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<1>(gbc.read<std::uint8_t>(gbc._rHL.u16));
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x4e, 0b01001110: BIT 1, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "BIT 1, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<1>(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x4f, 0b01001111: BIT 1, A
  GBC::GameBoyColor::Instruction {
    .description = "BIT 2, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<2>(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x50, 0b01010000: BIT 2, B
  GBC::GameBoyColor::Instruction {
    .description = "BIT 2, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<2>(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x51, 0b01010001: BIT 2, C
  GBC::GameBoyColor::Instruction {
    .description = "BIT 2, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<2>(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x52, 0b01010010: BIT 2, D
  GBC::GameBoyColor::Instruction {
    .description = "BIT 2, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<2>(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x53, 0b01010011: BIT 2, E
  GBC::GameBoyColor::Instruction {
    .description = "BIT 2, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<2>(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x54, 0b01010100: BIT 2, H
  GBC::GameBoyColor::Instruction {
    .description = "BIT 2, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<2>(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x55, 0b01010101: BIT 2, L
  GBC::GameBoyColor::Instruction {
    .description = "BIT 2, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<2>(gbc.read<std::uint8_t>(gbc._rHL.u16));
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x56, 0b01010110: BIT 2, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "BIT 2, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<2>(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x57, 0b01010111: BIT 2, A
  GBC::GameBoyColor::Instruction {
    .description = "BIT 3, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<3>(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x58, 0b01011000: BIT 3, B
  GBC::GameBoyColor::Instruction {
    .description = "BIT 3, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<3>(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x59, 0b01011001: BIT 3, C
  GBC::GameBoyColor::Instruction {
    .description = "BIT 3, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<3>(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x5a, 0b01011010: BIT 3, D
  GBC::GameBoyColor::Instruction {
    .description = "BIT 3, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<3>(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x5b, 0b01011011: BIT 3, E
  GBC::GameBoyColor::Instruction {
    .description = "BIT 3, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<3>(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x5c, 0b01011100: BIT 3, H
  GBC::GameBoyColor::Instruction {
    .description = "BIT 3, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<3>(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x5d, 0b01011101: BIT 3, L
  GBC::GameBoyColor::Instruction {
    .description = "BIT 3, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<3>(gbc.read<std::uint8_t>(gbc._rHL.u16));
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x5e, 0b01011110: BIT 3, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "BIT 3, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<3>(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x5f, 0b01011111: BIT 3, A
  GBC::GameBoyColor::Instruction {
    .description = "BIT 4, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<4>(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x60, 0b01100000: BIT 4, B
  GBC::GameBoyColor::Instruction {
    .description = "BIT 4, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<4>(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x61, 0b01100001: BIT 4, C
  GBC::GameBoyColor::Instruction {
    .description = "BIT 4, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<4>(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x62, 0b01100010: BIT 4, D
  GBC::GameBoyColor::Instruction {
    .description = "BIT 4, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<4>(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x63, 0b01100011: BIT 4, E
  GBC::GameBoyColor::Instruction {
    .description = "BIT 4, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<4>(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x64, 0b01100100: BIT 4, H
  GBC::GameBoyColor::Instruction {
    .description = "BIT 4, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<4>(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x65, 0b01100101: BIT 4, L
  GBC::GameBoyColor::Instruction {
    .description = "BIT 4, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<4>(gbc.read<std::uint8_t>(gbc._rHL.u16));
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x66, 0b01100110: BIT 4, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "BIT 4, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<4>(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x67, 0b01100111: BIT 4, A
  GBC::GameBoyColor::Instruction {
    .description = "BIT 5, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<5>(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x68, 0b01101000: BIT 5, B
  GBC::GameBoyColor::Instruction {
    .description = "BIT 5, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<5>(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x69, 0b01101001: BIT 5, C
  GBC::GameBoyColor::Instruction {
    .description = "BIT 5, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<5>(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x6a, 0b01101010: BIT 5, D
  GBC::GameBoyColor::Instruction {
    .description = "BIT 5, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<5>(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x6b, 0b01101011: BIT 5, E
  GBC::GameBoyColor::Instruction {
    .description = "BIT 5, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<5>(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x6c, 0b01101100: BIT 5, H
  GBC::GameBoyColor::Instruction {
    .description = "BIT 5, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<5>(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x6d, 0b01101101: BIT 5, L
  GBC::GameBoyColor::Instruction {
    .description = "BIT 5, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<5>(gbc.read<std::uint8_t>(gbc._rHL.u16));
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x6e, 0b01101110: BIT 5, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "BIT 5, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<5>(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x6f, 0b01101111: BIT 5, A
  GBC::GameBoyColor::Instruction {
    .description = "BIT 6, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<6>(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x70, 0b01110000: BIT 6, B
  GBC::GameBoyColor::Instruction {
    .description = "BIT 6, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<6>(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x71, 0b01110001: BIT 6, C
  GBC::GameBoyColor::Instruction {
    .description = "BIT 6, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<6>(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x72, 0b01110010: BIT 6, D
  GBC::GameBoyColor::Instruction {
    .description = "BIT 6, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<6>(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x73, 0b01110011: BIT 6, E
  GBC::GameBoyColor::Instruction {
    .description = "BIT 6, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<6>(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x74, 0b01110100: BIT 6, H
  GBC::GameBoyColor::Instruction {
    .description = "BIT 6, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<6>(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x75, 0b01110101: BIT 6, L
  GBC::GameBoyColor::Instruction {
    .description = "BIT 6, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<6>(gbc.read<std::uint8_t>(gbc._rHL.u16));
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x76, 0b01110110: BIT 6, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "BIT 6, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<6>(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x77, 0b01110111: BIT 6, A
  GBC::GameBoyColor::Instruction {
    .description = "BIT 7, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<7>(gbc._rBC.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x78, 0b01111000: BIT 7, B
  GBC::GameBoyColor::Instruction {
    .description = "BIT 7, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<7>(gbc._rBC.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x79, 0b01111001: BIT 7, C
  GBC::GameBoyColor::Instruction {
    .description = "BIT 7, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<7>(gbc._rDE.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x7a, 0b01111010: BIT 7, D
  GBC::GameBoyColor::Instruction {
    .description = "BIT 7, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<7>(gbc._rDE.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x7b, 0b01111011: BIT 7, E
  GBC::GameBoyColor::Instruction {
    .description = "BIT 7, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<7>(gbc._rHL.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x7c, 0b01111100: BIT 7, H
  GBC::GameBoyColor::Instruction {
    .description = "BIT 7, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<7>(gbc._rHL.u8[Register::Lo]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x7d, 0b01111101: BIT 7, L
  GBC::GameBoyColor::Instruction {
    .description = "BIT 7, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<7>(gbc.read<std::uint8_t>(gbc._rHL.u16));
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x7e, 0b01111110: BIT 7, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "BIT 7, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc.instructionBit<7>(gbc._rAF.u8[Register::Hi]);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x7f, 0b01111111: BIT 7, A
  GBC::GameBoyColor::Instruction {
    .description = "RES 0, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] &= ~(0b00000001 << 0);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x80, 0b10000000: RES 0, B
  GBC::GameBoyColor::Instruction {
    .description = "RES 0, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] &= ~(0b00000001 << 0);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x81, 0b10000001: RES 0, C
  GBC::GameBoyColor::Instruction {
    .description = "RES 0, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] &= ~(0b00000001 << 0);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x82, 0b10000010: RES 0, D
  GBC::GameBoyColor::Instruction {
    .description = "RES 0, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] &= ~(0b00000001 << 0);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x83, 0b10000011: RES 0, E
  GBC::GameBoyColor::Instruction {
    .description = "RES 0, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] &= ~(0b00000001 << 0);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x84, 0b10000100: RES 0, H
  GBC::GameBoyColor::Instruction {
    .description = "RES 0, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] &= ~(0b00000001 << 0);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x85, 0b10000101: RES 0, L
  GBC::GameBoyColor::Instruction {
    .description = "RES 0, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r &= ~(0b00000001 << 0);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x86, 0b10000110: RES 0, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RES 0, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] &= ~(0b00000001 << 0);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x87, 0b10000111: RES 0, A
  GBC::GameBoyColor::Instruction {
    .description = "RES 1, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] &= ~(0b00000001 << 1);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x88, 0b10001000: RES 1, B
  GBC::GameBoyColor::Instruction {
    .description = "RES 1, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] &= ~(0b00000001 << 1);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x89, 0b10001001: RES 1, C
  GBC::GameBoyColor::Instruction {
    .description = "RES 1, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] &= ~(0b00000001 << 1);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x8a, 0b10001010: RES 1, D
  GBC::GameBoyColor::Instruction {
    .description = "RES 1, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] &= ~(0b00000001 << 1);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x8b, 0b10001011: RES 1, E
  GBC::GameBoyColor::Instruction {
    .description = "RES 1, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] &= ~(0b00000001 << 1);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x8c, 0b10001100: RES 1, H
  GBC::GameBoyColor::Instruction {
    .description = "RES 1, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] &= ~(0b00000001 << 1);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x8d, 0b10001101: RES 1, L
  GBC::GameBoyColor::Instruction {
    .description = "RES 1, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r &= ~(0b00000001 << 1);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x8e, 0b10001110: RES 1, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RES 1, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] &= ~(0b00000001 << 1);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x8f, 0b10001111: RES 1, A
  GBC::GameBoyColor::Instruction {
    .description = "RES 2, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] &= ~(0b00000001 << 2);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x90, 0b10010000: RES 2, B
  GBC::GameBoyColor::Instruction {
    .description = "RES 2, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] &= ~(0b00000001 << 2);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x91, 0b10010001: RES 2, C
  GBC::GameBoyColor::Instruction {
    .description = "RES 2, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] &= ~(0b00000001 << 2);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x92, 0b10010010: RES 2, D
  GBC::GameBoyColor::Instruction {
    .description = "RES 2, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] &= ~(0b00000001 << 2);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x93, 0b10010011: RES 2, E
  GBC::GameBoyColor::Instruction {
    .description = "RES 2, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] &= ~(0b00000001 << 2);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x94, 0b10010100: RES 2, H
  GBC::GameBoyColor::Instruction {
    .description = "RES 2, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] &= ~(0b00000001 << 2);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x95, 0b10010101: RES 2, L
  GBC::GameBoyColor::Instruction {
    .description = "RES 2, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r &= ~(0b00000001 << 2);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x96, 0b10010110: RES 2, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RES 2, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] &= ~(0b00000001 << 2);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x97, 0b10010111: RES 2, A
  GBC::GameBoyColor::Instruction {
    .description = "RES 3, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] &= ~(0b00000001 << 3);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x98, 0b10011000: RES 3, B
  GBC::GameBoyColor::Instruction {
    .description = "RES 3, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] &= ~(0b00000001 << 3);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x99, 0b10011001: RES 3, C
  GBC::GameBoyColor::Instruction {
    .description = "RES 3, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] &= ~(0b00000001 << 3);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x9a, 0b10011010: RES 3, D
  GBC::GameBoyColor::Instruction {
    .description = "RES 3, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] &= ~(0b00000001 << 3);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x9b, 0b10011011: RES 3, E
  GBC::GameBoyColor::Instruction {
    .description = "RES 3, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] &= ~(0b00000001 << 3);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x9c, 0b10011100: RES 3, H
  GBC::GameBoyColor::Instruction {
    .description = "RES 3, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] &= ~(0b00000001 << 3);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x9d, 0b10011101: RES 3, L
  GBC::GameBoyColor::Instruction {
    .description = "RES 3, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r &= ~(0b00000001 << 3);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0x9e, 0b10011110: RES 3, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RES 3, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] &= ~(0b00000001 << 3);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0x9f, 0b10011111: RES 3, A
  GBC::GameBoyColor::Instruction {
    .description = "RES 4, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] &= ~(0b00000001 << 4);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xa0, 0b10100000: RES 4, B
  GBC::GameBoyColor::Instruction {
    .description = "RES 4, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] &= ~(0b00000001 << 4);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xa1, 0b10100001: RES 4, C
  GBC::GameBoyColor::Instruction {
    .description = "RES 4, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] &= ~(0b00000001 << 4);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xa2, 0b10100010: RES 4, D
  GBC::GameBoyColor::Instruction {
    .description = "RES 4, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] &= ~(0b00000001 << 4);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xa3, 0b10100011: RES 4, E
  GBC::GameBoyColor::Instruction {
    .description = "RES 4, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] &= ~(0b00000001 << 4);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xa4, 0b10100100: RES 4, H
  GBC::GameBoyColor::Instruction {
    .description = "RES 4, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] &= ~(0b00000001 << 4);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xa5, 0b10100101: RES 4, L
  GBC::GameBoyColor::Instruction {
    .description = "RES 4, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r &= ~(0b00000001 << 4);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xa6, 0b10100110: RES 4, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RES 4, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] &= ~(0b00000001 << 4);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xa7, 0b10100111: RES 4, A
  GBC::GameBoyColor::Instruction {
    .description = "RES 5, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] &= ~(0b00000001 << 5);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xa8, 0b10101000: RES 5, B
  GBC::GameBoyColor::Instruction {
    .description = "RES 5, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] &= ~(0b00000001 << 5);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xa9, 0b10101001: RES 5, C
  GBC::GameBoyColor::Instruction {
    .description = "RES 5, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] &= ~(0b00000001 << 5);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xaa, 0b10101010: RES 5, D
  GBC::GameBoyColor::Instruction {
    .description = "RES 5, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] &= ~(0b00000001 << 5);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xab, 0b10101011: RES 5, E
  GBC::GameBoyColor::Instruction {
    .description = "RES 5, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] &= ~(0b00000001 << 5);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xac, 0b10101100: RES 5, H
  GBC::GameBoyColor::Instruction {
    .description = "RES 5, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] &= ~(0b00000001 << 5);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xad, 0b10101101: RES 5, L
  GBC::GameBoyColor::Instruction {
    .description = "RES 5, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r &= ~(0b00000001 << 5);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xae, 0b10101110: RES 5, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RES 5, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] &= ~(0b00000001 << 5);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xaf, 0b10101111: RES 5, A
  GBC::GameBoyColor::Instruction {
    .description = "RES 6, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] &= ~(0b00000001 << 6);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xb0, 0b10110000: RES 6, B
  GBC::GameBoyColor::Instruction {
    .description = "RES 6, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] &= ~(0b00000001 << 6);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xb1, 0b10110001: RES 6, C
  GBC::GameBoyColor::Instruction {
    .description = "RES 6, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] &= ~(0b00000001 << 6);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xb2, 0b10110010: RES 6, D
  GBC::GameBoyColor::Instruction {
    .description = "RES 6, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] &= ~(0b00000001 << 6);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xb3, 0b10110011: RES 6, E
  GBC::GameBoyColor::Instruction {
    .description = "RES 6, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] &= ~(0b00000001 << 6);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xb4, 0b10110100: RES 6, H
  GBC::GameBoyColor::Instruction {
    .description = "RES 6, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] &= ~(0b00000001 << 6);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xb5, 0b10110101: RES 6, L
  GBC::GameBoyColor::Instruction {
    .description = "RES 6, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r &= ~(0b00000001 << 6);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xb6, 0b10110110: RES 6, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RES 6, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] &= ~(0b00000001 << 6);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xb7, 0b10110111: RES 6, A
  GBC::GameBoyColor::Instruction {
    .description = "RES 7, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] &= ~(0b00000001 << 7);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xb8, 0b10111000: RES 7, B
  GBC::GameBoyColor::Instruction {
    .description = "RES 7, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] &= ~(0b00000001 << 7);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xb9, 0b10111001: RES 7, C
  GBC::GameBoyColor::Instruction {
    .description = "RES 7, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] &= ~(0b00000001 << 7);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xba, 0b10111010: RES 7, D
  GBC::GameBoyColor::Instruction {
    .description = "RES 7, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] &= ~(0b00000001 << 7);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xbb, 0b10111011: RES 7, E
  GBC::GameBoyColor::Instruction {
    .description = "RES 7, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] &= ~(0b00000001 << 7);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xbc, 0b10111100: RES 7, H
  GBC::GameBoyColor::Instruction {
    .description = "RES 7, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] &= ~(0b00000001 << 7);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xbd, 0b10111101: RES 7, L
  GBC::GameBoyColor::Instruction {
    .description = "RES 7, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r &= ~(0b00000001 << 7);
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xbe, 0b10111110: RES 7, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "RES 7, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] &= ~(0b00000001 << 7);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xbf, 0b10111111: RES 7, A
  GBC::GameBoyColor::Instruction {
    .description = "SET 0, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] |= 0b00000001 << 0;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xc0, 0b11000000: SET 0, B
  GBC::GameBoyColor::Instruction {
    .description = "SET 0, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] |= 0b00000001 << 0;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xc1, 0b11000001: SET 0, C
  GBC::GameBoyColor::Instruction {
    .description = "SET 0, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] |= 0b00000001 << 0;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xc2, 0b11000010: SET 0, D
  GBC::GameBoyColor::Instruction {
    .description = "SET 0, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] |= 0b00000001 << 0;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xc3, 0b11000011: SET 0, E
  GBC::GameBoyColor::Instruction {
    .description = "SET 0, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] |= 0b00000001 << 0;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xc4, 0b11000100: SET 0, H
  GBC::GameBoyColor::Instruction {
    .description = "SET 0, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] |= 0b00000001 << 0;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xc5, 0b11000101: SET 0, L
  GBC::GameBoyColor::Instruction {
    .description = "SET 0, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r |= 0b00000001 << 0;
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xc6, 0b11000110: SET 0, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SET 0, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] |= 0b00000001 << 0;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xc7, 0b11000111: SET 0, A
  GBC::GameBoyColor::Instruction {
    .description = "SET 1, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] |= 0b00000001 << 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xc8, 0b11001000: SET 1, B
  GBC::GameBoyColor::Instruction {
    .description = "SET 1, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] |= 0b00000001 << 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xc9, 0b11001001: SET 1, C
  GBC::GameBoyColor::Instruction {
    .description = "SET 1, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] |= 0b00000001 << 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xca, 0b11001010: SET 1, D
  GBC::GameBoyColor::Instruction {
    .description = "SET 1, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] |= 0b00000001 << 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xcb, 0b11001011: SET 1, E
  GBC::GameBoyColor::Instruction {
    .description = "SET 1, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] |= 0b00000001 << 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xcc, 0b11001100: SET 1, H
  GBC::GameBoyColor::Instruction {
    .description = "SET 1, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] |= 0b00000001 << 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xcd, 0b11001101: SET 1, L
  GBC::GameBoyColor::Instruction {
    .description = "SET 1, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r |= 0b00000001 << 1;
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xce, 0b11001110: SET 1, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SET 1, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] |= 0b00000001 << 1;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xcf, 0b11001111: SET 1, A
  GBC::GameBoyColor::Instruction {
    .description = "SET 2, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] |= 0b00000001 << 2;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xd0, 0b11010000: SET 2, B
  GBC::GameBoyColor::Instruction {
    .description = "SET 2, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] |= 0b00000001 << 2;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xd1, 0b11010001: SET 2, C
  GBC::GameBoyColor::Instruction {
    .description = "SET 2, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] |= 0b00000001 << 2;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xd2, 0b11010010: SET 2, D
  GBC::GameBoyColor::Instruction {
    .description = "SET 2, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] |= 0b00000001 << 2;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xd3, 0b11010011: SET 2, E
  GBC::GameBoyColor::Instruction {
    .description = "SET 2, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] |= 0b00000001 << 2;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xd4, 0b11010100: SET 2, H
  GBC::GameBoyColor::Instruction {
    .description = "SET 2, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] |= 0b00000001 << 2;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xd5, 0b11010101: SET 2, L
  GBC::GameBoyColor::Instruction {
    .description = "SET 2, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r |= 0b00000001 << 2;
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xd6, 0b11010110: SET 2, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SET 2, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] |= 0b00000001 << 2;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xd7, 0b11010111: SET 2, A
  GBC::GameBoyColor::Instruction {
    .description = "SET 3, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] |= 0b00000001 << 3;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xd8, 0b11011000: SET 3, B
  GBC::GameBoyColor::Instruction {
    .description = "SET 3, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] |= 0b00000001 << 3;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xd9, 0b11011001: SET 3, C
  GBC::GameBoyColor::Instruction {
    .description = "SET 3, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] |= 0b00000001 << 3;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xda, 0b11011010: SET 3, D
  GBC::GameBoyColor::Instruction {
    .description = "SET 3, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] |= 0b00000001 << 3;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xdb, 0b11011011: SET 3, E
  GBC::GameBoyColor::Instruction {
    .description = "SET 3, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] |= 0b00000001 << 3;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xdc, 0b11011100: SET 3, H
  GBC::GameBoyColor::Instruction {
    .description = "SET 3, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] |= 0b00000001 << 3;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xdd, 0b11011101: SET 3, L
  GBC::GameBoyColor::Instruction {
    .description = "SET 3, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r |= 0b00000001 << 3;
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xde, 0b11011110: SET 3, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SET 3, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] |= 0b00000001 << 3;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xdf, 0b11011111: SET 3, A
  GBC::GameBoyColor::Instruction {
    .description = "SET 4, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] |= 0b00000001 << 4;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xe0, 0b11100000: SET 4, B
  GBC::GameBoyColor::Instruction {
    .description = "SET 4, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] |= 0b00000001 << 4;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xe1, 0b11100001: SET 4, C
  GBC::GameBoyColor::Instruction {
    .description = "SET 4, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] |= 0b00000001 << 4;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xe2, 0b11100010: SET 4, D
  GBC::GameBoyColor::Instruction {
    .description = "SET 4, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] |= 0b00000001 << 4;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xe3, 0b11100011: SET 4, E
  GBC::GameBoyColor::Instruction {
    .description = "SET 4, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] |= 0b00000001 << 4;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xe4, 0b11100100: SET 4, H
  GBC::GameBoyColor::Instruction {
    .description = "SET 4, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] |= 0b00000001 << 4;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xe5, 0b11100101: SET 4, L
  GBC::GameBoyColor::Instruction {
    .description = "SET 4, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r |= 0b00000001 << 4;
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xe6, 0b11100110: SET 4, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SET 4, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] |= 0b00000001 << 4;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xe7, 0b11100111: SET 4, A
  GBC::GameBoyColor::Instruction {
    .description = "SET 5, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] |= 0b00000001 << 5;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xe8, 0b11101000: SET 5, B
  GBC::GameBoyColor::Instruction {
    .description = "SET 5, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] |= 0b00000001 << 5;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xe9, 0b11101001: SET 5, C
  GBC::GameBoyColor::Instruction {
    .description = "SET 5, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] |= 0b00000001 << 5;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xea, 0b11101010: SET 5, D
  GBC::GameBoyColor::Instruction {
    .description = "SET 5, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] |= 0b00000001 << 5;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xeb, 0b11101011: SET 5, E
  GBC::GameBoyColor::Instruction {
    .description = "SET 5, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] |= 0b00000001 << 5;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xec, 0b11101100: SET 5, H
  GBC::GameBoyColor::Instruction {
    .description = "SET 5, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] |= 0b00000001 << 5;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xed, 0b11101101: SET 5, L
  GBC::GameBoyColor::Instruction {
    .description = "SET 5, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r |= 0b00000001 << 5;
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xee, 0b11101110: SET 5, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SET 5, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] |= 0b00000001 << 5;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xef, 0b11101111: SET 5, A
  GBC::GameBoyColor::Instruction {
    .description = "SET 6, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] |= 0b00000001 << 6;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xf0, 0b11110000: SET 6, B
  GBC::GameBoyColor::Instruction {
    .description = "SET 6, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] |= 0b00000001 << 6;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xf1, 0b11110001: SET 6, C
  GBC::GameBoyColor::Instruction {
    .description = "SET 6, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] |= 0b00000001 << 6;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xf2, 0b11110010: SET 6, D
  GBC::GameBoyColor::Instruction {
    .description = "SET 6, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] |= 0b00000001 << 6;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xf3, 0b11110011: SET 6, E
  GBC::GameBoyColor::Instruction {
    .description = "SET 6, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] |= 0b00000001 << 6;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xf4, 0b11110100: SET 6, H
  GBC::GameBoyColor::Instruction {
    .description = "SET 6, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] |= 0b00000001 << 6;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xf5, 0b11110101: SET 6, L
  GBC::GameBoyColor::Instruction {
    .description = "SET 6, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r |= 0b00000001 << 6;
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xf6, 0b11110110: SET 6, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SET 6, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] |= 0b00000001 << 6;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xf7, 0b11110111: SET 6, A
  GBC::GameBoyColor::Instruction {
    .description = "SET 7, B",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Hi] |= 0b00000001 << 7;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xf8, 0b11111000: SET 7, B
  GBC::GameBoyColor::Instruction {
    .description = "SET 7, C",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rBC.u8[Register::Lo] |= 0b00000001 << 7;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xf9, 0b11111001: SET 7, C
  GBC::GameBoyColor::Instruction {
    .description = "SET 7, D",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Hi] |= 0b00000001 << 7;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xfa, 0b11111010: SET 7, D
  GBC::GameBoyColor::Instruction {
    .description = "SET 7, E",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rDE.u8[Register::Lo] |= 0b00000001 << 7;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xfb, 0b11111011: SET 7, E
  GBC::GameBoyColor::Instruction {
    .description = "SET 7, H",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Hi] |= 0b00000001 << 7;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xfc, 0b11111100: SET 7, H
  GBC::GameBoyColor::Instruction {
    .description = "SET 7, L",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rHL.u8[Register::Lo] |= 0b00000001 << 7;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  },  // 0xfd, 0b11111101: SET 7, L
  GBC::GameBoyColor::Instruction {
    .description = "SET 7, (HL)",
    .instruction = [](GBC::GameBoyColor& gbc) {
      std::uint8_t  r = gbc.read<std::uint8_t>(gbc._rHL.u16);

      r |= 0b00000001 << 7;
      gbc.write<std::uint8_t>(gbc._rHL.u16, r);
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 16;
    }
  },  // 0xfe, 0b11111110: SET 7, (HL)
  GBC::GameBoyColor::Instruction {
    .description = "SET 7, A",
    .instruction = [](GBC::GameBoyColor& gbc) {
      gbc._rAF.u8[Register::Hi] |= 0b00000001 << 7;
      gbc._rPC.u16 += 1;
      gbc._cpuCycle += 8;
    }
  }   // 0xff, 0b11111111: SET 7, A
};

void  GBC::GameBoyColor::instructionAdd(std::uint8_t left, std::uint8_t right, std::uint8_t carry, std::uint8_t& result)
{
  result = left + right + carry;
  if (result == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  if ((left & 0b00001111) + (right & 0b00001111) + carry > 0b00001111)
    _rAF.u8[Register::Lo] |= Register::H;
  else
    _rAF.u8[Register::Lo] &= ~Register::H;
  if ((std::uint16_t)left + (std::uint16_t)right + (std::uint16_t)carry > 0b11111111)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
}

void  GBC::GameBoyColor::instructionAdd(std::uint16_t left, std::uint16_t right, std::uint16_t& result)
{
  result = left + right;
  _rAF.u8[Register::Lo] &= ~Register::N;
  if ((left & 0b0000111111111111) + (right & 0b0000111111111111) > 0b0000111111111111)
    _rAF.u8[Register::Lo] |= Register::H;
  else
    _rAF.u8[Register::Lo] &= ~Register::H;
  if ((std::uint32_t)left + (std::uint32_t)right > 0b1111111111111111)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
}

void  GBC::GameBoyColor::instructionSub(std::uint8_t left, std::uint8_t right, std::uint8_t carry, std::uint8_t& result)
{
  result = left - (right + carry);
  if (result == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] |= Register::N;
  if ((std::uint16_t)(left & 0b00001111) < (std::uint16_t)(right & 0b00001111) + (std::uint16_t)carry)
    _rAF.u8[Register::Lo] |= Register::H;
  else
    _rAF.u8[Register::Lo] &= ~Register::H;
  if ((std::uint16_t)left < (std::uint16_t)right + (std::uint16_t)carry)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
}

void  GBC::GameBoyColor::instructionCp(std::uint8_t left, std::uint8_t right)
{
  std::uint8_t  result;

  instructionSub(left, right, 0, result);
}

void  GBC::GameBoyColor::instructionInc(std::uint8_t& value)
{
  value += 1;
  if (value == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  if ((value & 0b00001111) == 0b00000000)
    _rAF.u8[Register::Lo] |= Register::H;
  else
    _rAF.u8[Register::Lo] &= ~Register::H;
}

void  GBC::GameBoyColor::instructionDec(std::uint8_t& value)
{
  value -= 1;
  if (value == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] |= Register::N;
  if ((value & 0b00001111) == 0b00001111)
    _rAF.u8[Register::Lo] &= ~Register::H;
  else
    _rAF.u8[Register::Lo] |= Register::H;
}

void  GBC::GameBoyColor::instructionAnd(std::uint8_t left, std::uint8_t right, std::uint8_t& result)
{
  result = left & right;
  if (result == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] |= Register::H;
  _rAF.u8[Register::Lo] &= ~Register::C;
}

void  GBC::GameBoyColor::instructionOr(std::uint8_t left, std::uint8_t right, std::uint8_t& result)
{
  result = left | right;
  if (result == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  _rAF.u8[Register::Lo] &= ~Register::C;
}

void  GBC::GameBoyColor::instructionXor(std::uint8_t left, std::uint8_t right, std::uint8_t& result)
{
  result = left ^ right;
  if (_rAF.u8[Register::Hi] == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  _rAF.u8[Register::Lo] &= ~Register::C;
}

void  GBC::GameBoyColor::instructionRlc(std::uint8_t& value)
{
  if (value == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  if (value & 0b10000000)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
  value = (value << 1) | (value >> 7);
}

void  GBC::GameBoyColor::instructionRrc(std::uint8_t& value)
{
  if (value == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  if (value & 0b00000001)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
  value =  (value >> 1) | ((value & 0b00000001) ? 0b10000000 : 0b00000000);
}

void  GBC::GameBoyColor::instructionRl(std::uint8_t& value)
{
  std::uint8_t  carry = (_rAF.u8[Register::Lo] & Register::C) ? 0b00000001 : 0b00000000;

  if ((value & 0b01111111) == 0 && carry == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  if (value & 0b10000000)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
  value = (value << 1) | carry;
}

void  GBC::GameBoyColor::instructionRr(std::uint8_t& value)
{
  std::uint8_t  carry = (_rAF.u8[Register::Lo] & Register::C) ? 0b10000000 : 0b00000000;

  if ((value & 0b11111110) == 0 && carry == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  if (value & 0b00000001)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
  value = (value >> 1) | carry;
}

void  GBC::GameBoyColor::instructionSla(std::uint8_t& value)
{
  if ((value & 0b01111111) == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  if (value & 0b10000000)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
  value <<= 1;
}

void  GBC::GameBoyColor::instructionSra(std::uint8_t& value)
{
  if ((value & 0b11111110) == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  if (value & 0b00000001)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
  value = (value >> 1) | (value & 0b10000000);
}

void  GBC::GameBoyColor::instructionSwap(std::uint8_t& value)
{
  value = (value >> 4) | (value << 4);
  if (value == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  _rAF.u8[Register::Lo] &= ~Register::C;
}

void  GBC::GameBoyColor::instructionSrl(std::uint8_t& value)
{
  if ((value & 0b11111110) == 0)
    _rAF.u8[Register::Lo] |= Register::Z;
  else
    _rAF.u8[Register::Lo] &= ~Register::Z;
  _rAF.u8[Register::Lo] &= ~Register::N;
  _rAF.u8[Register::Lo] &= ~Register::H;
  if (value & 0b00000001)
    _rAF.u8[Register::Lo] |= Register::C;
  else
    _rAF.u8[Register::Lo] &= ~Register::C;
  value >>= 1;
}

void  GBC::GameBoyColor::instructionPush(std::uint16_t value)
{
  _rSP.u16 -= 2;
  write<std::uint16_t>(_rSP.u16, value);
}

void  GBC::GameBoyColor::instructionPop(std::uint16_t& result)
{
  result = read<std::uint16_t>(_rSP.u16);
  _rSP.u16 += 2;
}

void  GBC::GameBoyColor::instructionCall(std::uint16_t addr, std::uint16_t ret)
{
  instructionPush(ret);
  _rPC.u16 = addr;
}

void  GBC::GameBoyColor::instructionRet()
{
  instructionPop(_rPC.u16);
}

GBC::GameBoyColor::GameBoyColor(const std::string& filename) :
  _boot(),
  _rom(),
  _vRam(),
  _eRam(),
  _wRam(),
  _oam(),
  _io(),
  _hRam(),
  _ie(0),
  _bgcRam(),
  _obcRam(),
  _cpu(CPU::CPURun),
  _ime(IME::IMEDisabled),
  _cpuCycle(0),
  _ppuCycle(0),
  _ppuObj(),
  _ppuLcd(),
  _header(),
  _rAF{ .u16 = 0x0000 },
  _rBC{ .u16 = 0x0000 },
  _rDE{ .u16 = 0x0000 },
  _rHL{ .u16 = 0x0000 },
  _rSP{ .u16 = 0xFFFE },
  _rPC{ .u16 = 0x0000 },
  _mbc()
{
  // Set screen size and initial color
  _ppuLcd.create(160, 144, sf::Color::White);

  // Load ROM
  load(filename);

  // Initialize memory banks
  _vRam.fill(0);
  _wRam.fill(0);
  _oam.fill(0);
  _io.fill(0);
  _hRam.fill(0);
  _bgcRam.fill(0);
  _obcRam.fill(0);

  // Initialize Joypad
  _io[Registers::RegisterJOYP] = 0b11111111;

  // Initialize MBC
  switch (_header.mbc) {
  case MemoryBankController::Type::None:
    break;

  case MemoryBankController::Type::MBC1:
    _mbc.mbc1.enable = 0;
    _mbc.mbc1.bank = 0b00000001;
    _eRam.resize(_header.ram_size, 0);
    break;

  case MemoryBankController::Type::MBC2:
    _mbc.mbc2.enable = 0;
    _mbc.mbc2.bank = 0b00000001;
    _eRam.resize(512, 0); // TODO: load external RAM from save file
    break;

  case MemoryBankController::Type::MBC3:
    _mbc.mbc3.enable = 0;
    _mbc.mbc3.rom = 1;
    _mbc.mbc3.ram = 0;
    _mbc.mbc3.latch = 0;
    _eRam.resize(_header.ram_size, 0);
    break;

  case MemoryBankController::Type::MBC5:
    _mbc.mbc5.enable = 0;
    _mbc.mbc5.rom = 1;
    _mbc.mbc5.ram = 0;
    _eRam.resize(_header.ram_size, 0);
    break;

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

GBC::GameBoyColor::~GameBoyColor()
{}

void  GBC::GameBoyColor::load(const std::string& filename)
{
  // Load ROM to memory
  loadFile(filename, _rom);

  // Parse ROM header
  loadHeader();

  // Load bootstrap bios to memory
  loadFile(Game::Config::ExecutablePath + "/assets/gbc/cgb_boot.bin", _boot);
}

void  GBC::GameBoyColor::loadFile(const std::string& filename, std::vector<std::uint8_t>& destination)
{
  std::ifstream file(filename, std::ifstream::binary);

  // Check if file open properly
  if (file.good() == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Reset ROM memory
  destination.clear();
  destination.resize(std::filesystem::file_size(filename), 0);

  // Load ROM
  file.read((char*)destination.data(), destination.size());

  // Check for error
  if (file.gcount() != destination.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  GBC::GameBoyColor::loadHeader()
{
  // Original Nintendo Logo Bitmap
  std::uint8_t  nintendo[] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
    0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
    0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
  };

  // Logo should be identical
  _header.logo = (std::memcmp(nintendo, _rom.data() + 0x0104, sizeof(nintendo)) == 0);

  // Game title
  _header.title.clear();
  for (unsigned int index = 0x0134; index <= 0x0143 && _rom[index] != '\0'; index++)
    _header.title += _rom[index];

  // CGB format (we don't handle intermediary format between GB & CGB)
  if (_rom[0x0143] & 0b10000000)
  {
    // 11 characters title
    _header.title = _header.title.substr(0, 11);

    // 4 characters manufacturer
    _header.manufacturer.clear();
    for (unsigned int index = 0x013F; index <= 0x0142 && _rom[index] != '\0'; index++)
      _header.manufacturer += _rom[index];

    // CGB flag
    switch (_rom[0x0143] & 0b11000000) {
    case 0x80:
      _header.cgb = Header::CGBFlag::CGBSupport;
      break;
    case 0xC0:
      _header.cgb = Header::CGBFlag::CGBOnly;
      break;
    default:
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
  }

  // GB format
  else {
    _header.manufacturer = "";
    _header.cgb = Header::CGBFlag::CGBNone;
  }

  // Old licensee code
  if (_rom[0x0144] == 0x01 && _rom[0x0145] == 0x4B)
    _header.licensee = _rom[0x014B];
  // New Lecensee code
  else {
    ((uint8_t*)&_header.licensee)[0] = _rom[0x0144];
    ((uint8_t*)&_header.licensee)[1] = _rom[0x0145];
  }

  // SGB flag
  switch (_rom[0x0146]) {
  case 0x00:
    _header.sgb = Header::SGBFlag::SGBNone;
    break;
  case 0x03:
    _header.sgb = Header::SGBFlag::SGBSupport;
    break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // Cartridge Type
  switch (_rom[0x0147]) {
  case 0x00:
  case 0x08:
  case 0x09:
    _header.mbc = MemoryBankController::Type::None;
    break;
  case 0x01:
  case 0x02:
  case 0x03:
    _header.mbc = MemoryBankController::Type::MBC1;
    break;
  case 0x05:
  case 0x06:
    _header.mbc = MemoryBankController::Type::MBC2;
    break;
  case 0x0F:
  case 0x10:
  case 0x11:
  case 0x12:
  case 0x13:
    _header.mbc = MemoryBankController::Type::MBC3;
    break;
  case 0x19:
  case 0x1A:
  case 0x1B:
  case 0x1C:
  case 0x1D:
  case 0x1E:
    _header.mbc = MemoryBankController::Type::MBC5;
    break;
  default:
    _header.mbc = MemoryBankController::Type::Unknow;
  }

  // ROM size
  switch (_rom[0x0148])
  {
  case 0x52:
    _header.rom_size = 72 * 16384;
    break;
  case 0x53:
    _header.rom_size = 80 * 16384;
    break;
  case 0x54:
    _header.rom_size = 96 * 16384;
    break;
  default:
    _header.rom_size = (0b00000010 << _rom[0x0148]) * 16384;
    break;
  }

  // RAM size
  switch (_rom[0x0149])
  {
  case 0x00:
    _header.ram_size = 0;
    break;
  case 0x01:
    _header.ram_size = 2 * 1024;
    break;
  case 0x02:
    _header.ram_size = 8 * 1024;
    break;
  case 0x03:
    _header.ram_size = 32 * 1024;
    break;
  case 0x04:
    _header.ram_size = 128 * 1024;
    break;
  case 0x05:
    _header.ram_size = 64 * 1024;
    break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // Region
  switch (_rom[0x014A]) {
  case 0x00:
    _header.region = Header::Region::RegionJP;
    break;
  case 0x01:
    _header.region = Header::Region::RegionUSEU;
    break;
  default:
    _header.region = Header::Region::RegionUnknow;
    break;
  }

  // Game ROM version
  _header.version = _rom[0x014C];

  std::uint8_t  header_checksum = 0;

  // Compute header checksum
  for (unsigned int index = 0x0134; index <= 0x014C; index++)
    header_checksum = header_checksum - _rom[index] - 1;

  // Check header checksum
  _header.header_checksum = (header_checksum == _rom[0x014D]);

  // Display warning if invalid
  if (_header.header_checksum == false)
    std::cerr << "[EmulationScene::load]: Warning, invalid header checksum." << std::endl;

  std::uint16_t global_checksum = 0;

  // Compute global checksum
  for (unsigned int index = 0; index < _rom.size(); index++)
    if (index < 0x014E || index > 0x014F)
      global_checksum += _rom[index];

  // Check global checksum
  _header.global_checksum = (global_checksum == _rom[0x014E] * 256 + _rom[0x014F]);

  // Display warning if invalid
  if (_header.global_checksum == false)
    std::cerr << "[EmulationScene::load]: Warning, invalid global checksum." << std::endl;

  // TODO: remove this
  std::cout << "[ROM HEADER]:" << std::endl;
  std::cout << "  logo: " << (_header.logo ? "true" : "false") << std::endl;
  std::cout << "  title: " << _header.title << std::endl;
  std::cout << "  manufacturer: " << _header.manufacturer << std::endl;
  std::cout << "  cgb: " << _header.cgb << std::endl;
  std::cout << "  licensee: " << _header.licensee << std::endl;
  std::cout << "  sgb: " << _header.sgb << std::endl;
  std::cout << "  mbc: " << _header.mbc << std::endl;
  std::cout << "  rom_size: " << _header.rom_size << std::endl;
  std::cout << "  ram_size: " << _header.ram_size << std::endl;
  std::cout << "  region: " << _header.region << std::endl;
  std::cout << "  version: " << (int)_header.version << std::endl;
  std::cout << "  header_checksum: " << (_header.header_checksum ? "true" : "false") << std::endl;
  std::cout << "  global_checksum: " << (_header.global_checksum ? "true" : "false") << std::endl;
}

void  GBC::GameBoyColor::simulate()
{
  std::uint64_t frame = _ppuCycle / (456 * 154);
  std::uint64_t cycle = _cpuCycle;
  std::uint64_t second = _cpuCycle / (4 * 1024 * 1024);

  // Simulate Joypad
  simulateKeys();

  // Execution loop
  while (frame == _ppuCycle / (456 * 154))
  {
    // Handle interrupt
    simulateInterrupt();

    // Run one instruction
    if (_cpu == CPU::CPURun)
      simulateInstruction();
    else
      _cpuCycle += 4;

    // Update graphics, audio and timer for the number of cycle executed
    for (; cycle < _cpuCycle; cycle += 4) {
      simulateTimer();
      if (cycle % ((_io[Registers::RegisterKEY1] & 0b10000000) ? 8 : 4) == 0) {
        simulateGraphics();
        simulateAudio();
      }
    }

    // Update timer for MBC3
    if (_header.mbc == MemoryBankController::Type::MBC3 && second != _cpuCycle / (4 * 1024 * 1024)) {
      if (_mbc.mbc3.halt == 0)
        _mbc.mbc3.time += _cpuCycle / (4 * 1024 * 1024) - second;
      second = _cpuCycle / (4 * 1024 * 1024);
    }
  }
}

void  GBC::GameBoyColor::simulateKeys()
{
  std::array<bool, Key::KeyCount> keys = {
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) < -0.5f,
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) > +0.5f,
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovX) < -0.5f,
    Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovX) > +0.5f,
    Game::Window::Instance().joystick().buttonDown(0, 7),
    Game::Window::Instance().joystick().buttonDown(0, 6),
    Game::Window::Instance().joystick().buttonDown(0, 1),
    Game::Window::Instance().joystick().buttonDown(0, 0)
  };

  // Joypad interrupt when a selected key is pressed
  if ((!(_io[Registers::RegisterJOYP] & 0b00010000) &&
    (_keys[Key::KeyDown] == false && keys[Key::KeyDown] == true ||
      _keys[Key::KeyUp] == false && keys[Key::KeyUp] == true ||
      _keys[Key::KeyLeft] == false && keys[Key::KeyLeft] == true ||
      _keys[Key::KeyRight] == false && keys[Key::KeyRight] == true))
    ||
    (!(_io[Registers::RegisterJOYP] & 0b00100000) &&
      (_keys[Key::KeyStart] == false && keys[Key::KeyStart] == true ||
        _keys[Key::KeySelect] == false && keys[Key::KeySelect] == true ||
        _keys[Key::KeyB] == false && keys[Key::KeyB] == true ||
        _keys[Key::KeyA] == false && keys[Key::KeyA] == true)))
    _io[Registers::RegisterIF] |= Interrupt::InterruptJoypad;

  // Save new key map
  _keys = keys;
}

void  GBC::GameBoyColor::simulateInterrupt()
{
  // Enable scheduled interrupt flag for next tick
  if (_ime == IME::IMEScheduled) {
    _ime = IME::IMEEnabled;
    return;
  }

  // Check if interrupts are enabled
  if (_ime != IME::IMEEnabled && _cpu != CPU::CPUHalt)
    return;

  // Define interrupt priority
  auto interrupts = {
    Interrupt::InterruptVBlank,
    Interrupt::InterruptLcdStat,
    Interrupt::InterruptTimer,
    Interrupt::InterruptSerial,
    Interrupt::InterruptJoypad
  };

  // Check each interrupt type
  for (auto interrupt : interrupts)
  {
    // Check interrupt flag on IE
    if ((_ie & interrupt) == 0)
      continue;

    std::uint8_t  rif = _io[Registers::RegisterIF];

    // Check interrupt flag on IF
    if ((rif & interrupt) == 0)
      continue;

    // Reset interrupt flag
    _io[Registers::RegisterIF] = rif & ~interrupt;

    // Call interrupt handler
    if (_ime == IME::IMEEnabled)
    {
      // Disable interrupt
      _ime = IME::IMEDisabled;

      // Resume execution
      _cpu = CPU::CPURun;

      // Call interrupt handler
      _rSP.u16 -= 2;
      write<std::uint16_t>(_rSP.u16, _rPC.u16);
      _rPC.u16 = 0x0040;
      while ((interrupt = (GameBoyColor::Interrupt)(interrupt >> 1)) != 0)
        _rPC.u16 += 0x0008;
      _cpuCycle += 20;
    }

    // CPU is halted, resume without calling interrupt handler
    else
      _cpu = CPU::CPURun;

    return;
  }
}

void  GBC::GameBoyColor::simulateInstruction()
{
  // Read instruction opcode
  auto opcode = read<std::uint8_t>(_rPC.u16);

  // TODO: remove this
  if (false) {
    if (false) {
      std::printf("\n");
      std::printf("  A: %3d 0x%02X    Z: %d   N: %d   H: %d   C: %d\n", _rAF.u8[Register::Hi], _rAF.u8[Register::Hi], (_rAF.u8[Register::Lo] & Register::Z) ? 1 : 0, (_rAF.u8[Register::Lo] & Register::N) ? 1 : 0, (_rAF.u8[Register::Lo] & Register::H) ? 1 : 0, (_rAF.u8[Register::Lo] & Register::C) ? 1 : 0);
      std::printf("  B: %3d 0x%02X    C: %3d 0x%02X   BC: %5d 0x%04X\n", _rBC.u8[Register::Hi], _rBC.u8[Register::Hi], _rBC.u8[Register::Lo], _rBC.u8[Register::Lo], _rBC.u16, _rBC.u16);
      std::printf("  D: %3d 0x%02X    E: %3d 0x%02X   DE: %5d 0x%04X\n", _rDE.u8[Register::Hi], _rDE.u8[Register::Hi], _rDE.u8[Register::Lo], _rDE.u8[Register::Lo], _rDE.u16, _rDE.u16);
      std::printf("  H: %3d 0x%02X    L: %3d 0x%02X   HL: %5d 0x%04X\n", _rHL.u8[Register::Hi], _rHL.u8[Register::Hi], _rHL.u8[Register::Lo], _rHL.u8[Register::Lo], _rHL.u16, _rHL.u16);
      std::printf("  SP: %5d 0x%04X             PC: %5d 0x%04X\n", _rSP.u16, _rSP.u16, _rPC.u16, _rPC.u16);
      std::printf("\n");
    }
    std::printf("[PC 0x%04X](OP 0x%02X) %s\n", _rPC.u16, opcode, opcode == 0xCB ? _instructionsCB[read<std::uint8_t>(_rPC.u16 + 1)].description.c_str() : _instructions[opcode].description.c_str());
  }

  // Execute instruction
  _instructions[opcode].instruction(*this);
}

void  GBC::GameBoyColor::simulateGraphics()
{
  // Only render lines 0 to 143
  if (_io[Registers::RegisterLY] < 144)
  {
    // Search OAM for OBJs that overlap current line
    if (_ppuCycle % 456 == 0) {
      simulateGraphicsMode2();

      // STAT mode 2 (OAM) interrupt
      if (_io[Registers::RegisterSTAT] & LcdStatus::LcdStatusMode2)
        _io[Registers::RegisterIF] |= Interrupt::InterruptLcdStat;
    }

    // Generate the picture
    else if (_ppuCycle % 456 == 80)
      simulateGraphicsMode3();

    // Trigger HBlank
    else if (_ppuCycle % 456 == 252)
    {
      // Set LCD status mode 0
      _io[Registers::RegisterSTAT] = (_io[Registers::RegisterSTAT] & 0b11111100) | 0b00000000;

      // STAT mode 0 (HBlank) interrupt
      if (_io[Registers::RegisterSTAT] & LcdStatus::LcdStatusMode0)
        _io[Registers::RegisterIF] |= Interrupt::InterruptLcdStat;
    }
  }

  // Trigger VBlank
  else if (_io[Registers::RegisterLY] == 144 && _ppuCycle % 456 == 0) {
    // Set LCD status mode 1
    _io[Registers::RegisterSTAT] = (_io[Registers::RegisterSTAT] & 0b11111100) | 0b00000001;

    // VBlank interrupt
    _io[Registers::RegisterIF] |= Interrupt::InterruptVBlank;

    // STAT mode 2 (VBlank) interrupt
    if (_io[Registers::RegisterSTAT] & LcdStatus::LcdStatusMode1)
      _io[Registers::RegisterIF] |= Interrupt::InterruptLcdStat;
  }

  // Next line
  if (_ppuCycle % 456 == 252)
  {
    // LY / LCY register comparison
    if (_io[Registers::RegisterLY] == _io[Registers::RegisterLYC]) {
      _io[Registers::RegisterSTAT] |= LcdStatus::LcdStatusEqual;

      // STAT compare (LY/LYC) interrupt
      if (_io[Registers::RegisterSTAT] & LcdStatus::LcdStatusCompare)
        _io[Registers::RegisterIF] |= Interrupt::InterruptLcdStat;
    }
    else
      _io[Registers::RegisterSTAT] &= ~LcdStatus::LcdStatusEqual;

    // Increment to next line
    _io[Registers::RegisterLY] += 1;
    
    // Limit to 144 + 10 lines
    if (_io[Registers::RegisterLY] >= 144 + 10)
      _io[Registers::RegisterLY] = 0;
  }

  // Advance in PPU simulation
  _ppuCycle += 4;
}

void  GBC::GameBoyColor::simulateGraphicsMode2()
{
  // Set LCD status mode 2
  _io[Registers::RegisterSTAT] = (_io[Registers::RegisterSTAT] & 0b11111100) | 0b00000010;

  // Clear OBJ list
  _ppuObj.clear();

  // Push Y matching OBJ in order
  for (std::uint8_t index = 0; index < _oam.size() / 4; index++)
    if (_oam[index * 4 + 0] <= _io[Registers::RegisterLY] + 16 && _oam[index * 4 + 0] + ((_io[Registers::RegisterLCDC] & LcdControl::LcdControlObjSize) ? 16 : 8) > _io[Registers::RegisterLY] + 16)
      _ppuObj.push_back(index);

  // Limit to 10 OBJ
  if (_ppuObj.size() > 10)
    _ppuObj.erase(std::next(_ppuObj.begin(), 10), _ppuObj.end());

  // In non-CGB mode, OBJ draw priority depends on its X position
  if (_io[Registers::RegisterOPRI] & 0b00000001)
    _ppuObj.sort([this](const auto& a, const auto& b) { return _oam[a * 4 + 1] < _oam[b * 4 + 1]; });
}

void  GBC::GameBoyColor::simulateGraphicsMode3()
{
  // Set LCD status mode 3
  _io[Registers::RegisterSTAT] = (_io[Registers::RegisterSTAT] & 0b11111100) | 0b00000011;

  // White screen if LCD disabled
  if (!(_io[Registers::RegisterLCDC] & LcdControl::LcdControlEnable)) {
    std::memset((std::uint8_t*)_ppuLcd.getPixelsPtr(), 0xFF, _ppuLcd.getSize().x * _ppuLcd.getSize().y * 4);
    return;
  }

  std::uint8_t  sc_y = _io[Registers::RegisterLY];
  std::uint8_t  bg_y = sc_y + _io[Registers::RegisterSCY];
  std::uint8_t  wn_y = sc_y - _io[Registers::RegisterWY];

  for (std::uint8_t sc_x = 0; sc_x < _ppuLcd.getSize().x; sc_x++)
  {
    std::uint8_t  bg_color_id = 255, bg_attributes = 0, bg_color_r, bg_color_g, bg_color_b;
    std::uint8_t  sp_color_id = 255, sp_attributes = 0, sp_color_r, sp_color_g, sp_color_b;

    // Window
    if ((!(_io[Registers::RegisterKEY0] & 0b00001100) || (_io[Registers::RegisterLCDC] & LcdControl::LcdControlWindowBackgroundEnable)) &&
      (_io[Registers::RegisterLCDC] & LcdControl::LcdControlWindowEnable) &&
      (sc_x + 7 >= _io[Registers::RegisterWX]) &&
      (sc_y >= _io[Registers::RegisterWY]))
    {
      std::uint8_t  wn_x = sc_x - _io[Registers::RegisterWX] + 7;
      std::uint16_t wn_tilemap = (_io[Registers::RegisterLCDC] & LcdControl::LcdControlWindowTilemap) ? 0x1C00 : 0x1800;
      std::uint16_t wn_tilemap_id = (((std::uint16_t)wn_y / 8) * 32) + ((std::uint16_t)wn_x / 8);
      bg_attributes = _vRam[wn_tilemap + wn_tilemap_id + 0x2000];
      std::uint8_t  wn_tile_id = _vRam[wn_tilemap + wn_tilemap_id];
      std::uint16_t wn_tile_index = ((std::uint16_t)wn_tile_id * 16)
        + ((_io[Registers::RegisterLCDC] & LcdControl::LcdControlData) ?
          0x0000 :
          ((wn_tile_id < 128) ? 0x1000 : 0x0000))
        + ((bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ?
          0x2000 :
          0x0000);

      // Flip tile coordinates
      if (bg_attributes & BackgroundAttributes::BackgroundAttributesYFlip)
        wn_y = (8 - 1) - wn_y % 8;
      if (bg_attributes & BackgroundAttributes::BackgroundAttributesXFlip)
        wn_x = (8 - 1) - wn_x % 8;

      bg_color_id =
        ((_vRam[wn_tile_index + (wn_y % 8) * 2 + 0] & (0b10000000 >> (wn_x % 8))) ? 1 : 0) +
        ((_vRam[wn_tile_index + (wn_y % 8) * 2 + 1] & (0b10000000 >> (wn_x % 8))) ? 2 : 0);

      // DMG color palette
      if (_io[Registers::RegisterKEY0] & 0b00001100)
        bg_color_id = (_io[Registers::RegisterBGP] >> (bg_color_id * 2)) & 0b00000011;

      // Get color from palette
      std::uint16_t wn_color =
        ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 0] << 0) +
        ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 1] << 8);

      bg_color_r = ((wn_color >> 0) & 0b00011111) * 8;
      bg_color_g = ((wn_color >> 5) & 0b00011111) * 8;
      bg_color_b = ((wn_color >> 10) & 0b00011111) * 8;
    }

    // Background
    else if (!(_io[Registers::RegisterKEY0] & 0b00001100) || (_io[Registers::RegisterLCDC] & LcdControl::LcdControlWindowBackgroundEnable))
    {
      std::uint8_t  bg_x = sc_x + _io[Registers::RegisterSCX];
      std::uint16_t bg_tilemap = (_io[Registers::RegisterLCDC] & LcdControl::LcdControlBackgroundTilemap) ? 0x1C00 : 0x1800;
      std::uint16_t bg_tilemap_id = (((std::uint16_t)bg_y / 8) * 32) + ((std::uint16_t)bg_x / 8);
      bg_attributes = _vRam[bg_tilemap + bg_tilemap_id + 0x2000];
      std::uint8_t  bg_tile_id = _vRam[bg_tilemap + bg_tilemap_id];
      std::uint16_t bg_tile_index = ((std::uint16_t)bg_tile_id * 16)
        + ((_io[Registers::RegisterLCDC] & LcdControl::LcdControlData) ?
          0x0000 :
          ((bg_tile_id < 128) ? 0x1000 : 0x0000))
        + ((bg_attributes & BackgroundAttributes::BackgroundAttributesBank) ?
          0x2000 :
          0x0000);

      // Flip tile coordinates
      if (bg_attributes & BackgroundAttributes::BackgroundAttributesYFlip)
        bg_y = (8 - 1) - bg_y % 8;
      if (bg_attributes & BackgroundAttributes::BackgroundAttributesXFlip)
        bg_x = (8 - 1) - bg_x % 8;

      bg_color_id =
        ((_vRam[bg_tile_index + (bg_y % 8) * 2 + 0] & (0b10000000 >> (bg_x % 8))) ? 1 : 0) +
        ((_vRam[bg_tile_index + (bg_y % 8) * 2 + 1] & (0b10000000 >> (bg_x % 8))) ? 2 : 0);

      // DMG color palette
      if (_io[Registers::RegisterKEY0] & 0b00001100)
        bg_color_id = (_io[Registers::RegisterBGP] >> (bg_color_id * 2)) & 0b00000011;

      // Get color from palette
      std::uint16_t bg_color =
        ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 0] << 0) +
        ((std::uint16_t)_bgcRam[((bg_attributes & BackgroundAttributes::BackgroundAttributesPalette) * 4 + bg_color_id) * 2 + 1] << 8);

      bg_color_r = ((bg_color >> 0) & 0b00011111) * 8;
      bg_color_g = ((bg_color >> 5) & 0b00011111) * 8;
      bg_color_b = ((bg_color >> 10) & 0b00011111) * 8;
    }

    // Sprite
    if (_io[Registers::RegisterLCDC] & LcdControl::LcdControlObjEnable)
    {
      // Find first matching sprite
      for (const auto& sp_index : _ppuObj) {
        std::uint8_t  sp_y = sc_y - _oam[sp_index * 4 + 0] + 16;
        std::uint8_t  sp_x = sc_x - _oam[sp_index * 4 + 1] + 8;

        // Sprite not on column
        if (sp_x >= 8)
          continue;

        std::uint8_t  sp_height = (_io[Registers::RegisterLCDC] & LcdControl::LcdControlObjSize) ? 16 : 8;
        sp_attributes = _oam[sp_index * 4 + 3];

        // Flip sprite coordinates
        if (sp_attributes & SpriteAttributes::SpriteAttributesYFlip)
          sp_y = (sp_height - 1) - sp_y % sp_height;
        if (sp_attributes & SpriteAttributes::SpriteAttributesXFlip)
          sp_x = (8 - 1) - sp_x % 8;

        std::uint8_t  sp_tile_id = (_oam[sp_index * 4 + 2] & ((sp_height == 8) ? 0b11111111 : 0b11111110)) + ((sp_y < 8) ? 0 : 1);
        std::uint16_t sp_tile_index = ((std::uint16_t)sp_tile_id * 16)
          + (!(sp_attributes & SpriteAttributes::SpriteAttributesBank) ?
            0x0000 :
            0x2000);
        sp_color_id =
          ((_vRam[sp_tile_index + (sp_y % 8) * 2 + 0] & (0b10000000 >> (sp_x % 8))) ? 1 : 0) +
          ((_vRam[sp_tile_index + (sp_y % 8) * 2 + 1] & (0b10000000 >> (sp_x % 8))) ? 2 : 0);

        // ID 0 is transparency
        if (sp_color_id == 0) {
          sp_color_id = 255;
          continue;
        }

        // DMG color palette
        if (_io[Registers::RegisterKEY0] & 0b00001100) {
          if (sp_attributes & SpriteAttributes::SpriteAttributesPaletteNonCgb) {
            sp_color_id = (_io[Registers::RegisterOBP1] >> (sp_color_id * 2)) & 0b00000011;
            sp_attributes = (sp_attributes & 0b11110000) | 0b00000001;
          }
          else {
            sp_color_id = (_io[Registers::RegisterOBP0] >> (sp_color_id * 2)) & 0b00000011;
            sp_attributes = (sp_attributes & 0b11110000) | 0b00000000;
          }
        }

        // Get color from palette
        std::uint16_t sp_color =
          ((std::uint16_t)_obcRam[((sp_attributes & SpriteAttributes::SpriteAttributesPaletteCgb) * 4 + sp_color_id) * 2 + 0] << 0) +
          ((std::uint16_t)_obcRam[((sp_attributes & SpriteAttributes::SpriteAttributesPaletteCgb) * 4 + sp_color_id) * 2 + 1] << 8);

        sp_color_r = ((sp_color >> 0) & 0b00011111) * 8;
        sp_color_g = ((sp_color >> 5) & 0b00011111) * 8;
        sp_color_b = ((sp_color >> 10) & 0b00011111) * 8;

        break;
      }
    }

    // Background and Window Master Priority, CGB mode only
    if (bg_color_id != 255 &&
      sp_color_id != 255 &&
      !(_io[Registers::RegisterKEY0] & 0b00001100) &&
      !(_io[Registers::RegisterLCDC] & LcdControl::LcdControlPriority))
      bg_color_id = 255;

    // Sprite priority over background and window
    else if (bg_color_id != 255 &&
      sp_color_id != 255 &&
      ((bg_attributes & BackgroundAttributes::BackgroundAttributesPriority) || (sp_attributes & SpriteAttributes::SpriteAttributesOverObj)) &&
      bg_color_id != 0)
      sp_color_id = 255;

    sf::Color color;

    // Select color to use
    if (sp_color_id != 255)
      color = sf::Color(sp_color_r, sp_color_g, sp_color_b);
    else if (bg_color_id != 255)
      color = sf::Color(bg_color_r, bg_color_g, bg_color_b);
    else
      color = sf::Color(255, 255, 255);

    // Draw pixel
    _ppuLcd.setPixel(sc_x, sc_y, color);
  }
}

void  GBC::GameBoyColor::simulateAudio()
{
  // TODO
}

void  GBC::GameBoyColor::simulateTimer()
{
  std::uint16_t div = ((std::uint16_t)_io[Registers::RegisterDIVHi] << 8) + (std::uint16_t)_io[Registers::RegisterDIVLo] + 4;

  // Update DIV timer
  _io[Registers::RegisterDIVHi] = (div >> 8) & 0b11111111;
  _io[Registers::RegisterDIVLo] = div & 0b11111111;

  // Update Timer only when enabled
  if (_io[Registers::RegisterTAC] & 0b00000100) {
    const std::array<int, 4>  modulo = { 1024, 16, 64, 256 };

    // Tick the timer
    if (div % modulo[_io[Registers::RegisterTAC] & 0b11] == 0) {
      _io[Registers::RegisterTIMA] += 1;

      // Interrupt when overflow
      if (_io[Registers::RegisterTIMA] == 0) {
        // Set interrupt flags
        _io[Registers::RegisterIF] |= Interrupt::InterruptTimer;

        // Reset timer
        _io[Registers::RegisterTIMA] = _io[Registers::RegisterTMA];
      }
    }
  }
}

const sf::Image& GBC::GameBoyColor::screen() const
{
  return _ppuLcd;
}

std::uint8_t  GBC::GameBoyColor::read(std::uint16_t addr)
{
  // 16 KiB ROM bank 00 from cartridge, usually a fixed bank
  // 16 KiB ROM Bank 01~NN Ffom cartridge, switchable bank via mapper (if any)
  // OR
  // Bootstrap sequence
  if (addr < 0x8000)
    return readRom(addr - 0x0000);

  // 8 KiB Video RAM (VRAM)
  // In CGB mode, switchable bank 0/1
  else if (addr < 0xA000)
    return readVRam(addr - 0x8000);

  // 8 KiB External RAM
  // From cartridge, switchable bank if any
  else if (addr < 0xC000)
    return readERam(addr - 0xA000);

  // 8 KiB Work RAM (WRAM)
  // In CGB mode, second half is switchable bank 1~7 
  else if (addr < 0xE000)
    return readWRam(addr - 0xC000);

  // Mirror of C000~DDFF (ECHO RAM)
  // Nintendo says use of this area is prohibited
  else if (addr < 0xFE00)
    return read(addr - 0x2000);

  // Sprite attribute table (OAM)	
  else if (addr < 0xFEA0)
    return readOam(addr - 0xFE00);

  // Not Usable
  // Nintendo says use of this area is prohibited
  else if (addr < 0xFF00)
    return 0x00;

  // I/O Registers
  else if (addr < 0xFF80)
    return readIo(addr - 0xFF00);

  // High RAM (HRAM)
  else if (addr < 0xFFFF)
    return readHRam(addr - 0xFF80);

  // Interrupt Enable register (IE)
  else
    return _ie;

  // Invalid address
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

std::uint8_t  GBC::GameBoyColor::readRom(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Read from boot if not disabled
  if (_io[Registers::RegisterBANK] == 0 && addr < _boot.size() && (addr <= 0x00FF || addr >= 0x0150))
    return _boot[addr];

  // Handle MBC behavior
  switch (_header.mbc) {
  case MemoryBankController::Type::None:
    // Get ROM value
    return _rom[addr % _rom.size()];

  case MemoryBankController::Type::MBC1:
  {
    std::uint8_t  bank = 0;

    // First half of ROM
    if (addr < 0x4000 && _mbc.mbc1.bank & 0b10000000)
        bank = _mbc.mbc1.bank & 0b01100000;

    // Second half of ROM
    else if (addr >= 0x4000)
      bank = (_mbc.mbc1.bank & 0b01100000) + std::clamp((_mbc.mbc1.bank & 0b00011111), 0b00000001, 0b00011111);

    // Get ROM value
    return _rom[(((std::size_t)bank * 0x4000) + (addr % 0x4000)) % _rom.size()];
  }

  case MemoryBankController::Type::MBC2:
  {
    std::uint8_t  bank = 0;

    // Get bank number for second half of ROM
    if (addr >= 0x4000)
      bank = std::clamp((_mbc.mbc2.bank & 0b00001111), 0b00000001, 0b00001111);

    // Get ROM value
    return _rom[(((std::size_t)bank * 0x4000) + (addr % 0x4000)) % _rom.size()];
  }

  case MemoryBankController::Type::MBC3:
  {
    std::uint8_t  bank = 0;

    // Get bank number for second half of ROM
    if (addr >= 0x4000)
      bank = std::clamp((_mbc.mbc3.rom & 0b01111111), 0b00000001, 0b01111111);

    // Get ROM value
    return _rom[(((std::size_t)bank * 0x4000) + (addr % 0x4000)) % _rom.size()];
  }

  case MemoryBankController::Type::MBC5:
  {
    std::uint8_t  bank = 0;

    // Get bank number for second half of ROM
    if (addr >= 0x4000)
      bank = _mbc.mbc5.rom & 0b0000000111111111;

    // Get ROM value
    return _rom[(((std::size_t)bank * 0x4000) + (addr % 0x4000)) % _rom.size()];
  }

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

std::uint8_t  GBC::GameBoyColor::readVRam(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Game Boy Color has 2 VRAM banks
  return _vRam[(std::uint16_t)(_io[Registers::RegisterVBK] & 0b00000001) * 0x2000 + addr];

  // TODO: lock when drawing ?
}

std::uint8_t  GBC::GameBoyColor::readERam(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Handle MBC behavior
  switch (_header.mbc) {
  case MemoryBankController::Type::None:
    // Get RAM value
    return _eRam[addr % _eRam.size()];

  case MemoryBankController::Type::MBC1:
  {
    // RAM not enabled
    if ((_mbc.mbc1.enable & 0x0A) != 0x0A)
      return 0xFF;

    std::uint8_t  bank = 0;

    // Get RAM bank
    if (_mbc.mbc1.bank & 0b10000000)
      bank = ((_mbc.mbc1.bank & 0b01100000) >> 5);

    // Get RAM value
    return _eRam[(bank * 0x2000 + addr) % _eRam.size()];
  }

  case MemoryBankController::Type::MBC2:
    // RAM not enabled
    if (_mbc.mbc2.enable != 0x0A)
      return 0xFF;
    // Get RAM value
    else
      return _eRam[addr % _eRam.size()];

  case MemoryBankController::Type::MBC3:
    // RAM / RTC registers not enabled
    if (_mbc.mbc3.enable != 0x0A)
      return 0xFF;
    // RAM bank
    else if (_mbc.mbc3.ram < 0x04)
      return _eRam[(_mbc.mbc3.ram * 0x2000 + addr) % _eRam.size()];
    // RTC seconds
    else if (_mbc.mbc3.ram == 0x08)
      return _mbc.mbc3.rtc / (1) % 60;
    // RTC minutes
    else if (_mbc.mbc3.ram == 0x09)
      return _mbc.mbc3.rtc / (60) % 60;
    // RTC hours
    else if (_mbc.mbc3.ram == 0x0A)
      return _mbc.mbc3.rtc / (60 * 60) % 60;
    // RTC days (lower 8 bits)
    else if (_mbc.mbc3.ram == 0x0B)
      return (_mbc.mbc3.rtc / (60 * 60 * 24)) & 0b11111111;
    // RTC days (9th bits, halt flag, day carry bit)
    else if (_mbc.mbc3.ram == 0x0C)
      return (((_mbc.mbc3.rtc / (60 * 60 * 24)) >> 9) & 0b00000001)
      | ((_mbc.mbc3.halt == 1) ? 0b01000000 : 0b00000000)
      | ((_mbc.mbc3.rtc / (60 * 60 * 24 * 512) > 0) ? 0b10000000 : 0b00000000);
    // Unknow bank
    else
      return 0xFF;

  case MemoryBankController::Type::MBC5:
    // RAM not enabled
    if (_mbc.mbc5.enable != 0x0A)
      return 0xFF;
    // Get RAM value
    else
      return _eRam[((std::size_t)(_mbc.mbc5.ram & 0b00001111) * 0x2000 + addr) % _eRam.size()];

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

std::uint8_t  GBC::GameBoyColor::readWRam(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // First half is alway bank 0
  if (addr < 0x1000)
    return _wRam[addr];

  // Second half is a WRAM bank
  else
    return _wRam[(std::clamp(_io[Registers::RegisterSVBK] & 0b00000111, 1, 7) * 0x1000) + (addr - 0x1000)];
}

std::uint8_t  GBC::GameBoyColor::readOam(std::uint16_t addr)
{
  // TODO: handle OAM lock at rendering

  // Out of bound address, should not happen
  if (addr >= 0x00A0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Get data from OAM
  return _oam[addr];
}

std::uint8_t  GBC::GameBoyColor::readIo(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= 0x0080)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  switch (addr) {
  case Registers::RegisterJOYP: // Joypad, R/W
    // Bits 7-6 are always set
    return _io[Registers::RegisterJOYP] | 0b11000000;

  case Registers::RegisterVBK:    // Video RAM Bank, R/W, CGB mode only
    return _io[Registers::RegisterVBK] | 0b11111110;

  case Registers::RegisterBCPD:   // Background Color Palette Data, R/W, reference byte in Background Color RAM at index BCPI, CGB mode only
    return _bgcRam[_io[Registers::RegisterBCPI] & 0b00111111];

  case Registers::RegisterOCPD:   // OBJ Color Palette Data, R/W, reference byte in OBJ Color RAM at index OCPI, CGB mode only
    return _obcRam[_io[Registers::RegisterOCPI] & 0b00111111];

  case Registers::RegisterSVBK:   // Work Ram Bank, R/W, CGB mode only
    return _io[Registers::RegisterSVBK] & 0b00000111;

  case Registers::RegisterDIVHi:  // High byte of DIV
  case Registers::RegisterTIMA:   // Timer Modulo, R/W
  case Registers::RegisterTMA:    // Timer Modulo, R/W
  case Registers::RegisterTAC:    // Time Control, R/W of bits 2-1-0
  case Registers::RegisterIF:     // Interrupt Flags, R/W
  case Registers::RegisterLCDC:   // LCD Control, R/W (see enum)
  case Registers::RegisterSTAT:   // LCD Status, R/W (see enum)
  case Registers::RegisterSCY:    // Scroll Y, R/W
  case Registers::RegisterSCX:    // Scroll X, R/W
  case Registers::RegisterLY:     // LCD Y Coordinate, R
  case Registers::RegisterLYC:    // LCD Y Coordinate Compare, R/W
  case Registers::RegisterDMA:    // DMA Transfer and Start Address, R/W
  case Registers::RegisterBGP:    // Background Palette Data, R/W, non CGB mode only
  case Registers::RegisterOBP0:   // OBJ 0 Palette Data, R/W, non CGB mode only
  case Registers::RegisterOBP1:   // OBJ 1 Palette Data, R/W, non CGB mode only
  case Registers::RegisterWY:     // Window Y Position, R/W
  case Registers::RegisterWX:     // Window X Position, R/W
  case Registers::RegisterKEY0:   // CPU Mode, R/W, see enum
  case Registers::RegisterKEY1:   // CPU Speed Switch, R/W, CGB mode only
  case Registers::RegisterHDMA1:  // New DMA Transfers source high byte, W, CGB mode only
  case Registers::RegisterHDMA2:  // New DMA Transfers source low byte, W, CGB mode only
  case Registers::RegisterHDMA3:  // New DMA Transfers destination high byte, W, CGB mode only
  case Registers::RegisterHDMA4:  // New DMA Transfers destination low byte, W, CGB mode only
  case Registers::RegisterHDMA5:  // Start New DMA Transfer
  case Registers::RegisterBCPI:   // Background Color Palette Index, R/W, CGB mode only
  case Registers::RegisterOCPI:   // OBJ Color Palette Index, R/W, CGB mode only
  case Registers::RegisterOPRI:   // OBJ Priority Mode, R/W, CGB mode only
  case 0x72:                      // Undocumented, R/W
  case 0x73:                      // Undocumented, R/W
  case 0x75:                      // Undocumented, R/W (bit 4-6)
    // Basic read, just return stored value
    return _io[addr];

  case Registers::RegisterDIVLo:  // Low byte of DIV, not accessible
  case Registers::RegisterBANK:   // Boot Bank Controller, W, 0 to enable Boot mapping in ROM
  default:                        // Invalid register
    // Default value in case of error
    return 0xFF;
  }
}

std::uint8_t  GBC::GameBoyColor::readHRam(std::uint16_t addr)
{
  // Out of bound address, should not happen
  if (addr >= _hRam.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Get data from HRAM
  return _hRam[addr];
}

void  GBC::GameBoyColor::write(std::uint16_t addr, std::uint8_t value)
{
  // 16 KiB ROM bank 00 from cartridge, usually a fixed bank
  // 16 KiB ROM Bank 01~NN Ffom cartridge, switchable bank via mapper (if any)
  // OR
  // Bootstrap sequence
  if (addr < 0x8000)
    writeRom(addr - 0x0000, value);

  // 8 KiB Video RAM (VRAM)
  // In CGB mode, switchable bank 0/1
  else if (addr < 0xA000)
    writeVRam(addr - 0x8000, value);

  // 8 KiB External RAM
  // From cartridge, switchable bank if any
  else if (addr < 0xC000)
    writeERam(addr - 0xA000, value);

  // 8 KiB Work RAM (WRAM)
  // In CGB mode, second half is switchable bank 1~7 
  else if (addr < 0xE000)
    writeWRam(addr - 0xC000, value);

  // Mirror of C000~DDFF (ECHO RAM)
  // Nintendo says use of this area is prohibited
  else if (addr < 0xFE00)
    write(addr - 0x2000, value);

  // Sprite attribute table (OAM)	
  else if (addr < 0xFEA0)
    writeOam(addr - 0xFE00, value);

  // Not Usable
  // Nintendo says use of this area is prohibited
  else if (addr < 0xFF00)
    return;

  // I/O Registers
  else if (addr < 0xFF80)
    writeIo(addr - 0xFF00, value);

  // High RAM (HRAM)
  else if (addr < 0xFFFF)
    writeHRam(addr - 0xFF80, value);

  // Interrupt Enable register (IE)
  else
    _ie = value;
}

void  GBC::GameBoyColor::writeRom(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x8000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Handle MBC behavior
  switch (_header.mbc) {
  case MemoryBankController::Type::None:
    // Does nothing
    break;

  case MemoryBankController::Type::MBC1:
    // RAM enable
    if (addr < 0x2000)
      _mbc.mbc1.enable = value;
    // ROM bank number
    else if (addr < 0x4000)
      _mbc.mbc1.bank = (_mbc.mbc1.bank & 0b11100000) | ((value & 0b00011111) << 0);
    // RAM/ROM upper bits 5-6
    else if (addr < 0x6000)
      _mbc.mbc1.bank = (_mbc.mbc1.bank & 0b10011111) | ((value & 0b00000011) << 5);
    // Banking mode
    else
      _mbc.mbc1.bank = (_mbc.mbc1.bank & 0b01111111) | ((value & 0b00000001) << 5);
    break;

  case MemoryBankController::Type::MBC2:
    // ROM bank number
    if (addr & 0b0000000100000000)
      _mbc.mbc2.bank = value & 0b00001111;
    // RAM enable
    else {
      _mbc.mbc2.enable = value;
      if (_mbc.mbc2.enable != 0x0A)
        ; // TODO: save external ram to file
    }
    break;

  case MemoryBankController::Type::MBC3:
    // RAM enable
    if (addr < 0x2000)
      _mbc.mbc3.enable = value;
    // ROM bank number
    else if (addr < 0x4000)
      _mbc.mbc3.rom = value & 0b01111111;
    // RAM bank number / RTC register select
    else if (addr < 0x6000)
      _mbc.mbc3.ram = value;
    // Latch Clock Data
    else {
      if (_mbc.mbc3.latch == 0x00 && value == 0x01)
        _mbc.mbc3.rtc = _mbc.mbc3.time;
      _mbc.mbc3.latch = value;
    }
    break;

  case MemoryBankController::Type::MBC5:
    // RAM enable
    if (addr < 0x2000)
      _mbc.mbc5.enable = value;
    // ROM  bank number (lower 8 bits)
    else if (addr < 0x3000)
      _mbc.mbc5.rom = (_mbc.mbc5.rom & 0b1111111100000000) | value;
    // ROM  bank number (9th bit)
    else if (addr < 0x4000)
      _mbc.mbc5.rom = (_mbc.mbc5.rom & 0b0000000011111111) | ((std::uint16_t)(value & 0b00000001) << 8);
    else if (addr < 0x6000)
      _mbc.mbc5.ram = value;
    break;

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

void  GBC::GameBoyColor::writeVRam(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Game Boy Color has 2 VRAM banks
  _vRam[(_io[Registers::RegisterVBK] & 0b00000001) * 0x2000 + addr] = value;

  // TODO: lock when drawing ?
}

void  GBC::GameBoyColor::writeERam(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Handle MBC behavior
  switch (_header.mbc) {
  case MemoryBankController::Type::None:
    // Write value to RAM
    _eRam[addr % _eRam.size()] = value;
    break;

  case MemoryBankController::Type::MBC1:
  {
    // RAM enabled
    if (_mbc.mbc1.enable == 0x0A)
      _eRam[((std::size_t)((_mbc.mbc1.bank & 0b10000000) ? ((_mbc.mbc1.bank & 0b01100000) >> 5) : 0) * 0x2000 + addr) % _eRam.size()] = value;
    break;
  }

  case MemoryBankController::Type::MBC2:
    // RAM enabled
    if (_mbc.mbc2.enable == 0x0A)
      _eRam[addr % _eRam.size()] = value;
    break;

  case MemoryBankController::Type::MBC3:
    // RAM enabled
    if (_mbc.mbc3.enable != 0x0A)
      break;
    // Write to RAM
    else if (_mbc.mbc3.ram < 0x04)
      _eRam[(_mbc.mbc3.ram * 0x2000 + addr) % _eRam.size()] = value;
    // RTC seconds
    else if (_mbc.mbc3.ram == 0x08) {
      if (_mbc.mbc3.halt == 1) {
        _mbc.mbc3.time = _mbc.mbc3.time - (_mbc.mbc3.time / (1) % 60 * (1)) + value * (1);
        _mbc.mbc3.rtc = _mbc.mbc3.time;
      }
    }
    // RTC minutes
    else if (_mbc.mbc3.ram == 0x09) {
      if (_mbc.mbc3.halt == 1) {
        _mbc.mbc3.time = _mbc.mbc3.time - (_mbc.mbc3.time / (60) % 60 * (60)) + value * (60);
        _mbc.mbc3.rtc = _mbc.mbc3.time;
      }
    }
    // RTC hours
    else if (_mbc.mbc3.ram == 0x0A) {
      if (_mbc.mbc3.halt == 1) {
        _mbc.mbc3.time = _mbc.mbc3.time - (_mbc.mbc3.time / (60 * 60) % 24 * (60 * 60)) + value * (60 * 60);
        _mbc.mbc3.rtc = _mbc.mbc3.time;
      }
    }
    // RTC days
    else if (_mbc.mbc3.ram == 0x0B) {
      if (_mbc.mbc3.halt == 1) {
        _mbc.mbc3.time = _mbc.mbc3.time - (_mbc.mbc3.time / (60 * 60 * 24) % 256 * (60 * 60 * 24)) + value * (60 * 60 * 24);
        _mbc.mbc3.rtc = _mbc.mbc3.time;
      }
    }
    // RTC days, halt flag and day carry
    else if (_mbc.mbc3.ram == 0x0C) {
      if (_mbc.mbc3.halt == 1) {
        _mbc.mbc3.time = _mbc.mbc3.time - (_mbc.mbc3.time / (60 * 60 * 24 * 256) % 512 * (60 * 60 * 24 * 256)) + (value & 0b00000001) * (60 * 60 * 24 * 256);
        _mbc.mbc3.halt = (value & 0b01000000) ? 1 : 0;
        _mbc.mbc3.time = _mbc.mbc3.time % (60 * 60 * 24 * 512) + ((value & 0b10000000) ? 1 : 0) * (60 * 60 * 24 * 512);
        _mbc.mbc3.rtc = _mbc.mbc3.time;
      }
    }
    break;

  case MemoryBankController::Type::MBC5:
    // RAM enabled
    if (_mbc.mbc5.enable == 0x0A)
      _eRam[((std::size_t)_mbc.mbc5.ram * 0x2000 + addr) % _eRam.size()] = value;
    break;

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

void  GBC::GameBoyColor::writeWRam(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x2000)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // First half is alway bank 0
  if (addr < 0x1000)
    _wRam[addr] = value;

  // Second half is a WRAM bank
  else
    _wRam[(std::clamp(_io[Registers::RegisterSVBK] & 0b00000111, 1, 7) * 0x1000) + (addr - 0x1000)] = value;
}

void  GBC::GameBoyColor::writeOam(std::uint16_t addr, std::uint8_t value)
{
  // TODO: handle OAM lock at rendering

    // Out of bound address, should not happen
  if (addr >= 0x00A0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Write data to OAM
  _oam[addr] = value;
}

void  GBC::GameBoyColor::writeIo(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= 0x0080)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  switch (addr) {
  case Registers::RegisterJOYP: // Joypad, R/W
    // Reset pressed keys and force bits 7-6 to true
    _io[Registers::RegisterJOYP] = value | 0b11001111;

    // Direction buttons
    if (!(_io[Registers::RegisterJOYP] & 0b00010000)) {
      if (_keys[Key::KeyDown] == true)
        _io[Registers::RegisterJOYP] &= 0b11110111;
      if (_keys[Key::KeyUp] == true)
        _io[Registers::RegisterJOYP] &= 0b11111011;
      if (_keys[Key::KeyLeft] == true)
        _io[Registers::RegisterJOYP] &= 0b11111101;
      if (_keys[Key::KeyRight] == true)
        _io[Registers::RegisterJOYP] &= 0b11111110;
    }

    // Action buttons
    if (!(_io[Registers::RegisterJOYP] & 0b00100000)) {
      if (_keys[Key::KeyStart] == true)
        _io[Registers::RegisterJOYP] &= 0b11110111;
      if (_keys[Key::KeySelect] == true)
        _io[Registers::RegisterJOYP] &= 0b11111011;
      if (_keys[Key::KeyB] == true)
        _io[Registers::RegisterJOYP] &= 0b11111101;
      if (_keys[Key::KeyA] == true)
        _io[Registers::RegisterJOYP] &= 0b11111110;
    }
    break;

  case Registers::RegisterDIVHi:
    // Always set to 0
    _io[Registers::RegisterDIVHi] = 0;
    break;

  case Registers::RegisterTAC:
    _io[Registers::RegisterTAC] = value & 0b00000111;

  case Registers::RegisterIF: // Interrupt Flags, R/W
    // Bits 7-6-5 are always set
    value |= 0b11100000;

    // Write value to register
    _io[Registers::RegisterIF] = value;
    break;

  case Registers::RegisterSTAT: // LCD Status, R/W (see enum)
    // Only bits 6-5-4-3 are writable 
    _io[Registers::RegisterSTAT] = (_io[Registers::RegisterSTAT] & 0b00000111) | (value & 0b01111000);
    break;

  case Registers::RegisterLYC:
    // Write value to register
    _io[Registers::RegisterLYC] = value;

    // LY / LCY register comparison
    if (_io[Registers::RegisterLY] == _io[Registers::RegisterLYC]) {
      _io[Registers::RegisterSTAT] |= LcdStatus::LcdStatusEqual;

      // STAT compare (LY/LYC) interrupt
      if (_io[Registers::RegisterSTAT] & LcdStatus::LcdStatusCompare)
        _io[Registers::RegisterIF] |= Interrupt::InterruptLcdStat;
    }
    else
      _io[Registers::RegisterSTAT] &= ~LcdStatus::LcdStatusEqual;
    break;

  case Registers::RegisterDMA:
    // Write value to register
    _io[Registers::RegisterDMA] = value;

    // Transfer data to OAM
    for (std::uint16_t index = 0; index < _oam.size(); index++)
      writeOam(index, read<std::uint8_t>(((std::uint16_t)_io[Registers::RegisterDMA] << 8) + index));

    // TODO: DMA transfer is 160 cycles long, lock memory access
    break;

  case Registers::RegisterKEY0:   // CPU Mode, R/W, see enum
    // Active only during boot mapping
    if (_io[Registers::RegisterBANK] == 0)
      _io[Registers::RegisterKEY0] = value;
    break;

  case Registers::RegisterBANK:   // Boot Bank Controller, W, 0 to enable Boot mapping in ROM
    // Active only during boot mapping
    if (_io[Registers::RegisterBANK] == 0)
      _io[Registers::RegisterBANK] = value;
    break;

  case Registers::RegisterHDMA5:  // Start New DMA Transfer, R/W, CGB mode only
  {
    std::uint16_t source = (((std::uint16_t)_io[Registers::RegisterHDMA1] << 8) + (std::uint16_t)_io[Registers::RegisterHDMA2]) & 0b1111111111110000;
    std::uint16_t destination = ((((std::uint16_t)_io[Registers::RegisterHDMA3] << 8) + (std::uint16_t)_io[Registers::RegisterHDMA4]) & 0b00011111111110000) | 0b1000000000000000;
    std::uint16_t length = ((std::uint16_t)(value & 0b01111111) + 1) * 0x10;
    
    printf("HDMA: mode %d, source %04x, dest %04x, length %d\n", (value & 0b10000000) ? 1 : 0, source, destination, length);

    // Transfer data to VRAM
    for (unsigned int count = 0; count < length; count++) {
      std::uint8_t  data;

      // Get data from source
      if (source >= 0x0000 && source < 0x8000)
        data = readRom(source - 0x0000);
      else if (source >= 0xA000 && source < 0xC000)
        data = readERam(source - 0xA000);
      else if (source >= 0xC000 && source < 0xE000)
        data = readWRam(source - 0xC000);
      else
        break;

      // Write data to destination
      if (destination >= 0x8000 && destination < 0xA000)
        writeVRam(destination - 0x8000, data);
      else
        break;

      // Increment addresses
      source += 1;
      destination += 1;
    }

    // Set register to completed transfer value
    _io[Registers::RegisterHDMA5] = 0xFF;

    // Write end addresses to HDMA registers
    _io[Registers::RegisterHDMA1] = (source >> 8) & 0b11111111;
    _io[Registers::RegisterHDMA2] = (source >> 0) & 0b11111111;
    _io[Registers::RegisterHDMA3] = (destination >> 8) & 0b11111111;
    _io[Registers::RegisterHDMA4] = (destination >> 0) & 0b11111111;

    _cpuCycle += length * ((_io[Registers::RegisterKEY1] & 0b10000000) ? 4 : 2);

    // TODO: respect HDMA transfer timing for mode 0 & 1
    break;
  }

  case Registers::RegisterBCPD:   // Background Color Palette Data, R/W, reference byte in BG Color RAM at index BCPI, CGB mode only
    // Store data in Background Color RAM
    _bgcRam[_io[Registers::RegisterBCPI] & 0b00111111] = value;

    // Increment pointer
    if (_io[Registers::RegisterBCPI] & 0b10000000)
      _io[Registers::RegisterBCPI] = 0b10000000 | (((_io[Registers::RegisterBCPI] & 0b00111111) + 1) & 0b00111111);
    break;

  case Registers::RegisterOCPD:   // OBJ Color Palette Data, R/W, reference byte in OBJ Color RAM at index OCPI, CGB mode only
    // Store data in OBJ Color RAM
    _obcRam[_io[Registers::RegisterOCPI] & 0b00111111] = value;

    // Increment pointer
    if (_io[Registers::RegisterOCPI] & 0b10000000)
      _io[Registers::RegisterOCPI] = 0b10000000 | (((_io[Registers::RegisterOCPI] & 0b00111111) + 1) & 0b00111111);
    break;

  case Registers::RegisterOPRI:   // OBJ Priority Mode, R/W, CGB mode only
    _io[Registers::RegisterOPRI] = value & 0b00000001;
    break;

  case Registers::RegisterSVBK:   // Work Ram Bank, R/W, CGB mode only
    if ((_io[Registers::RegisterKEY0] & 0b00001100) != CpuMode::CpuModeDmg)
      _io[Registers::RegisterSVBK] = value & 0b00000111;
    break;

  case Registers::RegisterTIMA:   // Timer Modulo, R/W
  case Registers::RegisterTMA:    // Timer Modulo, R/W
  case Registers::RegisterLCDC:   // LCD Control, R/W (see enum)
  case Registers::RegisterSCY:    // Scroll Y, R/W
  case Registers::RegisterSCX:    // Scroll X, R/W
  case Registers::RegisterBGP:    // Background Palette Data, R/W, non CGB mode only
  case Registers::RegisterOBP0:   // OBJ 0 Palette Data, R/W, non CGB mode only
  case Registers::RegisterOBP1:   // OBJ 1 Palette Data, R/W, non CGB mode only
  case Registers::RegisterWY:     // Window Y Position, R/W
  case Registers::RegisterWX:     // Window X Position, R/W
  case Registers::RegisterKEY1:   // CPU Speed Switch, R/W, CGB mode only
  case Registers::RegisterVBK:    // Video RAM Bank, R/W, CGB mode only
  case Registers::RegisterHDMA1:  // New DMA Transfers source high byte, W, CGB mode only
  case Registers::RegisterHDMA2:  // New DMA Transfers source low byte, W, CGB mode only
  case Registers::RegisterHDMA3:  // New DMA Transfers destination high byte, W, CGB mode only
  case Registers::RegisterHDMA4:  // New DMA Transfers destination low byte, W, CGB mode only
  case Registers::RegisterBCPI:   // Background Color Palette Index, R/W, CGB mode only
  case Registers::RegisterOCPI:   // OBJ Color Palette Index, R/W, CGB mode only
  case 0x72:                      // Undocumented, R/W
  case 0x73:                      // Undocumented, R/W
    _io[addr] = value;
    break;

  case 0x75: // Undocumented, R/W (bit 4-6)
    _io[addr] = value & 0b01110000;
    break;

  case Registers::RegisterDIVLo:  // Low byte of DIV, not accessible
  case Registers::RegisterLY:     // LCD Y Coordinate, R
  default:
    break;
  }
}

void  GBC::GameBoyColor::writeHRam(std::uint16_t addr, std::uint8_t value)
{
  // Out of bound address, should not happen
  if (addr >= _hRam.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Write data to HRAM
  _hRam[addr] = value;
}