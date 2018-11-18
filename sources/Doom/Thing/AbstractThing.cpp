#include "Doom/Thing/AbstractThing.hpp"
#include "Doom/Thing/AnimatedThing.hpp"
#include "Doom/Thing/NullThing.hpp"

DOOM::AbstractThing::AbstractThing(DOOM::Doom & doom, DOOM::Wad::RawLevel::Thing const & thing, int16_t radius, int16_t properties) :
  position((float)thing.x, (float)thing.y),
  height(0.f),
  angle(thing.angle / 360.f * 2.f * Math::Pi),
  type(thing.type),
  flag(thing.flag),
  radius(radius),
  properties(properties)
{}

DOOM::AbstractThing::AbstractThing(DOOM::Doom & doom, int16_t radius, int16_t properties) :
  position(0.f, 0.f),
  height(0.f),
  angle(0.f),
  type(-1),
  flag(0),
  radius(radius),
  properties(properties)
{}

bool	DOOM::AbstractThing::update(DOOM::Doom & doom, sf::Time)
{
  // Does nothing
  return false;
}

std::unique_ptr<DOOM::AbstractThing>	DOOM::AbstractThing::factory(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing)
{
  switch (thing.type)
  {
    // Artifact items
  case 2023:	// Berserk
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PSTR", "A", thing, 20, Artifact | Pickup);
  case 2026:	// Computer map
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PMAP", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2014:	// Health potion
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BON1", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2024:	// Invisibility
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PINS", "ABCD", thing, 20, Artifact | Pickup);
  case 2022:	// Invulnerability
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PINV", "ABCD", thing, 20, Artifact | Pickup);
  case 2045:	// Light amplification visor
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PVIS", "AB", thing, 20, Artifact | Pickup);
  case 83:	// Megasphere
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "MEGA", "ABCD", thing, 20, Artifact | Pickup);
  case 2013:	// Soul sphere
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SOUL", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2015:	// Spiritual armor
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BON2", "ABCDCB", thing, 20, Artifact | Pickup);

    // Powerups
  case 8:	// Backpack
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BPAK", "A", thing, 20, Pickup);
  case 2019:	// Blue armor
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "ARM2", "AB", thing, 20, Pickup);
  case 2018:	// Green armor
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "ARM1", "AB", thing, 20, Pickup);
  case 2012:	// Medikit
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "MEDI", "A", thing, 20, Pickup);
  case 2025:	// Radiation suit
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SUIT", "A", thing, 20, Pickup);
  case 2011:	// Stimpack
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "STIM", "A", thing, 20, Pickup);

    // Weapons
  case 2006:	// BFG 9000
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BFUG", "A", thing, 20, Pickup);
  case 2002:	// Chaingun
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "MGUN", "A", thing, 20, Pickup);
  case 2005:	// Chainsaw
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CSAW", "A", thing, 20, Pickup);
  case 2004:	// Plasma rifle
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PLAS", "A", thing, 20, Pickup);
  case 2003:	// Rocket launcher
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "LAUN", "A", thing, 20, Pickup);
  case 2001:	// Shotgun
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SHOT", "A", thing, 20, Pickup);
  case 82:	// Super shotgun
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SGN2", "A", thing, 20, Pickup);

    // Ammunition
  case 2007:	// Ammo clip
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CLIP", "A", thing, 20, Pickup);
  case 2048:	// Box of ammo
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "AMMO", "A", thing, 20, Pickup);
  case 2046:	// Box of rockets
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BROK", "A", thing, 20, Pickup);
  case 2049:	// Box of shells
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SBOX", "A", thing, 20, Pickup);
  case 2047:	// Cell charge
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CELL", "A", thing, 20, Pickup);
  case 17:	// Cell charge pack
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CELP", "A", thing, 20, Pickup);
  case 2010:	// Rocket
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "ROCK", "A", thing, 20, Pickup);
  case 2008:	// Shotgun shells
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SHEL", "A", thing, 20, Pickup);

    // Keys
  case 5:	// Blue keycard
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BKEY", "AB", thing, 20, Pickup);
  case 40:	// Blue skull key
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BSKU", "AB", thing, 20, Pickup);
  case 13:  	// Red keycard
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "RKEY", "AB", thing, 20, Pickup);
  case 38:	// Red skull key
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "RSKU", "AB", thing, 20, Pickup);
  case 6:	// Yellow keycard
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "YKEY", "AB", thing, 20, Pickup);
  case 39:	// Yellow skull key
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "YSKU", "AB", thing, 20, Pickup);

    // Monsters (TODO)

    // Obstacles
  case 43:	// Burnt tree
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TRE1", "A", thing, 16, Obstacle);
  case 35:	// Candelabra
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CBRA", "A", thing, 16, Obstacle);
  case 41:	// Evil eye
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CEYE", "ABCB", thing, 16, Obstacle);
  case 28:	// Five skulls "shish kebab"
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL2", "A", thing, 16, Obstacle);
  case 42:	// Floating skull
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "KSKU", "ABC", thing, 16, Obstacle);
  case 2028:	// Floor lamp
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COLU", "A", thing, 16, Obstacle);
  case 53:	// Hanging leg
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR5", "A", thing, 16, Obstacle | Hanging);
  case 52:	// Hanging pair of leg
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR4", "A", thing, 16, Obstacle | Hanging);
  case 78:	// Hanging torso, brain removed
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB6", "A", thing, 16, Obstacle | Hanging);
  case 75:	// Hanging torso, looking down
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB3", "A", thing, 16, Obstacle | Hanging);
  case 77:	// Hanging torso, looking up
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB5", "A", thing, 16, Obstacle | Hanging);
  case 76:	// Hanging torso, open skull
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB4", "A", thing, 16, Obstacle | Hanging);
  case 50:	// Hanging victim, arms out
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR2", "A", thing, 16, Obstacle | Hanging);
  case 74:	// Hanging victim, guts and brain removed
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB2", "A", thing, 16, Obstacle | Hanging);
  case 73:	// Hanging victim, guts removed
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HDB1", "A", thing, 16, Obstacle | Hanging);
  case 51:	// Hanging victim, one-legged
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR3", "A", thing, 16, Obstacle | Hanging);
  case 49:	// Hanging victim, twitching
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR1", "BACA", thing, 16, Obstacle | Hanging);
  case 25:	// Impaled human
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL1", "A", thing, 16, Obstacle);
  case 54:	// Large brown tree
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TRE2", "A", thing, 32, Obstacle);
  case 29:	// Pile of skulls and candles
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL3", "AB", thing, 16, Obstacle);
  case 55:	// Short blue firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SMBT", "ABCD", thing, 16, Obstacle);
  case 56:	// Short green firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SMGT", "ABCD", thing, 16, Obstacle);
  case 31:	// Short green pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL2", "A", thing, 16, Obstacle);
  case 36:	// Short green pillar with beating heart
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL5", "AB", thing, 16, Obstacle);
  case 57:	// Short red firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SMRT", "ABCD", thing, 16, Obstacle);
  case 33:	// Short red pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL4", "A", thing, 16, Obstacle);
  case 37:	// Short red pillar with skull
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL6", "A", thing, 16, Obstacle);
  case 86:	// Short techno floor lamp
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TLP2", "ABCD", thing, 16, Obstacle);
  case 27:	// Skull on a pole
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL4", "A", thing, 16, Obstacle);
  case 47:	// Stalagmite
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SMIT", "A", thing, 16, Obstacle);
  case 44:	// Tall blue firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TBLU", "ABCD", thing, 16, Obstacle);
  case 45:	// Tall green firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TGRN", "ABCD", thing, 16, Obstacle);
  case 30:	// Tall green pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL1", "A", thing, 16, Obstacle);
  case 46:	// Tall red firestick
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TRED", "ABCD", thing, 16, Obstacle);
  case 32:	// Tall red pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "COL3", "A", thing, 16, Obstacle);
  case 85:	// Tall techno floor lamp
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TLMP", "ABCD", thing, 16, Obstacle);
  case 48:	// Tall techno pillar
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "ELEC", "A", thing, 16, Obstacle);
  case 26:	// Twitching impaled human
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL6", "AB", thing, 16, Obstacle);

    // Decorations
  case 10:	// Bloody mess
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PLAY", "W", thing, 16, None);
  case 12:	// Bloody mess
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PLAY", "W", thing, 16, None);
  case 34:	// Candle
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "CAND", "A", thing, 16, None);
  case 22:	// Dead cacodemon
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "HEAD", "L", thing, 31, None);
  case 21:	// Dead demon
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SARG", "N", thing, 30, None);
  case 18:	// Dead former human
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POSS", "L", thing, 20, None);
  case 19:	// Dead former sergeant
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SPOS", "L", thing, 20, None);
  case 20:	// Dead imp
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "TROO", "M", thing, 20, None);
  case 23:	// Dead lost soul (invisible)
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "SKUL", "K", thing, 16, None);
  case 15:	// Dead player
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "PLAY", "N", thing, 16, None);
  case 62:	// Hanging leg
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR5", "A", thing, 16, Hanging);
  case 60:	// Hanging pair of legs
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR4", "A", thing, 16, Hanging);
  case 59:	// Hanging victim, arms out
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR2", "A", thing, 16, Hanging);
  case 61:	// Hanging victimn one-legged
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR3", "A", thing, 16, Hanging);
  case 63:	// Hanging victim, twitching
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "GOR1", "BACA", thing, 16, Hanging);
  case 79:	// Pool of blood
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POB1", "A", thing, 16, None);
  case 80:	// Pool of blood
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POB2", "A", thing, 16, None);
  case 24:	// Pool of blood and flesh
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "POL5", "A", thing, 16, None);
  case 81:	// Pool of brains
    return std::make_unique<DOOM::AnimatedThing<>>(doom, "BRS1", "A", thing, 16, None);

    // Other (TODO)

  default:
    return std::make_unique<DOOM::NullThing>(doom, thing);
  }
}
