#include "Doom/Doom.hpp"
#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Flat/AnimatedFlat.hpp"
#include "Doom/Flat/StaticFlat.hpp"

const DOOM::StaticFlat    DOOM::AbstractFlat::_Null = DOOM::StaticFlat();
const DOOM::AbstractFlat& DOOM::AbstractFlat::Null = _Null;

DOOM::AbstractFlat::AbstractFlat(DOOM::Doom& doom)
{}

void                                DOOM::AbstractFlat::update(DOOM::Doom& doom, float elapsed)
{}

std::unique_ptr<DOOM::AbstractFlat> DOOM::AbstractFlat::factory(DOOM::Doom& doom, uint64_t name, const DOOM::Wad::RawResources::Flat& flat)
{
  // List of animated flats and their frames
  static std::vector<std::vector<uint64_t>> const animations = {
    { Game::Utilities::str_to_key<uint64_t>("NUKAGE1"), Game::Utilities::str_to_key<uint64_t>("NUKAGE2"), Game::Utilities::str_to_key<uint64_t>("NUKAGE3") },                              // Green slime, nukage
    { Game::Utilities::str_to_key<uint64_t>("FWATER1"), Game::Utilities::str_to_key<uint64_t>("FWATER2"), Game::Utilities::str_to_key<uint64_t>("FWATER3"), Game::Utilities::str_to_key<uint64_t>("FWATER4") }, // Blue water
    { Game::Utilities::str_to_key<uint64_t>("SWATER1"), Game::Utilities::str_to_key<uint64_t>("SWATER2"), Game::Utilities::str_to_key<uint64_t>("SWATER3"), Game::Utilities::str_to_key<uint64_t>("SWATER4") }, // Blue water
    { Game::Utilities::str_to_key<uint64_t>("LAVA1"), Game::Utilities::str_to_key<uint64_t>("LAVA2"), Game::Utilities::str_to_key<uint64_t>("LAVA3"), Game::Utilities::str_to_key<uint64_t>("LAVA4") },         // Lava
    { Game::Utilities::str_to_key<uint64_t>("BLOOD1"), Game::Utilities::str_to_key<uint64_t>("BLOOD2"), Game::Utilities::str_to_key<uint64_t>("BLOOD3") },                                 // Blood
    { Game::Utilities::str_to_key<uint64_t>("RROCK05"), Game::Utilities::str_to_key<uint64_t>("RROCK06"), Game::Utilities::str_to_key<uint64_t>("RROCK07"), Game::Utilities::str_to_key<uint64_t>("RROCK08") }, // Large molten rock
    { Game::Utilities::str_to_key<uint64_t>("SLIME01"), Game::Utilities::str_to_key<uint64_t>("SLIME02"), Game::Utilities::str_to_key<uint64_t>("SLIME03"), Game::Utilities::str_to_key<uint64_t>("SLIME04") }, // Brown water
    { Game::Utilities::str_to_key<uint64_t>("SLIME05"), Game::Utilities::str_to_key<uint64_t>("SLIME06"), Game::Utilities::str_to_key<uint64_t>("SLIME07"), Game::Utilities::str_to_key<uint64_t>("SLIME08") }, // Brown slime
    { Game::Utilities::str_to_key<uint64_t>("SLIME09"), Game::Utilities::str_to_key<uint64_t>("SLIME10"), Game::Utilities::str_to_key<uint64_t>("SLIME11"), Game::Utilities::str_to_key<uint64_t>("SLIME12") }  // Small molten rock
  };

  // Check if flat is part of an animated sequence
  for (const auto& sequence : animations)
    for (uint64_t const frame : sequence)
      if (name == frame)
        return std::make_unique<DOOM::AnimatedFlat<>>(doom, sequence);

  // Default static flat
  return std::make_unique<DOOM::StaticFlat>(doom, flat);
}
