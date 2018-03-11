#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Flat/AnimatedFlat.hpp"
#include "Doom/Flat/StaticFlat.hpp"

Game::AbstractFlat::AbstractFlat()
{}

Game::AbstractFlat::~AbstractFlat()
{}

void	Game::AbstractFlat::update(sf::Time)
{}

Game::AbstractFlat *	Game::AbstractFlat::factory(Game::Wad const & wad, uint64_t name, Game::Wad::RawResources::Flat const & flat)
{
  // List of animated flats and their frames
  static std::vector<std::vector<uint64_t>> const	animations =
  {
    { Game::str_to_key("NUKAGE1"), Game::str_to_key("NUKAGE2"), Game::str_to_key("NUKAGE3") },					// Green slime, nukage
    { Game::str_to_key("FWATER1"), Game::str_to_key("FWATER2"), Game::str_to_key("FWATER3"), Game::str_to_key("FWATER4") },	// Blue water
    { Game::str_to_key("SWATER1"), Game::str_to_key("SWATER2"), Game::str_to_key("SWATER3"), Game::str_to_key("SWATER4") },	// Blue water
    { Game::str_to_key("LAVA1"), Game::str_to_key("LAVA2"), Game::str_to_key("LAVA3"), Game::str_to_key("LAVA4") },		// Lava
    { Game::str_to_key("BLOOD1"), Game::str_to_key("BLOOD2"), Game::str_to_key("BLOOD3") },					// Blood
    { Game::str_to_key("RROCK05"), Game::str_to_key("RROCK06"), Game::str_to_key("RROCK07"), Game::str_to_key("RROCK08") },	// Large molten rock
    { Game::str_to_key("SLIME01"), Game::str_to_key("SLIME02"), Game::str_to_key("SLIME03"), Game::str_to_key("SLIME04") },	// Brown water
    { Game::str_to_key("SLIME05"), Game::str_to_key("SLIME06"), Game::str_to_key("SLIME07"), Game::str_to_key("SLIME08") },	// Brown slime
    { Game::str_to_key("SLIME09"), Game::str_to_key("SLIME10"), Game::str_to_key("SLIME11"), Game::str_to_key("SLIME12") }	// Small molten rock
  };

  // Check if flat is part of an animated sequence
  for (std::vector<uint64_t> const & sequence : animations)
    for (uint64_t const frame : sequence)
      if (name == frame)
	return new Game::AnimatedFlat(wad, sequence);
  
  // Default static flat
  return new Game::StaticFlat(flat);
}