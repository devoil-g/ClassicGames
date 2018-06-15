#include "Doom/Flat/StaticFlat.hpp"	

DOOM::StaticFlat::StaticFlat(DOOM::Wad::RawResources::Flat const & flat) :
  AbstractFlat(),
  _flat(sizeof(DOOM::Wad::RawResources::Flat::texture) / sizeof(uint8_t))
{
  // Copy raw flat
  std::memcpy(_flat.data(), flat.texture, sizeof(DOOM::Wad::RawResources::Flat::texture));
}

DOOM::StaticFlat::~StaticFlat()
{}

std::vector<uint8_t> const &	DOOM::StaticFlat::flat() const
{
  // Return flat
  return _flat;
}