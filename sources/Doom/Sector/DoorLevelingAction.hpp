#ifndef _DOOR_LEVELING_ACTION_HPP_
#define _DOOR_LEVELING_ACTION_HPP_

#include <list>

#include <SFML/System/Time.hpp>

#include "Doom/Doom.hpp"

namespace DOOM
{
  class DoorLevelingAction : public DOOM::Doom::Level::Sector::AbstractAction
  {
  public:
    enum Speed
    {
      SpeedSlow = 1,	// Move 1 units/tic
      SpeedNormal = 2,	// Move 2 units/tic
      SpeedFast = 4,	// Move 4 units/tic
      SpeedTurbo = 8	// Move 8 units/tic
    };

    enum State
    {
      StateOpen,	// Open the door
      StateClose,	// Close the door
      StateForceClose,	// Close the door without bouncing on thing
      StateWait,	// Wait for the spcified time
      StateForceWait	// Wait for a fixed time
    };

  private:
    static const int	ForceWait;	// Tic for a force wait

    std::list<DOOM::DoorLevelingAction::State>	_states;	// Door states to perform
    const DOOM::DoorLevelingAction::Speed	_speed;		// Speed of door
    const int					_wait;		// Tic to wait
    sf::Time					_elapsed;	// Time elapsed since begining of state

    sf::Time	updateOpen(DOOM::Doom::Level::Sector & sector, sf::Time elapsed);	// Update an openning door. Return exceding time.
    sf::Time	updateClose(DOOM::Doom::Level::Sector & sector, sf::Time elapsed);	// Update a closing door. Return exceding time.
    sf::Time	updateForceClose(DOOM::Doom::Level::Sector & sector, sf::Time elapsed);	// Update a closing door. Don't bounce on player. head Return exceding time.
    sf::Time	updateWait(DOOM::Doom::Level::Sector & sector, sf::Time elapsed);	// Update a waiting door. Return exceding time.
    sf::Time	updateForceWait(DOOM::Doom::Level::Sector & sector, sf::Time elapsed);	// Update a waiting door. Wait for a fixed time. Return exceding time.

  public:
    DoorLevelingAction(std::list<DOOM::DoorLevelingAction::State> && states, DOOM::DoorLevelingAction::Speed speed = DOOM::DoorLevelingAction::Speed::SpeedNormal, int wait = 0);
    ~DoorLevelingAction() override;

    void	update(DOOM::Doom::Level::Sector & sector, sf::Time elapsed) override;	// Update door action
  };
};

#endif