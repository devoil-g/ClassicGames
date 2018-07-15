#include <functional>

#include "Doom/Sector/DoorLevelingAction.hpp"

const int	DOOM::DoorLevelingAction::ForceWait = (int)(sf::seconds(4.f).asMicroseconds() / DOOM::Doom::Tic.asMicroseconds());

DOOM::DoorLevelingAction::DoorLevelingAction(std::list<DOOM::DoorLevelingAction::State> && states, DOOM::DoorLevelingAction::Speed speed, int wait) :
  DOOM::Doom::Level::Sector::AbstractAction(),
  _states(std::move(states)),
  _speed(speed),
  _wait(wait),
  _elapsed(sf::Time::Zero)
{}

DOOM::DoorLevelingAction::~DoorLevelingAction()
{}

void	DOOM::DoorLevelingAction::update(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
{
  while (_states.empty() == false) {
    switch (_states.front())
    {
    case DOOM::DoorLevelingAction::State::StateOpen:
      elapsed = updateOpen(sector, elapsed);
      break;
    case DOOM::DoorLevelingAction::State::StateClose:
      elapsed = updateClose(sector, elapsed);
      break;
    case DOOM::DoorLevelingAction::State::StateForceClose:
      elapsed = updateForceClose(sector, elapsed);
      break;
    case DOOM::DoorLevelingAction::State::StateWait:
      elapsed = updateWait(sector, elapsed);
      break;
    case DOOM::DoorLevelingAction::State::StateForceWait:
      elapsed = updateForceWait(sector, elapsed);
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
    remove(sector);
  }
}

sf::Time	DOOM::DoorLevelingAction::updateOpen(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
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

sf::Time	DOOM::DoorLevelingAction::updateClose(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
{
  // Instant close if under floor
  if (sector.ceiling() < sector.floor()) {
    sector.ceiling() = sector.floor();
    return elapsed;
  }

  // TODO: add check for things in sector
  if (false) {
    _states.push_front(DOOM::DoorLevelingAction::State::StateForceWait);
    _states.push_front(DOOM::DoorLevelingAction::State::StateOpen);
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

sf::Time	DOOM::DoorLevelingAction::updateForceClose(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
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

sf::Time	DOOM::DoorLevelingAction::updateWait(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
{
  _elapsed += elapsed;

  // Return remaining time if any
  return std::max(_elapsed - (DOOM::Doom::Tic * (float)_wait), sf::Time::Zero);
}

sf::Time	DOOM::DoorLevelingAction::updateForceWait(DOOM::Doom::Level::Sector & sector, sf::Time elapsed)
{
  _elapsed += elapsed;

  // Return remaining time if any
  return std::max(_elapsed - (DOOM::Doom::Tic * (float)DOOM::DoorLevelingAction::ForceWait), sf::Time::Zero);
}