#ifndef _STATIC_FLAT_HPP_
#define _STATIC_FLAT_HPP_

#include "Doom/Doom.hpp"

namespace DOOM
{
  class StaticFlat : public virtual DOOM::Doom::Resources::AbstractFlat
  {
  public:
    static DOOM::StaticFlat	NullFlat;

  private:
    std::array<uint8_t, 4096>	_flat;	// Flat texture buffer

    StaticFlat() = default;

  public:
    StaticFlat(DOOM::Doom & doom, const DOOM::Wad::RawResources::Flat & flat);
    ~StaticFlat() override = default;

    const std::array<uint8_t, 4096> &	flat() const override;
  };
};

#endif