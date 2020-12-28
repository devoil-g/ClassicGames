#include "Doom/States/TransitionDoomState.hpp"
#include "System/Window.hpp"

DOOM::TransitionDoomState::TransitionDoomState(Game::StateMachine& machine, DOOM::Doom& doom, const sf::Image& start, const sf::Image& end):
  Game::AbstractState(machine),
  _doom(doom),
  _start(start),
  _end(end),
  _transition(),
  _offsets(DOOM::Doom::RenderWidth / 2)
{
  // Check images are valid DOOM ratio
  if (_start.getSize().x % DOOM::Doom::RenderWidth != 0 ||
    _end.getSize().x % DOOM::Doom::RenderWidth != 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

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

void    DOOM::TransitionDoomState::draw()
{
  // Remove DOOM base rendering target
  _doom.image.create(0, 0);

  float scale_x = (float)_start.getSize().x / (float)Game::Window::Instance().window().getSize().x;
  float scale_y = (float)_start.getSize().y / (float)Game::Window::Instance().window().getSize().y * DOOM::Doom::RenderStretching;

  // Scale transition image
  if (scale_x < scale_y)
    _transition.create((unsigned int)(_start.getSize().x / scale_x * scale_y), _start.getSize().y);
  else
    _transition.create(_start.getSize().x, (unsigned int)(_start.getSize().y / scale_y * scale_x));

  //  Render transition
  for (unsigned int x = 0; x < _transition.getSize().x; x++) {
    const auto& offset = _offsets.at(x * _offsets.size() / _transition.getSize().x);
    unsigned int  offset_y;

    // Slow start for 16 ticks
    if (offset <= sf::Time::Zero)
      offset_y = 0;
    else if (offset.asSeconds() / DOOM::Doom::Tic.asSeconds() <= 16.f)
      offset_y = (unsigned int)((std::pow((offset.asSeconds() / DOOM::Doom::Tic.asSeconds() / 32.f), 2) * 32.f * 8) * ((float)_transition.getSize().y / (float)DOOM::Doom::RenderHeight) + 0.5f);
    else
      offset_y = (unsigned int)(((offset.asSeconds() / DOOM::Doom::Tic.asSeconds()) * 8.f - 64.f) * ((float)_transition.getSize().y / (float)DOOM::Doom::RenderHeight) + 0.5f);

    // Draw transparent column
    for (unsigned int y = 0; y < std::min(offset_y, _transition.getSize().y); y++)
      _transition.setPixel(x, y, sf::Color(0, 0, 0, 0));

    // Draw start image column
    for (unsigned int y = offset_y; y < _transition.getSize().y; y++) {
      if (x < (_transition.getSize().x - _start.getSize().x) / 2 || x >= _start.getSize().x + (_transition.getSize().x - _start.getSize().x) / 2 ||
        y - offset_y < (_transition.getSize().y - _start.getSize().y) / 2 || y - offset_y >= _start.getSize().y + (_transition.getSize().y - _start.getSize().y) / 2)
        _transition.setPixel(x, y, sf::Color::Black);
      else
        _transition.setPixel(x, y, _start.getPixel(x - (_transition.getSize().x - _start.getSize().x) / 2, (y - offset_y) - (_transition.getSize().y - _start.getSize().y) / 2));
    }
  }

  // Draw images
  drawImage(_end);
  drawImage(_transition);
}

void    DOOM::TransitionDoomState::drawImage(sf::Image& image)
{
  static sf::Texture texture;
  static sf::Sprite  sprite;

  // Update texture on VRam
  if (texture.create(image.getSize().x, image.getSize().y) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  texture.update(image);

  // Making sure the texture is not filtered
  texture.setSmooth(false);

  // Update sprite texture
  sprite.setTexture(texture, true);

  // Compute sprite scale and position
  float	scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)image.getSize().x, (float)Game::Window::Instance().window().getSize().y / ((float)image.getSize().y * DOOM::Doom::RenderStretching));
  float	pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)image.getSize().x * scale)) / 2.f);
  float	pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)image.getSize().y * scale * DOOM::Doom::RenderStretching)) / 2.f);

  // Position sprite in window
  sprite.setScale(sf::Vector2f(scale, scale * DOOM::Doom::RenderStretching));
  sprite.setPosition(sf::Vector2f(pos_x, pos_y));

  // Draw DOOM rendering target
  Game::Window::Instance().window().draw(sprite);
}