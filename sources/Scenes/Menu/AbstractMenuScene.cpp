#include "Scenes/Menu/AbstractMenuScene.hpp"
#include <SFML/Graphics.hpp>

#include "Scenes/SceneMachine.hpp"
#include "System/Window.hpp"
#include "Math/Math.hpp"

Game::AbstractMenuScene::AbstractMenuScene(Game::SceneMachine& machine) :
  Game::AbstractScene(machine),
  _title(Game::FontLibrary::Instance().get(Game::Config::ExecutablePath / "assets" / "fonts" / "04b03.ttf")),
  _items(),
  _footer("", [](Item&) {}),
  _select(-1),
  _target(0),
  _scroll(0.f)
{
  // Default title and footer
  title("");
  footer("Return", [this](Item&) { _machine.pop(); });
}

Game::AbstractMenuScene::~AbstractMenuScene() = default;

void  Game::AbstractMenuScene::title(const std::string& string)
{
  // Change text of title
  _title.setString(string);
  _title.setCharacterSize(128);
}

void  Game::AbstractMenuScene::add(const std::string& string, const std::function<void(Item&)>& callback)
{
  // Add new item to menu
  _items.emplace_back(string, callback);
  _items.back().setCharacterSize(128);
}

void  Game::AbstractMenuScene::footer(const std::string& string, const std::function<void(Item&)>& callback)
{
  // Change footer menu
  _footer = Item(string, callback);
  _footer.setCharacterSize(128);
}

void  Game::AbstractMenuScene::clear()
{
  // Clear menu
  _items.clear();
  _select = -1;
  _scroll = 0.f;
}

bool  Game::AbstractMenuScene::empty() const
{
  // Return true if menu is empty
  return _items.empty();
}

bool  Game::AbstractMenuScene::update(float elapsed)
{
  // Return to previous menu
  if (Game::Window::Instance().mouse().buttonPressed(Game::Window::MouseButton::Right) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Escape) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 1) == true)
  {
    _machine.pop();
    return false;
  }

  // Move down in menu with keyboard or joystick
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Down) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::S) == true ||
    Game::Window::Instance().mouse().wheel() < 0 ||
    (Game::Window::Instance().joystick().relative(0, Game::Window::JoystickAxis::PovY) < -0.9f && Game::Window::Instance().joystick().position(0, Game::Window::JoystickAxis::PovY) < -0.1f)) {
    if (_select == -1)
      _target = _select = _target % _items.size();
    else
      _target = _select = Math::Modulo((int)_select + 1, (int)_items.size() + 1);
  }

  // Move up in menu with keyboard or joystick
  if (Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Up) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Z) == true ||
    Game::Window::Instance().mouse().wheel() > 0 ||
    (Game::Window::Instance().joystick().relative(0, Game::Window::JoystickAxis::PovY) > +0.9f && Game::Window::Instance().joystick().position(0, Game::Window::JoystickAxis::PovY) > +0.1f)) {
    if (_select == -1)
      _target = _select = _target % _items.size();
    else
      _target = _select = Math::Modulo((int)_select - 1, (int)_items.size() + 1);
  }

  // Select menu if mouse cursor is over when moving
  if ((Game::Window::Instance().mouse().relative().x() != 0 || Game::Window::Instance().mouse().relative().y() != 0))
  {
    // Default to unselected
    _select = -1;

    sf::Vector2f  mouse = {
      (float)Game::Window::Instance().mouse().position().x(),
      (float)Game::Window::Instance().mouse().position().y()
    };

    // Menu items
    for (int index = 0; index < (int)_items.size(); index++)
      if (_items[index].getGlobalBounds().contains(mouse) == true)
        _target = _select = index;

    // Menu footer
    if (_footer.getGlobalBounds().contains(mouse) == true)
      _target = _select = (int)_items.size();
  }

  // Scrolling
  if (_target != -1 && _target < _items.size()) {
    sf::FloatRect bound = _items[_target].getGlobalBounds();

    if (_target == _select)
      bound.size.y /= 1.28f;

    if (bound.position.y < Game::Window::Instance().getSize().y() / 3.f)
      _scroll += elapsed * 10.f * (Game::Window::Instance().getSize().y() / 3.f - bound.position.y);
    else if (bound.position.y + bound.size.y > Game::Window::Instance().getSize().y() * 2.f / 3.f)
      _scroll -= elapsed * 10.f * (bound.position.y + bound.size.y - Game::Window::Instance().getSize().y() * 2.f / 3.f);
  }

  // Select menu with mouse, keyboard or joystick
  if ((Game::Window::Instance().mouse().buttonReleased(Game::Window::MouseButton::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Space) == true ||
    Game::Window::Instance().keyboard().keyPressed(Game::Window::Key::Enter) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 0) == true)
    && _select != -1)
  {
    int selected = _select;

    // Unselect item
    _select = -1;

    // Menu item
    if (selected < (int)_items.size())
      _items[selected].select();
    // Footer
    else if (selected == _items.size())
      _footer.select();
  }

  return false;
}

void  Game::AbstractMenuScene::draw()
{
  drawItems();

  sf::Image   image(sf::Vector2u(1, 12));
  sf::Texture texture;
  
  image.setPixel({ 0, 0 }, sf::Color(0, 0, 0, 255));
  image.setPixel({ 0, 1 }, sf::Color(0, 0, 0, 255));
  image.setPixel({ 0, 2 }, sf::Color(0, 0, 0, 255));
  image.setPixel({ 0, 3 }, sf::Color(0, 0, 0, 0));
  image.setPixel({ 0, 4 }, sf::Color(0, 0, 0, 0));
  image.setPixel({ 0, 5 }, sf::Color(0, 0, 0, 0));
  image.setPixel({ 0, 6 }, sf::Color(0, 0, 0, 0));
  image.setPixel({ 0, 7 }, sf::Color(0, 0, 0, 0));
  image.setPixel({ 0, 8 }, sf::Color(0, 0, 0, 0));
  image.setPixel({ 0, 9 }, sf::Color(0, 0, 0, 255));
  image.setPixel({ 0, 10 }, sf::Color(0, 0, 0, 255));
  image.setPixel({ 0, 11 }, sf::Color(0, 0, 0, 255));
  if (texture.loadFromImage(image) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  texture.setSmooth(true);

  sf::Sprite  sprite(texture);

  sprite.setScale({
    (float)Game::Window::Instance().getSize().x() / (float)image.getSize().x,
    (float)Game::Window::Instance().getSize().y() / (float)image.getSize().y
    });
  Game::Window::Instance().draw(sprite);

  drawTitle();
  drawFooter();
}

void  Game::AbstractMenuScene::drawTitle()
{
  float scale = std::min(
    Game::Window::Instance().getSize().x() / (2.f * _title.getLocalBounds().size.x),
    Game::Window::Instance().getSize().y() / (8.f * _title.getLocalBounds().size.y)
  );

  // Scale title
  _title.setScale({ scale, scale });

  _title.setOrigin({
    _title.getLocalBounds().size.x / 2.f,
    _title.getLocalBounds().size.y / 2.f
    });

  // Position title
  _title.setPosition({
    Game::Window::Instance().getSize().x() / 2.f,
    Game::Window::Instance().getSize().y() * 1.f / 8.f
    });

  // Draw title
  Game::Window::Instance().draw(_title);
}

void  Game::AbstractMenuScene::drawItems()
{
  float width = 0.f;
  float height = 0.f;

  // Get biggest item
  for (const auto& item : _items) {
    width = std::max(width, item.getLocalBounds().size.x);
    height = std::max(height, item.getLocalBounds().size.y);
  }

  // Scale of items
  float scale = std::min(
    Game::Window::Instance().getSize().x() * 2.f / (3.f * width),
    Game::Window::Instance().getSize().y() / (16.f * height)
  );

  // Set position and draw menu items
  for (unsigned int index = 0; index < (int)_items.size(); index++)
  {
    // Set scale of each item
    _items[index].setScale({ scale, scale });

    // Bigger selected item
    if (index == _select)
      _items[index].scale({ 1.28f, 1.28f });

    // Center origin
    _items[index].setOrigin({ _items[index].getLocalBounds().size.x / 2.f, _items[index].getLocalBounds().size.y });

    // Position item
    _items[index].setPosition({
      Game::Window::Instance().getSize().x() / 2.f,
      Game::Window::Instance().getSize().y() / 3.f + _scroll + height * scale * 2.f * index
      });

    // Draw item
    Game::Window::Instance().draw(_items[index]);
  }
}

void  Game::AbstractMenuScene::drawFooter()
{
  float scale = std::min(
    Game::Window::Instance().getSize().x() * 2.f / (3.f * _footer.getLocalBounds().size.x),
    Game::Window::Instance().getSize().y() / (16.f * _footer.getLocalBounds().size.y)
  );

  // Set footer scale
  _footer.setScale({ scale, scale });
  if (_select == _items.size())
    _footer.scale({ 1.28f, 1.28f });

  // Center origin
  _footer.setOrigin({
    _footer.getLocalBounds().size.x / 2.f,
    _footer.getLocalBounds().size.y / 2.f
    });

  // Position footer
  _footer.setPosition({
    Game::Window::Instance().getSize().x() / 2.f,
    Game::Window::Instance().getSize().y() * 7.f / 8.f
    });

  // Draw item
  Game::Window::Instance().draw(_footer);
}