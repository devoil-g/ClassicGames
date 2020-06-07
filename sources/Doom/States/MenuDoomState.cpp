#include <iostream>

#include "Doom/States/MenuDoomState.hpp"
#include "Doom/States/GameDoomState.hpp"
#include "Doom/Thing/PlayerThing.hpp"
#include "System/Window.hpp"

const unsigned int  DOOM::MenuDoomState::SkullDuration = 8;

DOOM::MenuDoomState::MenuDoomState(Game::StateMachine& machine, DOOM::Doom& doom) :
  Game::AbstractState(machine),
  _doom(doom),
  _episode(1),
  _backCamera(),
  _backImage(),
  _backTexture(),
  _backSprite(),

  _menuIndex(DOOM::MenuDoomState::MenuEnum::MenuMain),
  _menuCursor(1),
  _menuDesc({
    Menu{ // MenuMain
      {
        { DOOM::str_to_key("M_DOOM"), false, 94, 2, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_NGAME"), true, 97, 64, sf::Keyboard::N, [this]() { _menuIndex = MenuEpisode; _menuCursor = 1; }, []() {}, []() {} },
        { DOOM::str_to_key("M_OPTION"), true, 97, 80, sf::Keyboard::O, [this]() { _menuIndex = MenuOptions; _menuCursor = 2; }, []() {}, []() {} },
        { DOOM::str_to_key("M_LOADG"), false, 97, 96, sf::Keyboard::L, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_SAVEG"), false, 97, 112, sf::Keyboard::S, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_RDTHIS"), true, 97, 128, sf::Keyboard::R, [this]() { _menuIndex = MenuRead1; _menuCursor = 0; }, []() {}, []() {} },
        { DOOM::str_to_key("M_QUITG"), true, 97, 144, sf::Keyboard::Q, [this]() { _machine.clear(); }, []() {}, []() {} }
      },
      {},
      [this]() { _machine.pop(); }
    },
    Menu{ // MenuEpisode
      {
        { DOOM::str_to_key("M_EPISOD"), false, 54, 38, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_EPI1"), true, 48, 63, sf::Keyboard::K, [this]() { _episode = 1; _menuIndex = MenuSkill; _menuCursor = _doom.skill + 2; }, []() {}, []() {} },
        { DOOM::str_to_key("M_EPI2"), true, 48, 79, sf::Keyboard::T, [this]() { _episode = 2; _menuIndex = MenuSkill; _menuCursor = _doom.skill + 2; }, []() {}, []() {} },
        { DOOM::str_to_key("M_EPI3"), true, 48, 95, sf::Keyboard::I, [this]() { _episode = 3; _menuIndex = MenuSkill; _menuCursor = _doom.skill + 2; }, []() {}, []() {} },
        { DOOM::str_to_key("M_EPI4"), true, 48, 111, sf::Keyboard::T, [this]() { _episode = 4; _menuIndex = MenuSkill; _menuCursor = _doom.skill + 2; }, []() {}, []() {} }
      },
      {},
      [this]() { _menuIndex = MenuMain; _menuCursor = 1; }
    },
    Menu{ // MenuSkill
      {
        { DOOM::str_to_key("M_NEWG"), false, 96, 14, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_SKILL"), false, 54, 38, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_JKILL"), true, 48, 63, sf::Keyboard::I, [this]() { _doom.skill = DOOM::Enum::Skill::SkillBaby; _doom.setLevel({ _episode, 1 }); _machine.swap<DOOM::GameDoomState>(_doom); }, []() {}, []() {} },
        { DOOM::str_to_key("M_ROUGH"), true, 48, 79, sf::Keyboard::H, [this]() { _doom.skill = DOOM::Enum::Skill::SkillEasy; _doom.setLevel({ _episode, 1 }); _machine.swap<DOOM::GameDoomState>(_doom); }, []() {}, []() {} },
        { DOOM::str_to_key("M_HURT"), true, 48, 95, sf::Keyboard::H, [this]() { _doom.skill = DOOM::Enum::Skill::SkillMedium; _doom.setLevel({ _episode, 1 }); _machine.swap<DOOM::GameDoomState>(_doom); }, []() {}, []() {} },
        { DOOM::str_to_key("M_ULTRA"), true, 48, 111, sf::Keyboard::U, [this]() { _doom.skill = DOOM::Enum::Skill::SkillHard; _doom.setLevel({ _episode, 1 }); _machine.swap<DOOM::GameDoomState>(_doom); }, []() {}, []() {} },
        { DOOM::str_to_key("M_NMARE"), true, 48, 127, sf::Keyboard::N, [this]() { _doom.skill = DOOM::Enum::Skill::SkillNightmare; _doom.setLevel({ _episode, 1 }); _machine.swap<DOOM::GameDoomState>(_doom); }, []() {}, []() {} }
      },
      {},
      [this]() { _menuIndex = MenuEpisode; _menuCursor = _episode; }
    },
    Menu{ // MenuOptions
      {
        { DOOM::str_to_key("M_OPTTTL"), false, 108, 15, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_ENDGAM"), false, 60, 37, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_MESSG"), true, 60, 53, sf::Keyboard::M, [this]() { _doom.message = !_doom.message; }, []() {}, []() {} },
        { DOOM::str_to_key("M_DETAIL"), true, 60, 69, sf::Keyboard::G, []() { DOOM::Doom::RenderScale = DOOM::Doom::RenderScale % 2 + 1; }, []() {}, []() {} },
        { DOOM::str_to_key("M_SCRNSZ"), false, 60, 85, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_MSENS"), true, 60, 117, sf::Keyboard::M, [this]() { _doom.sensivity = std::clamp(((int)(_doom.sensivity * 8) + 1) / 8.f, 0.f, 1.f); }, [this]() { _doom.sensivity = std::clamp(((int)(_doom.sensivity * 8) - 1) / 8.f, 0.f, 1.f); }, [this]() { _doom.sensivity = std::clamp(((int)(_doom.sensivity * 8) + 1) / 8.f, 0.f, 1.f); } },
        { DOOM::str_to_key("M_SVOL"), true, 60, 149, sf::Keyboard::S, [this]() { _menuIndex = MenuVolume; _menuCursor = 1; }, []() {}, []() {} }
      },
      {
        { 60, 101, [this]() { return 8; } },
        { 60, 133, [this]() { return (int)(_doom.sensivity * 8); } }
      },
      [this]() { _menuIndex = MenuMain; _menuCursor = 2; }
    },
    Menu{ // MenuVolume
      {
        { DOOM::str_to_key("M_SVOL"), false, 60, 38, sf::Keyboard::Unknown, []() {}, []() {}, []() {} },
        { DOOM::str_to_key("M_SFXVOL"), true, 80, 64, sf::Keyboard::S, [this]() { _doom.sfx = std::clamp(((int)(_doom.sfx * 8) + 1) / 8.f, 0.f, 1.f); }, [this]() { _doom.sfx = std::clamp(((int)(_doom.sfx * 8) - 1) / 8.f, 0.f, 1.f); }, [this]() { _doom.sfx = std::clamp(((int)(_doom.sfx * 8) + 1) / 8.f, 0.f, 1.f); } },
        { DOOM::str_to_key("M_MUSVOL"), true, 80, 96, sf::Keyboard::M, [this]() { _doom.music = std::clamp(((int)(_doom.music * 8) + 1) / 8.f, 0.f, 1.f); }, [this]() { _doom.music = std::clamp(((int)(_doom.music * 8) - 1) / 8.f, 0.f, 1.f); }, [this]() { _doom.music = std::clamp(((int)(_doom.music * 8) + 1) / 8.f, 0.f, 1.f); } }
      },
      {
        { 80, 80, [this]() { return (int)(_doom.sfx * 8); } },
        { 80, 112, [this]() { return (int)(_doom.music * 8); } }
      },
      [this]() { _menuIndex = MenuOptions; _menuCursor = 6; }
    },
    Menu{ // MenuRead1
      {
        { DOOM::str_to_key("HELP1"), true, 0, 0, sf::Keyboard::Unknown, [this]() { _menuIndex = MenuRead2; _menuCursor = 0; }, []() {}, []() {} }
      },
      {},
      [this]() { _menuIndex = MenuMain; _menuCursor = 5; }
    },
    Menu{ // MenuRead2
      {
        { DOOM::str_to_key("HELP2"), true, 0, 0, sf::Keyboard::Unknown, [this]() { _menuIndex = MenuMain; _menuCursor = 5; }, []() {}, []() {} }
      },
      {},
      [this]() { _menuIndex = MenuMain; _menuCursor = 5; }
    }
  }),
  _menuElapsed(sf::Time::Zero),
  _menuImage(),
  _menuTexture(),
  _menuSprite()

{
  const auto  levels = _doom.getLevels();
  
  // Select a random level
  _doom.setLevel(*std::next(levels.begin(), std::rand() % levels.size()));

  std::list<const DOOM::AbstractThing*>  spawns;

  // List camera position from deathmatch spawn points
  for (const auto& thing : _doom.level.things)
    if (thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWNDM)
      spawns.emplace_back(thing.get());

  // Fallback player spawn point when no deathmatch spawn
  if (spawns.empty() == true)
    for (const auto& thing : _doom.level.things)
      if (thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN1 || 
        thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN2 || 
        thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN3 || 
        thing->type == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN4)
        spawns.emplace_back(thing.get());

  // Critical error
  if (spawns.empty() == true )
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  const auto& spawn = *std::next(spawns.begin(), std::rand() % spawns.size());

  // Select a random camera position
  _backCamera.position = spawn->position + Math::Vector<3>(0.f, 0.f, 56.f * 0.73f);
  _backCamera.angle = spawn->angle + Math::DegToRad(30.f);

  // Initialize menu image
  _menuImage.create(DOOM::Doom::RenderWidth, DOOM::Doom::RenderHeight);
  _menuTexture.create(DOOM::Doom::RenderWidth, DOOM::Doom::RenderHeight);
  _menuTexture.setSmooth(false);
  _menuSprite.setTexture(_menuTexture, true);

  // Handle menu for special versions
  switch (_doom.mode) {
  case DOOM::Enum::Mode::ModeCommercial:
    break;

    // Remove the fourth episode
  case DOOM::Enum::Mode::ModeShareware:
  case DOOM::Enum::Mode::ModeRegistered:
    _menuDesc[DOOM::MenuDoomState::MenuEnum::MenuEpisode].items.pop_back();

    // We are fine
  case DOOM::Enum::Mode::ModeRetail:
  default:
    break;
  }
}

void  DOOM::MenuDoomState::updateSelect()
{
  // Call left arrow handle
  std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->select();
}

void  DOOM::MenuDoomState::updateUp()
{
  // Find first previous selectable item
  while (std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor = Math::Modulo(--_menuCursor, (int)_menuDesc[_menuIndex].items.size()))->selectable == false);
}

void  DOOM::MenuDoomState::updateDown()
{
  // Find first next selectable item
  while (std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor = Math::Modulo(++_menuCursor, (int)_menuDesc[_menuIndex].items.size()))->selectable == false);
}

void  DOOM::MenuDoomState::updateLeft()
{
  // Call left arrow handle
  std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->left();
}

void  DOOM::MenuDoomState::updateRight()
{
  // Call right arrow handle
  std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->right();
}

void  DOOM::MenuDoomState::updateEscape()
{
  // Call ESC handle
  _menuDesc[_menuIndex].escape();
}

bool  DOOM::MenuDoomState::update(sf::Time elapsed)
{
  // Update background camera
  _backCamera.angle -= elapsed.asSeconds() * Math::Pi / 36.f;

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
      { Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovY) < +0.9f && Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovY) != 0.f, [this]() { updateUp(); } },
      { Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovY) < -0.9f && Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovY) != 0.f, [this]() { updateDown(); } },
      { Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovX) < -0.9f && Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovX) != 0.f, [this]() { updateLeft(); } },
      { Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovX) < +0.9f && Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovX) != 0.f, [this]() { updateRight(); } },
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

void	DOOM::MenuDoomState::drawMenu(const DOOM::Doom::Resources::Texture& texture, int texture_x, int texture_y, bool shadow)
{
  for (int x = 0; x < texture.width; x++) {
    for (const DOOM::Doom::Resources::Texture::Column::Span& span : texture.columns[x].spans) {
      for (int y = 0; y < span.pixels.size(); y++) {
        int pixel_x = x - texture.left + texture_x;
        int pixel_y = y + span.offset - texture.top + texture_y;
        
        // Compensate for not displayed status bar
        if (_menuIndex != MenuRead1 && _menuIndex != MenuRead2)
          pixel_y += 16;

        // Draw pixel
        if (pixel_x >= 0 && pixel_x < (int)_menuImage.getSize().x && pixel_y >= 0 && pixel_y < (int)_menuImage.getSize().y)
          _menuImage.setPixel(pixel_x, pixel_y, _doom.resources.palettes[0][span.pixels[y]]);

        // Draw shadow pixel
        if (shadow == true && pixel_x + 1 >= 0 && pixel_x + 1 < (int)_menuImage.getSize().x && pixel_y + 1 >= 0 && pixel_y + 1 < (int)_menuImage.getSize().y)
          _menuImage.setPixel(pixel_x + 1, pixel_y + 1, sf::Color(0, 0, 0, 127));
      }
    }
  }
}

void	DOOM::MenuDoomState::draw()
{
  sf::Vector2u  size = { DOOM::Doom::RenderWidth * DOOM::Doom::RenderScale, DOOM::Doom::RenderHeight * DOOM::Doom::RenderScale };

  // Update background rendering target size
  if (_backImage.getSize() != size) {
    _backImage.create(size.x, size.y);
    _backTexture.create(size.x, size.y);
    _backTexture.setSmooth(false);
    _backSprite.setTexture(_backTexture, true);
  }

  // Clear rendering targets
  std::memset((void*)_backImage.getPixelsPtr(), 0, _backImage.getSize().x * _backImage.getSize().y * sizeof(sf::Color));
  std::memset((void*)_menuImage.getPixelsPtr(), 0, _menuImage.getSize().x * _menuImage.getSize().y * sizeof(sf::Color));

  // Render background
  _backCamera.render(_doom, _backImage, sf::Rect<int16_t>(0, 0, size.x, size.y));

  // Greyscale background
  for (unsigned int y = 0; y < size.y; y++)
    for (unsigned int x = 0; x < size.x; x++) {
      sf::Color color = _backImage.getPixel(x, y);
      sf::Uint8 grey = (sf::Uint8)(((int)color.r + (int)color.g + (int)color.b) / 3);

      _backImage.setPixel(x, y, sf::Color(grey, grey, grey));
    }

  // Draw menu items
  for (const Menu::Item& item : _menuDesc[_menuIndex].items) {
    const auto& texture = _doom.resources.menus.find(item.texture);

    // Crash if no texture
    if (texture == _doom.resources.menus.end())
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Draw item
    drawMenu(texture->second, item.x, item.y, true);
  }

  const auto& sliderLeft = _doom.resources.menus.find(DOOM::str_to_key("M_THERML"));
  const auto& sliderMiddle = _doom.resources.menus.find(DOOM::str_to_key("M_THERMM"));
  const auto& sliderRight = _doom.resources.menus.find(DOOM::str_to_key("M_THERMR"));
  const auto& sliderCursor = _doom.resources.menus.find(DOOM::str_to_key("M_THERMO"));

  // Crash if missing slider texture
  if (sliderLeft == _doom.resources.menus.end() ||
    sliderMiddle == _doom.resources.menus.end() ||
    sliderRight == _doom.resources.menus.end() ||
    sliderCursor == _doom.resources.menus.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Draw menus sliders
  for (const Menu::Slider& slider : _menuDesc[_menuIndex].sliders) {
    drawMenu(sliderLeft->second, slider.x, slider.y, true);
    for (int x = 0; x <= 8; x++)
      drawMenu(sliderMiddle->second, slider.x + 8 * (x + 1), slider.y, true);
    drawMenu(sliderRight->second, slider.x + 8 * (9 + 1), slider.y, true);
    drawMenu(sliderCursor->second, slider.x + 8 * (slider.get() + 1), slider.y, false);
  }


  const auto& skull = _doom.resources.menus.find(DOOM::str_to_key((unsigned int)(_menuElapsed.asSeconds() / DOOM::Doom::Tic.asSeconds()) < SkullDuration ? "M_SKULL1" : "M_SKULL2"));

  // Crash if no skull
  if (skull == _doom.resources.menus.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Draw menu skull
  drawMenu(skull->second,
    std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->x - 32,
    std::next(_menuDesc[_menuIndex].items.begin(), _menuCursor)->y - 5,
    true);

  // Update textures
  _backTexture.update(_backImage);
  _menuTexture.update(_menuImage);

  // Compute sprite scale and position
  float	scale = std::min((float)Game::Window::Instance().window().getSize().x / (float)DOOM::Doom::RenderWidth, (float)Game::Window::Instance().window().getSize().y / ((float)DOOM::Doom::RenderHeight * DOOM::Doom::RenderStretching));
  float	pos_x = (((float)Game::Window::Instance().window().getSize().x - ((float)DOOM::Doom::RenderWidth * scale)) / 2.f);
  float	pos_y = (((float)Game::Window::Instance().window().getSize().y - ((float)DOOM::Doom::RenderHeight * scale * DOOM::Doom::RenderStretching)) / 2.f);

  // Position sprite in window
  _backSprite.setScale(sf::Vector2f(scale / DOOM::Doom::RenderScale, scale * DOOM::Doom::RenderStretching / DOOM::Doom::RenderScale));
  _backSprite.setPosition(sf::Vector2f(pos_x, pos_y));
  _menuSprite.setScale(sf::Vector2f(scale, scale * DOOM::Doom::RenderStretching));
  _menuSprite.setPosition(sf::Vector2f(pos_x, pos_y));

  // Draw sprite
  Game::Window::Instance().window().draw(_backSprite);
  Game::Window::Instance().window().draw(_menuSprite);
}