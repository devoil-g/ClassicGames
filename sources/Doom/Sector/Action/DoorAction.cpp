#include <functional>

#include "Doom/Sector/Action/DoorAction.hpp"

const int	DOOM::DoorAction::ForceWait = (int)(sf::seconds(4.f).asMicroseconds() / DOOM::Doom::Tic.asMicroseconds());

DOOM::DoorAction::DoorAction(DOOM::AbstractSector & sector, std::list<DOOM::DoorAction::State> && states, DOOM::DoorAction::Speed speed, int wait) :
  DOOM::AbstractSector::AbstractAction(sector),
  _states(std::move(states)),
  _speed(speed),
  _wait(wait),
  _elapsed(sf::Time::Zero)
{}

DOOM::DoorAction::~DoorAction()
{}

void	DOOM::DoorAction::update(sf::Time elapsed)
{
  while (_states.empty() == false) {
    switch (_states.front())
    {
    case DOOM::DoorAction::State::StateOpen:
      elapsed = updateOpen(elapsed);
      break;
    case DOOM::DoorAction::State::StateClose:
      elapsed = updateClose(elapsed);
      break;
    case DOOM::DoorAction::State::StateForceClose:
      elapsed = updateForceClose(elapsed);
      break;
    case DOOM::DoorAction::State::StateWait:
      elapsed = updateWait(elapsed);
      break;
    case DOOM::DoorAction::State::StateForceWait:
      elapsed = updateForceWait(elapsed);
      break;
    default:	// Handle error (should not happen)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }
    
    // Stop if no more elapsed time
    if (elapsed == sf::Time::Zero)
      break;

    // Pop ended state
    _states.pop_front();
    _elapsed = sf::Time::Zero;
  }

  // Update ceiling base value when animation ended and remove action from sector
  if (_states.empty() == true) {
    sector.baseCeiling() = sector.ceiling();
    remove();
  }
}

sf::Time	DOOM::DoorAction::updateOpen(sf::Time elapsed)
{
  const float	top = sector.getNeighborLowestCeiling() - 4.f;

  // Instant open if above top
  if (sector.ceiling() > top) {
    sector.ceiling() = top;
    return elapsed;
  }

  // Raise door
  sector.ceiling() += elapsed.asSeconds() * _speed / DOOM::Doom::Tic.asSeconds();

  // Compute exceding time
  if (sector.ceiling() > top) {
    sf::Time	exceding = sf::seconds((sector.ceiling() - top) / _speed * DOOM::Doom::Tic.asSeconds());

    sector.ceiling() = top;
    return exceding;
  }
  else {
    return sf::Time::Zero;
  }
}

sf::Time	DOOM::DoorAction::updateClose(sf::Time elapsed)
{
  // Instant close if under floor
  if (sector.ceiling() < sector.floor()) {
    sector.ceiling() = sector.floor();
    return elapsed;
  }

  // TODO: add check for things in sector
  if (false) {
    _states.push_front(DOOM::DoorAction::State::StateForceWait);
    _states.push_front(DOOM::DoorAction::State::StateOpen);
    return elapsed;
  }

  // Lower door
  sector.ceiling() -= elapsed.asSeconds() * _speed / DOOM::Doom::Tic.asSeconds();

  // Compute exceding time
  if (sector.ceiling() < sector.floor()) {
    sf::Time	exceding = sf::seconds((sector.floor() - sector.ceiling()) / _speed * DOOM::Doom::Tic.asSeconds());

    sector.ceiling() = sector.floor();
    return exceding;
  }
  else {
    return sf::Time::Zero;
  }
}

sf::Time	DOOM::DoorAction::updateForceClose(sf::Time elapsed)
{
  // Instant close if under floor
  if (sector.ceiling() < sector.floor()) {
    sector.ceiling() = sector.floor();
    return elapsed;
  }

  // TODO: add check for things in sector
  if (false) {
    return elapsed;
  }

  // Lower door
  sector.ceiling() -= elapsed.asSeconds() * _speed / DOOM::Doom::Tic.asSeconds();

  // Compute exceding time
  if (sector.ceiling() < sector.floor()) {
    sf::Time	exceding = sf::seconds((sector.floor() - sector.ceiling()) / _speed * DOOM::Doom::Tic.asSeconds());

    sector.ceiling() = sector.floor();
    return exceding;
  }
  else {
    return sf::Time::Zero;
  }
}

sf::Time	DOOM::DoorAction::updateWait(sf::Time elapsed)
{
  _elapsed += elapsed;

  // Return remaining time if any
  return std::max(_elapsed - (DOOM::Doom::Tic * (float)_wait), sf::Time::Zero);
}

sf::Time	DOOM::DoorAction::updateForceWait(sf::Time elapsed)
{
  _elapsed += elapsed;

  // Return remaining time if any
  return std::max(_elapsed - (DOOM::Doom::Tic * (float)DOOM::DoorAction::ForceWait), sf::Time::Zero);
}