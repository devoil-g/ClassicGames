#include "Doom/Thing/AbstractNonePhysicsThing.hpp"

DOOM::AbstractNonePhysicsThing::AbstractNonePhysicsThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties) :
  DOOM::AbstractThing(doom, thing, height, radius, properties)
{}

DOOM::AbstractNonePhysicsThing::~AbstractNonePhysicsThing() = default;

void	DOOM::AbstractNonePhysicsThing::teleport(DOOM::Doom & doom, const Math::Vector<2> & destination, float angle)
{
  // Does nothing
}

void	DOOM::AbstractNonePhysicsThing::thrust(const Math::Vector<2> & acceleration)
{
  // Does nothing
}