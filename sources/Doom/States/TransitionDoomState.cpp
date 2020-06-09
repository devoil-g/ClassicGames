#include "Doom/States/TransitionDoomState.hpp"
#include "System/Window.hpp"

DOOM::TransitionDoomState::TransitionDoomState(Game::StateMachine& machine, const sf::Image& start, const sf::Image& end):
  Game::AbstractState(machine),
  _start(start),
  _end(end),
  _image(),
  _texture(),
  _sprite(),
  _offsets(DOOM::Doom::RenderWidth / 2)
{
  // Check image are valid DOOM ratio
  if ((float)_start.getSize().x / (float)_start.getSize().y != (float)DOOM::Doom::RenderWidth / (float)DOOM::Doom::RenderHeight ||
    (float)_end.getSize().x / (float)_end.getSize().y != (float)DOOM::Doom::RenderWidth / (float)DOOM::Doom::RenderHeight)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  sf::Vector2u  size(std::max(_start.getSize().x, _end.getSize().x), std::max(_start.getSize().y, _end.getSize().y));

  // Initialize graphical components
  _image.create(size.x, size.y);
  _texture.create(size.x, size.y);
  _texture.setSmooth(false);
  _texture.update(_image);
  _sprite.setTexture(_texture);

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
  for (unsigned int x = 0; x < _image.getSize().x; x++) {
    const auto&   offset = _offsets.at(x * _offsets.size() / _image.getSize().x);
    unsigned int  offset_y;

    // Slow start for 16 ticks
    if (offset <= sf::Time::Zero)
      offset_y = 0;
    else if (offset.asSeconds() / DOOM::Doom::Tic.asSeconds() <= 16.f)
      offset_y = (unsigned int)((std::pow((offset.asSeconds() / DOOM::Doom::Tic.asSeconds() / 32.f), 2) * 32.f * 8) * ((float)_image.getSize().y / (float)DOOM::Doom::RenderHeight) + 0.5f);
    else
      offset_y = (unsigned int)(((offset.asSeconds() / DOOM::Doom::Tic.asSeconds()) * 8.f - 64.f) * ((float)_image.getSize().y / (float)DOOM::Doom::RenderHeight) + 0.5f);

    // Draw end image column
    for (unsigned int y = 0; y < std::min(offset_y, _image.getSize().y); y++)
      _image.setPixel(x, y, _end.getPixel(x * _end.getSize().x / _image.getSize().x, y * _end.getSize().y / _image.getSize().y));

    // Draw start image column
    for (unsigned int y = offset_y; y < _image.getSize().y; y++)
      _image.setPixel(x, y, _start.getPixel(x * _start.getSize().x / _image.getSize().x, (y - offset_y) * _start.getSize().y / _start.getSize().y));
  }

  // Update texture
  _texture.update(_image);

  // Compute sprite scale and position
  float	scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)_texture.getSize().x, (float)Game::Window::Instance().window().getSize().y / ((float)_texture.getSize().y * DOOM::Doom::RenderStretching));
  float	pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)_texture.getSize().x * scale)) / 2.f);
  float	pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)_texture.getSize().y * scale * DOOM::Doom::RenderStretching)) / 2.f);

  // Position sprite in window
  _sprite.setScale(sf::Vector2f(scale, scale * DOOM::Doom::RenderStretching));
  _sprite.setPosition(sf::Vector2f(pos_x, pos_y));

  // Draw sprite
  Game::Window::Instance().window().draw(_sprite);
}