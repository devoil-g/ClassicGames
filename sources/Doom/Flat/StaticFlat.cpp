#include "Doom/Flat/StaticFlat.hpp"	

DOOM::StaticFlat	DOOM::StaticFlat::NullFlat;

DOOM::StaticFlat::StaticFlat(DOOM::Doom & doom, DOOM::Wad::RawResources::Flat const & flat) :
  DOOM::Doom::Resources::AbstractFlat(doom),
  _flat()
{
  // Copy raw flat
  std::memcpy(_flat.data(), flat.texture, sizeof(DOOM::Wad::RawResources::Flat::texture));
}

std::array<uint8_t, 4096> const &	DOOM::StaticFlat::flat() const
{
  // Return flat
  return _flat;
}