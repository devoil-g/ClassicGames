#ifndef _ABSTRACT_SECTOR_HPP_
#define _ABSTRACT_SECTOR_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Flat/AbstractFlat.hpp"

namespace DOOM
{
  class AbstractSector
  {
  public:
    class AbstractSectorAction
    {
    public:
      static const std::vector<int16_t>	LevelingTypes;
      static const std::vector<int16_t>	LightingTypes;

      static DOOM::AbstractSector::AbstractSectorAction *	factory(DOOM::AbstractSector & sector, int16_t type);

      DOOM::AbstractSector &	sector;	// Reference on sector in which action is performed

      AbstractSectorAction(DOOM::AbstractSector & sector);
      virtual ~AbstractSectorAction();

      virtual void	update(sf::Time elapsed) = 0;	// Update sector's action
    };

    static DOOM::AbstractSector *	factory(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);

    enum Special
    {
      Normal = 0x0000,			// Normal
      LightBlinkRandom = 0x0001,	// Blink random
      LightBlink05 = 0x0002,		// Blink 0.5 second
      LightBlink10 = 0x0003,		// Blink 1.0 second
      Damage20Blink05 = 0x0004,		// 20 % damage every 32 tics plus light blink 0.5 second
      Damage10 = 0x0005,		// 10 % damage every 32 tics
      Damage5 = 0x0007,			// 5 % damage every 32 tics
      LightOscillates = 0x0008,		// Oscillates
      Secret = 0x0009,			// Player entering this sector gets credit for finding a secret
      DoorClose = 0x000A,		// 30 seconds after level start, ceiling closes like a door
      End = 0x000B,			// Cancel God mode if active, 20 % damage every 32 tics, when player dies, level ends
      LightBlink05Sync = 0x000C,	// Blink 0.5 second, synchronized
      LightBlink10Sync = 0x000D,	// Blink 1.0 second, synchronized
      DoorOpen = 0x000E,		// 300 seconds after level start, ceiling opens like a door
      Damage20 = 0x0010,		// 20 % damage per second
      LightFlickers = 0x0011		// Flickers randomly (fire)
    };

    uint64_t			floor_name, ceiling_name;	// Textures names
    const DOOM::AbstractFlat	*floor_flat, *ceiling_flat;	// Textures pointers (null if sky)
    int16_t			tag;				// Sector/linedef tag

    enum Speed
    {
      SpeedSlow = 8,	// Move 8 units/tic
      SpeedNormal = 16,	// Move 16 units/tic
      SpeedFast = 32,	// Move 32 units/tic
      SpeedTurbo = 64	// Move 64 units/tic
    };

  protected:
    int16_t			_light;		// Sector original light level
    float			_floor;		// Sector floor original height
    float			_ceiling;	// Sector ceiling original height
    std::vector<int16_t>	_neighbors;	// List of neighbor sectors
    const DOOM::Doom &		_doom;		// Reference to DOOM instance

  private:
    std::unique_ptr<DOOM::AbstractSector::AbstractSectorAction>	_leveling;	// Current floor/ceiling leveling effect
    std::unique_ptr<DOOM::AbstractSector::AbstractSectorAction>	_lighting;	// Current lighting effect

  public:
    AbstractSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
    virtual ~AbstractSector();

    virtual void	update(sf::Time elapsed) = 0;	// Update sector

    virtual int16_t	light() const;		// Get current sector light level
    virtual float	floor() const;		// Get current floor height
    virtual float	ceiling() const;	// Get current ceiling height
    virtual float	damage() const;		// Get damage par second

    void	action(int16_t type);	// Add action to sector if possible
    
    float	getNeighborLowestFloor() const;		// Get lowest neighbor floor level
    float	getNeighborHighestFloor() const;	// Get highest neighbor floor level
    float	getNeighborNextLowestFloor() const;	// Get next lowest neighbor floor level
    float	getNeighborNextHighestFloor() const;	// Get next highest neighbor floor level

    float	getNeighborLowestCeiling() const;	// Get lowest neighbor floor level
    float	getNeighborHighestCeiling() const;	// Get highest neighbor floor level
    float	getNeighborNextLowestCeiling() const;	// Get next lowest neighbor floor level
    float	getNeighborNextHighestCeiling() const;	// Get next highest neighbor floor level

    int16_t	getNeighborLowestLight() const;		// Get lowest neighbor light level
    int16_t	getNeighborHighestLight() const;	// Get highest neighbor light level
  };
};

#endif