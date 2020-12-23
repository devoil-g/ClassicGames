#include "Doom/States/TransitionDoomState.hpp"
#include "System/Window.hpp"

DOOM::TransitionDoomState::TransitionDoomState(Game::StateMachine& machine, DOOM::Doom& doom, const sf::Image& start, const sf::Image& end):
  Game::AbstractState(machine),
  _doom(doom),
  _start(start),
  _end(end),
  _offsets(DOOM::Doom::RenderWidth / 2)
{
  // TODO: support any size

  // Check images are valid DOOM ratio
  if ((float)_start.getSize().x / (float)_start.getSize().y != (float)DOOM::Doom::RenderWidth / (float)DOOM::Doom::RenderHeight ||
    (float)_end.getSize().x / (float)_end.getSize().y != (float)DOOM::Doom::RenderWidth / (float)DOOM::Doom::RenderHeight)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  sf::Vector2u  size(std::max(_start.getSize().x, _end.getSize().x), std::max(_start.getSize().y, _end.getSize().y));

  // Initialize graphical components
  _doom.image.create(size.x, size.y);
  
  // Randomly set timers
  _offsets.front() = DOOM::Doom::Tic * (float)(-std::rand() % 16);
  for (unsigned int i = 1; i < _offsets.size(); i++)
    _offsets[i] = std::clamp(_offsets[i - 1] + (DOOM::Doom::Tic * (float)(std::rand() % 3 - 1)), DOOM::Doom::Tic * -15.f, sf::Time::Zero);
}

bool	DOOM::TransitionDoomState::update(sf::Time elapsed)
{
  bool  done = true;

  // Update offsets
  for (auto& offset : _offsets) {
    offset += elapsed;

    if (offset < DOOM::Doom::Tic * 33.f)
      done = false;
  }

  // Pop transition state when completed
  if (done == true) {
    _machine.pop();
    return false;
  }

  return false;
}

void	DOOM::TransitionDoomState::draw()
{
  // Render transition
  for (unsigned int x = 0; x < _doom.image.getSize().x; x++) {
    const auto&   offset = _offsets.at(x * _offsets.size() / _doom.image.getSize().x);
    unsigned int  offset_y;

    // Slow start for 16 ticks
    if (offset <= sf::Time::Zero)
      offset_y = 0;
    else if (offset.asSeconds() / DOOM::Doom::Tic.asSeconds() <= 16.f)
      offset_y = (unsigned int)((std::pow((offset.asSeconds() / DOOM::Doom::Tic.asSeconds() / 32.f), 2) * 32.f * 8) * ((float)_doom.image.getSize().y / (float)DOOM::Doom::RenderHeight) + 0.5f);
    else
      offset_y = (unsigned int)(((offset.asSeconds() / DOOM::Doom::Tic.asSeconds()) * 8.f - 64.f) * ((float)_doom.image.getSize().y / (float)DOOM::Doom::RenderHeight) + 0.5f);

    // Draw end image column
    for (unsigned int y = 0; y < std::min(offset_y, _doom.image.getSize().y); y++)
      _doom.image.setPixel(x, y, _end.getPixel(x * _end.getSize().x / _doom.image.getSize().x, y * _end.getSize().y / _doom.image.getSize().y));

    // Draw start image column
    for (unsigned int y = offset_y; y < _doom.image.getSize().y; y++)
      _doom.image.setPixel(x, y, _start.getPixel(x * _start.getSize().x / _doom.image.getSize().x, (y - offset_y) * _start.getSize().y / _start.getSize().y));
  }
}