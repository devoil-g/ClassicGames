#include <iostream>

#include "Doom/Scenes/SplashDoomScene.hpp"
#include "Doom/Scenes/StartDoomScene.hpp"
#include "Scenes/LoadingScene.hpp"
#include "Scenes/MessageScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Library/FontLibrary.hpp"
#include "System/Window.hpp"

DOOM::StartDoomScene::StartDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom) :
  Game::AbstractScene(machine),
  _doom(doom),
  _players({ -1, -1, -1, -1 }),
  _subtitle(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), "select your controller"),
  _controllers(
    {
      sf::Text(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), "PRESS START"),
      sf::Text(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), "PRESS START"),
      sf::Text(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), "PRESS START"),
      sf::Text(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), "PRESS START")
    }
  ),
  _ready(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf"), "READY"),
  _elapsed(0.f),
  _textureTitle(), _textureKeyboard(), _textureController(),
  _spriteTitle(_textureTitle), _spriteKeyboard(_textureKeyboard), _spriteController(_textureController)
{
  // Load keyboard & controller icons
  if (_textureKeyboard.loadFromFile((Game::Config::ExecutablePath / "assets" / "textures" / "keyboard.png").string()) == false ||
    _textureController.loadFromFile((Game::Config::ExecutablePath / "assets" / "textures" / "controller.png").string()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  sf::Image image = _doom.resources.getMenu(Game::Utilities::str_to_key<std::uint64_t>("M_DOOM")).image(_doom);

  // Load title texture
  if (_textureTitle.resize(image.getSize()) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  _textureTitle.update(image);
  _textureTitle.setSmooth(false);

  // Set sprite texture
  _spriteTitle.setTexture(_textureTitle, true);
  _spriteKeyboard.setTexture(_textureKeyboard, true);
  _spriteController.setTexture(_textureController, true);

  // Clear DOOM rendering target
  _doom.image.resize({ 1, 1 }, sf::Color(0, 0, 0, 0));
}

bool  DOOM::StartDoomScene::update(float elapsed)
{
  // Add elapsed time to counter
  _elapsed += elapsed;

  // Keyboard start
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Space) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Enter) == true ||
    Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Left) == true)
    updateRegister(0);

  // Keyboard/mouse cancel
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Escape) == true ||
    Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Right) == true)
    updateUnregister(0);

  // Joysticks handle
  for (unsigned int id = 0; id < Game::Window::JoystickCount; id++)
  {
    // Joystick start
    if (Game::Window::Instance().joystick().buttonPressed(id, 7) == true ||
      Game::Window::Instance().joystick().buttonPressed(id, 0) == true)
      updateRegister(id + 1);

    // Joyatick cancel
    if (Game::Window::Instance().joystick().buttonPressed(id, 1) == true)
      updateUnregister(id + 1);
  }

  return false;
}

void  DOOM::StartDoomScene::updateRegister(const int id)
{
  // Start game if player is already registered
  for (int player : _players)
    if (player == id)
    {
      // Add players to game
      for (int player : _players)
        _doom.addPlayer(player);

      // Push loading screen
      _machine.swap<DOOM::SplashDoomScene>(_doom);

      return;
    }

  // Find a slot for new player
  for (int& player : _players)
    if (player == -1) {
      player = id;
      return;
    }
}

void  DOOM::StartDoomScene::updateUnregister(const int id)
{
  // Find and unregister player
  for (int& player : _players)
    if (player == id) {
      player = -1;
      return;
    }
}

void  DOOM::StartDoomScene::draw()
{
  // Set text for each player slot
  for (int index = 0; index < _players.size(); index++) {
    switch (_players[index]) {
    case -1:  // No controller
    {
      float         intensity = std::sqrt(1.f - (std::cos(_elapsed * 4.f) + 1.f) / 2.f);
      std::uint8_t  color = (std::uint8_t)(255 * intensity);

      _controllers[index].setString("PRESS START");
      _controllers[index].setFillColor(sf::Color(color, color, color));
      break;
    }
    case 0:   // Keyboard/mouse
      _controllers[index].setString("Keyboard");
      _controllers[index].setFillColor(sf::Color::White);
      break;
    default:  // Controller
      _controllers[index].setString("Controller #" + std::to_string(_players[index] - 1));
      _controllers[index].setFillColor(sf::Color::White);
      break;
    }
  }

  float subtitleScale = std::min((Game::Window::Instance().getSize().y() * 0.07f) / _textureTitle.getSize().y, (Game::Window::Instance().getSize().x() * 0.175f) / _textureTitle.getSize().x);
  float readyScale = 0.75f + 0.25f * (std::cos(_elapsed * 4.f) + 1.f) / 2.f;
  float titleScale = std::min((Game::Window::Instance().getSize().y() * 0.3f) / _textureTitle.getSize().y, (Game::Window::Instance().getSize().x() * 0.75f) / _textureTitle.getSize().x);
  float keyboardScale = std::min((Game::Window::Instance().getSize().y() * 0.3f) / _textureKeyboard.getSize().y, (Game::Window::Instance().getSize().x() * 0.85f) / (_textureKeyboard.getSize().x * _controllers.size()));
  float controllerScale = std::min((Game::Window::Instance().getSize().y() * 0.3f) / _textureController.getSize().y, (Game::Window::Instance().getSize().x() * 0.85f) / (_textureController.getSize().x * _controllers.size()));

  // Set texts size
  _subtitle.setCharacterSize(30);
  _subtitle.setScale({ subtitleScale, subtitleScale });
  for (int index = 0; index < _controllers.size(); index++)
    _controllers[index].setCharacterSize(Game::Window::Instance().getSize().y() / 24);
  _ready.setCharacterSize(Game::Window::Instance().getSize().y() / 12);
  _ready.setScale({ readyScale, readyScale });
  _spriteTitle.setScale({ titleScale, titleScale });
  _spriteKeyboard.setScale({ keyboardScale, keyboardScale });
  _spriteController.setScale({ controllerScale, controllerScale});

  // Set texts scale
  for (int index = 0; index < _controllers.size(); index++) {
    _controllers[index].setScale({ 1.f, 1.f });
    if (_controllers[index].getGlobalBounds().size.x > Game::Window::Instance().getSize().x() / _controllers.size() * 0.75f) {
      float scale = (Game::Window::Instance().getSize().x() * 0.75f) / (_controllers[index].getGlobalBounds().size.x * _controllers.size());

      _controllers[index].setScale({ scale, scale });
    }
  }

  // Set texts position
  _spriteTitle.setPosition({
    (Game::Window::Instance().getSize().x() - _spriteTitle.getGlobalBounds().size.x) / 2.f,
    (Game::Window::Instance().getSize().y() - _spriteTitle.getGlobalBounds().size.y) / 16.f
    });
  _subtitle.setPosition({
    (Game::Window::Instance().getSize().x() - _subtitle.getGlobalBounds().size.x) / 2.f,
    (Game::Window::Instance().getSize().y() - _subtitle.getGlobalBounds().size.y) * 5.f / 16.f
    });
  for (int index = 0; index < _controllers.size(); index++)
    _controllers[index].setPosition({
      Game::Window::Instance().getSize().x()* (index * 2.f + 1.f) / (_players.size() * 2.f) - _controllers[index].getGlobalBounds().size.x / 2.f,
      (Game::Window::Instance().getSize().y() - _controllers[index].getGlobalBounds().size.y) * 12.f / 16.f
      });
  _ready.setPosition({
    (Game::Window::Instance().getSize().x() - _ready.getGlobalBounds().size.x) / 2.f,
    (Game::Window::Instance().getSize().y() - _ready.getGlobalBounds().size.y) * 14.5f / 16.f
    });

  // Draw texts
  Game::Window::Instance().draw(_spriteTitle);
  Game::Window::Instance().draw(_subtitle);
  for (int index = 0; index < _controllers.size(); index++) {
    Game::Window::Instance().draw(_controllers[index]);
    switch (_players[index]) {
    case -1:
      break;
    case 0:
      _spriteKeyboard.setPosition({
        Game::Window::Instance().getSize().x()* (index * 2.f + 1.f) / (_players.size() * 2.f) - _spriteKeyboard.getGlobalBounds().size.x / 2,
        Game::Window::Instance().getSize().y() * 8.f / 16.f - _spriteKeyboard.getGlobalBounds().size.y / 2
        });
      Game::Window::Instance().draw(_spriteKeyboard);
      break;
    default:
      _spriteController.setPosition({
        Game::Window::Instance().getSize().x()* (index * 2.f + 1.f) / (_players.size() * 2.f) - _spriteController.getGlobalBounds().size.x / 2,
        Game::Window::Instance().getSize().y() * 8.f / 16.f - _spriteController.getGlobalBounds().size.y / 2
        });
      Game::Window::Instance().draw(_spriteController);
      break;
    }
  }

  if (*std::max_element(_players.begin(), _players.end()) > -1)
    Game::Window::Instance().draw(_ready);
}