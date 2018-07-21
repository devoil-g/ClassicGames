#ifndef _ABSTRACT_FLAT_HPP_
#define _ABSTRACT_FLAT_HPP_

#include <memory>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"

namespace DOOM
{
  class StaticFlat;

  class AbstractFlat
  {
  private:
    static const DOOM::StaticFlat	_Null;	// Null flat

  public:
    static const DOOM::AbstractFlat &	Null;	// Null flat abstract holder

    static std::unique_ptr<DOOM::AbstractFlat>	factory(DOOM::Doom & doom, uint64_t name, const DOOM::Wad::RawResources::Flat & flat);

  protected:
    AbstractFlat() = default;

  public:
    AbstractFlat(DOOM::Doom & doom);
    virtual ~AbstractFlat() = default;

    virtual void				update(DOOM::Doom & doom, sf::Time elapsed);	// Update flat
    virtual const std::array<uint8_t, 4096> &	flat() const = 0;				// Return flat to be displayed
  };
};

#endif