#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Thing/AbstractThing.hpp"
#include "States/DoomState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::DoomState::DoomState() :
  _elapsed(sf::seconds(0)),
  _doom(),
  _camera()
{
  // Load a level
  _doom.load(Game::Config::ExecutablePath + "/assets/levels/doom.wad");

  // Set first level
  if (_doom.getLevel().empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _doom.setLevel(_doom.getLevel().front());

  // Initialize camera position
  _camera.position = Math::Vector<2>(0.f, 0.f);
  _camera.height = 0.f;
  _camera.angle = 0.f;

  // Set player initial position
  for (const std::unique_ptr<DOOM::AbstractThing> & thing : _doom.level.things)
    if (thing->type == 1)
    {
      _camera.position = thing->position;
      _camera.height = _doom.level.sectors[_doom.level.sector(thing->position).first].floor() + 32.f;
      _camera.angle = thing->angle;
      break;
    }
}

Game::DoomState::~DoomState()
{}

bool	Game::DoomState::update(sf::Time elapsed)
{
  // Update time
  _elapsed += elapsed;

  // Return to previous menu
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Right) ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) ||
    Game::Window::Instance().joystick().buttonPressed(0, 1))
  {
    Game::StateMachine::Instance().pop();
    return false;
  }

  // Update player informations
  if (updatePlayer(elapsed) == true)
    return false;

  // Update game components
  _doom.update(elapsed);

  return false;
}

bool	Game::DoomState::updatePlayer(sf::Time elapsed)
{
  // Arbitrary values
  float const	speed = 256.f;
  float const	turn = Math::DegToRad(128.f);

  // Move player vertically
  _camera.height += Game::Window::Instance().keyboard().key(sf::Keyboard::Space) ? +speed * elapsed.asSeconds() : 0;
  _camera.height += Game::Window::Instance().keyboard().key(sf::Keyboard::LControl) ? -speed * elapsed.asSeconds() : 0;

  // Turn player
  _camera.angle += Game::Window::Instance().keyboard().key(sf::Keyboard::Left) ? +turn * elapsed.asSeconds() : 0;
  _camera.angle += Game::Window::Instance().keyboard().key(sf::Keyboard::Right) ? -turn * elapsed.asSeconds() : 0;

  Math::Vector<2>	d;

  // Accumulate player movement
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Q)) d.y() += +speed * elapsed.asSeconds();
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::D)) d.y() += -speed * elapsed.asSeconds();
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Z)) d.x() += +speed * elapsed.asSeconds();
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::S)) d.x() += -speed * elapsed.asSeconds();
  
  // Apply movement vector with rotation
  _camera.position.x() += std::cos(_camera.angle) * d.x() - std::sin(_camera.angle) * d.y();
  _camera.position.y() += std::sin(_camera.angle) * d.x() + std::cos(_camera.angle) * d.y();

  // Change field of view angle
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::O)) _camera.fov += Math::DegToRad(+20.f) * elapsed.asSeconds();
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::L)) _camera.fov += Math::DegToRad(-20.f) * elapsed.asSeconds();
  _camera.fov = std::clamp(_camera.fov, Math::DegToRad(15.f), Math::DegToRad(125.f));
  
  // Change player camera vertical direction
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Up)) _camera.orientation += +turn * elapsed.asSeconds();
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Down)) _camera.orientation += -turn * elapsed.asSeconds();
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Up) == false && Game::Window::Instance().keyboard().key(sf::Keyboard::Down) == false)
    _camera.orientation -= _camera.orientation * std::min(elapsed.asSeconds() * 4.2f, 1.f);
  _camera.orientation = std::clamp(_camera.orientation, Math::DegToRad(-30.f), Math::DegToRad(+30.f));
  
  // Change current level
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::P) || Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::M))
  {
    std::list<std::pair<uint8_t, uint8_t>>	levels = _doom.getLevel();
    std::pair<uint8_t, uint8_t>			next =
      Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::P) == true ?
      (std::next(std::find(levels.begin(), levels.end(), _doom.level.episode)) != levels.end() ?
	*std::next(std::find(levels.begin(), levels.end(), _doom.level.episode)) :
	levels.front())
      :
      (std::find(levels.begin(), levels.end(), _doom.level.episode) != levels.begin() ?
	*std::prev(std::find(levels.begin(), levels.end(), _doom.level.episode)) :
	levels.back());

    Game::StateMachine::Instance().push(new Game::LoadingState(std::async(std::launch::async, [this, next]
    {
      // Load new level
      try
      {
	_doom.setLevel(next);
      }
      catch (std::exception)
      {
	return static_cast<Game::AbstractState *>(nullptr);
      }

      // Set new camera position
      for (const std::unique_ptr<DOOM::AbstractThing> & thing : _doom.level.things)
	if (thing->type == 1)
	{
	  _camera.position = thing->position;
	  _camera.height = _doom.level.sectors[_doom.level.sector(thing->position).first].floor() + 32.f;
	  _camera.angle = thing->angle;
	  break;
	}
      
      return static_cast<Game::AbstractState *>(nullptr);
    })));

    return true;
  }
  
  return false;
}

void	Game::DoomState::draw()
{
  // TODO: compute image
  drawCamera();

  // Draw image to screen
  drawImage();
}

void	Game::DoomState::drawCamera()
{
  // Draw cameras
  _camera.render(_doom);
}

void	Game::DoomState::drawImage()
{
  // Load texture as sprite
  float	scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)_camera.image().getSize().x, (float)Game::Window::Instance().window().getSize().y / (float)_camera.image().getSize().y);
  float	pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)_camera.image().getSize().x * scale)) / 2.f);
  float	pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)_camera.image().getSize().y * scale)) / 2.f);

  // Position sprite in window
  _camera.sprite().setScale(sf::Vector2f(scale, scale));
  _camera.sprite().setPosition(sf::Vector2f(pos_x, pos_y));

  // Draw sprite
  Game::Window::Instance().window().draw(_camera.sprite());
}
