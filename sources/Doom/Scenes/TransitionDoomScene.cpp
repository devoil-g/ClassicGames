#include "Doom/Scenes/TransitionDoomScene.hpp"
#include "System/Window.hpp"

DOOM::TransitionDoomScene::TransitionDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, const sf::Image& start, const sf::Image& end):
  Game::AbstractScene(machine),
  _doom(doom),
  _startImage(start),
  _endTexture(),
  _endSprite(),
  _transitionImage(),
  _transitionTexture(),
  _offsets(DOOM::Doom::RenderWidth / 2)
{
  // Randomly set timers
  _offsets.front() = DOOM::Doom::Tic * (float)(-std::rand() % 16);
  for (unsigned int i = 1; i < _offsets.size(); i++)
    _offsets[i] = std::clamp(_offsets[i - 1] + (DOOM::Doom::Tic * (float)(std::rand() % 3 - 1)), DOOM::Doom::Tic * -15.f, sf::Time::Zero);
  
  float scale_x = (float)_startImage.getSize().x / (float)Game::Window::Instance().window().getSize().x;
  float scale_y = (float)_startImage.getSize().y / (float)Game::Window::Instance().window().getSize().y * DOOM::Doom::RenderStretching;

  // Scale transition image
  if (scale_x < scale_y)
    _transitionImage.create((unsigned int)(_startImage.getSize().x / scale_x * scale_y), _startImage.getSize().y);
  else
    _transitionImage.create(_startImage.getSize().x, (unsigned int)(_startImage.getSize().y / scale_y * scale_x));

  // Create textures
  if (_endTexture.create(end.getSize().x, end.getSize().y) == false ||
    _transitionTexture.create(_transitionImage.getSize().x, _transitionImage.getSize().y) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set texture parameters
  _endTexture.setSmooth(false);
  _transitionTexture.setSmooth(false);

  // Update end image
  _endTexture.update(end);

  // Set sprites
  _endSprite.setTexture(_endTexture, true);
  _transitionSprite.setTexture(_transitionTexture, true);

  // Remove DOOM base rendering target
  _doom.image.create(0, 0);
}

bool  DOOM::TransitionDoomScene::update(sf::Time elapsed)
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

void  DOOM::TransitionDoomScene::draw()
{
  //  Render transition
  for (unsigned int x = 0; x < _transitionImage.getSize().x; x++) {
    const auto&   offset = _offsets.at(x * _offsets.size() / _transitionImage.getSize().x);
    unsigned int  offset_y;

    // Slow start for 16 ticks
    if (offset <= sf::Time::Zero)
      offset_y = 0;
    else if (offset.asSeconds() / DOOM::Doom::Tic.asSeconds() <= 16.f)
      offset_y = (unsigned int)((Math::Pow<2>((offset.asSeconds() / DOOM::Doom::Tic.asSeconds() / 32.f)) * 32.f * 8) * ((float)_transitionImage.getSize().y / (float)DOOM::Doom::RenderHeight) + 0.5f);
    else
      offset_y = (unsigned int)(((offset.asSeconds() / DOOM::Doom::Tic.asSeconds()) * 8.f - 64.f) * ((float)_transitionImage.getSize().y / (float)DOOM::Doom::RenderHeight) + 0.5f);

    // Draw transparent column
    for (unsigned int y = 0; y < std::min(offset_y, _transitionImage.getSize().y); y++)
      _transitionImage.setPixel(x, y, sf::Color(0, 0, 0, 0));

    // Draw start image column
    for (unsigned int y = offset_y; y < _transitionImage.getSize().y; y++) {
      if (x < (_transitionImage.getSize().x - _startImage.getSize().x) / 2 || x >= _startImage.getSize().x + (_transitionImage.getSize().x - _startImage.getSize().x) / 2 ||
        y - offset_y < (_transitionImage.getSize().y - _startImage.getSize().y) / 2 || y - offset_y >= _startImage.getSize().y + (_transitionImage.getSize().y - _startImage.getSize().y) / 2)
        _transitionImage.setPixel(x, y, sf::Color::Black);
      else
        _transitionImage.setPixel(x, y, _startImage.getPixel(x - (_transitionImage.getSize().x - _startImage.getSize().x) / 2, (y - offset_y) - (_transitionImage.getSize().y - _startImage.getSize().y) / 2));
    }
  }

  // Update transition texture
  _transitionTexture.update(_transitionImage);

  // Draw sprites
  Game::Window::Instance().draw(_endSprite, DOOM::Doom::RenderStretching);
  Game::Window::Instance().draw(_transitionSprite, DOOM::Doom::RenderStretching);
}