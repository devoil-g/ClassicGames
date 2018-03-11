#include "Doom/Thing/AbstractThing.hpp"
#include "Doom/Thing/AnimatedThing.hpp"
#include "Doom/Thing/NullThing.hpp"
#include "Doom/Doom.hpp"

Game::AbstractThing::AbstractThing(Game::Doom const &, Game::Wad::RawLevel::Thing const & thing, int16_t radius, int16_t properties) :
  position((float)thing.x, (float)thing.y),
  height(0),
  angle(thing.angle / 360.f * 2.f * Math::Pi),
  type(thing.type),
  flag(thing.flag),
  radius(radius),
  properties(properties)
{}

Game::AbstractThing::~AbstractThing()
{}

bool	Game::AbstractThing::update(sf::Time)
{
  // Do nothing
  return false;
}

Game::AbstractThing *	Game::AbstractThing::factory(Game::Doom const & doom, Game::Wad::RawLevel::Thing const & thing)
{
  switch (thing.type)
  {
    // Artifact items
  case 2023:	// Berserk
    return new Game::AnimatedThing(doom, "PSTR", "A", thing, 20, Artifact | Pickup);
  case 2026:	// Computer map
    return new Game::AnimatedThing(doom, "PMAP", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2014:	// Health potion
    return new Game::AnimatedThing(doom, "BON1", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2024:	// Invisibility
    return new Game::AnimatedThing(doom, "PINS", "ABCD", thing, 20, Artifact | Pickup);
  case 2022:	// Invulnerability
    return new Game::AnimatedThing(doom, "PINV", "ABCD", thing, 20, Artifact | Pickup);
  case 2045:	// Light amplification visor
    return new Game::AnimatedThing(doom, "PVIS", "AB", thing, 20, Artifact | Pickup);
  case 83:	// Megasphere
    return new Game::AnimatedThing(doom, "MEGA", "ABCD", thing, 20, Artifact | Pickup);
  case 2013:	// Soul sphere
    return new Game::AnimatedThing(doom, "SOUL", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2015:	// Spiritual armor
    return new Game::AnimatedThing(doom, "BON2", "ABCDCB", thing, 20, Artifact | Pickup);

  // Powerups
  case 8:	// Backpack
    return new Game::AnimatedThing(doom, "BPAK", "A", thing, 20, Pickup);
  case 2019:	// Blue armor
    return new Game::AnimatedThing(doom, "ARM2", "AB", thing, 20, Pickup);
  case 2018:	// Green armor
    return new Game::AnimatedThing(doom, "ARM1", "AB", thing, 20, Pickup);
  case 2012:	// Medikit
    return new Game::AnimatedThing(doom, "MEDI", "A", thing, 20, Pickup);
  case 2025:	// Radiation suit
    return new Game::AnimatedThing(doom, "SUIT", "A", thing, 20, Pickup);
  case 2011:	// Stimpack
    return new Game::AnimatedThing(doom, "STIM", "A", thing, 20, Pickup);

  // Weapons
  case 2006:	// BFG 9000
    return new Game::AnimatedThing(doom, "BFUG", "A", thing, 20, Pickup);
  case 2002:	// Chaingun
    return new Game::AnimatedThing(doom, "MGUN", "A", thing, 20, Pickup);
  case 2005:	// Chainsaw
    return new Game::AnimatedThing(doom, "CSAW", "A", thing, 20, Pickup);
  case 2004:	// Plasma rifle
    return new Game::AnimatedThing(doom, "PLAS", "A", thing, 20, Pickup);
  case 2003:	// Rocket launcher
    return new Game::AnimatedThing(doom, "LAUN", "A", thing, 20, Pickup);
  case 2001:	// Shotgun
    return new Game::AnimatedThing(doom, "SHOT", "A", thing, 20, Pickup);
  case 82:	// Super shotgun
    return new Game::AnimatedThing(doom, "SGN2", "A", thing, 20, Pickup);

  // Ammunition
  case 2007:	// Ammo clip
    return new Game::AnimatedThing(doom, "CLIP", "A", thing, 20, Pickup);
  case 2048:	// Box of ammo
    return new Game::AnimatedThing(doom, "AMMO", "A", thing, 20, Pickup);
  case 2046:	// Box of rockets
    return new Game::AnimatedThing(doom, "BROK", "A", thing, 20, Pickup);
  case 2049:	// Box of shells
    return new Game::AnimatedThing(doom, "SBOX", "A", thing, 20, Pickup);
  case 2047:	// Cell charge
    return new Game::AnimatedThing(doom, "CELL", "A", thing, 20, Pickup);
  case 17:	// Cell charge pack
    return new Game::AnimatedThing(doom, "CELP", "A", thing, 20, Pickup);
  case 2010:	// Rocket
    return new Game::AnimatedThing(doom, "ROCK", "A", thing, 20, Pickup);
  case 2008:	// Shotgun shells
    return new Game::AnimatedThing(doom, "SHEL", "A", thing, 20, Pickup);

  // Monsters (TODO)

  // Obstacles
  case 70:	// Burning barrel
    return new Game::AnimatedThing(doom, "FCAN", "A", thing, 10, Obstacle);
  case 43:	// Burnt tree
    return new Game::AnimatedThing(doom, "TRE1", "A", thing, 16, Obstacle);
  case 35:	// Candelabra
    return new Game::AnimatedThing(doom, "CBRA", "A", thing, 16, Obstacle);
  case 41:	// Evil eye
    return new Game::AnimatedThing(doom, "CEYE", "ABCB", thing, 16, Obstacle);
  case 28:	// Five skulls "shish kebab"
    return new Game::AnimatedThing(doom, "POL2", "A", thing, 16, Obstacle);
  case 42:	// Floating skull
    return new Game::AnimatedThing(doom, "KSKU", "ABC", thing, 16, Obstacle);
  case 2028:	// Floor lamp
    return new Game::AnimatedThing(doom, "COLU", "A", thing, 16, Obstacle);
  case 53:	// Hanging leg
    return new Game::AnimatedThing(doom, "GOR5", "A", thing, 16, Obstacle | Hanging);
  case 52:	// Hanging pair of leg
    return new Game::AnimatedThing(doom, "GOR4", "A", thing, 16, Obstacle | Hanging);
  case 78:	// Hanging torso, brain removed
    return new Game::AnimatedThing(doom, "HDB6", "A", thing, 16, Obstacle | Hanging);
  case 75:	// Hanging torso, looking down
    return new Game::AnimatedThing(doom, "HDB3", "A", thing, 16, Obstacle | Hanging);
  case 77:	// Hanging torso, looking up
    return new Game::AnimatedThing(doom, "HDB5", "A", thing, 16, Obstacle | Hanging);
  case 76:	// Hanging torso, open skull
    return new Game::AnimatedThing(doom, "HDB4", "A", thing, 16, Obstacle | Hanging);
  case 50:	// Hanging victim, arms out
    return new Game::AnimatedThing(doom, "GOR2", "A", thing, 16, Obstacle | Hanging);
  case 74:	// Hanging victim, guts and brain removed
    return new Game::AnimatedThing(doom, "HDB2", "A", thing, 16, Obstacle | Hanging);
  case 73:	// Hanging victim, guts removed
    return new Game::AnimatedThing(doom, "HDB1", "A", thing, 16, Obstacle | Hanging);
  case 51:	// Hanging victim, one-legged
    return new Game::AnimatedThing(doom, "GOR3", "A", thing, 16, Obstacle | Hanging);
  case 49:	// Hanging victim, twitching
    return new Game::AnimatedThing(doom, "GOR1", "BACA", thing, 16, Obstacle | Hanging);
  case 25:	// Impaled human
    return new Game::AnimatedThing(doom, "POL1", "A", thing, 16, Obstacle);
  case 54:	// Large brown tree
    return new Game::AnimatedThing(doom, "TRE2", "A", thing, 32, Obstacle);
  case 29:	// Pile of skulls and candles
    return new Game::AnimatedThing(doom, "POL3", "AB", thing, 16, Obstacle);
  case 55:	// Short blue firestick
    return new Game::AnimatedThing(doom, "SMBT", "ABCD", thing, 16, Obstacle);
  case 56:	// Short green firestick
    return new Game::AnimatedThing(doom, "SMGT", "ABCD", thing, 16, Obstacle);
  case 31:	// Short green pillar
    return new Game::AnimatedThing(doom, "COL2", "A", thing, 16, Obstacle);
  case 36:	// Short green pillar with beating heart
    return new Game::AnimatedThing(doom, "COL5", "AB", thing, 16, Obstacle);
  case 57:	// Short red firestick
    return new Game::AnimatedThing(doom, "SMRT", "ABCD", thing, 16, Obstacle);
  case 33:	// Short red pillar
    return new Game::AnimatedThing(doom, "COL4", "A", thing, 16, Obstacle);
  case 37:	// Short red pillar with skull
    return new Game::AnimatedThing(doom, "COL6", "A", thing, 16, Obstacle);
  case 86:	// Short techno floor lamp
    return new Game::AnimatedThing(doom, "TLP2", "ABCD", thing, 16, Obstacle);
  case 27:	// Skull on a pole
    return new Game::AnimatedThing(doom, "POL4", "A", thing, 16, Obstacle);
  case 47:	// Stalagmite
    return new Game::AnimatedThing(doom, "SMIT", "A", thing, 16, Obstacle);
  case 44:	// Tall blue firestick
    return new Game::AnimatedThing(doom, "TBLU", "ABCD", thing, 16, Obstacle);
  case 45:	// Tall green firestick
    return new Game::AnimatedThing(doom, "TGRN", "ABCD", thing, 16, Obstacle);
  case 30:	// Tall green pillar
    return new Game::AnimatedThing(doom, "COL1", "A", thing, 16, Obstacle);
  case 46:	// Tall red firestick
    return new Game::AnimatedThing(doom, "TRED", "ABCD", thing, 16, Obstacle);
  case 32:	// Tall red pillar
    return new Game::AnimatedThing(doom, "COL3", "A", thing, 16, Obstacle);
  case 85:	// Tall techno floor lamp
    return new Game::AnimatedThing(doom, "TLMP", "ABCD", thing, 16, Obstacle);
  case 48:	// Tall techno pillar
    return new Game::AnimatedThing(doom, "ELEC", "A", thing, 16, Obstacle);
  case 26:	// Twitching impaled human
    return new Game::AnimatedThing(doom, "POL6", "AB", thing, 16, Obstacle);
  
  // Decorations
  case 10:	// Bloody mess
    return new Game::AnimatedThing(doom, "PLAY", "W", thing, 16, None);
  case 12:	// Bloody mess
    return new Game::AnimatedThing(doom, "PLAY", "W", thing, 16, None);
  case 34:	// Candle
    return new Game::AnimatedThing(doom, "CAND", "A", thing, 16, None);
  case 22:	// Dead cacodemon
    return new Game::AnimatedThing(doom, "HEAD", "L", thing, 31, None);
  case 21:	// Dead demon
    return new Game::AnimatedThing(doom, "SARG", "N", thing, 30, None);
  case 18:	// Dead former human
    return new Game::AnimatedThing(doom, "POSS", "L", thing, 20, None);
  case 19:	// Dead former sergeant
    return new Game::AnimatedThing(doom, "SPOS", "L", thing, 20, None);
  case 20:	// Dead imp
    return new Game::AnimatedThing(doom, "TROO", "M", thing, 20, None);
  case 23:	// Dead lost soul (invisible)
    return new Game::AnimatedThing(doom, "SKUL", "K", thing, 16, None);
  case 15:	// Dead player
    return new Game::AnimatedThing(doom, "PLAY", "N", thing, 16, None);
  case 62:	// Hanging leg
    return new Game::AnimatedThing(doom, "GOR5", "A", thing, 16, Hanging);
  case 60:	// Hanging pair of legs
    return new Game::AnimatedThing(doom, "GOR4", "A", thing, 16, Hanging);
  case 59:	// Hanging victim, arms out
    return new Game::AnimatedThing(doom, "GOR2", "A", thing, 16, Hanging);
  case 61:	// Hanging victimn one-legged
    return new Game::AnimatedThing(doom, "GOR3", "A", thing, 16, Hanging);
  case 63:	// Hanging victim, twitching
    return new Game::AnimatedThing(doom, "GOR1", "BACA", thing, 16, Hanging);
  case 79:	// Pool of blood
    return new Game::AnimatedThing(doom, "POB1", "A", thing, 16, None);
  case 80:	// Pool of blood
    return new Game::AnimatedThing(doom, "POB2", "A", thing, 16, None);
  case 24:	// Pool of blood and flesh
    return new Game::AnimatedThing(doom, "POL5", "A", thing, 16, None);
  case 81:	// Pool of brains
    return new Game::AnimatedThing(doom, "BRS1", "A", thing, 16, None);

    // Other (TODO)

  default:
    return new Game::NullThing(doom, thing);
  }

  // Should not happen
  return nullptr;
}