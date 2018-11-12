#include <iostream>

#include "States/Doom/GameDoomState.hpp"
#include "States/Doom/StartDoomState.hpp"
#include "States/LoadingState.hpp"
#include "States/MessageState.hpp"
#include "States/StateMachine.hpp"
#include "System/Config.hpp"
#include "System/Library/FontLibrary.hpp"
#include "System/Window.hpp"


Game::StartDoomState::StartDoomState() :
  _players({ -1, -1, -1, -1 }),
  _title("DOOM", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf")),
  _subtitle("Select your controller", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf")),
  _controllers(
    {
      sf::Text("PRESS START", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf")),
      sf::Text("PRESS START", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf")),
      sf::Text("PRESS START", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf")),
      sf::Text("PRESS START", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf"))
    }
  ),
  _ready("READY", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf")),
  _elapsed(sf::Time::Zero),
  _textureKeyboard(), _textureController(),
  _spriteKeyboard(), _spriteController()
{
  // Load keyboard & controller icons
  if (_textureKeyboard.loadFromFile(Game::Config::ExecutablePath + "assets/textures/keyboard.png") == false ||
    _textureController.loadFromFile(Game::Config::ExecutablePath + "assets/textures/controller.png") == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Set sprite texture
  _spriteKeyboard.setTexture(_textureKeyboard, true);
  _spriteController.setTexture(_textureController, true);
}

Game::StartDoomState::~StartDoomState()
{}

bool	Game::StartDoomState::update(sf::Time elapsed)
{
  // Add elapsed time to counter
  _elapsed += elapsed;

  // Keyboard start
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true ||
    Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Left) == true)
    if (updateRegister(0) == false)
      return false;

  // Keyboard/mouse cancel
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) == true ||
    Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Right) == true)
    if (updateUnregister(0) == false)
      return false;

  // Joysticks handle
  for (unsigned int id = 0; id < sf::Joystick::Count; id++)
  {
    // Joystick start
    if (Game::Window::Instance().joystick().buttonPressed(id, 7) == true ||
      Game::Window::Instance().joystick().buttonPressed(id, 0) == true)
      if (updateRegister(id + 1) == false)
	return false;

    // Joyatick cancel
    if (Game::Window::Instance().joystick().buttonPressed(id, 1) == true)
      if (updateUnregister(id + 1) == false)
	return false;
  }

  return false;
}

bool	Game::StartDoomState::updateRegister(const int id)
{
  // Check if the player is already registered
  for (int player : _players)
    if (player == id) {
      // Get player controller list
      std::array<int, 4>	players = _players;

      // Load and start DOOM game
      Game::StateMachine::Instance().swap(new Game::LoadingState(std::async(std::launch::async, [players]
      {
	try
	{
	  Game::GameDoomState *	state = new Game::GameDoomState();

	  // Add players to the game
	  for (int player : players)
	    if (player != -1)
	      state->addPlayer(player);

	  return static_cast<Game::AbstractState *>(state);
	}
	catch (std::exception exception)
	{
	  std::cerr << "[MainMenuState::selectDoom] Warning, failed to load file: '" << std::string(exception.what()) << "'." << std::endl;

	  // Return an error message
	  return static_cast<Game::AbstractState *>(new Game::MessageState("Error: failed to load WAD file.\n"));
	}
      })));

      // TODO: launch new game
      return false;
    }

  // Find a slot for new player
  for (int & player : _players)
    if (player == -1) {
      player = id;
      return true;
    }

  // Should not happen
  return true;
}

bool	Game::StartDoomState::updateUnregister(const int id)
{
  // Find and unregister player
  for (int & player : _players)
    if (player == id) {
      player = -1;
      return true;
    }

  // Player is not registered, go to previous menu
  Game::StateMachine::Instance().pop();
  return false;
}

void	Game::StartDoomState::draw()
{
  // Set text for each player slot
  for (int index = 0; index < _players.size(); index++) {
    switch (_players[index]) {
    case -1:	// No controller
    {
      float	intensity = std::sqrtf(1.f - (std::cos(_elapsed.asSeconds() * 4.f) + 1.f) / 2.f);

      _controllers[index].setString("PRESS START");
      _controllers[index].setFillColor(sf::Color((sf::Uint8)(255 * intensity), (sf::Uint8)(255 * intensity), (sf::Uint8)(255 * intensity)));
      break;
    }
    case 0:	// Keyboard/mouse
      _controllers[index].setString("Keyboard");
      _controllers[index].setFillColor(sf::Color::White);
      break;
    default:	// Controller
      _controllers[index].setString("Controller #" + std::to_string(_players[index] - 1));
      _controllers[index].setFillColor(sf::Color::White);
      break;
    }
  }

  // Set texts size
  _title.setCharacterSize(Game::Window::Instance().window().getSize().y / 12);
  _subtitle.setCharacterSize(Game::Window::Instance().window().getSize().y / 24);
  for (int index = 0; index < _controllers.size(); index++)
    _controllers[index].setCharacterSize(Game::Window::Instance().window().getSize().y / 24);
  _ready.setCharacterSize(Game::Window::Instance().window().getSize().y / 12);
  _ready.setScale(
    0.75f + 0.25f * (std::cos(_elapsed.asSeconds() * 4.f) + 1.f) / 2.f,
    0.75f + 0.25f * (std::cos(_elapsed.asSeconds() * 4.f) + 1.f) / 2.f
  );
  _spriteKeyboard.setScale(
    std::min((Game::Window::Instance().window().getSize().y * 0.3f) / _textureKeyboard.getSize().y, (Game::Window::Instance().window().getSize().x * 0.85f) / (_textureKeyboard.getSize().x * _controllers.size())),
    std::min((Game::Window::Instance().window().getSize().y * 0.3f) / _textureKeyboard.getSize().y, (Game::Window::Instance().window().getSize().x * 0.85f) / (_textureKeyboard.getSize().x * _controllers.size()))
  );
  _spriteController.setScale(
    std::min((Game::Window::Instance().window().getSize().y * 0.3f) / _textureController.getSize().y, (Game::Window::Instance().window().getSize().x * 0.85f) / (_textureController.getSize().x * _controllers.size())),
    std::min((Game::Window::Instance().window().getSize().y * 0.3f) / _textureController.getSize().y, (Game::Window::Instance().window().getSize().x * 0.85f) / (_textureController.getSize().x * _controllers.size()))
  );

  // Set texts scale
  for (int index = 0; index < _controllers.size(); index++) {
    _controllers[index].setScale(1.f, 1.f);
    if (_controllers[index].getGlobalBounds().width > Game::Window::Instance().window().getSize().x / _controllers.size() * 0.75f)
      _controllers[index].setScale(
	(Game::Window::Instance().window().getSize().x * 0.75f) / (_controllers[index].getGlobalBounds().width * _controllers.size()),
	(Game::Window::Instance().window().getSize().x * 0.75f) / (_controllers[index].getGlobalBounds().width * _controllers.size())
      );
  }

  // Set texts position
  _title.setPosition(
    (Game::Window::Instance().window().getSize().x - _title.getLocalBounds().width) / 2.f,
    (Game::Window::Instance().window().getSize().y - _title.getLocalBounds().height) * 1.f / 16.f
  );
  _subtitle.setPosition(
    (Game::Window::Instance().window().getSize().x - _subtitle.getLocalBounds().width) / 2.f,
    (Game::Window::Instance().window().getSize().y - _subtitle.getLocalBounds().height) * 2.5f / 16.f
  );
  for (int index = 0; index < _controllers.size(); index++)
    _controllers[index].setPosition(
      Game::Window::Instance().window().getSize().x * (index * 2.f + 1.f) / (_players.size() * 2.f) - _controllers[index].getGlobalBounds().width / 2.f,
      (Game::Window::Instance().window().getSize().y - _controllers[index].getGlobalBounds().height) * 12.f / 16.f
    );
  _ready.setPosition(
    (Game::Window::Instance().window().getSize().x - _ready.getLocalBounds().width * _ready.getScale().x) / 2.f,
    (Game::Window::Instance().window().getSize().y - _ready.getLocalBounds().height * _ready.getScale().y) * 14.5f / 16.f
  );

  // Draw texts
  Game::Window::Instance().window().draw(_title);
  Game::Window::Instance().window().draw(_subtitle);
  for (int index = 0; index < _controllers.size(); index++) {
    Game::Window::Instance().window().draw(_controllers[index]);
    switch (_players[index]) {
    case -1:
      break;
    case 0:
      _spriteKeyboard.setPosition(
	Game::Window::Instance().window().getSize().x * (index * 2.f + 1.f) / (_players.size() * 2.f) - _spriteKeyboard.getGlobalBounds().width / 2,
	Game::Window::Instance().window().getSize().y * 8.f / 16.f - _spriteKeyboard.getGlobalBounds().height / 2
      );
      Game::Window::Instance().window().draw(_spriteKeyboard);
      break;
    default:
      _spriteController.setPosition(
	Game::Window::Instance().window().getSize().x * (index * 2.f + 1.f) / (_players.size() * 2.f) - _spriteController.getGlobalBounds().width / 2,
	Game::Window::Instance().window().getSize().y * 8.f / 16.f - _spriteController.getGlobalBounds().height / 2
      );
      Game::Window::Instance().window().draw(_spriteController);
      break;
    }
  }
  if (*std::max_element(_players.begin(), _players.end()) > -1)
    Game::Window::Instance().window().draw(_ready);

}