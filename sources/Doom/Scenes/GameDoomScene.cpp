#include <future>
#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Thing/PlayerThing.hpp"
#include "Doom/Scenes/GameDoomScene.hpp"
#include "Doom/Scenes/IntermissionDoomScene.hpp"
#include "Doom/Scenes/MenuDoomScene.hpp"
#include "Doom/Scenes/SplashDoomScene.hpp"
#include "Doom/Scenes/TextDoomScene.hpp"
#include "Doom/Scenes/TransitionDoomScene.hpp"
#include "Scenes/SceneMachine.hpp"
#include "System/Config.hpp"
#include "System/Window.hpp"

#include "System/Audio/Sound.hpp"

DOOM::GameDoomScene::GameDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom) :
  Game::AbstractScene(machine),
  _doom(doom)
{
  // Cancel if no level loaded
  if (_doom.level.episode == std::pair<uint8_t, uint8_t>{ 0, 0 })
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

bool  DOOM::GameDoomScene::update(sf::Time elapsed)
{
  int alive = 0;

  // Count alive player
  for (const auto& player : _doom.level.players)
    if (player.get().health > 0.f)
      alive += 1;

  // Restart level when no player alive
  if (alive == 0) {
    for (const auto& player : _doom.level.players) {
      if ((player.get().controller == 0 && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true) ||
        (player.get().controller > 0 && Game::Window::Instance().joystick().buttonPressed(player.get().controller - 1, 0) == true)) {
        auto start = _doom.image;

        // Reload current level, reset players
        _doom.setLevel(_doom.level.episode, true);

        // Transition to next level
        draw();
        _machine.push<DOOM::TransitionDoomScene>(_doom, start, _doom.image);

        return false;
      }
    }
  }

  // Pause menu
  for (const auto& player : _doom.level.players) {
    if ((player.get().controller == 0 && Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) == true) ||
      (player.get().controller > 0 && Game::Window::Instance().joystick().buttonPressed(player.get().controller - 1, 7) == true)) {
      _machine.push<DOOM::MenuDoomScene>(_doom);
      return false;
    }
  }

  // Add new players (keyboard)
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true)
    _doom.addPlayer(0);

  // Add new players (joystick)
  for (unsigned int id = 0; id < sf::Joystick::Count; id++)
    if (Game::Window::Instance().joystick().buttonPressed(id, 0) == true)
      _doom.addPlayer(id + 1);

  // Update game components
  _doom.update(elapsed);

  // TODO: remove this
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::F4) == true) {
    for (const auto& player : _doom.level.players) {
      // Max ammos
      for (int i = 0; i < player.get().statusbar.ammos.size(); i++)
        player.get().statusbar.ammos[i] = player.get().statusbar.maximum[i];
      if (player.get().health > 0.f) {
        player.get().health = 200.f;
        player.get().statusbar.armor = 199.f;
        _doom.level.things.emplace_back(std::make_unique<DOOM::AbstractThing>(_doom, DOOM::Enum::ThingType::ThingType_MISC1, DOOM::Enum::ThingFlag::FlagNone, player.get().position.x(), player.get().position.y(), 0.f));
      }
      // Backpack
      if (player.get().statusbar.maximum[DOOM::Enum::Ammo::AmmoBullet] < 400)
        _doom.level.things.emplace_back(std::make_unique<DOOM::AbstractThing>(_doom, DOOM::Enum::ThingType::ThingType_MISC24, DOOM::Enum::ThingFlag::FlagNone, player.get().position.x(), player.get().position.y(), 0.f));
      // BFG
      if (player.get().statusbar.weapons[DOOM::Enum::Weapon::WeaponBFG9000] == false)
        _doom.level.things.emplace_back(std::make_unique<DOOM::AbstractThing>(_doom, DOOM::Enum::ThingType::ThingType_MISC25, DOOM::Enum::ThingFlag::FlagNone, player.get().position.x(), player.get().position.y(), 0.f));
      // Chaingun
      if (player.get().statusbar.weapons[DOOM::Enum::Weapon::WeaponChaingun] == false)
        _doom.level.things.emplace_back(std::make_unique<DOOM::AbstractThing>(_doom, DOOM::Enum::ThingType::ThingType_CHAINGUN, DOOM::Enum::ThingFlag::FlagNone, player.get().position.x(), player.get().position.y(), 0.f));
      // Chainsaw
      if (player.get().statusbar.weapons[DOOM::Enum::Weapon::WeaponChainsaw] == false)
        _doom.level.things.emplace_back(std::make_unique<DOOM::AbstractThing>(_doom, DOOM::Enum::ThingType::ThingType_MISC26, DOOM::Enum::ThingFlag::FlagNone, player.get().position.x(), player.get().position.y(), 0.f));
      // Rocket launcher
      if (player.get().statusbar.weapons[DOOM::Enum::Weapon::WeaponRocketLauncher] == false)
        _doom.level.things.emplace_back(std::make_unique<DOOM::AbstractThing>(_doom, DOOM::Enum::ThingType::ThingType_MISC27, DOOM::Enum::ThingFlag::FlagNone, player.get().position.x(), player.get().position.y(), 0.f));
      // Plasma rifle
      if (player.get().statusbar.weapons[DOOM::Enum::Weapon::WeaponPlasmaGun] == false)
        _doom.level.things.emplace_back(std::make_unique<DOOM::AbstractThing>(_doom, DOOM::Enum::ThingType::ThingType_MISC28, DOOM::Enum::ThingFlag::FlagNone, player.get().position.x(), player.get().position.y(), 0.f));
      // Shotgun
      if (player.get().statusbar.weapons[DOOM::Enum::Weapon::WeaponShotgun] == false)
        _doom.level.things.emplace_back(std::make_unique<DOOM::AbstractThing>(_doom, DOOM::Enum::ThingType::ThingType_SHOTGUN, DOOM::Enum::ThingFlag::FlagNone, player.get().position.x(), player.get().position.y(), 0.f));
      // Super shotgun
      if (_doom.mode == DOOM::Enum::Mode::ModeCommercial && player.get().statusbar.weapons[DOOM::Enum::Weapon::WeaponSuperShotgun] == false)
        _doom.level.things.emplace_back(std::make_unique<DOOM::AbstractThing>(_doom, DOOM::Enum::ThingType::ThingType_SUPERSHOTGUN, DOOM::Enum::ThingFlag::FlagNone, player.get().position.x(), player.get().position.y(), 0.f));


    }
  }

  // TODO: remove this
  // Change level
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::F3) == true) {
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

  // TODO: remove this
  // End level
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::F2) == true) {
    _doom.level.end = DOOM::Enum::End::EndNormal;
  }

  // Detect level end
  if (_doom.level.end != DOOM::Enum::End::EndNone) {
    end();
    return false;
  }

  return false;
}

void  DOOM::GameDoomScene::draw()
{
  std::pair<int, int> grid((int)_doom.level.players.size(), (int)_doom.level.players.size());

  // Compute grid size
  while (true) {
    if (((grid.first - 1) * DOOM::Doom::RenderScale + grid.first * DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale) / ((grid.second - 1) * DOOM::Doom::RenderScale + grid.second * DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale * DOOM::Doom::RenderStretching) > (float)Game::Window::Instance().window().getSize().x / (float)Game::Window::Instance().window().getSize().y) {
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
  if (_doom.image.getSize().x != (grid.first - 1) * DOOM::Doom::RenderScale + grid.first * DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale || _doom.image.getSize().y != (grid.second - 1) * DOOM::Doom::RenderScale + grid.second * DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale)
    _doom.image.create((grid.first - 1) * DOOM::Doom::RenderScale + grid.first * DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, (grid.second - 1) * DOOM::Doom::RenderScale + grid.second * DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale);

  // Clear rendering target
  std::memset((void *)_doom.image.getPixelsPtr(), 0, _doom.image.getSize().x * _doom.image.getSize().y * sizeof(sf::Color));

  // List of rendering tasks
  std::list<std::future<void>>  tasks;
  
  // Render each player camera on its own thread
  for (int y = 0; y < grid.second; y++)
    for (int x = 0; x < grid.first; x++)
      if (y * grid.first + x < _doom.level.players.size()) {
        tasks.push_back(std::async(std::launch::async, [this, grid, x, y] {
          _doom.level.players[y * grid.first + x].get().draw(_doom, _doom.image, sf::Rect<int16_t>(x * DOOM::Doom::RenderScale + x * DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, y * DOOM::Doom::RenderScale + y * DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale, DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale), DOOM::Doom::RenderScale);
        }));
      }

  // Wait for rendering tasks to complete
  for (auto& task : tasks)
    task.wait();
}

void  DOOM::GameDoomScene::addPlayer(int controller)
{
  // Add player to current game
  _doom.addPlayer(controller);
}

void  DOOM::GameDoomScene::end()
{
  std::pair<uint8_t, uint8_t> previous = _doom.level.episode;
  std::pair<uint8_t, uint8_t> next = { 0, 0 };

  // Select next level, depending on DOOM version. Level M0:E0 lead to main menu.
  switch (_doom.mode) {
  case DOOM::Enum::Mode::ModeShareware:   // DOOM 1 shareware, E1, M9
  {
    const std::map<std::pair<uint8_t, uint8_t>, std::pair<uint8_t, uint8_t>>  shareware = {
      // KNEE-DEEP IN THE DEAD
      { { 1, 1 }, { 1, 2 } }, // HANGAR
      { { 1, 2 }, { 1, 3 } }, // NUCLEAR PLANT
      { { 1, 3 }, { 1, 4 } }, // TOXIN REFINERY
      { { 1, 4 }, { 1, 5 } }, // COMMAND CONTROL
      { { 1, 5 }, { 1, 6 } }, // PHOBOS LAB
      { { 1, 6 }, { 1, 7 } }, // CENTRAL PROCESSING
      { { 1, 7 }, { 1, 8 } }, // COMPUTER STATION
      // { { 1, 8 }, {} },    // PHOBOS ANOMALY
      { { 1, 9 }, { 1, 4 } }  // MILITARY BASE
    };

    // Secret level goes to level 9
    if (_doom.level.end == DOOM::Enum::End::EndSecret)
      next = { _doom.level.episode.first, 9 };
    else if (shareware.find(_doom.level.episode) != shareware.end())
      next = shareware.find(_doom.level.episode)->second;
  } break;

  case DOOM::Enum::Mode::ModeCommercial:  // DOOM 2 retail, E1 M34
  {
    const std::map<std::pair<uint8_t, uint8_t>, std::pair<uint8_t, uint8_t>>  commercial = {
      { { 1, 1 }, { 1, 2 } },   // ENTRYWAY
      { { 1, 2 }, { 1, 3 } },   // UNDERHALLS
      { { 1, 3 }, { 1, 4 } },   // THE GANTLET
      { { 1, 4 }, { 1, 5 } },   // THE FOCUS
      { { 1, 5 }, { 1, 6 } },   // THE WASTE TUNNELS
      { { 1, 6 }, { 1, 7 } },   // THE CRUSHER
      { { 1, 7 }, { 1, 8 } },   // DEAD SIMPLE
      { { 1, 8 }, { 1, 9 } },   // TRICKS AND TRAPS
      { { 1, 9 }, { 1, 10 } },  // THE PIT
      { { 1, 10 }, { 1, 11 } }, // REFUELING BASE
      { { 1, 11 }, { 1, 12 } }, // CIRCLE OF DEATH
      { { 1, 12 }, { 1, 13 } }, // THE FACTORY
      { { 1, 13 }, { 1, 14 } }, // DOWNTOWN
      { { 1, 14 }, { 1, 15 } }, // THE INMOST DENS
      { { 1, 15 }, { 1, 16 } }, // INDUSTRIAL ZONE
      { { 1, 16 }, { 1, 17 } }, // SUBURBS
      { { 1, 17 }, { 1, 18 } }, // TENEMENTS
      { { 1, 18 }, { 1, 19 } }, // THE COURTYARD
      { { 1, 19 }, { 1, 20 } }, // THE CITADEL
      { { 1, 20 }, { 1, 21 } }, // GOTCHA!
      { { 1, 21 }, { 1, 22 } }, // NIRVANA
      { { 1, 22 }, { 1, 23 } }, // THE CATACOMBS
      { { 1, 23 }, { 1, 24 } }, // BARRELS O' FUN
      { { 1, 24 }, { 1, 25 } }, // THE CHASM
      { { 1, 25 }, { 1, 26 } }, // BLOODFALLS
      { { 1, 26 }, { 1, 27 } }, // THE ABANDONED MINES
      { { 1, 27 }, { 1, 28 } }, // MONSTER CONDO
      { { 1, 28 }, { 1, 29 } }, // THE SPIRIT WORLD
      { { 1, 29 }, { 1, 30 } }, // THE LIVING END
      // { { 1, 30 }, {} },     // ICON OF SIN
      { { 1, 31 }, { 1, 16 } }, // WOLFENSTEIN
      { { 1, 32 }, { 1, 16 } }, // GROSSE
    };

    // Secret level goes to level 31 or 32
    if (_doom.level.end == DOOM::Enum::End::EndSecret)
      next = (_doom.level.episode == std::pair<uint8_t, uint8_t>(1, 31)) ? std::pair<uint8_t, uint8_t>(1, 32) : std::pair<uint8_t, uint8_t>(1, 31);
    else if (commercial.find(_doom.level.episode) != commercial.end())
      next = commercial.find(_doom.level.episode)->second;
  } break;

  case DOOM::Enum::Mode::ModeRegistered:  // DOOM 1 registered, E3, M27
  {
    const std::map<std::pair<uint8_t, uint8_t>, std::pair<uint8_t, uint8_t>>  registered = {
      // KNEE-DEEP IN THE DEAD
      { { 1, 1 }, { 1, 2 } }, // HANGAR
      { { 1, 2 }, { 1, 3 } }, // NUCLEAR PLANT
      { { 1, 3 }, { 1, 4 } }, // TOXIN REFINERY
      { { 1, 4 }, { 1, 5 } }, // COMMAND CONTROL
      { { 1, 5 }, { 1, 6 } }, // PHOBOS LAB
      { { 1, 6 }, { 1, 7 } }, // CENTRAL PROCESSING
      { { 1, 7 }, { 1, 8 } }, // COMPUTER STATION
      // { { 1, 8 }, {} },    // PHOBOS ANOMALY
      { { 1, 9 }, { 1, 4 } }, // MILITARY BASE
      // THE SHORES OF HELL
      { { 2, 1 }, { 2, 2 } }, // DEIMOS ANOMALY
      { { 2, 2 }, { 2, 3 } }, // CONTAINMENT AREA
      { { 2, 3 }, { 2, 4 } }, // REFINERY
      { { 2, 4 }, { 2, 5 } }, // DEIMOS LAB
      { { 2, 5 }, { 2, 6 } }, // COMMAND CENTER
      { { 2, 6 }, { 2, 7 } }, // HALLS OF THE DAMNED
      { { 2, 7 }, { 2, 8 } }, // SPAWNING VATS
      // { { 2, 8 }, {} },    // TOWER OF BABEL
      { { 2, 9 }, { 2, 6 } }, // FORTRESS OF MYSTERY
      // INFERNO
      { { 3, 1 }, { 3, 2 } }, // HELL KEEP
      { { 3, 2 }, { 3, 3 } }, // SLOUGH OF DESPAIR
      { { 3, 3 }, { 3, 4 } }, // PANDEMONIUM
      { { 3, 4 }, { 3, 5 } }, // HOUSE OF PAIN
      { { 3, 5 }, { 3, 6 } }, // UNHOLY CATHEDRAL
      { { 3, 6 }, { 3, 7 } }, // MT. EREBUS
      { { 3, 7 }, { 3, 8 } }, // GATE TO LIMBO
      // { { 3, 8 }, {} },    // DIS
      { { 3, 9 }, { 3, 7 } }  // WARRENS
    };

    // Secret level goes to level 9
    if (_doom.level.end == DOOM::Enum::End::EndSecret)
      next = { _doom.level.episode.first, 9 };
    else if (registered.find(_doom.level.episode) != registered.end())
      next = registered.find(_doom.level.episode)->second;
  } break;

  case DOOM::Enum::Mode::ModeRetail:      // DOOM 1 retail, E4, M36
  {
    const std::map<std::pair<uint8_t, uint8_t>, std::pair<uint8_t, uint8_t>>  retail = {
      // KNEE-DEEP IN THE DEAD
      { { 1, 1 }, { 1, 2 } }, // HANGAR
      { { 1, 2 }, { 1, 3 } }, // NUCLEAR PLANT
      { { 1, 3 }, { 1, 4 } }, // TOXIN REFINERY
      { { 1, 4 }, { 1, 5 } }, // COMMAND CONTROL
      { { 1, 5 }, { 1, 6 } }, // PHOBOS LAB
      { { 1, 6 }, { 1, 7 } }, // CENTRAL PROCESSING
      { { 1, 7 }, { 1, 8 } }, // COMPUTER STATION
      // { { 1, 8 }, {} },    // PHOBOS ANOMALY
      { { 1, 9 }, { 1, 4 } }, // MILITARY BASE
      // THE SHORES OF HELL
      { { 2, 1 }, { 2, 2 } }, // DEIMOS ANOMALY
      { { 2, 2 }, { 2, 3 } }, // CONTAINMENT AREA
      { { 2, 3 }, { 2, 4 } }, // REFINERY
      { { 2, 4 }, { 2, 5 } }, // DEIMOS LAB
      { { 2, 5 }, { 2, 6 } }, // COMMAND CENTER
      { { 2, 6 }, { 2, 7 } }, // HALLS OF THE DAMNED
      { { 2, 7 }, { 2, 8 } }, // SPAWNING VATS
      // { { 2, 8 }, {} },    // TOWER OF BABEL
      { { 2, 9 }, { 2, 6 } }, // FORTRESS OF MYSTERY
      // INFERNO
      { { 3, 1 }, { 3, 2 } }, // HELL KEEP
      { { 3, 2 }, { 3, 3 } }, // SLOUGH OF DESPAIR
      { { 3, 3 }, { 3, 4 } }, // PANDEMONIUM
      { { 3, 4 }, { 3, 5 } }, // HOUSE OF PAIN
      { { 3, 5 }, { 3, 6 } }, // UNHOLY CATHEDRAL
      { { 3, 6 }, { 3, 7 } }, // MT. EREBUS
      { { 3, 7 }, { 3, 8 } }, // GATE TO LIMBO
      // { { 3, 8 }, {} },    // DIS
      { { 3, 9 }, { 3, 7 } }, // WARRENS
      // THY FLESH CONSUMED
      { { 4, 1 }, { 4, 2 } }, // HELL BENEATH
      { { 4, 2 }, { 4, 3 } }, // PERFECT HATRED
      { { 4, 3 }, { 4, 4 } }, // SEVER THE WICKED
      { { 4, 4 }, { 4, 5 } }, // UNRULY EVIL
      { { 4, 5 }, { 4, 6 } }, // THEY WILL REPENT
      { { 4, 6 }, { 4, 7 } }, // AGAINST THEE WICKEDLY
      { { 4, 7 }, { 4, 8 } }, // AND HELL FOLLOWED
      // { { 4, 8 }, {} },    // UNTO THE CRUEL
      { { 4, 9 }, { 4, 3 } }  // FEAR
    };

    // Secret level goes to level 9
    if (_doom.level.end == DOOM::Enum::End::EndSecret)
      next = { _doom.level.episode.first, 9 };
    else if (retail.find(_doom.level.episode) != retail.end())
      next = retail.find(_doom.level.episode)->second;
  } break;

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    break;
  }

  std::pair<uint64_t, std::string> text = { 0, "" };

  // Select text to be displayed
  switch (_doom.mode) {
  case DOOM::Enum::Mode::ModeShareware:   // DOOM 1 shareware, E1, M9
  case DOOM::Enum::Mode::ModeRegistered:  // DOOM 1 registered, E3, M27
  case DOOM::Enum::Mode::ModeRetail:      // DOOM 1 retail, E4, M36
  {
    const std::array<std::pair<uint64_t, std::string>, 4> doom = {
      std::pair<uint64_t, std::string>(Game::Utilities::str_to_key<uint64_t>("FLOOR4_8"), std::string("Once you beat the big badasses and \nclean out the moon base you're supposed\nto win, aren't you? Aren't you? Where's\nyour fat reward and ticket home? What\nthe hell is this? It's not supposed to\nend this way!\n\nIt stinks like rotten meat, but looks\nlike the lost Deimos base.  Looks like\nyou're stuck on The Shores of Hell.\nThe only way out is through.\n\nTo continue the DOOM experience, play\nThe Shores of Hell and its amazing\nsequel, Inferno!\n")),
      std::pair<uint64_t, std::string>(Game::Utilities::str_to_key<uint64_t>("SFLR6_1"), std::string("You've done it! The hideous cyber-\ndemon lord that ruled the lost Deimos\nmoon base has been slain and you\nare triumphant! But ... where are\nyou? You clamber to the edge of the\nmoon and look down to see the awful\ntruth.\n\nDeimos floats above Hell itself!\nYou've never heard of anyone escaping\nfrom Hell, but you'll make the bastards\nsorry they ever heard of you! Quickly,\nyou rappel down to  the surface of\nHell.\n\nNow, it's on to the final chapter of\nDOOM! -- Inferno.")),
      std::pair<uint64_t, std::string>(Game::Utilities::str_to_key<uint64_t>("MFLR8_4"), std::string("The loathsome spiderdemon that\nmasterminded the invasion of the moon\nbases and caused so much death has had\nits ass kicked for all time.\n\nA hidden doorway opens and you enter.\nYou've proven too tough for Hell to\ncontain, and now Hell at last plays\nfair -- for you emerge from the door\nto see the green fields of Earth!\nHome at last.\n\nYou wonder what's been happening on\nEarth while you were battling evil\nunleashed. It's good that no Hell-\nspawn could have come through that\ndoor with you ...")),
      std::pair<uint64_t, std::string>(Game::Utilities::str_to_key<uint64_t>("MFLR8_3"), std::string("the spider mastermind must have sent forth\nits legions of hellspawn before your\nfinal confrontation with that terrible\nbeast from hell.  but you stepped forward\nand brought forth eternal damnation and\nsuffering upon the horde as a true hero\nwould in the face of something so evil.\n\nbesides, someone was gonna pay for what\nhappened to daisy, your pet rabbit.\n\nbut now, you see spread before you more\npotential pain and gibbitude as a nation\nof demons run amok among our cities.\n\nnext stop, hell on earth!"))
    };

    // Text at the end of the episode
    if (previous.first >= 1 && previous.first <= doom.size() && next == std::pair<uint8_t, uint8_t>(0, 0))
      text = doom[previous.first - 1];
  } break;

  case DOOM::Enum::Mode::ModeCommercial:  // DOOM 2 retail, E1 M34
  {
    const std::map<uint8_t, std::pair<uint64_t, std::string>> doom2 = {
      { 7, { Game::Utilities::str_to_key<uint64_t>("SLIME16"), std::string("YOU HAVE ENTERED DEEPLY INTO THE INFESTED\nSTARPORT. BUT SOMETHING IS WRONG. THE\nMONSTERS HAVE BROUGHT THEIR OWN REALITY\nWITH THEM, AND THE STARPORT'S TECHNOLOGY\nIS BEING SUBVERTED BY THEIR PRESENCE.\n\nAHEAD, YOU SEE AN OUTPOST OF HELL, A\nFORTIFIED ZONE. IF YOU CAN GET PAST IT,\nYOU CAN PENETRATE INTO THE HAUNTED HEART\nOF THE STARBASE AND FIND THE CONTROLLING\nSWITCH WHICH HOLDS EARTH'S POPULATION\nHOSTAGE.") } },
      { 12, { Game::Utilities::str_to_key<uint64_t>("RROCK14"), std::string("YOU HAVE WON! YOUR VICTORY HAS ENABLED\nHUMANKIND TO EVACUATE EARTH AND ESCAPE\nTHE NIGHTMARE.  NOW YOU ARE THE ONLY\nHUMAN LEFT ON THE FACE OF THE PLANET.\nCANNIBAL MUTATIONS, CARNIVOROUS ALIENS,\nAND EVIL SPIRITS ARE YOUR ONLY NEIGHBORS.\nYOU SIT BACK AND WAIT FOR DEATH, CONTENT\nTHAT YOU HAVE SAVED YOUR SPECIES.\n\nBUT THEN, EARTH CONTROL BEAMS DOWN A\nMESSAGE FROM SPACE: \"SENSORS HAVE LOCATED\nTHE SOURCE OF THE ALIEN INVASION. IF YOU\nGO THERE, YOU MAY BE ABLE TO BLOCK THEIR\nENTRY.  THE ALIEN BASE IS IN THE HEART OF\nYOUR OWN HOME CITY, NOT FAR FROM THE\nSTARPORT.\" SLOWLY AND PAINFULLY YOU GET\nUP AND RETURN TO THE FRAY.") } },
      { 21, { Game::Utilities::str_to_key<uint64_t>("RROCK07"), std::string("YOU ARE AT THE CORRUPT HEART OF THE CITY,\nSURROUNDED BY THE CORPSES OF YOUR ENEMIES.\nYOU SEE NO WAY TO DESTROY THE CREATURES'\nENTRYWAY ON THIS SIDE, SO YOU CLENCH YOUR\nTEETH AND PLUNGE THROUGH IT.\n\nTHERE MUST BE A WAY TO CLOSE IT ON THE\nOTHER SIDE. WHAT DO YOU CARE IF YOU'VE\nGOT TO GO THROUGH HELL TO GET TO IT?") } },
      { 0, { Game::Utilities::str_to_key<uint64_t>("RROCK17"), std::string("THE HORRENDOUS VISAGE OF THE BIGGEST\nDEMON YOU'VE EVER SEEN CRUMBLES BEFORE\nYOU, AFTER YOU PUMP YOUR ROCKETS INTO\nHIS EXPOSED BRAIN. THE MONSTER SHRIVELS\nUP AND DIES, ITS THRASHING LIMBS\nDEVASTATING UNTOLD MILES OF HELL'S\nSURFACE.\n\nYOU'VE DONE IT. THE INVASION IS OVER.\nEARTH IS SAVED. HELL IS A WRECK. YOU\nWONDER WHERE BAD FOLKS WILL GO WHEN THEY\nDIE, NOW. WIPING THE SWEAT FROM YOUR\nFOREHEAD YOU BEGIN THE LONG TREK BACK\nHOME. REBUILDING EARTH OUGHT TO BE A\nLOT MORE FUN THAN RUINING IT WAS.\n") } },
      { 31, { Game::Utilities::str_to_key<uint64_t>("RROCK13"), std::string("CONGRATULATIONS, YOU'VE FOUND THE SECRET\nLEVEL! LOOKS LIKE IT'S BEEN BUILT BY\nHUMANS, RATHER THAN DEMONS. YOU WONDER\nWHO THE INMATES OF THIS CORNER OF HELL\nWILL BE.") } },
      { 32, { Game::Utilities::str_to_key<uint64_t>("RROCK19"), std::string("CONGRATULATIONS, YOU'VE FOUND THE\nSUPER SECRET LEVEL!  YOU'D BETTER\nBLAZE THROUGH THIS ONE!\n") } }
    };

    // Text before these episodes
    if (_doom.level.episode.first == 1 && doom2.find(next.second) != doom2.end())
      text = doom2.find(next.second)->second;
  } break;

  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    break;
  }

  // Save references as 'this' is gonna be deleted
  auto& machine = _machine;
  auto& doom = _doom;
  auto  statistics = _doom.level.statistics;

  // Draw last frame of game
  _machine.draw();

  sf::Image start = _doom.image;

  // Reset scenes
  machine.clear();

  // Load next level
  if (next != std::pair<uint8_t, uint8_t>(0, 0)) {
    doom.setLevel(next);
    machine.push<DOOM::GameDoomScene>(doom);
  }

  // Go to main menu
  else {
    machine.push<DOOM::SplashDoomScene>(doom);
    machine.push<DOOM::MenuDoomScene>(doom);
  }

  // Push text screen
  if (text.first != 0) {
    machine.push<DOOM::TextDoomScene>(doom, text.second, text.first);
  }

  // Swap to game with intermission
  machine.push<DOOM::IntermissionDoomScene>(doom, previous, next, statistics);

  // Simulate first frame of intermission
  machine.draw();

  // Push transition
  machine.push<DOOM::TransitionDoomScene>(doom, start, doom.image);
}