#ifndef _STATIC_FLAT_HPP_
#define _STATIC_FLAT_HPP_

#include <vector>

#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Wad.hpp"

namespace DOOM
{
  class StaticFlat : public virtual DOOM::AbstractFlat
  {
  private:
    std::vector<uint8_t>	_flat;

  public:
    StaticFlat(const DOOM::Wad::RawResources::Flat & flat);
    virtual ~StaticFlat();

    const std::vector<uint8_t> &	flat() const override;
  };
};

#endif