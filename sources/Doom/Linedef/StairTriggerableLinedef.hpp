#ifndef _STAIR_TRIGGERABLE_LINEDEF_HPP_
#define _STAIR_TRIGGERABLE_LINEDEF_HPP_

#include <functional>
#include <set>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Linedef/NullLinedef.hpp"
#include "Doom/Sector/FloorLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumAction::Speed Speed,
    unsigned int Step
  >
  class StairTriggerableLinedef : public DOOM::AbstractLinedef
  {
  private:
    DOOM::Doom &	_doom;	// Reference to game instance

    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	trigger(const DOOM::AbstractThing & thing)	// Does nothing if wrong event triggered
    {}

    inline void	triggerStair(int16_t sector_index)	// Build stairs from sector
    {
      // Does nothing if action already running
      if (_doom.level.sectors[sector_index].leveling().get() != nullptr)
	return;

      int16_t	step_index = sector_index;
      float	step_height = _doom.level.sectors[sector_index].baseFloor();

      // Start stair building from tagged sector
      do
      {
	// Increment step height
	step_height += Step;

	// Start step rising
	_doom.level.sectors[step_index].leveling(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, Speed, DOOM::EnumAction::Crush::CrushFalse>>((float)step_height));
	
	int16_t	step_new = -1;
	for (const std::unique_ptr<DOOM::AbstractLinedef> & linedef : _doom.level.linedefs)
	  if (linedef->front != -1 && linedef->back != -1 &&
	    _doom.level.sidedefs[linedef->front].sector == step_index &&
	    _doom.level.sectors[_doom.level.sidedefs[linedef->back].sector].leveling().get() == nullptr &&
	    _doom.level.sectors[_doom.level.sidedefs[linedef->front].sector].floor_name == _doom.level.sectors[_doom.level.sidedefs[linedef->back].sector].floor_name)
	  {
	    step_new = _doom.level.sidedefs[linedef->back].sector;
	    break;
	  }

	// Continue stair building if new step found
	if (step_new != -1)
	  step_index = step_new;

	// No next step found, interrupt
	else
	  return;

      } while (true);
    }
    
    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	trigger(const DOOM::AbstractThing & thing)	// Trigger event if correct event triggered
    {
      // Trigger tagged sectors
      for (unsigned int sector_index = 0; sector_index < _doom.level.sectors.size(); sector_index++)
	if (_doom.level.sectors[sector_index].tag == tag)
	  triggerStair(sector_index);

      // Replace current sector by a normal sector
      for (std::unique_ptr<DOOM::AbstractLinedef> & linedef : _doom.level.linedefs)
	if (linedef.get() == this) {
	  linedef.reset(new DOOM::NullLinedef(*this));
	  return;
	}

      // Linedef not found
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }

  public:
    StairTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractLinedef(linedef),
      _doom(doom)
    {}

    ~StairTriggerableLinedef()
    {}

    virtual void	update(sf::Time elapsed) override	// Update linedef
    {
      pushed(*_doom.level.things.front().get());
      switched(*_doom.level.things.front().get());
      walkover(*_doom.level.things.front().get());
      gunfire(*_doom.level.things.front().get());
    }

    virtual void	pushed(const DOOM::AbstractThing & thing) override	// To call when linedef is pushed by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerPushed>(thing);
    }

    virtual void	switched(const DOOM::AbstractThing & thing) override	// To call when linedef is switched (used) by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerSwitched>(thing);
    }

    virtual void	walkover(const DOOM::AbstractThing & thing) override	// To call when thing walk over the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerWalkover>(thing);
    }

    virtual void	gunfire(const DOOM::AbstractThing & thing) override	// To call when thing shot the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerGunfire>(thing);
    }
  };
};

#endif