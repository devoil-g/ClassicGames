#include <future>
#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Thing/PlayerThing.hpp"
#include "Doom/States/GameDoomState.hpp"
#include "Doom/States/IntermissionDoomState.hpp"
#include "Doom/States/MenuDoomState.hpp"
#include "Doom/States/TransitionDoomState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

#include "System/Sound.hpp"

DOOM::GameDoomState::GameDoomState(Game::StateMachine& machine, DOOM::Doom& doom) :
  Game::AbstractState(machine),
  _doom(doom)
{
  // Cancel if no level loaded
  if (_doom.level.episode == std::pair<uint8_t, uint8_t>{ 0, 0 })
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

bool	DOOM::GameDoomState::update(sf::Time elapsed)
{
  // Return to previous menu
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) == true)
  {
    _machine.swap<DOOM::MenuDoomState>(_doom);
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

  // TODO: remove this
  // Change level
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::F2) == true) {
    auto  levels = _doom.getLevels();
    auto  next = std::find(levels.begin(), levels.end(), _doom.level.episode);

    // Get next level
    if (next == levels.end() || (++next) == levels.end())
      next = levels.begin();
    
    // Load next level
    _doom.setLevel(*next);
  }

  // TODO: remove this
  // Change level
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::F1) == true) {
    auto  levels = _doom.getLevels();
    auto  next = std::find(levels.rbegin(), levels.rend(), _doom.level.episode);

    // Get next level
    if (next == levels.rend() || (++next) == levels.rend())
      next = levels.rbegin();

    // Load previous level
    _doom.setLevel(*next);
  }

  // Detect level end
  if (_doom.level.end != DOOM::Enum::End::EndNone) {
    // Draw last frame of game
    _machine.draw();

    sf::Image start = _doom.image;

    // Save references as 'this' is gonna be deleted
    Game::StateMachine& machine = _machine;
    DOOM::Doom& doom = _doom;

    // Swap to game with intermission
    machine.swap<DOOM::IntermissionDoomState>(doom, doom.level.end == DOOM::Enum::End::EndSecret);

    // Simulate first frame of intermission
    machine.draw();

    // Push transition
    machine.push<DOOM::TransitionDoomState>(doom, start, doom.image);
  }

  return false;
}

void	DOOM::GameDoomState::draw()
{
  std::pair<int, int>	grid((int)_doom.level.players.size(), (int)_doom.level.players.size());

  // Compute grid size
  while (true) {
    if ((grid.first * DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale) / (grid.second * DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale * DOOM::Doom::RenderStretching) > (float)Game::Window::Instance().window().getSize().x / (float)Game::Window::Instance().window().getSize().y) {
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
  if (_doom.image.getSize().x != grid.first * DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale || _doom.image.getSize().y != (int)(grid.second * DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale))
    _doom.image.create(grid.first * DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, (int)(grid.second * DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale));

  // Clear rendering target
  std::memset((void *)_doom.image.getPixelsPtr(), 0, _doom.image.getSize().x * _doom.image.getSize().y * sizeof(sf::Color));

  // List of rendering tasks
  std::list<std::future<void>>	tasks;
  
  // Render each player camera on its own thread
  for (int y = 0; y < grid.second; y++)
    for (int x = 0; x < grid.first; x++)
      if (y * grid.first + x < _doom.level.players.size()) {
	tasks.push_back(std::async(std::launch::async, [this, grid, x, y] {
          DOOM::PlayerThing&  player = _doom.level.players[y * grid.first + x];

	  player.camera.render(_doom, _doom.image, sf::Rect<int16_t>(x * DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, y * DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale, DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, (DOOM::Doom::RenderHeight - 32) * DOOM::Doom::RenderScale), player.cameraMode(), player.cameraPalette());
	  player.statusbar.render(_doom, _doom.image, sf::Rect<int16_t>(x * DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, ((y + 1) * DOOM::Doom::RenderHeight - 32) * DOOM::Doom::RenderScale, DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, 32 * DOOM::Doom::RenderScale));
	}));
      }

  // Wait for rendering tasks to complete
  for (std::future<void> & task : tasks)
    task.wait();
}

void	DOOM::GameDoomState::addPlayer(int controller)
{
  // Add player to current game
  _doom.addPlayer(controller);
}