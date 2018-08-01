#ifndef _CRUSHER_LEVELING_ACTION_HPP_
#define _CRUSHER_LEVELING_ACTION_HPP_

#include "Doom/Action/AbstractStoppableAction.hpp"

namespace DOOM
{
  template<DOOM::EnumAction::Speed Speed, DOOM::EnumAction::Silent Silent = DOOM::EnumAction::Silent::SilentFalse>
  class CrusherLevelingAction : public DOOM::AbstractStoppableAction<DOOM::EnumAction::Type::TypeLeveling>
  {
  private:
    DOOM::EnumAction::CrusherState	_state;		// Crusher current state

    sf::Time	updateRaise(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
    {
      // Raise crusher
      sector.ceiling_current += elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // Compute exceding time
      if (sector.ceiling_current > sector.ceiling_base) {
	sf::Time	exceding = std::min(sf::seconds((sector.ceiling_current - sector.ceiling_base) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	sector.ceiling_current = sector.ceiling_base;
	_state = DOOM::EnumAction::CrusherState::CrusherStateLower;
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

    sf::Time	updateLower(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
    {
      // Lower crusher
      sector.ceiling_current -= elapsed.asSeconds() * Speed / DOOM::Doom::Tic.asSeconds();

      // TODO: handle collision

      // Compute exceding time
      if (sector.ceiling_current < sector.floor_base + 8.f) {
	sf::Time	exceding = std::min(sf::seconds((sector.floor_base + 8.f - sector.ceiling_current) / Speed * DOOM::Doom::Tic.asSeconds()), elapsed);

	sector.ceiling_current = sector.floor_base + 8.f;
	_state = DOOM::EnumAction::CrusherState::CrusherStateRaise;
	return exceding;
      }
      else {
	return sf::Time::Zero;
      }
    }

  public:
    CrusherLevelingAction(DOOM::Doom & doom) :
      DOOM::AbstractStoppableAction<DOOM::EnumAction::Type::TypeLeveling>(doom),
      _state(DOOM::EnumAction::CrusherState::CrusherStateLower)
    {}

    ~CrusherLevelingAction() override = default;

    void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override	// Update door action
    {
      // Stop action if requested
      if (_stop == true) {
	remove(sector);
	return;
      }

      // Update action states
      while (elapsed != sf::Time::Zero) {
	switch (_state)
	{
	case DOOM::EnumAction::CrusherState::CrusherStateRaise:
	  elapsed = updateRaise(doom, sector, elapsed);
	  break;
	case DOOM::EnumAction::CrusherState::CrusherStateLower:
	  elapsed = updateLower(doom, sector, elapsed);
	  break;
	
	default:	// Handle error (should not happen)
	  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
	}
      }
    }
  };
};

#endif