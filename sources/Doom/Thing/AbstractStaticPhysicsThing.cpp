#include "Doom/Thing/AbstractStaticPhysicsThing.hpp"

DOOM::AbstractStaticPhysicsThing::AbstractStaticPhysicsThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties) :
  DOOM::AbstractThing(doom, thing, height, radius, properties),
  _sector(doom.level.getSector(position.convert<2>()).first)
{
  // Add thing to blockmap
  doom.level.blockmap.addThing(*this, position.convert<2>());
}

DOOM::AbstractStaticPhysicsThing::~AbstractStaticPhysicsThing() = default;

bool	DOOM::AbstractStaticPhysicsThing::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Update only Z position
  position.z() = (properties & DOOM::AbstractThing::Properties::Hanging) ?
    doom.level.sectors[_sector].ceiling_current :
    doom.level.sectors[_sector].floor_current;

  return false;
}

void	DOOM::AbstractStaticPhysicsThing::teleport(DOOM::Doom & doom, const Math::Vector<2> & destination, float angle)
{
  // Does nothing
}

void	DOOM::AbstractStaticPhysicsThing::thrust(const Math::Vector<2> & acceleration)
{
  // Does nothing, acceleration is ignored as thing is static
}