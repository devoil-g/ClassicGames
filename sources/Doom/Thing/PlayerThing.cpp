#include <iostream>
#include <functional>
#include <unordered_map>

#include "Doom/Thing/PlayerThing.hpp"
#include "System/Window.hpp"

const float	DOOM::PlayerThing::VerticalLimit = Math::DegToRad(22.5f); // 
const float	DOOM::PlayerThing::TurningSpeed = 2.f / 3.f * Math::Pi;   // 
const float	DOOM::PlayerThing::WalkingSpeed = 24.f;			  // 24 unit per frame (3 tics)
const float	DOOM::PlayerThing::RunningSpeed = 50.f;			  // 50 unit per frame (3 tics)

DOOM::PlayerThing::PlayerThing(DOOM::Doom & doom, int id, int controller) :
  DOOM::AbstractThing(doom, DOOM::Enum::ThingType::ThingType_PLAYER, DOOM::Enum::ThingFlag::FlagNone, 0.f, 0.f, 0.f),
  _running(false),
  id(id),
  controller(controller),
  camera(),
  statusbar(doom, id),
  armor(DOOM::Enum::Armor::ArmorNone)
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

  // Update camera position
  camera.position = position;
  camera.position.z() += height * 0.73f;

  // Initial ammo
  statusbar.ammos[DOOM::Enum::Ammo::AmmoBullet] = 50;
}

bool	DOOM::PlayerThing::update(DOOM::Doom & doom, sf::Time elapsed)
{
  // Update player using controller
  if (controller == 0)
    updateKeyboard(doom, elapsed);
  else
    updateController(doom, elapsed);

  // Update core components and physics
  DOOM::AbstractThing::update(doom, elapsed);

  // Update camera position
  camera.position = position;
  camera.position.z() += height * 0.73f;

  // Update status bar (TODO: complete this)
  statusbar.health = health;

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
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true)
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
  P_LineSwitch(doom, 64.f, position + Math::Vector<3>(0.f, 0.f, height / 2.f), Math::Vector<3>(std::cos(angle), std::sin(angle), std::tan(camera.orientation)));
}

void	DOOM::PlayerThing::updateFire(DOOM::Doom & doom, sf::Time elapsed)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol);
  P_LineAttack(doom, 1000.f, position + Math::Vector<3>(0.f, 0.f, height / 2.f), Math::Vector<3>(std::cos(angle), std::sin(angle), std::tan(camera.orientation)), 5 + std::rand() % 11);
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

bool  DOOM::PlayerThing::pickup(DOOM::Doom& doom, DOOM::AbstractThing& item)
{
  // Map of pick-upable items
  static const std::unordered_map<DOOM::Enum::ThingType, std::function<bool(DOOM::PlayerThing&, DOOM::AbstractThing&)>> items = {
    { DOOM::Enum::ThingType::ThingType_MISC0, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupArmor(DOOM::Enum::Armor::ArmorSecurity); } },                                                                                                                                          // Green armor
    { DOOM::Enum::ThingType::ThingType_MISC1, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupArmor(DOOM::Enum::Armor::ArmorMega); } },                                                                                                                                              // Blue armor
    { DOOM::Enum::ThingType::ThingType_MISC2, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupHealth(1, 200); } },                                                                                                                                                                   // Health potion
    { DOOM::Enum::ThingType::ThingType_MISC3, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupArmor(DOOM::Enum::Armor::ArmorSecurity, 1); } },                                                                                                                                       // Spiritual armor
    { DOOM::Enum::ThingType::ThingType_MISC4, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorBlue, DOOM::Enum::KeyType::KeyTypeKeycard); } },                                                                                                     // Blue keycard
    { DOOM::Enum::ThingType::ThingType_MISC5, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorRed, DOOM::Enum::KeyType::KeyTypeKeycard); } },                                                                                                      // Red keycard
    { DOOM::Enum::ThingType::ThingType_MISC6, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorYellow, DOOM::Enum::KeyType::KeyTypeKeycard); } },                                                                                                   // Yellow keycard
    { DOOM::Enum::ThingType::ThingType_MISC7, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorYellow, DOOM::Enum::KeyType::KeyTypeSkullkey); } },                                                                                                  // Yellow skull key
    { DOOM::Enum::ThingType::ThingType_MISC8, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorRed, DOOM::Enum::KeyType::KeyTypeSkullkey); } },                                                                                                     // Red skull key
    { DOOM::Enum::ThingType::ThingType_MISC9, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorBlue, DOOM::Enum::KeyType::KeyTypeSkullkey); } },                                                                                                    // Blue skull key
    { DOOM::Enum::ThingType::ThingType_MISC10, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupHealth(10, 100); } },                                                                                                                                                                 // Stimpack
    { DOOM::Enum::ThingType::ThingType_MISC11, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupHealth(25, 100); } },                                                                                                                                                                 // Medikit
    { DOOM::Enum::ThingType::ThingType_MISC12, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupHealth(100, 200); } },                                                                                                                                                                // Soul sphere
    { DOOM::Enum::ThingType::ThingType_MEGA, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupHealth(200, 200) | player.pickupArmor(DOOM::Enum::Armor::ArmorMega); } },                                                                                                               // Soul sphere
    { DOOM::Enum::ThingType::ThingType_CLIP, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(DOOM::Enum::Ammo::AmmoBullet, (item.flags & DOOM::Enum::ThingProperty::ThingProperty_Dropped) ? 5 : 10); } },                                                                      // Ammo clip
    { DOOM::Enum::ThingType::ThingType_MISC17, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(DOOM::Enum::Ammo::AmmoBullet, 50); } },                                                                                                                                          // Box of ammo
    { DOOM::Enum::ThingType::ThingType_MISC18, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(DOOM::Enum::Ammo::AmmoRocket, 1); } },                                                                                                                                           // Rocket
    { DOOM::Enum::ThingType::ThingType_MISC19, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(DOOM::Enum::Ammo::AmmoRocket, 1); } },                                                                                                                                           // Box of rockets
    { DOOM::Enum::ThingType::ThingType_MISC20, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(DOOM::Enum::Ammo::AmmoCell, 20); } },                                                                                                                                            // Cell charge
    { DOOM::Enum::ThingType::ThingType_MISC21, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(DOOM::Enum::Ammo::AmmoCell, 100); } },                                                                                                                                           // Cell charge pack
    { DOOM::Enum::ThingType::ThingType_MISC22, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(DOOM::Enum::Ammo::AmmoShell, 4); } },                                                                                                                                            // Shotgun shells
    { DOOM::Enum::ThingType::ThingType_MISC23, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(DOOM::Enum::Ammo::AmmoShell, 20); } },                                                                                                                                           // Box of shells
    { DOOM::Enum::ThingType::ThingType_MISC24, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupBackpack(); } },                                                                                                                                                                      // Backpack
    { DOOM::Enum::ThingType::ThingType_MISC25, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupWeapon(DOOM::Enum::Weapon::WeaponBFG9000) | player.pickupAmmo(DOOM::Enum::Ammo::AmmoCell, 40); } },                                                                                   // BFG 9000
    { DOOM::Enum::ThingType::ThingType_CHAINGUN, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupWeapon(DOOM::Enum::Weapon::WeaponChaingun) | player.pickupAmmo(DOOM::Enum::Ammo::AmmoBullet, (item.flags & DOOM::Enum::ThingProperty::ThingProperty_Dropped) ? 10 : 20); } },       // Chaingun
    { DOOM::Enum::ThingType::ThingType_MISC26, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupWeapon(DOOM::Enum::Weapon::WeaponChainsaw); } },                                                                                                                                      // Chainsaw
    { DOOM::Enum::ThingType::ThingType_MISC27, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupWeapon(DOOM::Enum::Weapon::WeaponRocketLauncher) | player.pickupAmmo(DOOM::Enum::Ammo::AmmoRocket, 2); } },                                                                           // Rocket launcher
    { DOOM::Enum::ThingType::ThingType_MISC28, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupWeapon(DOOM::Enum::Weapon::WeaponPlasmaGun) | player.pickupAmmo(DOOM::Enum::Ammo::AmmoCell, 40); } },                                                                                 // Plasma rifle
    { DOOM::Enum::ThingType::ThingType_SHOTGUN, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupWeapon(DOOM::Enum::Weapon::WeaponShotgun) | player.pickupAmmo(DOOM::Enum::Ammo::AmmoShell, (item.flags & DOOM::Enum::ThingProperty::ThingProperty_Dropped) ? 4 : 8); } },            // Shotgun
    { DOOM::Enum::ThingType::ThingType_SUPERSHOTGUN, [](DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupWeapon(DOOM::Enum::Weapon::WeaponSuperShotgun) | player.pickupAmmo(DOOM::Enum::Ammo::AmmoShell, (item.flags & DOOM::Enum::ThingProperty::ThingProperty_Dropped) ? 4 : 8); } },  // Super shotgun
  };

  /* TODO:
    ThingType_INV,            // Invulnerability
    ThingType_MISC13,         // Berserk
    ThingType_INS,            // Invisibility
    ThingType_MISC14,         // Radiation suit
    ThingType_MISC15,         // Computer map
    ThingType_MISC16,         // Light amplification visor
  */

  auto  iterator = items.find(item.type);

  // Pick-up item if known
  if (iterator != items.end())
    return iterator->second(*this, item);
  else {
    std::cerr << "[DOOM::PlayerThing] Warning, pickup type '" << item.type << "' not supported." << std::endl;
    return false;
  }
}

bool  DOOM::PlayerThing::pickupWeapon(DOOM::Enum::Weapon type)
{
  // Cancel if weapon already in inventory
  if (statusbar.weapons[type] == true)
    return false;

  // Pickup gun
  statusbar.weapons[type] = true;
  return true;
}

bool  DOOM::PlayerThing::pickupAmmo(DOOM::Enum::Ammo type, unsigned int quantity)
{
  // Don't pickup if already full
  if (statusbar.ammos[type] >= statusbar.maximum[type])
    return false;

  statusbar.ammos[type] = std::min(statusbar.maximum[type], statusbar.ammos[type] + quantity);
  return true;
}

bool  DOOM::PlayerThing::pickupBackpack()
{
  bool  backpack = false;

  // Increase ammo count
  if (statusbar.maximum[DOOM::Enum::Ammo::AmmoBullet] != 400 ||
    statusbar.maximum[DOOM::Enum::Ammo::AmmoShell] != 100 ||
    statusbar.maximum[DOOM::Enum::Ammo::AmmoRocket] != 100 ||
    statusbar.maximum[DOOM::Enum::Ammo::AmmoCell] != 600) {
    statusbar.maximum[DOOM::Enum::Ammo::AmmoBullet] = 400;
    statusbar.maximum[DOOM::Enum::Ammo::AmmoShell] = 100;
    statusbar.maximum[DOOM::Enum::Ammo::AmmoRocket] = 100;
    statusbar.maximum[DOOM::Enum::Ammo::AmmoCell] = 600;
    backpack = true;
  }

  // TODO: pickup double in "I'm Too Young To Die" and "Nightmare!" skill levels
  // Pickup ammos
  return backpack |
    pickupAmmo(DOOM::Enum::Ammo::AmmoBullet, 10) |
    pickupAmmo(DOOM::Enum::Ammo::AmmoShell, 4) |
    pickupAmmo(DOOM::Enum::Ammo::AmmoRocket, 1) |
    pickupAmmo(DOOM::Enum::Ammo::AmmoCell, 20);
}

bool  DOOM::PlayerThing::pickupKey(DOOM::Enum::KeyColor color, DOOM::Enum::KeyType type)
{
  // Add key to player
  statusbar.keys[color] = type;

  // TODO: don't remove card if multiplayer
  if (false)
    return false;

  return true;
}

bool  DOOM::PlayerThing::pickupHealth(unsigned int quantity, unsigned int maximum)
{
  // Does not pickup if already full
  if (health >= (int)maximum)
    return false;

  // Add health
  health = std::min(health + quantity, maximum);

  return true;
}

bool  DOOM::PlayerThing::pickupArmor(DOOM::Enum::Armor type, unsigned int quantity)
{
  // Does not pickup if already full
  if (statusbar.armor >= DOOM::Enum::Armor::ArmorMega)
    return false;

  // Add armor
  statusbar.armor = std::min(statusbar.armor + quantity, (unsigned int)DOOM::Enum::Armor::ArmorMega);

  // Apply new armor type if better
  if (armor < type)
    armor = type;

  return true;
}

bool  DOOM::PlayerThing::pickupArmor(DOOM::Enum::Armor type)
{
  // Does not pickup if already full
  if (statusbar.armor >= (unsigned int)type)
    return false;

  // Set new armor
  armor = type;
  statusbar.armor = type;

  // Remove item
  return true;
}

void  DOOM::PlayerThing::damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, DOOM::AbstractThing& origin, int damage)
{
  int protection = 0;

  // Compute protection
  switch (armor) {
  case DOOM::Enum::Armor::ArmorSecurity:
    protection = std::min((int)statusbar.armor, damage / 3);
    break;
  case DOOM::Enum::Armor::ArmorMega:
    protection = std::min((int)statusbar.armor, damage / 2);
    break;
  default:
    protection = 0;
    break;
  }

  // Remove protection from armor
  statusbar.armor -= protection;
  if (statusbar.armor == 0)
    armor = DOOM::Enum::Armor::ArmorNone;

  // Apply remaining damage
  DOOM::AbstractThing::damage(doom, attacker, origin, damage - protection);
}

bool  DOOM::PlayerThing::key(DOOM::Enum::KeyColor color) const
{
  // Check if player has a keycard or a skullkey of given color
  return statusbar.keys[color] != DOOM::Enum::KeyType::KeyTypeNone;
}