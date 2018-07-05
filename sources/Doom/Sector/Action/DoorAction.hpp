#ifndef _DOOR_ACTION_HPP_
#define _DOOR_ACTION_HPP_

#include <list>

#include <SFML/System/Time.hpp>

#include "Doom/Sector/AbstractSector.hpp"

namespace DOOM
{
  class DoorAction : public DOOM::AbstractSector::AbstractAction
  {
  public:
    enum Speed
    {
      SpeedSlow = 1,	// Move 8 units/tic
      SpeedNormal = 2,	// Move 16 units/tic
      SpeedFast = 4,	// Move 32 units/tic
      SpeedTurbo = 8	// Move 64 units/tic
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

    std::list<DOOM::DoorAction::State>	_states;	// Door states to perform
    const DOOM::DoorAction::Speed	_speed;		// Speed of door
    const int				_wait;		// Tic to wait
    sf::Time				_elapsed;	// Time elapsed since begining of state

  protected:
    sf::Time	updateOpen(sf::Time elapsed);		// Update an openning door. Return exceding time.
    sf::Time	updateClose(sf::Time elapsed);		// Update a closing door. Return exceding time.
    sf::Time	updateForceClose(sf::Time elapsed);	// Update a closing door. Don't bounce on player head Return exceding time.
    sf::Time	updateWait(sf::Time elapsed);		// Update a waiting door. Return exceding time.
    sf::Time	updateForceWait(sf::Time elapsed);	// Update a waiting door. Wait for a fixed time. Return exceding time.

  public:
    DoorAction(DOOM::AbstractSector & sector, std::list<DOOM::DoorAction::State> && states, DOOM::DoorAction::Speed speed = DOOM::DoorAction::Speed::SpeedNormal, int wait = 0);
    ~DoorAction();

    void	update(sf::Time elapsed);	// Update door action
  };
};

#endif