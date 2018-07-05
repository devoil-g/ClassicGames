#ifndef _ABSTRACT_SECTOR_HPP_
#define _ABSTRACT_SECTOR_HPP_

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Flat/AbstractFlat.hpp"

namespace DOOM
{
  // TODO: remove sector abstraction
  class AbstractSector
  {
  public:
    class AbstractAction
    {
    public:
      static const std::vector<int16_t>	LevelingTypes;
      static const std::vector<int16_t>	LightingTypes;

      static DOOM::AbstractSector::AbstractAction *	factory(DOOM::AbstractSector & sector, int16_t type);

    protected:
      DOOM::AbstractSector &	sector;	// Reference on sector in which action is performed

    public:
      AbstractAction(DOOM::AbstractSector & sector);
      virtual ~AbstractAction() = 0;

      virtual void	update(sf::Time elapsed) = 0;	// Update sector's action
      void		remove();			// Remove action from sector
    };

    friend AbstractAction;
    
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

  protected:
    int16_t	_light, _baseLight;	// Sector base and current light level
    float	_floor, _baseFloor;	// Sector base and current floor height
    float	_ceiling, _baseCeiling;	// Sector base and current ceiling height
    float	_damage, _baseDamage;	// Sector base and current damage/sec

    std::vector<int16_t>	_neighbors;	// List of neighbor sectors
    const DOOM::Doom &		_doom;		// Reference to DOOM instance

  private:
    std::unique_ptr<DOOM::AbstractSector::AbstractAction>	_leveling;	// Current floor/ceiling leveling effect
    std::unique_ptr<DOOM::AbstractSector::AbstractAction>	_lighting;	// Current lighting effect

  public:
    AbstractSector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
    virtual ~AbstractSector() = 0;

    virtual void	update(sf::Time elapsed);	// Update sector

    // Const getter (TODO: light)
    inline virtual int16_t	light() const { return _light; }
    inline int16_t	baseLight() const { return _baseLight; }
    inline float	floor() const { return _floor; }
    inline float	baseFloor() const { return _baseFloor; }
    inline float	ceiling() const { return _ceiling; }
    inline float	baseCeiling() const { return _baseCeiling; }
    inline float	damage() const { return _damage; }
    inline float	baseDamage() const { return _baseDamage; }

    // Setter (TODO: light)
    // inline int16_t	light() { return _light; }
    inline int16_t &	baseLight() { return _baseLight; }
    inline float &	floor() { return _floor; }
    inline float &	baseFloor() { return _baseFloor; }
    inline float &	ceiling() { return _ceiling; }
    inline float &	baseCeiling() { return _baseCeiling; }
    inline float &	damage() { return _damage; }
    inline float &	baseDamage() { return _baseDamage; }

    void	action(int16_t type);	// Add action to sector if possible
    
    float	getNeighborLowestFloor() const;				// Get lowest neighbor floor level
    float	getNeighborHighestFloor() const;			// Get highest neighbor floor level
    float	getNeighborNextLowestFloor(float height) const;		// Get next lowest neighbor floor level from height
    float	getNeighborNextHighestFloor(float height) const;	// Get next highest neighbor floor level from height

    float	getNeighborLowestCeiling() const;			// Get lowest neighbor floor level
    float	getNeighborHighestCeiling() const;			// Get highest neighbor floor level
    float	getNeighborNextLowestCeiling(float height) const;	// Get next lowest neighbor floor level from height
    float	getNeighborNextHighestCeiling(float height) const;	// Get next highest neighbor floor level from height

    int16_t	getNeighborLowestLight() const;		// Get lowest neighbor light level
    int16_t	getNeighborHighestLight() const;	// Get highest neighbor light level
  };
};

#endif