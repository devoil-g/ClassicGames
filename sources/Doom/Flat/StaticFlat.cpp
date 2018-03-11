#include "Doom/Flat/StaticFlat.hpp"	

Game::StaticFlat::StaticFlat(Game::Wad::RawResources::Flat const & flat) :
  AbstractFlat(),
  _flat(sizeof(Game::Wad::RawResources::Flat::texture) / sizeof(uint8_t))
{
  // Copy raw flat
  std::memcpy(_flat.data(), flat.texture, sizeof(Game::Wad::RawResources::Flat::texture));
}

Game::StaticFlat::~StaticFlat()
{}

std::vector<uint8_t> const &	Game::StaticFlat::flat() const
{
  // Return flat
  return _flat;
}