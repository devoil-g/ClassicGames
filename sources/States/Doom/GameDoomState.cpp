#include <future>
#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Thing/PlayerThing.hpp"
#include "States/Doom/GameDoomState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

Game::GameDoomState::GameDoomState() :
  _doom()
{
  // Load WAD
  _doom.load(Game::Config::ExecutablePath + "assets/levels/doom.wad");

  // Check that at least one level is available
  if (_doom.getLevel().empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load level
  _doom.setLevel(_doom.getLevel().front());
}

bool	Game::GameDoomState::update(sf::Time elapsed)
{
  // Return to previous menu
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) == true)
  {
    Game::StateMachine::Instance().pop();
    return false;
  }

  // Update game components
  _doom.update(elapsed);

  return false;
}

void	Game::GameDoomState::draw()
{
  std::pair<int, int>	grid((int)_doom.level.players.size(), (int)_doom.level.players.size());

  // Compute grid size
  while (true) {
    if ((grid.first * DOOM::Doom::RenderWidth) / (grid.second * DOOM::Doom::RenderHeight * DOOM::Doom::RenderStretching) > (float)Game::Window::Instance().window().getSize().x / (float)Game::Window::Instance().window().getSize().y) {
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
  if (_image.getSize().x != grid.first * DOOM::Doom::RenderWidth || _image.getSize().y != (int)(grid.second * DOOM::Doom::RenderHeight)) {
    _image.create(grid.first * DOOM::Doom::RenderWidth, (int)(grid.second * DOOM::Doom::RenderHeight));
    if (_texture.create(grid.first * DOOM::Doom::RenderWidth, (int)(grid.second * DOOM::Doom::RenderHeight)) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _sprite.setTexture(_texture, true);
  }

  // Clear rendering target
  std::memset((void *)_image.getPixelsPtr(), 0, _image.getSize().x * _image.getSize().y * sizeof(sf::Color));

  std::list<std::future<void>>	tasks;
  Game::GameDoomState &		game = *this;

  // Render each player camera on its own thread
  for (int y = 0; y < grid.second; y++)
    for (int x = 0; x < grid.first; x++)
      if (y * grid.first + x < _doom.level.players.size()) {
	tasks.push_back(std::async(std::launch::async, [&game, grid, x, y]
	{
	  game._doom.level.players[y * grid.first + x].get().camera.render(game._doom, game._image, sf::Rect<int16_t>(x * DOOM::Doom::RenderWidth, y * DOOM::Doom::RenderHeight, DOOM::Doom::RenderWidth, DOOM::Doom::RenderHeight - 32));
	}));
      }

  // Wait for rendering to complete
  for (std::future<void> & task : tasks)
    task.wait();

  // Update texture on VRam
  _texture.update(_image);

  // Compute sprote scale and position
  float	scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)_image.getSize().x, (float)Game::Window::Instance().window().getSize().y / ((float)_image.getSize().y * DOOM::Doom::RenderStretching));
  float	pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)_image.getSize().x * scale)) / 2.f);
  float	pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)_image.getSize().y * scale * DOOM::Doom::RenderStretching)) / 2.f);

  // Position sprite in window
  _sprite.setScale(sf::Vector2f(scale, scale * DOOM::Doom::RenderStretching));
  _sprite.setPosition(sf::Vector2f(pos_x, pos_y));

  // Draw sprite
  Game::Window::Instance().window().draw(_sprite);
}

void	Game::GameDoomState::addPlayer(int controller)
{
  // Add player to current game
  _doom.addPlayer(controller);
}

/*
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
*/