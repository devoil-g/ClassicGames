#include "Doom/Flat/StaticFlat.hpp"

DOOM::StaticFlat::StaticFlat(DOOM::Doom& doom, const DOOM::Wad::RawResources::Flat& flat) :
  DOOM::AbstractFlat(doom),
  _flat()
{
  // Copy raw flat
  std::memcpy(_flat.data(), flat.texture, sizeof(_flat));
}

const std::array<uint8_t, 4096>& DOOM::StaticFlat::flat() const
{
  // Return flat
  return _flat;
}