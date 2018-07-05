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
  // Handlers map
  static const std::unordered_map < DOOM::DoorAction::State, std::function<sf::Time(DOOM::DoorAction *, sf::Time)>>	handlers =
  {
    { StateOpen, &DOOM::DoorAction::updateOpen },
    { StateClose, &DOOM::DoorAction::updateClose },
    { StateForceClose, &DOOM::DoorAction::updateForceClose },
    { StateWait, &DOOM::DoorAction::updateWait },
    { StateForceWait, &DOOM::DoorAction::updateForceWait }
  };

  while (_states.empty() == false) {
    std::unordered_map<DOOM::DoorAction::State, std::function<sf::Time(DOOM::DoorAction *, sf::Time)>>::const_iterator	handler = handlers.find(_states.front());

    // Handle error (should not happen)
    if (handler == handlers.end())
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Call state handler
    elapsed = handler->second(this, elapsed);

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