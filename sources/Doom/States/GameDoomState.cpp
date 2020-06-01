#include <future>
#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Thing/PlayerThing.hpp"
#include "Doom/States/GameDoomState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

#include "System/Sound.hpp"

unsigned int		DOOM::GameDoomState::RenderScale = 1;

DOOM::GameDoomState::GameDoomState(Game::StateMachine& machine, DOOM::Doom& doom) :
  Game::AbstractState(machine),
  _doom(doom),
  _image(),
  _texture(),
  _sprite()
{
  // Set initial level
  _doom.setLevel({ 1, 3 });
}

bool	DOOM::GameDoomState::update(sf::Time elapsed)
{
  // Return to previous menu
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) == true)
  {
    _machine.pop();
    return false;
  }

  // Add new players (keyboard)
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true || 
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true)
    _doom.addPlayer(0);

  // Add new players (joystick)
  for (unsigned int id = 0; id < sf::Joystick::Count; id++)
    if (Game::Window::Instance().joystick().buttonPressed(id, 7) == true ||
      Game::Window::Instance().joystick().buttonPressed(id, 0) == true)
      _doom.addPlayer(id + 1);

  // Update game components
  _doom.update(elapsed);

  return false;
}

void	DOOM::GameDoomState::draw()
{
  std::pair<int, int>	grid((int)_doom.level.players.size(), (int)_doom.level.players.size());

  // Compute grid size
  while (true) {
    if ((grid.first * DOOM::Doom::RenderWidth * DOOM::GameDoomState::RenderScale) / (grid.second * DOOM::Doom::RenderHeight * DOOM::GameDoomState::RenderScale * DOOM::Doom::RenderStretching) > (float)Game::Window::Instance().window().getSize().x / (float)Game::Window::Instance().window().getSize().y) {
      if ((grid.first - 1) * grid.second >= _doom.level.players.size())
	grid.first--;
      else if (grid.first * (grid.second - 1) >= _doom.level.players.size())
	grid.second--;
      else
	break;
    }
    else {
      if (grid.first * (grid.second - 1) >= _doom.level.players.size())
	grid.second--;
      else if ((grid.first - 1) * grid.second >= _doom.level.players.size())
	grid.first--;
      else
	break;
    }
  }

  // Resize rendering target if necessary
  if (_image.getSize().x != grid.first * DOOM::Doom::RenderWidth * DOOM::GameDoomState::RenderScale || _image.getSize().y != (int)(grid.second * DOOM::Doom::RenderHeight * DOOM::GameDoomState::RenderScale))
  {
    // Resize RAM image
    _image.create(grid.first * DOOM::Doom::RenderWidth * DOOM::GameDoomState::RenderScale, (int)(grid.second * DOOM::Doom::RenderHeight * DOOM::GameDoomState::RenderScale));

    // Resize VRAM texture
    if (_texture.create(grid.first * DOOM::Doom::RenderWidth * DOOM::GameDoomState::RenderScale, (int)(grid.second * DOOM::Doom::RenderHeight * DOOM::GameDoomState::RenderScale)) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Set new sprite texture
    _sprite.setTexture(_texture, true);

    // Making sure the texture is not filtered
    _texture.setSmooth(false);
  }

  // Clear rendering target
  std::memset((void *)_image.getPixelsPtr(), 0, _image.getSize().x * _image.getSize().y * sizeof(sf::Color));

  // List of rendering tasks
  std::list<std::future<void>>	tasks;
  
  // Render each player camera on its own thread
  for (int y = 0; y < grid.second; y++)
    for (int x = 0; x < grid.first; x++)
      if (y * grid.first + x < _doom.level.players.size()) {
	tasks.push_back(std::async(std::launch::async, [this, grid, x, y] {
          DOOM::PlayerThing&  player = _doom.level.players[y * grid.first + x];

	  player.camera.render(_doom, _image, sf::Rect<int16_t>(x * DOOM::Doom::RenderWidth * DOOM::GameDoomState::RenderScale, y * DOOM::Doom::RenderHeight * DOOM::GameDoomState::RenderScale, DOOM::Doom::RenderWidth * DOOM::GameDoomState::RenderScale, (DOOM::Doom::RenderHeight - 32) * DOOM::GameDoomState::RenderScale), player.cameraMode(), player.cameraPalette());
	  player.statusbar.render(_doom, _image, sf::Rect<int16_t>(x * DOOM::Doom::RenderWidth * DOOM::GameDoomState::RenderScale, ((y + 1) * DOOM::Doom::RenderHeight - 32) * DOOM::GameDoomState::RenderScale, DOOM::Doom::RenderWidth * DOOM::GameDoomState::RenderScale, 32 * DOOM::GameDoomState::RenderScale));
	}));
      }

  // Wait for rendering tasks to complete
  for (std::future<void> & task : tasks)
    task.wait();

  // Update texture on VRam
  _texture.update(_image);

  // Compute sprite scale and position
  float	scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)_image.getSize().x, (float)Game::Window::Instance().window().getSize().y / ((float)_image.getSize().y * DOOM::Doom::RenderStretching));
  float	pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)_image.getSize().x * scale)) / 2.f);
  float	pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)_image.getSize().y * scale * DOOM::Doom::RenderStretching)) / 2.f);

  // Position sprite in window
  _sprite.setScale(sf::Vector2f(scale, scale * DOOM::Doom::RenderStretching));
  _sprite.setPosition(sf::Vector2f(pos_x, pos_y));

  // Draw sprite
  Game::Window::Instance().window().draw(_sprite);
}

void	DOOM::GameDoomState::addPlayer(int controller)
{
  // Add player to current game
  _doom.addPlayer(controller);
}