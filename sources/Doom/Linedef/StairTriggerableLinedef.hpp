#ifndef _STAIR_TRIGGERABLE_LINEDEF_HPP_
#define _STAIR_TRIGGERABLE_LINEDEF_HPP_

#include "Doom/Doom.hpp"
#include "Doom/Linedef/NullLinedef.hpp"
#include "Doom/Sector/FloorLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumAction::Speed Speed,
    unsigned int Step
  >
  class StairTriggerableLinedef : public DOOM::Doom::Level::AbstractLinedef
  {
  private:
    template<DOOM::EnumLinedef::Trigger _Trigger>
    inline std::enable_if_t<(Trigger & _Trigger) == 0>	trigger(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)	// Does nothing if wrong event triggered
    {}

    inline void	triggerStair(DOOM::Doom & doom, int16_t sector_index)	// Build stairs from sector
    {
      // Does nothing if action already running
      if (doom.level.sectors[sector_index].action<DOOM::EnumAction::Type::TypeLeveling>().get() != nullptr)
	return;

      int16_t	step_index = sector_index;
      float	step_height = doom.level.sectors[sector_index].floor_base;

      // Start stair building from tagged sector
      do
      {
	// Increment step height
	step_height += Step;

	// Start step rising
	doom.level.sectors[step_index].action<DOOM::EnumAction::Type::TypeLeveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, Speed, DOOM::EnumAction::Crush::CrushFalse>>(doom, (float)step_height));
	
	int16_t	step_new = -1;
	for (const std::unique_ptr<DOOM::Doom::Level::AbstractLinedef> & linedef : doom.level.linedefs)
	  if (linedef->front != -1 && linedef->back != -1 &&
	    doom.level.sidedefs[linedef->front].sector == step_index &&
	    doom.level.sectors[doom.level.sidedefs[linedef->back].sector].action<DOOM::EnumAction::Type::TypeLeveling>().get() == nullptr &&
	    doom.level.sectors[doom.level.sidedefs[linedef->front].sector].floor_name == doom.level.sectors[doom.level.sidedefs[linedef->back].sector].floor_name)
	  {
	    step_new = doom.level.sidedefs[linedef->back].sector;
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
    inline std::enable_if_t<(Trigger & _Trigger) != 0>	trigger(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing)	// Trigger event if correct event triggered
    {
      // Trigger tagged sectors
      for (unsigned int sector_index = 0; sector_index < doom.level.sectors.size(); sector_index++)
	if (doom.level.sectors[sector_index].tag == tag)
	  triggerStair(doom, sector_index);

      // Replace current sector by a normal sector
      for (std::unique_ptr<DOOM::Doom::Level::AbstractLinedef> & linedef : doom.level.linedefs)
	if (linedef.get() == this) {
	  linedef = std::move(std::make_unique<DOOM::NullLinedef>(doom, *this));
	  return;
	}

      // Linedef not found
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }

  public:
    StairTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::Doom::Level::AbstractLinedef(doom, linedef)
    {}

    ~StairTriggerableLinedef() = default;

    virtual void	update(DOOM::Doom & doom, sf::Time elapsed) override	// Update linedef
    {
      // TODO: remove this (note: may crash because of deleted instance)
      pushed(doom, *doom.level.things.front().get());
      switched(doom, *doom.level.things.front().get());
      walkover(doom, *doom.level.things.front().get());
      gunfire(doom, *doom.level.things.front().get());
    }

    virtual void	pushed(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing) override	// To call when linedef is pushed by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerPushed>(doom, thing);
    }

    virtual void	switched(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing) override	// To call when linedef is switched (used) by thing
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerSwitched>(doom, thing);
    }

    virtual void	walkover(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing) override	// To call when thing walk over the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerWalkover>(doom, thing);
    }

    virtual void	gunfire(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing) override	// To call when thing shot the linedef
    {
      trigger<DOOM::EnumLinedef::Trigger::TriggerGunfire>(doom, thing);
    }
  };
};

#endif