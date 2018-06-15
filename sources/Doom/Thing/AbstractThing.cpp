#include "Doom/Doom.hpp"
#include "Doom/Thing/AbstractThing.hpp"
#include "Doom/Thing/AnimatedThing.hpp"
#include "Doom/Thing/NullThing.hpp"

DOOM::AbstractThing::AbstractThing(DOOM::Doom const &, DOOM::Wad::RawLevel::Thing const & thing, int16_t radius, int16_t properties) :
  position((float)thing.x, (float)thing.y),
  height(0),
  angle(thing.angle / 360.f * 2.f * Math::Pi),
  type(thing.type),
  flag(thing.flag),
  radius(radius),
  properties(properties)
{}

DOOM::AbstractThing::~AbstractThing()
{}

bool	DOOM::AbstractThing::update(sf::Time)
{
  // Do nothing
  return false;
}

DOOM::AbstractThing *	DOOM::AbstractThing::factory(DOOM::Doom const & doom, DOOM::Wad::RawLevel::Thing const & thing)
{
  switch (thing.type)
  {
    // Artifact items
  case 2023:	// Berserk
    return new DOOM::AnimatedThing(doom, "PSTR", "A", thing, 20, Artifact | Pickup);
  case 2026:	// Computer map
    return new DOOM::AnimatedThing(doom, "PMAP", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2014:	// Health potion
    return new DOOM::AnimatedThing(doom, "BON1", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2024:	// Invisibility
    return new DOOM::AnimatedThing(doom, "PINS", "ABCD", thing, 20, Artifact | Pickup);
  case 2022:	// Invulnerability
    return new DOOM::AnimatedThing(doom, "PINV", "ABCD", thing, 20, Artifact | Pickup);
  case 2045:	// Light amplification visor
    return new DOOM::AnimatedThing(doom, "PVIS", "AB", thing, 20, Artifact | Pickup);
  case 83:	// Megasphere
    return new DOOM::AnimatedThing(doom, "MEGA", "ABCD", thing, 20, Artifact | Pickup);
  case 2013:	// Soul sphere
    return new DOOM::AnimatedThing(doom, "SOUL", "ABCDCB", thing, 20, Artifact | Pickup);
  case 2015:	// Spiritual armor
    return new DOOM::AnimatedThing(doom, "BON2", "ABCDCB", thing, 20, Artifact | Pickup);

  // Powerups
  case 8:	// Backpack
    return new DOOM::AnimatedThing(doom, "BPAK", "A", thing, 20, Pickup);
  case 2019:	// Blue armor
    return new DOOM::AnimatedThing(doom, "ARM2", "AB", thing, 20, Pickup);
  case 2018:	// Green armor
    return new DOOM::AnimatedThing(doom, "ARM1", "AB", thing, 20, Pickup);
  case 2012:	// Medikit
    return new DOOM::AnimatedThing(doom, "MEDI", "A", thing, 20, Pickup);
  case 2025:	// Radiation suit
    return new DOOM::AnimatedThing(doom, "SUIT", "A", thing, 20, Pickup);
  case 2011:	// Stimpack
    return new DOOM::AnimatedThing(doom, "STIM", "A", thing, 20, Pickup);

  // Weapons
  case 2006:	// BFG 9000
    return new DOOM::AnimatedThing(doom, "BFUG", "A", thing, 20, Pickup);
  case 2002:	// Chaingun
    return new DOOM::AnimatedThing(doom, "MGUN", "A", thing, 20, Pickup);
  case 2005:	// Chainsaw
    return new DOOM::AnimatedThing(doom, "CSAW", "A", thing, 20, Pickup);
  case 2004:	// Plasma rifle
    return new DOOM::AnimatedThing(doom, "PLAS", "A", thing, 20, Pickup);
  case 2003:	// Rocket launcher
    return new DOOM::AnimatedThing(doom, "LAUN", "A", thing, 20, Pickup);
  case 2001:	// Shotgun
    return new DOOM::AnimatedThing(doom, "SHOT", "A", thing, 20, Pickup);
  case 82:	// Super shotgun
    return new DOOM::AnimatedThing(doom, "SGN2", "A", thing, 20, Pickup);

  // Ammunition
  case 2007:	// Ammo clip
    return new DOOM::AnimatedThing(doom, "CLIP", "A", thing, 20, Pickup);
  case 2048:	// Box of ammo
    return new DOOM::AnimatedThing(doom, "AMMO", "A", thing, 20, Pickup);
  case 2046:	// Box of rockets
    return new DOOM::AnimatedThing(doom, "BROK", "A", thing, 20, Pickup);
  case 2049:	// Box of shells
    return new DOOM::AnimatedThing(doom, "SBOX", "A", thing, 20, Pickup);
  case 2047:	// Cell charge
    return new DOOM::AnimatedThing(doom, "CELL", "A", thing, 20, Pickup);
  case 17:	// Cell charge pack
    return new DOOM::AnimatedThing(doom, "CELP", "A", thing, 20, Pickup);
  case 2010:	// Rocket
    return new DOOM::AnimatedThing(doom, "ROCK", "A", thing, 20, Pickup);
  case 2008:	// Shotgun shells
    return new DOOM::AnimatedThing(doom, "SHEL", "A", thing, 20, Pickup);

  // Monsters (TODO)

  // Obstacles
  case 70:	// Burning barrel
    return new DOOM::AnimatedThing(doom, "FCAN", "A", thing, 10, Obstacle);
  case 43:	// Burnt tree
    return new DOOM::AnimatedThing(doom, "TRE1", "A", thing, 16, Obstacle);
  case 35:	// Candelabra
    return new DOOM::AnimatedThing(doom, "CBRA", "A", thing, 16, Obstacle);
  case 41:	// Evil eye
    return new DOOM::AnimatedThing(doom, "CEYE", "ABCB", thing, 16, Obstacle);
  case 28:	// Five skulls "shish kebab"
    return new DOOM::AnimatedThing(doom, "POL2", "A", thing, 16, Obstacle);
  case 42:	// Floating skull
    return new DOOM::AnimatedThing(doom, "KSKU", "ABC", thing, 16, Obstacle);
  case 2028:	// Floor lamp
    return new DOOM::AnimatedThing(doom, "COLU", "A", thing, 16, Obstacle);
  case 53:	// Hanging leg
    return new DOOM::AnimatedThing(doom, "GOR5", "A", thing, 16, Obstacle | Hanging);
  case 52:	// Hanging pair of leg
    return new DOOM::AnimatedThing(doom, "GOR4", "A", thing, 16, Obstacle | Hanging);
  case 78:	// Hanging torso, brain removed
    return new DOOM::AnimatedThing(doom, "HDB6", "A", thing, 16, Obstacle | Hanging);
  case 75:	// Hanging torso, looking down
    return new DOOM::AnimatedThing(doom, "HDB3", "A", thing, 16, Obstacle | Hanging);
  case 77:	// Hanging torso, looking up
    return new DOOM::AnimatedThing(doom, "HDB5", "A", thing, 16, Obstacle | Hanging);
  case 76:	// Hanging torso, open skull
    return new DOOM::AnimatedThing(doom, "HDB4", "A", thing, 16, Obstacle | Hanging);
  case 50:	// Hanging victim, arms out
    return new DOOM::AnimatedThing(doom, "GOR2", "A", thing, 16, Obstacle | Hanging);
  case 74:	// Hanging victim, guts and brain removed
    return new DOOM::AnimatedThing(doom, "HDB2", "A", thing, 16, Obstacle | Hanging);
  case 73:	// Hanging victim, guts removed
    return new DOOM::AnimatedThing(doom, "HDB1", "A", thing, 16, Obstacle | Hanging);
  case 51:	// Hanging victim, one-legged
    return new DOOM::AnimatedThing(doom, "GOR3", "A", thing, 16, Obstacle | Hanging);
  case 49:	// Hanging victim, twitching
    return new DOOM::AnimatedThing(doom, "GOR1", "BACA", thing, 16, Obstacle | Hanging);
  case 25:	// Impaled human
    return new DOOM::AnimatedThing(doom, "POL1", "A", thing, 16, Obstacle);
  case 54:	// Large brown tree
    return new DOOM::AnimatedThing(doom, "TRE2", "A", thing, 32, Obstacle);
  case 29:	// Pile of skulls and candles
    return new DOOM::AnimatedThing(doom, "POL3", "AB", thing, 16, Obstacle);
  case 55:	// Short blue firestick
    return new DOOM::AnimatedThing(doom, "SMBT", "ABCD", thing, 16, Obstacle);
  case 56:	// Short green firestick
    return new DOOM::AnimatedThing(doom, "SMGT", "ABCD", thing, 16, Obstacle);
  case 31:	// Short green pillar
    return new DOOM::AnimatedThing(doom, "COL2", "A", thing, 16, Obstacle);
  case 36:	// Short green pillar with beating heart
    return new DOOM::AnimatedThing(doom, "COL5", "AB", thing, 16, Obstacle);
  case 57:	// Short red firestick
    return new DOOM::AnimatedThing(doom, "SMRT", "ABCD", thing, 16, Obstacle);
  case 33:	// Short red pillar
    return new DOOM::AnimatedThing(doom, "COL4", "A", thing, 16, Obstacle);
  case 37:	// Short red pillar with skull
    return new DOOM::AnimatedThing(doom, "COL6", "A", thing, 16, Obstacle);
  case 86:	// Short techno floor lamp
    return new DOOM::AnimatedThing(doom, "TLP2", "ABCD", thing, 16, Obstacle);
  case 27:	// Skull on a pole
    return new DOOM::AnimatedThing(doom, "POL4", "A", thing, 16, Obstacle);
  case 47:	// Stalagmite
    return new DOOM::AnimatedThing(doom, "SMIT", "A", thing, 16, Obstacle);
  case 44:	// Tall blue firestick
    return new DOOM::AnimatedThing(doom, "TBLU", "ABCD", thing, 16, Obstacle);
  case 45:	// Tall green firestick
    return new DOOM::AnimatedThing(doom, "TGRN", "ABCD", thing, 16, Obstacle);
  case 30:	// Tall green pillar
    return new DOOM::AnimatedThing(doom, "COL1", "A", thing, 16, Obstacle);
  case 46:	// Tall red firestick
    return new DOOM::AnimatedThing(doom, "TRED", "ABCD", thing, 16, Obstacle);
  case 32:	// Tall red pillar
    return new DOOM::AnimatedThing(doom, "COL3", "A", thing, 16, Obstacle);
  case 85:	// Tall techno floor lamp
    return new DOOM::AnimatedThing(doom, "TLMP", "ABCD", thing, 16, Obstacle);
  case 48:	// Tall techno pillar
    return new DOOM::AnimatedThing(doom, "ELEC", "A", thing, 16, Obstacle);
  case 26:	// Twitching impaled human
    return new DOOM::AnimatedThing(doom, "POL6", "AB", thing, 16, Obstacle);
  
  // Decorations
  case 10:	// Bloody mess
    return new DOOM::AnimatedThing(doom, "PLAY", "W", thing, 16, None);
  case 12:	// Bloody mess
    return new DOOM::AnimatedThing(doom, "PLAY", "W", thing, 16, None);
  case 34:	// Candle
    return new DOOM::AnimatedThing(doom, "CAND", "A", thing, 16, None);
  case 22:	// Dead cacodemon
    return new DOOM::AnimatedThing(doom, "HEAD", "L", thing, 31, None);
  case 21:	// Dead demon
    return new DOOM::AnimatedThing(doom, "SARG", "N", thing, 30, None);
  case 18:	// Dead former human
    return new DOOM::AnimatedThing(doom, "POSS", "L", thing, 20, None);
  case 19:	// Dead former sergeant
    return new DOOM::AnimatedThing(doom, "SPOS", "L", thing, 20, None);
  case 20:	// Dead imp
    return new DOOM::AnimatedThing(doom, "TROO", "M", thing, 20, None);
  case 23:	// Dead lost soul (invisible)
    return new DOOM::AnimatedThing(doom, "SKUL", "K", thing, 16, None);
  case 15:	// Dead player
    return new DOOM::AnimatedThing(doom, "PLAY", "N", thing, 16, None);
  case 62:	// Hanging leg
    return new DOOM::AnimatedThing(doom, "GOR5", "A", thing, 16, Hanging);
  case 60:	// Hanging pair of legs
    return new DOOM::AnimatedThing(doom, "GOR4", "A", thing, 16, Hanging);
  case 59:	// Hanging victim, arms out
    return new DOOM::AnimatedThing(doom, "GOR2", "A", thing, 16, Hanging);
  case 61:	// Hanging victimn one-legged
    return new DOOM::AnimatedThing(doom, "GOR3", "A", thing, 16, Hanging);
  case 63:	// Hanging victim, twitching
    return new DOOM::AnimatedThing(doom, "GOR1", "BACA", thing, 16, Hanging);
  case 79:	// Pool of blood
    return new DOOM::AnimatedThing(doom, "POB1", "A", thing, 16, None);
  case 80:	// Pool of blood
    return new DOOM::AnimatedThing(doom, "POB2", "A", thing, 16, None);
  case 24:	// Pool of blood and flesh
    return new DOOM::AnimatedThing(doom, "POL5", "A", thing, 16, None);
  case 81:	// Pool of brains
    return new DOOM::AnimatedThing(doom, "BRS1", "A", thing, 16, None);

    // Other (TODO)

  default:
    return new DOOM::NullThing(doom, thing);
  }
}