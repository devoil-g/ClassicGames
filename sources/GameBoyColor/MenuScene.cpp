#include "GameBoyColor/MenuScene.hpp"
#include "System/Window.hpp"

const std::unordered_map<char, GBC::MenuScene::Character>  GBC::MenuScene::Characters = {
  { 'A', {.width = 7, .bitmap = {
      0b00000000,
      0b00001000,
      0b00001000,
      0b00010100,
      0b00010100,
      0b00111110,
      0b00100010,
      0b01110111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'a', {.width = 4, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000110,
      0b00001001,
      0b00000111,
      0b00001001,
      0b00000111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'B', {.width = 5, .bitmap = {
      0b00000000,
      0b00011110,
      0b00001001,
      0b00001001,
      0b00001110,
      0b00001001,
      0b00001001,
      0b00011110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'b', {.width = 5, .bitmap = {
      0b00000000,
      0b00001000,
      0b00011000,
      0b00001110,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00010110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'C', {.width = 5, .bitmap = {
      0b00000000,
      0b00000111,
      0b00001001,
      0b00010000,
      0b00010000,
      0b00010000,
      0b00001001,
      0b00000110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'c', {.width = 4, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000111,
      0b00001001,
      0b00001000,
      0b00001001,
      0b00000110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'D', {.width = 5, .bitmap = {
      0b00000000,
      0b00011110,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00011110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'd', {.width = 5, .bitmap = {
      0b00000000,
      0b00000110,
      0b00000010,
      0b00001110,
      0b00010010,
      0b00010010,
      0b00010011,
      0b00001110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'E', {.width = 5, .bitmap = {
      0b00000000,
      0b00011111,
      0b00001001,
      0b00001000,
      0b00001110,
      0b00001000,
      0b00001001,
      0b00011111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'e', {.width = 4, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000110,
      0b00001001,
      0b00001111,
      0b00001000,
      0b00000110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'F', {.width = 5, .bitmap = {
      0b00000000,
      0b00011111,
      0b00001001,
      0b00001000,
      0b00001110,
      0b00001000,
      0b00001000,
      0b00011100,
      0b00000000,
      0b00000000,
    }}
  },
  { 'f', {.width = 3, .bitmap = {
      0b00000000,
      0b00000001,
      0b00000010,
      0b00000111,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'G', {.width = 5, .bitmap = {
      0b00000000,
      0b00001111,
      0b00010001,
      0b00010000,
      0b00010011,
      0b00010001,
      0b00010001,
      0b00001110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'g', {.width = 5, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000010,
      0b00001100,
      0b00010010,
      0b00010010,
      0b00001100,
      0b00011110,
      0b00010001,
      0b00001110,
    }}
  },
  { 'H', {.width = 6, .bitmap = {
      0b00000000,
      0b00111011,
      0b00010010,
      0b00010010,
      0b00011110,
      0b00010010,
      0b00010010,
      0b00111011,
      0b00000000,
      0b00000000,
    }}
  },
  { 'h', {.width = 6, .bitmap = {
      0b00000000,
      0b00010000,
      0b00110000,
      0b00011100,
      0b00010010,
      0b00010010,
      0b00010010,
      0b00111011,
      0b00000000,
      0b00000000,
    }}
  },
  { 'I', {.width = 3, .bitmap = {
      0b00000000,
      0b00000111,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'i', {.width = 3, .bitmap = {
      0b00000000,
      0b00000010,
      0b00000000,
      0b00000010,
      0b00000110,
      0b00000010,
      0b00000010,
      0b00000111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'J', {.width = 4, .bitmap = {
      0b00000000,
      0b00000111,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00001100,
    }}
  },
  { 'j', {.width = 2, .bitmap = {
      0b00000000,
      0b00000001,
      0b00000000,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000010,
    }}
  },
  { 'K', {.width = 6, .bitmap = {
      0b00000000,
      0b00111011,
      0b00010010,
      0b00010100,
      0b00011000,
      0b00010100,
      0b00010010,
      0b00111011,
      0b00000000,
      0b00000000,
    }}
  },
  { 'k', {.width = 5, .bitmap = {
      0b00000000,
      0b00001000,
      0b00011000,
      0b00001011,
      0b00001010,
      0b00001100,
      0b00001010,
      0b00011011,
      0b00000000,
      0b00000000,
    }}
  },
  { 'L', {.width = 5, .bitmap = {
      0b00000000,
      0b00011100,
      0b00001000,
      0b00001000,
      0b00001000,
      0b00001000,
      0b00001001,
      0b00011111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'l', {.width = 3, .bitmap = {
      0b00000000,
      0b00000010,
      0b00000110,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'M', {.width = 7, .bitmap = {
      0b00000000,
      0b01100011,
      0b00110110,
      0b00101010,
      0b00100010,
      0b00100010,
      0b00100010,
      0b01110111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'm', {.width = 8, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b10110100,
      0b11011010,
      0b10010010,
      0b10010010,
      0b11011011,
      0b00000000,
      0b00000000,
    }}
  },
  { 'N', {.width = 7, .bitmap = {
      0b00000000,
      0b01100111,
      0b00110010,
      0b00110010,
      0b00101010,
      0b00101010,
      0b00100110,
      0b01110110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'n', {.width = 6, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00010100,
      0b00111010,
      0b00010010,
      0b00010010,
      0b00111011,
      0b00000000,
      0b00000000,
    }}
  },
  { 'O', {.width = 5, .bitmap = {
      0b00000000,
      0b00001110,
      0b00010001,
      0b00010001,
      0b00010001,
      0b00010001,
      0b00010001,
      0b00001110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'o', {.width = 4, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000110,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00000110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'P', {.width = 5, .bitmap = {
      0b00000000,
      0b00011110,
      0b00001001,
      0b00001001,
      0b00001110,
      0b00001000,
      0b00001000,
      0b00011100,
      0b00000000,
      0b00000000,
    }}
  },
  { 'p', {.width = 5, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00001110,
      0b00011001,
      0b00001001,
      0b00001001,
      0b00001110,
      0b00001000,
      0b00011100,
    }}
  },
  { 'Q', {.width = 5, .bitmap = {
      0b00000000,
      0b00001110,
      0b00010001,
      0b00010001,
      0b00010001,
      0b00010001,
      0b00010001,
      0b00001110,
      0b00000100,
      0b00000010,
    }}
  },
  { 'q', {.width = 5, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00001110,
      0b00010011,
      0b00010010,
      0b00010010,
      0b00001110,
      0b00000010,
      0b00000111,
    }}
  },
  { 'R', {.width = 6, .bitmap = {
      0b00000000,
      0b00111100,
      0b00010010,
      0b00010010,
      0b00011100,
      0b00010010,
      0b00010010,
      0b00111011,
      0b00000000,
      0b00000000,
    }}
  },
  { 'r', {.width = 4, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00001011,
      0b00000100,
      0b00000100,
      0b00000100,
      0b00001110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'S', {.width = 4, .bitmap = {
      0b00000000,
      0b00000111,
      0b00001001,
      0b00001000,
      0b00000110,
      0b00000001,
      0b00001001,
      0b00001110,
      0b00000000,
      0b00000000,
    }}
  },
  { 's', {.width = 4, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000111,
      0b00001001,
      0b00000110,
      0b00001001,
      0b00001110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'T', {.width = 5, .bitmap = {
      0b00000000,
      0b00011111,
      0b00010101,
      0b00000100,
      0b00000100,
      0b00000100,
      0b00000100,
      0b00001110,
      0b00000000,
      0b00000000,
    }}
  },
  { 't', {.width = 3, .bitmap = {
      0b00000000,
      0b00000010,
      0b00000010,
      0b00000111,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000001,
      0b00000000,
      0b00000000,
    }}
  },
  { 'U', {.width = 7, .bitmap = {
      0b00000000,
      0b01110111,
      0b00100010,
      0b00100010,
      0b00100010,
      0b00100010,
      0b00100010,
      0b00011100,
      0b00000000,
      0b00000000,
    }}
  },
  { 'u', {.width = 5, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00011011,
      0b00010010,
      0b00010010,
      0b00010010,
      0b00001101,
      0b00000000,
      0b00000000,
    }}
  },
  { 'V', {.width = 7, .bitmap = {
      0b00000000,
      0b01110111,
      0b00100010,
      0b00100010,
      0b00010100,
      0b00010100,
      0b00001000,
      0b00001000,
      0b00000000,
      0b00000000,
    }}
  },
  { 'v', {.width = 5, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00011011,
      0b00001010,
      0b00001010,
      0b00000100,
      0b00000100,
      0b00000000,
      0b00000000,
    }}
  },
  { 'W', {.width = 9, .bitmap = {
      0b00000000,
      0b111000111,
      0b010000010,
      0b010010010,
      0b001010100,
      0b001010100,
      0b000101000,
      0b000101000,
      0b000000000,
      0b000000000,
    }}
  },
  { 'w', {.width = 7, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b01101011,
      0b00101010,
      0b00101010,
      0b00010100,
      0b00010100,
      0b00000000,
      0b00000000,
    }}
  },
  { 'X', {.width = 5, .bitmap = {
      0b00000000,
      0b00011011,
      0b00001010,
      0b00001010,
      0b00000100,
      0b00001010,
      0b00001010,
      0b00011011,
      0b00000000,
      0b00000000,
    }}
  },
  { 'x', {.width = 5, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00011011,
      0b00001010,
      0b00000100,
      0b00001010,
      0b00011011,
      0b00000000,
      0b00000000,
    }}
  },
  { 'Y', {.width = 5, .bitmap = {
      0b00000000,
      0b00011011,
      0b00001010,
      0b00001010,
      0b00000100,
      0b00000100,
      0b00000100,
      0b00001110,
      0b00000000,
      0b00000000,
    }}
  },
  { 'y', {.width = 5, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00011011,
      0b00001010,
      0b00001010,
      0b00000100,
      0b00000100,
      0b00001000,
      0b00001000,
    }}
  },
  { 'Z', {.width = 4, .bitmap = {
      0b00000000,
      0b00001111,
      0b00001001,
      0b00000010,
      0b00000100,
      0b00001000,
      0b00001001,
      0b00001111,
      0b00000000,
      0b00000000,
    }}
  },
  { 'z', {.width = 4, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00001111,
      0b00001010,
      0b00000100,
      0b00001001,
      0b00001111,
      0b00000000,
      0b00000000,
    }}
  },
  { '0', {.width = 4, .bitmap = {
      0b00000000,
      0b00000110,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00000110,
      0b00000000,
      0b00000000,
    }}
  },
  { '1', {.width = 3, .bitmap = {
      0b00000000,
      0b00000010,
      0b00000110,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000111,
      0b00000000,
      0b00000000,
    }}
  },
  { '2', {.width = 4, .bitmap = {
      0b00000000,
      0b00000110,
      0b00001001,
      0b00000001,
      0b00000010,
      0b00000100,
      0b00001001,
      0b00001111,
      0b00000000,
      0b00000000,
    }}
  },
  { '3', {.width = 4, .bitmap = {
      0b00000000,
      0b00000110,
      0b00001001,
      0b00000010,
      0b00000110,
      0b00000001,
      0b00000001,
      0b00001001,
      0b00000110,
      0b00000000,
    }}
  },
  { '4', {.width = 5, .bitmap = {
      0b00000000,
      0b00000010,
      0b00000110,
      0b00001010,
      0b00010010,
      0b00011111,
      0b00000010,
      0b00000111,
      0b00000000,
      0b00000000,
    }}
  },
  { '5', {.width = 4, .bitmap = {
      0b00000000,
      0b00001111,
      0b00001000,
      0b00001110,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000010,
      0b00001100,
      0b00000000,
    }}
  },
  { '6', {.width = 4, .bitmap = {
      0b00000010,
      0b00000100,
      0b00001000,
      0b00001110,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00000110,
      0b00000000,
      0b00000000,
    }}
  },
  { '7', {.width = 4, .bitmap = {
      0b00000000,
      0b00001111,
      0b00001001,
      0b00000001,
      0b00000010,
      0b00000100,
      0b00000100,
      0b00000100,
      0b00000000,
      0b00000000,
    }}
  },
  { '8', {.width = 4, .bitmap = {
      0b00000000,
      0b00000110,
      0b00001001,
      0b00001001,
      0b00000110,
      0b00001001,
      0b00001001,
      0b00000110,
      0b00000000,
      0b00000000,
    }}
  },
  { '9', {.width = 4, .bitmap = {
      0b00000000,
      0b00000110,
      0b00001001,
      0b00001001,
      0b00001001,
      0b00000111,
      0b00000001,
      0b00000010,
      0b00000100,
      0b00000000,
    }}
  },
  { ' ', {.width = 2, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  },
  { '.', {.width = 1, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000001,
      0b00000000,
      0b00000000,
    }}
  },
  { ',', {.width = 2, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000001,
      0b00000010,
      0b00000000,
    }}
  },
  { ';', {.width = 2, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000001,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000001,
      0b00000010,
      0b00000000,
    }}
  },
  { ':', {.width = 1, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000001,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000001,
      0b00000000,
      0b00000000,
    }}
  },
  { '?', {.width = 4, .bitmap = {
      0b00000000,
      0b00000110,
      0b00001001,
      0b00001001,
      0b00000010,
      0b00000010,
      0b00000000,
      0b00000010,
      0b00000000,
      0b00000000,
    }}
  },
  { '!', {.width = 4, .bitmap = {
      0b00000000,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000000,
      0b00000001,
      0b00000000,
      0b00000000,
    }}
  },
  { '+', {.width = 3, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000010,
      0b00000010,
      0b00000111,
      0b00000010,
      0b00000010,
      0b00000000,
      0b00000000,
    }}
  },
  { '-', {.width = 3, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000111,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  },
  { '/', {.width = 3, .bitmap = {
      0b00000000,
      0b00000001,
      0b00000001,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000100,
      0b00000100,
      0b00000000,
      0b00000000,
    }}
  },
  { '\\', {.width = 3, .bitmap = {
      0b00000000,
      0b00000100,
      0b00000100,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000001,
      0b00000001,
      0b00000000,
      0b00000000,
    }}
  },
  { '*', {.width = 3, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000101,
      0b00000010,
      0b00000101,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  },
  { '"', {.width = 3, .bitmap = {
      0b00000000,
      0b00000101,
      0b00000101,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  },
  { '\'', {.width = 1, .bitmap = {
      0b00000000,
      0b00000001,
      0b00000001,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  },
  { '#', {.width = 5, .bitmap = {
      0b00000000,
      0b00001010,
      0b00011111,
      0b00001010,
      0b00001010,
      0b00001010,
      0b00011111,
      0b00001010,
      0b00000000,
      0b00000000,
    }}
  },
  { '$', {.width = 5, .bitmap = {
      0b00000100,
      0b00001111,
      0b00010100,
      0b00010100,
      0b00001110,
      0b00000101,
      0b00000101,
      0b00011110,
      0b00000100,
      0b00000000,
    }}
  },
  { '%', {.width = 3, .bitmap = {
      0b00000000,
      0b00000101,
      0b00000001,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000100,
      0b00000101,
      0b00000000,
      0b00000000,
    }}
  },
  { '&', {.width = 5, .bitmap = {
      0b00000000,
      0b00001100,
      0b00010010,
      0b00010010,
      0b00001100,
      0b00010011,
      0b00010010,
      0b00001101,
      0b00000000,
      0b00000000,
    }}
  },
  { '(', {.width = 2, .bitmap = {
      0b00000001,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000001,
      0b00000000,
    }}
  },
  { ')', {.width = 2, .bitmap = {
      0b00000010,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000010,
      0b00000000,
    }}
  },
  { '<', {.width = 3, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000001,
      0b00000010,
      0b00000100,
      0b00000010,
      0b00000001,
      0b00000000,
      0b00000000,
    }}
  },
  { '>', {.width = 3, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000100,
      0b00000010,
      0b00000001,
      0b00000010,
      0b00000100,
      0b00000000,
      0b00000000,
    }}
  },
  { '=', {.width = 3, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000111,
      0b00000000,
      0b00000111,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  },
  { '@', {.width = 5, .bitmap = {
      0b00000000,
      0b00001110,
      0b00010001,
      0b00010111,
      0b00010101,
      0b00010011,
      0b00010000,
      0b00001110,
      0b00000000,
      0b00000000,
    }}
  },
  { '[', {.width = 2, .bitmap = {
      0b00000000,
      0b00000011,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000011,
      0b00000000,
      0b00000000,
    }}
  },
  { ']', {.width = 2, .bitmap = {
      0b00000000,
      0b00000011,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000001,
      0b00000011,
      0b00000000,
      0b00000000,
    }}
  },
  { '^', {.width = 3, .bitmap = {
      0b00000000,
      0b00000010,
      0b00000101,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  },
  { '_', {.width = 3, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000111,
      0b00000000,
    }}
  },
  { '`', {.width = 2, .bitmap = {
      0b00000000,
      0b00000010,
      0b00000001,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  },
  { '{', {.width = 3, .bitmap = {
      0b00000001,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000100,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000001,
      0b00000000,
    }}
  },
  { '}', {.width = 3, .bitmap = {
      0b00000100,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000001,
      0b00000010,
      0b00000010,
      0b00000010,
      0b00000100,
      0b00000000,
    }}
  },
  { '~', {.width = 4, .bitmap = {
      0b00000000,
      0b00000101,
      0b00001010,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  },
  { '\0', {.width = 0, .bitmap = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
    }}
  }

};

const std::unordered_map<Game::Window::Key, std::string>  GBC::MenuScene::Keys = {
  { Game::Window::Key::A, "A" },
  { Game::Window::Key::B, "B" },
  { Game::Window::Key::C, "C" },
  { Game::Window::Key::D, "D" },
  { Game::Window::Key::E, "E" },
  { Game::Window::Key::F, "F" },
  { Game::Window::Key::G, "G" },
  { Game::Window::Key::H, "H" },
  { Game::Window::Key::I, "I" },
  { Game::Window::Key::J, "J" },
  { Game::Window::Key::K, "K" },
  { Game::Window::Key::L, "L" },
  { Game::Window::Key::M, "M" },
  { Game::Window::Key::N, "N" },
  { Game::Window::Key::O, "O" },
  { Game::Window::Key::P, "P" },
  { Game::Window::Key::Q, "Q" },
  { Game::Window::Key::R, "R" },
  { Game::Window::Key::S, "S" },
  { Game::Window::Key::T, "T" },
  { Game::Window::Key::U, "U" },
  { Game::Window::Key::V, "V" },
  { Game::Window::Key::W, "W" },
  { Game::Window::Key::X, "X" },
  { Game::Window::Key::Y, "Y" },
  { Game::Window::Key::Z, "Z" },

  { Game::Window::Key::Num0, "Num0" },
  { Game::Window::Key::Num1, "Num1" },
  { Game::Window::Key::Num2, "Num2" },
  { Game::Window::Key::Num3, "Num3" },
  { Game::Window::Key::Num4, "Num4" },
  { Game::Window::Key::Num5, "Num5" },
  { Game::Window::Key::Num6, "Num6" },
  { Game::Window::Key::Num7, "Num7" },
  { Game::Window::Key::Num8, "Num8" },
  { Game::Window::Key::Num9, "Num9" },

  { Game::Window::Key::LControl, "LCtrl" },
  { Game::Window::Key::LShift, "LShift" },
  { Game::Window::Key::LAlt, "LAlt" },
  { Game::Window::Key::RControl, "RCtrl" },
  { Game::Window::Key::RShift, "RShift" },
  { Game::Window::Key::RAlt, "RAlt" },

  { Game::Window::Key::LBracket, "[" },
  { Game::Window::Key::RBracket, "]" },
  { Game::Window::Key::Semicolon, ";" },
  { Game::Window::Key::Comma, "," },
  { Game::Window::Key::Period, "." },
  { Game::Window::Key::Apostrophe, "'" },
  { Game::Window::Key::Slash, "/" },
  { Game::Window::Key::Backslash, "\\" },
  { Game::Window::Key::Grave, "`" },
  { Game::Window::Key::Equal, "=" },
  { Game::Window::Key::Hyphen, "-" },
  { Game::Window::Key::Space, "Space" },
  { Game::Window::Key::Enter, "Enter" },
  { Game::Window::Key::Backspace, "Backspace" },
  { Game::Window::Key::Tab, "Tab" },
  { Game::Window::Key::PageUp, "PageUp" },
  { Game::Window::Key::PageDown, "PageDown" },
  { Game::Window::Key::End, "End" },
  { Game::Window::Key::Home, "Home" },
  { Game::Window::Key::Insert, "Insert" },
  { Game::Window::Key::Delete, "Delete" },
  { Game::Window::Key::Add, "+" },
  { Game::Window::Key::Subtract, "Num-" },
  { Game::Window::Key::Multiply, "*" },
  { Game::Window::Key::Divide, "Num/" },

  { Game::Window::Key::Left, "Left" },
  { Game::Window::Key::Right, "Right" },
  { Game::Window::Key::Up, "Up" },
  { Game::Window::Key::Down, "Down" },

  { Game::Window::Key::Numpad0, "Num0" },
  { Game::Window::Key::Numpad1, "Num1" },
  { Game::Window::Key::Numpad2, "Num2" },
  { Game::Window::Key::Numpad3, "Num3" },
  { Game::Window::Key::Numpad4, "Num4" },
  { Game::Window::Key::Numpad5, "Num5" },
  { Game::Window::Key::Numpad6, "Num6" },
  { Game::Window::Key::Numpad7, "Num7" },
  { Game::Window::Key::Numpad8, "Num8" },
  { Game::Window::Key::Numpad9, "Num9" }
};

GBC::MenuScene::MenuScene(Game::SceneMachine& machine, GBC::GameBoyColor& gbc) :
  Game::AbstractScene(machine),
  _image({ GBC::PixelProcessingUnit::ScreenWidth, GBC::PixelProcessingUnit::ScreenHeight }, sf::Color(0, 0, 0, 127)),
  _texture(_image),
  _gbc(gbc),
  _items({
    Item{.x = 16, .y = 8, .text = "Save state" },
    Item{.x = (int)_image.getSize().x - 16 - (int)width("Load state"), .y = 6, .text = "Load state" },
    Item{.x = 16, .y = 24, .text = "Up" },
    Item{.x = 16, .y = 36, .text = "Down" },
    Item{.x = 16, .y = 48, .text = "Left" },
    Item{.x = 16, .y = 60, .text = "Right" },
    Item{.x = 16, .y = 78, .text = "A" },
    Item{.x = 16, .y = 90, .text = "B" },
    Item{.x = 16, .y = 102, .text = "Start" },
    Item{.x = 16, .y = 114, .text = "Select" },
    Item{.x = ((int)_image.getSize().x - (int)width("Exit")) / 2, .y = (int)_image.getSize().y - 16, .text = "Exit" }
    }),
  _index(0),
  _press(false)
{
  // Assign keys
  _index = 2; bind(_gbc.bind(GBC::GameBoyColor::Key::KeyUp));
  _index = 3; bind(_gbc.bind(GBC::GameBoyColor::Key::KeyDown));
  _index = 4; bind(_gbc.bind(GBC::GameBoyColor::Key::KeyLeft));
  _index = 5; bind(_gbc.bind(GBC::GameBoyColor::Key::KeyRight));
  _index = 6; bind(_gbc.bind(GBC::GameBoyColor::Key::KeyA));
  _index = 7; bind(_gbc.bind(GBC::GameBoyColor::Key::KeyB));
  _index = 8; bind(_gbc.bind(GBC::GameBoyColor::Key::KeyStart));
  _index = 9; bind(_gbc.bind(GBC::GameBoyColor::Key::KeySelect));

  // Reset index
  _index = 0;

  // Render the menu
  renderMenu();
}

bool  GBC::MenuScene::update(float elapsed)
{
  auto& window = Game::Window::Instance();

  // Return to GameBoy
  if (window.keyboard().keyPressed(Game::Window::Key::Escape) == true) {
    _machine.pop();
    return false;
  }

  // Move in menu
  if (_press == false) {
    if (window.keyboard().keyPressed(Game::Window::Key::Down) == true)
      move((_index == 0) ? +2 : +1);
    else if (window.keyboard().keyPressed(Game::Window::Key::Right) == true && _index == 0)
      move(+1);
    else if (window.keyboard().keyPressed(Game::Window::Key::Up) == true)
      move((_index == 2) ? -2 : -1);
    else if (window.keyboard().keyPressed(Game::Window::Key::Left) == true && _index == 1)
      move(-1);
    else if (window.keyboard().keyPressed(Game::Window::Key::Enter) == true ||
      window.keyboard().keyPressed(Game::Window::Key::Space) == true)
      select();
  }

  // Press key to bind
  else {
    for (auto [key, _] : Keys) {
      if (window.keyboard().keyPressed(key) == true) {
        bind(key);
        break;
      }
    }
  }

  return false;
}

void  GBC::MenuScene::draw()
{
  auto& window = Game::Window::Instance();

  // Draw GBC rendering target
  window.draw(_gbc.lcd());

  // Render menu texture
  window.draw(_texture);
}

void  GBC::MenuScene::move(int offset)
{
  // Clamp new index
  _index = (((int)_index + offset) % (int)_items.size() + (int)_items.size()) % (int)_items.size();

  // Render menu
  renderMenu();
}

void  GBC::MenuScene::select()
{
  switch (_index) {
  case 0: // Quick save
    _gbc.save(0);
    _machine.pop();
    break;
  case 1: // Quick load
    _gbc.load(0);
    _machine.pop();
    break;
  case 2: // Bind key Up
  case 3: // Bind key Down
  case 4: // Bind key Left
  case 5: // Bind key Right
  case 6: // Bind key A
  case 7: // Bind key B
  case 8: // Bind key Start
  case 9: // Bind key Select
    _press = true;
    renderPress();
    break;
  case 10:  // Exit
  {
    auto& machine = _machine;
    machine.pop();
    machine.pop();
  }
  break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

void  GBC::MenuScene::bind(Game::Window::Key key)
{
  std::string name;
  std::string keyname = Keys.contains(key) == true ? Keys.at(key) : "???";
  
  switch (_index) {
  case 2: // Bind key Up
    name = "Up";
    _gbc.bind(GBC::GameBoyColor::Key::KeyUp, key);
    break;
  case 3: // Bind key Down
    name = "Down";
    _gbc.bind(GBC::GameBoyColor::Key::KeyDown, key);
    break;
  case 4: // Bind key Left
    name = "Left";
    _gbc.bind(GBC::GameBoyColor::Key::KeyLeft, key);
    break;
  case 5: // Bind key Right
    name = "Right";
    _gbc.bind(GBC::GameBoyColor::Key::KeyRight, key);
    break;
  case 6: // Bind key A
    name = "A";
    _gbc.bind(GBC::GameBoyColor::Key::KeyA, key);
    break;
  case 7: // Bind key B
    name = "B";
    _gbc.bind(GBC::GameBoyColor::Key::KeyB, key);
    break;
  case 8: // Bind key Start
    name = "Start";
    _gbc.bind(GBC::GameBoyColor::Key::KeyStart, key);
    break;
  case 9: // Bind key Select
    name = "Select";
    _gbc.bind(GBC::GameBoyColor::Key::KeySelect, key);
    break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
  
  int length = _image.getSize().x - (16 * 2) - width(name + " ") - width(" " + keyname) - 1;

  // Rewrite menu item
  _items[_index].text = name + " ";
  for (; length >= width('.') + 1; length -= width('.') + 1)
    _items[_index].text += '.';
  for (; length >= width('\0') + 1; length -= width('\0') + 1)
    _items[_index].text += '\0';
  _items[_index].text += " " + keyname;

  // Render menu
  _press = false;
  renderMenu();
}

void  GBC::MenuScene::renderMenu()
{
  // Reset image
  _image = sf::Image(_image.getSize(), sf::Color(0, 0, 0, 127));

  // Draw each items
  for (auto index = 0; index < _items.size(); index++) {
    const auto& item = _items[index];

    // Selected item, add brackets
    if (index == _index) {
      Item cpy;

      cpy.text = "[ " + item.text + " ]";
      cpy.x = item.x - width("[ ") - 1;
      cpy.y = item.y;

      render(cpy);
    }

    // Simple render
    else
      render(item);
  }

  // Update texture
  _texture.update(_image);
}

void  GBC::MenuScene::renderPress()
{
  // Reset image
  _image = sf::Image(_image.getSize(), sf::Color(0, 0, 0, 127));
  
  Item  item = {
    .x = (int)(_image.getSize().x - width(">>> Press key <<<")) / 2,
    .y = (int)(_image.getSize().y - 10) / 2,
    .text = ">>> Press key <<<"
  };

  // Render text
  render(item);

  // Update texture
  _texture.update(_image);
}

void  GBC::MenuScene::render(const GBC::MenuScene::Item& item)
{
  // Render shadow
  render(item.x + 1, item.y + 1, item.text, sf::Color(0, 0, 0, 192));

  // Render text
  render(item.x, item.y, item.text, sf::Color::White);
}

void  GBC::MenuScene::render(int x, int y, const std::string& string, sf::Color color)
{
  for (auto c : string) {
    auto& character = Characters.contains(c) == true ? Characters.at(c) : Characters.at('?');

    // Render character
    render(x, y, character, color);

    // Move cursor
    x += character.width + 1;
  }
}

void  GBC::MenuScene::render(int x, int y, char c, sf::Color color)
{
  // Render character bitmap
  render(x, y, Characters.contains(c) == true ? Characters.at(c) : Characters.at('?'), color);
}

void  GBC::MenuScene::render(int x, int y, Character character, sf::Color color)
{
  // Draw character
  for (auto h = std::max(0, -y); h < character.bitmap.size() && h + y < _image.getSize().y; h++) {
    for (auto w = std::max(0, -x); w < character.width && w + x < _image.getSize().x; w++)
    {
      // Render only active pixels
      if (character.bitmap[h] & (1 << (character.width - 1 - w)))
        _image.setPixel({ (unsigned int)(w + x), (unsigned int)(h + y) }, color);
    }
  }
}

unsigned int GBC::MenuScene::width(const std::string& string) const
{
  unsigned int total = 0;

  // Sum every character width
  for (auto c : string)
    total += width(c) + 1;

  // Remove last spacing
  return total > 0 ? total - 1 : total;
}

unsigned int GBC::MenuScene::width(char c) const
{
  auto it = Characters.find(c);

  // Character not supported
  if (it == Characters.end())
    return 0;

  // Get character width
  else
    return it->second.width;
}