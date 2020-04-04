#include "Doom/Doom.hpp"
#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Flat/AnimatedFlat.hpp"
#include "Doom/Flat/StaticFlat.hpp"

const DOOM::StaticFlat    DOOM::AbstractFlat::_Null;
const DOOM::AbstractFlat& DOOM::AbstractFlat::Null = _Null;

DOOM::AbstractFlat::AbstractFlat(DOOM::Doom& doom)
{}

void                                DOOM::AbstractFlat::update(DOOM::Doom& doom, sf::Time)
{}

std::unique_ptr<DOOM::AbstractFlat> DOOM::AbstractFlat::factory(DOOM::Doom& doom, uint64_t name, DOOM::Wad::RawResources::Flat const& flat)
{
  // List of animated flats and their frames
  static std::vector<std::vector<uint64_t>> const	animations = {
    { DOOM::str_to_key("NUKAGE1"), DOOM::str_to_key("NUKAGE2"), DOOM::str_to_key("NUKAGE3") },                              // Green slime, nukage
    { DOOM::str_to_key("FWATER1"), DOOM::str_to_key("FWATER2"), DOOM::str_to_key("FWATER3"), DOOM::str_to_key("FWATER4") }, // Blue water
    { DOOM::str_to_key("SWATER1"), DOOM::str_to_key("SWATER2"), DOOM::str_to_key("SWATER3"), DOOM::str_to_key("SWATER4") }, // Blue water
    { DOOM::str_to_key("LAVA1"), DOOM::str_to_key("LAVA2"), DOOM::str_to_key("LAVA3"), DOOM::str_to_key("LAVA4") },         // Lava
    { DOOM::str_to_key("BLOOD1"), DOOM::str_to_key("BLOOD2"), DOOM::str_to_key("BLOOD3") },                                 // Blood
    { DOOM::str_to_key("RROCK05"), DOOM::str_to_key("RROCK06"), DOOM::str_to_key("RROCK07"), DOOM::str_to_key("RROCK08") }, // Large molten rock
    { DOOM::str_to_key("SLIME01"), DOOM::str_to_key("SLIME02"), DOOM::str_to_key("SLIME03"), DOOM::str_to_key("SLIME04") }, // Brown water
    { DOOM::str_to_key("SLIME05"), DOOM::str_to_key("SLIME06"), DOOM::str_to_key("SLIME07"), DOOM::str_to_key("SLIME08") }, // Brown slime
    { DOOM::str_to_key("SLIME09"), DOOM::str_to_key("SLIME10"), DOOM::str_to_key("SLIME11"), DOOM::str_to_key("SLIME12") }  // Small molten rock
  };

  // Check if flat is part of an animated sequence
  for (std::vector<uint64_t> const& sequence : animations)
    for (uint64_t const frame : sequence)
      if (name == frame)
        return std::make_unique<DOOM::AnimatedFlat<>>(doom, sequence);

  // Default static flat
  return std::make_unique<DOOM::StaticFlat>(doom, flat);
}
