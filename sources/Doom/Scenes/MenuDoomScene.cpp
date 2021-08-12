#include <iostream>

#include "Doom/Scenes/MenuDoomScene.hpp"
#include "Doom/Scenes/GameDoomScene.hpp"
#include "Doom/Scenes/TransitionDoomScene.hpp"
#include "Doom/Thing/PlayerThing.hpp"
#include "System/Sound.hpp"
#include "System/Window.hpp"

const unsigned int  DOOM::MenuDoomScene::SkullDuration = 8;

DOOM::MenuDoomScene::MenuDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, bool random) :
  Game::AbstractScene(machine),
  _doom(doom),
  _episode(1),
  _camera(),
  _menuIndex(DOOM::MenuDoomScene::MenuEnum::MenuMain),
  _menuCursor(1),
  _menuDesc({
    Menu{ // MenuMain
      {
        { DOOM::str_to_key("M_DOOM"), false, 94, 2, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_NGAME"), true, 97, 64, sf::Keyboard::N, [this]() { if (_doom.mode == DOOM::Enum::Mode::ModeCommercial) { _episode = 1; _menuIndex = MenuSkill; _menuCursor = _doom.skill + 2; } else { _menuIndex = MenuEpisode; _menuCursor = 1; } _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} },
        { DOOM::str_to_key("M_OPTION"), true, 97, 80, sf::Keyboard::O, [this]() { _menuIndex = MenuOptions; _menuCursor = 2; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} },
        { DOOM::str_to_key("M_LOADG"), false, 97, 96, sf::Keyboard::L, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_SAVEG"), false, 97, 112, sf::Keyboard::S, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_RDTHIS"), true, 97, 128, sf::Keyboard::R, [this]() { _menuIndex = MenuRead1; _menuCursor = 0; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} },
        { DOOM::str_to_key("M_QUITG"), true, 97, 144, sf::Keyboard::Q, [this]() { _machine.clear(); }, []() {}, []() {} }
      },
      {},
      [this]() { _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchx); _machine.pop(); }
    },
    Menu{ // MenuEpisode
      {
        { DOOM::str_to_key("M_EPISOD"), false, 54, 38, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_EPI1"), true, 48, 63, sf::Keyboard::K, [this]() { _episode = 1; _menuIndex = MenuSkill; _menuCursor = _doom.skill + 2; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} },
        { DOOM::str_to_key("M_EPI2"), true, 48, 79, sf::Keyboard::T, [this]() { _episode = 2; _menuIndex = MenuSkill; _menuCursor = _doom.skill + 2; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} },
        { DOOM::str_to_key("M_EPI3"), true, 48, 95, sf::Keyboard::I, [this]() { _episode = 3; _menuIndex = MenuSkill; _menuCursor = _doom.skill + 2; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} },
        { DOOM::str_to_key("M_EPI4"), true, 48, 111, sf::Keyboard::T, [this]() { _episode = 4; _menuIndex = MenuSkill; _menuCursor = _doom.skill + 2; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} }
      },
      {},
      [this]() { _menuIndex = MenuMain; _menuCursor = 1; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchn); }
    },
    Menu{ // MenuSkill
      {
        { DOOM::str_to_key("M_NEWG"), false, 96, 14, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_SKILL"), false, 54, 38, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_JKILL"), true, 48, 63, sf::Keyboard::I, [this]() { _doom.skill = DOOM::Enum::Skill::SkillBaby; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); start(); }, []() {}, []() {} },
        { DOOM::str_to_key("M_ROUGH"), true, 48, 79, sf::Keyboard::H, [this]() { _doom.skill = DOOM::Enum::Skill::SkillEasy; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); start(); }, []() {}, []() {} },
        { DOOM::str_to_key("M_HURT"), true, 48, 95, sf::Keyboard::H, [this]() { _doom.skill = DOOM::Enum::Skill::SkillMedium; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); start(); }, []() {}, []() {} },
        { DOOM::str_to_key("M_ULTRA"), true, 48, 111, sf::Keyboard::U, [this]() { _doom.skill = DOOM::Enum::Skill::SkillHard; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); start(); }, []() {}, []() {} },
        { DOOM::str_to_key("M_NMARE"), true, 48, 127, sf::Keyboard::N, [this]() { _doom.skill = DOOM::Enum::Skill::SkillNightmare; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); start(); }, []() {}, []() {} }
      },
      {},
      [this]() { _menuIndex = MenuEpisode; _menuCursor = _episode; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchn); }
    },
    Menu{ // MenuOptions
      {
        { DOOM::str_to_key("M_OPTTTL"), false, 108, 15, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_ENDGAM"), false, 60, 37, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_MESSG"), true, 60, 53, sf::Keyboard::M, [this]() { _doom.message = !_doom.message; std::next(_menuDesc[MenuOptions].items.begin(), 3)->texture = DOOM::str_to_key(_doom.message == true ? "M_MSGON" : "M_MSGOFF"); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} },
        { DOOM::str_to_key(_doom.message == true ? "M_MSGON" : "M_MSGOFF"), false, 180, 53, sf::Keyboard::Unknown, [this]() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_DETAIL"), true, 60, 69, sf::Keyboard::G, [this]() { DOOM::Doom::RenderScale = DOOM::Doom::RenderScale % 2 + 1; std::next(_menuDesc[MenuOptions].items.begin(), 5)->texture = DOOM::str_to_key(DOOM::Doom::RenderScale == 1 ? "M_GDLOW" : "M_GDHIGH"); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} },
        { DOOM::str_to_key(DOOM::Doom::RenderScale == 1 ? "M_GDLOW" : "M_GDHIGH"), false, 235, 69, sf::Keyboard::Unknown, [this]() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_SCRNSZ"), false, 60, 85, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_MSENS"), true, 60, 117, sf::Keyboard::M, [this]() { _doom.sensivity = std::clamp(((int)(_doom.sensivity * 8) + 1) / 8.f, 0.f, 1.f); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov); }, [this]() { _doom.sensivity = std::clamp(((int)(_doom.sensivity * 8) - 1) / 8.f, 0.f, 1.f); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov); }, [this]() { _doom.sensivity = std::clamp(((int)(_doom.sensivity * 8) + 1) / 8.f, 0.f, 1.f); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov); } },
        { DOOM::str_to_key("M_SVOL"), true, 60, 149, sf::Keyboard::S, [this]() { _menuIndex = MenuVolume; _menuCursor = 1; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} }
      },
      {
        { 60, 101, [this]() { return 8; } },
        { 60, 133, [this]() { return (int)(_doom.sensivity * 8); } }
      },
      [this]() { _menuIndex = MenuMain; _menuCursor = 2; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchn); }
    },
    Menu{ // MenuVolume
      {
        { DOOM::str_to_key("M_SVOL"), false, 60, 38, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_SFXVOL"), true, 80, 64, sf::Keyboard::S, [this]() { _doom.sfx = std::clamp(((int)(_doom.sfx * 8) + 1) / 8.f, 0.f, 1.f); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov); }, [this]() { _doom.sfx = std::clamp(((int)(_doom.sfx * 8) - 1) / 8.f, 0.f, 1.f); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov); }, [this]() { _doom.sfx = std::clamp(((int)(_doom.sfx * 8) + 1) / 8.f, 0.f, 1.f); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov); } },
        { DOOM::str_to_key("M_MUSVOL"), true, 80, 96, sf::Keyboard::M, [this]() { _doom.music = std::clamp(((int)(_doom.music * 8) + 1) / 8.f, 0.f, 1.f); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov); }, [this]() { _doom.music = std::clamp(((int)(_doom.music * 8) - 1) / 8.f, 0.f, 1.f); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov); }, [this]() { _doom.music = std::clamp(((int)(_doom.music * 8) + 1) / 8.f, 0.f, 1.f); _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_stnmov); } }
      },
      {
        { 80, 80, [this]() { return (int)(_doom.sfx * 8); } },
        { 80, 112, [this]() { return (int)(_doom.music * 8); } }
      },
      [this]() { _menuIndex = MenuOptions; _menuCursor = 6; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchn); }
    },
    Menu{ // MenuRead1
      {
        { DOOM::str_to_key("HELP1"), true, 0, 0, sf::Keyboard::Unknown, [this]() { _menuIndex = MenuRead2; _menuCursor = 0; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} }
      },
      {},
      [this]() { _menuIndex = MenuMain; _menuCursor = 5; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchn); }
    },
    Menu{ // MenuRead2
      {
        { DOOM::str_to_key("HELP2"), true, 0, 0, sf::Keyboard::Unknown, [this]() { _menuIndex = MenuMain; _menuCursor = 5; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol); }, []() {}, []() {} }
      },
      {},
      [this]() { _menuIndex = MenuMain; _menuCursor = 5; _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchn); }
    }
  }),
  _menuElapsed(sf::Time::Zero),
  _menuImage()
{
  // Select a random level
  if (random == true) {
    const auto  levels = _doom.getLevels();

    _doom.setLevel(*std::next(levels.begin(), std::rand() % levels.size()));
  }

  std::list<const DOOM::AbstractThing*>  spawns;

  // List camera position from deathmatch spawn points
  for (const auto& thing : _doom.level.things)
    if (thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWNDM)
      spawns.emplace_back(thing.get());

  // Fallback to player spawn point when no deathmatch spawn
  if (spawns.empty() == true)
    for (const auto& thing : _doom.level.things)
      if (thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN1 || 
        thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN2 || 
        thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN3 ||
        thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN4)
        spawns.emplace_back(thing.get());

  // Critical error
  if (spawns.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  const auto& spawn = *std::next(spawns.begin(), std::rand() % spawns.size());

  // Select a random camera position
  _camera.position = spawn->position + Math::Vector<3>(0.f, 0.f, 56.f * 0.73f);
  _camera.angle = spawn->angle + Math::DegToRad(30.f);

  // Handle menu for special versions
  switch (_doom.mode) {
  case DOOM::Enum::Mode::ModeCommercial:
    // TODO: probably something ot do with DOOM II
    // NOTE: WTF am I suppose to do here?! m_menu.c:1867
    // This seems to be an hack for ReadThis! screen on some version,
    // additional checks below fix this problem
    // Remove the fourth episode
    _menuDesc[DOOM::MenuDoomScene::MenuEnum::MenuEpisode].items.pop_back();
    break;

  case DOOM::Enum::Mode::ModeShareware:
  case DOOM::Enum::Mode::ModeRegistered:
    // Remove the fourth episode
    _menuDesc[DOOM::MenuDoomScene::MenuEnum::MenuEpisode].items.pop_back();
    break;

    // We are fine
  case DOOM::Enum::Mode::ModeRetail:
  default:
    break;
  }

  // Additional check for help screen
  if (_doom.resources.menus.find(_menuDesc[MenuRead1].items.front().texture) == _doom.resources.menus.end())
    _menuDesc[MenuRead1].items.front().texture = DOOM::str_to_key("HELP");
  if (_doom.resources.menus.find(_menuDesc[MenuRead2].items.front().texture) == _doom.resources.menus.end())
    _menuDesc[MenuRead2].items.front().texture = DOOM::str_to_key("CREDIT");

  // Initialize pre-rendering target of menu
  _menuImage.create(DOOM::Doom::RenderWidth, DOOM::Doom::RenderHeight);
}

void  DOOM::MenuDoomScene::start()
{
  // Copy current screen to buffer
  sf::Image start(_doom.image);

  // Load requested level
  switch (_doom.mode) {
  case DOOM::Enum::Mode::ModeRegistered:
  case DOOM::Enum::Mode::ModeRetail:
  case DOOM::Enum::Mode::ModeShareware:
    _doom.setLevel({ _episode, 1 }, true);
    break;
  case DOOM::Enum::Mode::ModeCommercial:
    if (_episode == 1)
      _doom.setLevel({ 1, 1 }, true);
    else if (_episode == 2)
      _doom.setLevel({ 1, 12 }, true);
    else if (_episode == 3)
      _doom.setLevel({ 1, 21 }, true);
    else
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    break;
  default:
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // Save references as 'this' is gonna be deleted
  Game::SceneMachine& machine = _machine;
  DOOM::Doom&         doom = _doom;

  // Push game state
  machine.clear();
  machine.push<DOOM::GameDoomScene>(doom);

  // Draw first image of game
  machine.draw();

  // Push transition
  machine.push<DOOM::TransitionDoomScene>(doom, start, doom.image);
}

void  DOOM::MenuDoomScene::updateSelect()
{
  // Call left arrow handle
  std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->select();
}

void  DOOM::MenuDoomScene::updateUp()
{
  int start = _menuCursor;

  // Find first previous selectable item
  while (std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor = Math::Modulo(--_menuCursor, (int)_menuDesc[_menuIndex].items.size()))->selectable == false);

  // Play sound
  if (start != _menuCursor)
    _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
}

void  DOOM::MenuDoomScene::updateDown()
{
  int start = _menuCursor;

  // Find first next selectable item
  while (std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor = Math::Modulo(++_menuCursor, (int)_menuDesc[_menuIndex].items.size()))->selectable == false);

  // Play sound
  if (start != _menuCursor)
    _doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pstop);
}

void  DOOM::MenuDoomScene::updateLeft()
{
  // Call left arrow handle
  std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->left();
}

void  DOOM::MenuDoomScene::updateRight()
{
  // Call right arrow handle
  std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->right();
}

void  DOOM::MenuDoomScene::updateEscape()
{
  // Call ESC handle
  _menuDesc[_menuIndex].escape();
}

bool  DOOM::MenuDoomScene::update(sf::Time elapsed)
{
  // Update background camera
  _camera.angle -= elapsed.asSeconds() * Math::Pi / 36.f;

  // Update skull animation
  _menuElapsed = sf::microseconds((_menuElapsed.asMicroseconds() + elapsed.asMicroseconds()) % (DOOM::Doom::Tic.asMicroseconds() * SkullDuration * 2));

  int controller = _doom.level.players.front().get().controller;

  // List of actions
  const std::list<std::pair<bool, std::function<void()>>> actions =
    controller == 0 ?
    std::list<std::pair<bool, std::function<void()>>>{
      { Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true || Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true, [this]() { updateSelect(); } },
      { Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Up) == true || Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Z) == true, [this]() { updateUp(); } },
      { Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Down) == true || Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true, [this]() { updateDown(); } },
      { Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Left) == true || Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Q) == true, [this]() { updateLeft(); } },
      { Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Right) == true || Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::D) == true, [this]() { updateRight(); } },
      { Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) == true, [this]() { updateEscape(); } },
    }
    :
    std::list<std::pair<bool, std::function<void()>>>{
      { Game::Window::Instance().joystick().buttonPressed(controller - 1, 0) == true, [this]() { updateSelect(); } },
      { Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovY) > +0.9f && Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovY) != 0.f, [this]() { updateUp(); } },
      { Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovY) < -0.9f && Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovY) != 0.f, [this]() { updateDown(); } },
      { Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovX) < -0.9f && Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovX) != 0.f, [this]() { updateLeft(); } },
      { Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovX) > +0.9f && Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovX) != 0.f, [this]() { updateRight(); } },
      { Game::Window::Instance().joystick().buttonPressed(controller - 1, 1) == true, [this]() { updateEscape(); } },
    };

  // Execute first available action
  for (const auto& action : actions) {
    if (action.first == true) {
      action.second();
      return false;
    }
  }

  return false;
}

void  DOOM::MenuDoomScene::draw()
{
  sf::Vector2u  size = { DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale };

  // Update rendering target size
  if (_doom.image.getSize() != size)
    _doom.image.create(size.x, size.y);

  // Clear rendering targets
  std::memset((void*)_doom.image.getPixelsPtr(), 0, _doom.image.getSize().x * _doom.image.getSize().y * sizeof(sf::Color));

  // Render background
  _camera.render(_doom, _doom.image, sf::Rect<int16_t>(0, 0, size.x, size.y));

  // Greyscale background
  for (unsigned int y = 0; y < size.y; y++)
    for (unsigned int x = 0; x < size.x; x++) {
      sf::Color color = _doom.image.getPixel(x, y);
      sf::Uint8 grey = (sf::Uint8)(((int)color.r + (int)color.g + (int)color.b) / 3);

      _doom.image.setPixel(x, y, sf::Color(grey, grey, grey));
    }

  // Clear menu rendering target
  std::memset((void*)_menuImage.getPixelsPtr(), 0, _menuImage.getSize().x * _menuImage.getSize().y * sizeof(sf::Color));

  // Compensate for not displayed status bar
  int offset_y = (_menuIndex != MenuRead1 && _menuIndex != MenuRead2) ? 16 : 0;

  // Draw menu items
  for (const Menu::Item& item : _menuDesc[_menuIndex].items) {
    // Draw item
    _doom.resources.getMenu(item.texture).draw(_doom, _menuImage, { item.x, item.y + offset_y }, { 1, 1 });
  }

  const auto& sliderLeft = _doom.resources.getMenu(DOOM::str_to_key("M_THERML"));
  const auto& sliderMiddle = _doom.resources.getMenu(DOOM::str_to_key("M_THERMM"));
  const auto& sliderRight = _doom.resources.getMenu(DOOM::str_to_key("M_THERMR"));
  const auto& sliderCursor = _doom.resources.getMenu(DOOM::str_to_key("M_THERMO"));

  // Draw menus sliders
  for (const auto& slider : _menuDesc[_menuIndex].sliders) {
    sliderLeft.draw(_doom, _menuImage, { slider.x, slider.y + offset_y }, { 1, 1 });
    for (int x = 0; x <= 8; x++)
      sliderMiddle.draw(_doom, _menuImage, { slider.x + 8 * (x + 1), slider.y + offset_y }, { 1, 1 });
    sliderRight.draw(_doom, _menuImage, { slider.x + 8 * (9 + 1), slider.y + offset_y }, { 1, 1 });
    sliderCursor.draw(_doom, _menuImage, { slider.x + 8 * (slider.get() + 1), slider.y + offset_y }, { 1, 1 });
  }

  // Draw menu skull
  _doom.resources.getMenu(DOOM::str_to_key((unsigned int)(_menuElapsed.asSeconds() / DOOM::Doom::Tic.asSeconds()) < SkullDuration ? "M_SKULL1" : "M_SKULL2")).draw(_doom, _menuImage,
    {
      std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->x - 32,
      std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->y - 5 + offset_y
    }, { 1, 1 });

  // Draw menu over background
  for (unsigned int y = 0; y < size.y; y++)
    for (unsigned int x = 0; x < size.x; x++) {
      sf::Color color = _menuImage.getPixel(x / DOOM::Doom::RenderScale, y / DOOM::Doom::RenderScale);
      
      // Draw only visible pixels
      if (color != sf::Color(0, 0, 0, 0)) {
        _doom.image.setPixel(x, y, color);

        // Drop shadow
        if (x + DOOM::Doom::RenderScale < _doom.image.getSize().x && y + DOOM::Doom::RenderScale < _doom.image.getSize().y) {
          sf::Color shadow = _doom.image.getPixel(x + DOOM::Doom::RenderScale, y + DOOM::Doom::RenderScale);

          shadow.r /= 2;
          shadow.g /= 2;
          shadow.b /= 2;

          _doom.image.setPixel(x + DOOM::Doom::RenderScale, y + DOOM::Doom::RenderScale, shadow);
        }
      }
    }
}