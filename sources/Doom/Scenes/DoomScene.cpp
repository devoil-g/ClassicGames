#include <iostream>

#include "Doom/Scenes/DoomScene.hpp"
#include "Doom/Scenes/StartDoomScene.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"
#include "System/Audio/Sound.hpp"

DOOM::DoomScene::DoomScene(Game::SceneMachine& machine, const std::filesystem::path& wad, DOOM::Enum::Mode mode) :
  Game::AbstractScene(machine),
  _doom(),
  _game()
{
  // Load WAD
  _doom.load(wad, mode);

  // Check that at least one level is available
  if (_doom.getLevels().empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Push initial state
  _game.push<DOOM::StartDoomScene>(_doom);
}

DOOM::DoomScene::~DoomScene()
{
  // Interrupt playing DOOM sounds to avoid reading deleted buffers
  Game::Audio::Sound::Instance().clear();
}

bool  DOOM::DoomScene::update(float elapsed)
{
  // Update game
  if (_game.update(elapsed) == true) {
    _machine.pop();
    return false;
  }

  return false;
}

void  DOOM::DoomScene::draw()
{
  // Draw game in DOOM rendering target
  _game.draw();

  // Only draw if rendering target is visible
  if (_doom.image.getSize().x != 0 && _doom.image.getSize().y != 0)
  {
    // Update texture on VRam
    if (_texture.getSize() != _doom.image.getSize() && _texture.create(_doom.image.getSize().x, _doom.image.getSize().y) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    _texture.update(_doom.image);

    // Making sure the texture is not filtered
    _texture.setSmooth(false);

    // Update sprite texture
    _sprite.setTexture(_texture, true);

    // Draw DOOM rendering target
    Game::Window::Instance().draw(_sprite, DOOM::Doom::RenderStretching);
  }
}