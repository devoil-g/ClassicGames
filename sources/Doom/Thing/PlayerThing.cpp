#include "Doom/Thing/PlayerThing.hpp"
#include "System/Window.hpp"

const float	DOOM::PlayerThing::VerticalLimit = Math::DegToRad(22.5f);
const float	DOOM::PlayerThing::TurningSpeed = 2.f / 3.f * Math::Pi;
const float	DOOM::PlayerThing::WalkingSpeed = 24.f;			// 24 unit per frame (3 tics)
const float	DOOM::PlayerThing::RunningSpeed = 50.f;			// 50 unit per frame (3 tics)

DOOM::PlayerThing::PlayerThing(DOOM::Doom & doom, int id, int controller) :
  DOOM::AbstractThing(doom, DOOM::Enum::ThingType::ThingType_PLAYER, 0.f, 0.f, 0.f),
  _running(false),
  id(id),
  controller(controller),
  camera(),
  statusbar(doom, id),
  armor(DOOM::Enum::Armor::ArmorSecurity)
{
  // Check controller ID
  if (controller < 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Register player in DOOM
  doom.level.players.push_back(std::reference_wrapper<DOOM::PlayerThing>(*this));

  // Set player initial position
  for (const std::unique_ptr<DOOM::AbstractThing> & thing : doom.level.things)
    if (thing->attributs.id == id) {
      doom.level.blockmap.moveThing(*this, position.convert<2>(), thing->position.convert<2>());
      position = thing->position;
      angle = thing->angle;
      break;
    }
}

bool	DOOM::PlayerThing::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // TODO: remove this
  _elapsed += elapsed;

  // Update player using controller
  if (controller == 0)
    updateKeyboard(doom, elapsed);
  else
    updateController(doom, elapsed);

  // Update core components and physics
  DOOM::AbstractThing::update(doom, elapsed);

  // Update camera position
  camera.position = position;
  camera.position.z() += 41.1f;

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

  // Perform a fire action
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Left) == true)
    updateFire(doom, elapsed);
}

void	DOOM::PlayerThing::updateKeyboardTurn(DOOM::Doom & doom, sf::Time elapsed)
{
  // Turn player
  float	horizontal = 0.f;
  float	vertical = 0.f;

  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Left) == true)	// Turn left
    horizontal += 1.f;
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Right) == true)	// Turn right
    horizontal -= 1.f;

  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Up) == true)	// Turn up
    vertical += 1.f;
  if (Game::Window::Instance().keyboard().key(sf::Keyboard::Down) == true)	// Turn down
    vertical -= 1.f;

  // Apply rotation to player
  updateTurn(doom, elapsed, horizontal, vertical);
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

  // Perform a fire action
  if (Game::Window::Instance().joystick().buttonPressed(controller - 1, 1) == true)
    updateFire(doom, elapsed);
}

void	DOOM::PlayerThing::updateControllerTurn(DOOM::Doom & doom, sf::Time elapsed)
{
  // Apply rotation to player
  updateTurn(doom, elapsed,
    std::fabsf(Game::Window::Instance().joystick().position(controller - 1, 4)) / 100.f > 0.2f ? -Game::Window::Instance().joystick().position(controller - 1, 4) / 100.f : 0.f,
    std::fabsf(Game::Window::Instance().joystick().position(controller - 1, 5)) / 100.f > 0.2f ? -Game::Window::Instance().joystick().position(controller - 1, 5) / 100.f : 0.f
  );
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

void	DOOM::PlayerThing::updateTurn(DOOM::Doom & doom, sf::Time elapsed, float horizontal, float vertical)
{
  // Apply sprinting to turning
  horizontal *= (_running == true ? DOOM::PlayerThing::RunningSpeed / DOOM::PlayerThing::WalkingSpeed : 1.f);

  // Apply turning angle and time to player
  angle += horizontal * DOOM::PlayerThing::TurningSpeed * elapsed.asSeconds();

  // Handle vertical angle
  camera.orientation += (DOOM::PlayerThing::VerticalLimit * vertical - camera.orientation) * (1.f - std::pow(0.1f, elapsed.asSeconds()));
  
  // Update camera
  camera.angle = angle;
}

void	DOOM::PlayerThing::updateMove(DOOM::Doom & doom, sf::Time elapsed, Math::Vector<2> movement)
{
  // Limit movement
  movement.x() = std::clamp(movement.x(), -1.f, +1.f);
  movement.y() = std::clamp(movement.y(), -1.f, +1.f);

  // Apply speed to movement
  movement.x() *= (_running == true ? DOOM::PlayerThing::RunningSpeed : DOOM::PlayerThing::WalkingSpeed) * 1.f;
  movement.y() *= (_running == true ? DOOM::PlayerThing::RunningSpeed : DOOM::PlayerThing::WalkingSpeed) * 0.8f;

  // Apply time to movement
  // movement *= elapsed.asSeconds();

  // Apply rotation to movement
  movement = Math::Vector<2>(
    movement.x() * std::cosf(angle) + movement.y() * std::sinf(angle),
    movement.x() * std::sinf(angle) - movement.y() * std::cosf(angle)
    );

  // Apply movement to current position
  thrust(Math::Vector<3>(movement.x(), movement.y(), 0.f) * elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds());
}

void	DOOM::PlayerThing::updateUse(DOOM::Doom & doom, sf::Time elapsed)
{
  Math::Vector<2>	ray(std::cos(angle), std::sin(angle));

  // Use linedef
  // TODO: consider things as obstacles
  for (int16_t index : doom.level.getLinedefs(position.convert<2>(), ray, 64.f))
    if (doom.level.linedefs[index]->switched(doom, *this) == true)
      break;
}

void	DOOM::PlayerThing::updateFire(DOOM::Doom & doom, sf::Time elapsed)
{
  Math::Vector<2>	ray(std::cos(angle), std::sin(angle));

  // Use linedef
  // TODO: consider things as obstacles
  for (int16_t index : doom.level.getLinedefs(position.convert<2>(), ray, std::numeric_limits<float>::max()))
    if (doom.level.linedefs[index]->gunfire(doom, *this) == true)
      break;
}

/*
bool	DOOM::PlayerThing::pickupAmmo(DOOM::Enum::Ammo type, unsigned int number)
{
  // Pickup ammuntions if limit not reached
  if (statusbar.ammos[type] < statusbar.maximum[type]) {
    statusbar.ammos[type] = std::min(statusbar.ammos[type] + number, statusbar.maximum[type]);
    return true;
  }

  // Ignore if limit reached
  else {
    return false;
  }
}

bool	DOOM::PlayerThing::pickupWeapon(DOOM::Enum::Weapon type)
{
  // Pickup arm if not already in inventory
  if (statusbar.weapons[type] == false) {
    statusbar.weapons[type] = true;
    return true;
  }

  // Ignore if limit reached
  else {
    return false;
  }
}

bool	DOOM::PlayerThing::pickupKey(DOOM::Enum::KeyType type, DOOM::Enum::KeyColor color)
{
  // Pickup key if none of this color is owned
  if (statusbar.keys[color] == DOOM::Enum::KeyType::KeyTypeNone) {
    statusbar.keys[color] = type;
    return true;
  }

  // Ignore if limit reached
  else {
    return false;
  }
}

bool	DOOM::PlayerThing::pickupHealth(unsigned int number, unsigned int limit)
{
  // Pickup health if limit not reached
  if (statusbar.health < limit) {
    statusbar.health = std::min(statusbar.health + number, limit);
    return true;
  }

  // Ignore if limit reached
  else {
  return false;
  }
}

bool	DOOM::PlayerThing::pickupArmor(DOOM::Enum::Armor type, unsigned int number, unsigned int limit)
{
  // Pickup armor if limit not reached
  if (statusbar.armor < limit) {
    statusbar.armor = std::min(statusbar.armor + number, limit);
    if (type != DOOM::Enum::Armor::ArmorNone)
      armor = type;
    return true;
  }

  // Ignore if limit reached
  else {
    return false;
  }
}

bool	DOOM::PlayerThing::pickupBackpack(const std::array<unsigned int, DOOM::Enum::Ammo::AmmoCount> & capacity)
{
  bool	improved = false;

  // Pickup backpack if capacity can be improved
  for (unsigned int index = 0; index < DOOM::Enum::Ammo::AmmoCount; index++) {
    if (statusbar.maximum[index] < capacity[index]) {
      statusbar.maximum[index] = capacity[index];
      improved = true;
    }
  }

  return improved;
}
*/
