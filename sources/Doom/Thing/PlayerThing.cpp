#include "Doom/Thing/PlayerThing.hpp"
#include "System/Window.hpp"

const float	DOOM::PlayerThing::TurningSpeed = 2.f / 3.f * Math::Pi;
const float	DOOM::PlayerThing::WalkingSpeed = 24.f / 3.f;			// 24 unit per frame (3 tics)
const float	DOOM::PlayerThing::RunningSpeed = 50.f / 3.f;			// 50 unit per frame (3 tics)

DOOM::PlayerThing::PlayerThing(DOOM::Doom & doom, int id, int controller) :
  DOOM::AbstractThing(doom, 16, Obstacle),
  _sprites(doom.resources.animations.find(DOOM::str_to_key("PLAY"))->second),
  _running(false),
  id(id),
  controller(controller),
  camera(),
  statusbar(doom, id),
  health(0), armor(0),
  bullet(0), shell(0), rocket(0), cell(0)
{
  // Check controller ID
  if (controller < 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Register player in DOOM
  doom.level.players.push_back(std::reference_wrapper<DOOM::PlayerThing>(*this));

  // Set player initial position
  for (const std::unique_ptr<DOOM::AbstractThing> & thing : doom.level.things)
    if (thing->type == id) {
      position = thing->position;
      height = doom.level.sectors[doom.level.getSector(thing->position).first].floor_current + 41.f;
      angle = thing->angle;
      break;
    }
}

bool	DOOM::PlayerThing::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // TODO: remove this
  _elapsed += elapsed;

  // TODO: update player using controller
  if (controller == 0)
    updateKeyboard(doom, elapsed);
  else
    updateController(doom, elapsed);

  // Always return false as a player is never deleted
  return false;
}

void	DOOM::PlayerThing::updateKeyboard(DOOM::Doom & doom, sf::Time elapsed)
{
  updateKeyboardTurn(doom, elapsed);
  updateKeyboardMove(doom, elapsed);

  // Perform a use action
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true)
    updateUse(doom, elapsed);
}

void	DOOM::PlayerThing::updateKeyboardTurn(DOOM::Doom & doom, sf::Time elapsed)
{
  // Turn player
  float	angle = 0.f;

  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Left) == true)	// Turn left
    angle += 1.f;
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Right) == true)	// Turn right
    angle -= 1.f;

  // Apply rotation to player
  updateTurn(doom, elapsed, angle);
}

void	DOOM::PlayerThing::updateKeyboardMove(DOOM::Doom & doom, sf::Time elapsed)
{
  // Move player
  Math::Vector<2>	movement(0.f, 0.f);

  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Z) == true)	// Move forward
    movement += Math::Vector<2>(+1.f, 0.f);
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::S) == true)	// Move backward
    movement += Math::Vector<2>(-1.f, 0.f);
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Q) == true)	// Strafe left
    movement += Math::Vector<2>(0.f, -1.f);
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::D) == true)	// Strafe right
    movement += Math::Vector<2>(0.f, +1.f);

  // Handle running
  _running = Game::Window::Instance().keyboard().key(sf::Keyboard::LShift);

  // Apply movement to player position
  updateMove(doom, elapsed, movement);
}

void	DOOM::PlayerThing::updateController(DOOM::Doom & doom, sf::Time elapsed)
{
  updateControllerTurn(doom, elapsed);
  updateControllerMove(doom, elapsed);

  // Perform a use action
  if (Game::Window::Instance().joystick().buttonPressed(controller - 1, 0) == true)
    updateUse(doom, elapsed);
}

void	DOOM::PlayerThing::updateControllerTurn(DOOM::Doom & doom, sf::Time elapsed)
{
  // Apply rotation to player
  updateTurn(doom, elapsed, std::fabsf(Game::Window::Instance().joystick().position(controller - 1, 4)) / 100.f > 0.2f ? -Game::Window::Instance().joystick().position(controller - 1, 4) / 100.f : 0.f);
}

void	DOOM::PlayerThing::updateControllerMove(DOOM::Doom & doom, sf::Time elapsed)
{
  // Move player
  Math::Vector<2>	movement(
    std::fabsf(Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::Y)) / 100.f > 0.2f ? -Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::Y) / 100.f : 0.f,
    std::fabs(Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::X)) / 100.f > 0.2f ? +Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::X) / 100.f : 0.f
  );
  
  // Handle running (left stick click)
  if (movement.length() < 0.72f)
    _running = false;
  if (Game::Window::Instance().joystick().button(controller - 1, 8) == true)
    _running = true;

  // Apply movement to player position
  updateMove(doom, elapsed, movement);
}

void	DOOM::PlayerThing::updateTurn(DOOM::Doom & doom, sf::Time elapsed, float turn)
{
  // Apply sprinting to turning
  turn *= (_running == true ? DOOM::PlayerThing::RunningSpeed / DOOM::PlayerThing::WalkingSpeed : 1.f);

  // Apply turning angle and time to player
  angle += turn * DOOM::PlayerThing::TurningSpeed * elapsed.asSeconds();

  // Update camera
  camera.angle = angle;
}

void	DOOM::PlayerThing::updateMove(DOOM::Doom & doom, sf::Time elapsed, Math::Vector<2> movement)
{
  // Limit movement
  if (movement.length() > 1.f)
    movement /= movement.length();

  // Apply speed to movement
  movement *= (_running == true ? DOOM::PlayerThing::RunningSpeed : DOOM::PlayerThing::WalkingSpeed) / DOOM::Doom::Tic.asSeconds();

  // Apply time to movement
  movement *= elapsed.asSeconds();

  // Apply rotation to movement
  movement = Math::Vector<2>(
    movement.x() * std::cosf(angle) + movement.y() * std::sinf(angle),
    movement.x() * std::sinf(angle) - movement.y() * std::cosf(angle)
    );

  // TODO: check collision

  // Apply movement to current position
  position += movement;
  height = doom.level.sectors[doom.level.getSector(position).first].floor_current + 41.f;

  // Update camera
  camera.position = position;
  camera.height = height;
}

void	DOOM::PlayerThing::updateUse(DOOM::Doom & doom, sf::Time elapsed)
{
  Math::Vector<2>	ray(std::cos(angle), std::sin(angle));

  // Use linedef
  // TODO: consider things as obstacles
  for (int16_t index : doom.level.getLinedefs(position, ray, 64.f))
    doom.level.linedefs[index]->switched(doom, *this);
}

const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	DOOM::PlayerThing::sprite(float angle) const
{
  // TODO: remove this
  return _sprites[(int)(_elapsed.asSeconds()) % _sprites.size()][Math::Modulo((int)(angle * 4.f / Math::Pi + 16.5f), 8)];

  static const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>	frame = { std::ref(DOOM::Doom::Resources::Texture::Null), false };

  // TODO: compute player sprite to use

  // Return a default empty texture
  return frame;
}