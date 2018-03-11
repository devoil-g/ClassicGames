#ifndef _STATIC_FLAT_HPP_
#define _STATIC_FLAT_HPP_

#include <vector>

#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Wad.hpp"

namespace Game
{
  class StaticFlat : public virtual AbstractFlat
  {
  private:
    std::vector<uint8_t>	_flat;

  public:
    StaticFlat(const Game::Wad::RawResources::Flat &);
    virtual ~StaticFlat();

    const std::vector<uint8_t> &	flat() const override;
  };
};

#endif