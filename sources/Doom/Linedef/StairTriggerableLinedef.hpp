#ifndef _STAIR_TRIGGERABLE_LINEDEF_HPP_
#define _STAIR_TRIGGERABLE_LINEDEF_HPP_

#include "Doom/Linedef/AbstractTriggerableLinedef.hpp"
#include "Doom/Action/FloorLevelingAction.hpp"

namespace DOOM
{
  template<
    DOOM::EnumLinedef::Trigger Trigger,
    DOOM::EnumAction::Speed Speed,
    unsigned int Step
  >
  class StairTriggerableLinedef : public DOOM::AbstractTriggerableLinedef<Trigger, DOOM::EnumLinedef::Repeat::RepeatFalse>
  {
  private:
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
	doom.level.sectors[step_index].action<DOOM::EnumAction::Type::TypeLeveling>(std::make_unique<DOOM::FloorLevelingAction<DOOM::EnumAction::Direction::DirectionUp, Speed>>(doom, (float)step_height));
	
	int16_t	step_new = -1;
	for (const std::unique_ptr<DOOM::AbstractLinedef> & linedef : doom.level.linedefs)
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

    void	trigger(DOOM::Doom & doom, DOOM::AbstractThing & thing) override	// Trigger stair builder on tagged event
    {
      // Handle error
      if (tag == 0)
      {
	std::cerr << "[StairTriggerableLinedef:trigger] Warning, invalid pushed tag (" << tag << ")." << std::endl;
	return;

	// TODO: solve problem of level 20
	throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      }

      // Trigger tagged sectors
      for (unsigned int sector_index = 0; sector_index < doom.level.sectors.size(); sector_index++)
	if (doom.level.sectors[sector_index].tag == tag)
	  triggerStair(doom, sector_index);
    }

  public:
    StairTriggerableLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef) :
      DOOM::AbstractTriggerableLinedef<Trigger, DOOM::EnumLinedef::Repeat::RepeatFalse>(doom, linedef)
    {}

    ~StairTriggerableLinedef() = default;
  };
};

#endif