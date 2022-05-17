#include "Scenes/Menu/AbstractMenuScene.hpp"
#include <SFML/Graphics.hpp>

#include "Scenes/SceneMachine.hpp"
#include "System/Window.hpp"
#include "Math/Math.hpp"

Game::AbstractMenuScene::AbstractMenuScene(Game::SceneMachine& machine) :
  Game::AbstractScene(machine),
  _title("", Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf")),
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

bool  Game::AbstractMenuScene::update(sf::Time elapsed)
{
  // Return to previous menu
  if (Game::Window::Instance().mouse().buttonPressed(sf::Mouse::Button::Right) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Escape) == true ||
    Game::Window::Instance().joystick().buttonPressed(0, 1) == true)
  {
    _machine.pop();
    return false;
  }

  // Move down in menu with keyboard or joystick
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Down) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::S) == true ||
    Game::Window::Instance().mouse().wheel() < 0 ||
    (Game::Window::Instance().joystick().relative(0, sf::Joystick::Axis::PovY) < -0.9f && Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) < -0.1f)) {
    if (_select == -1)
      _target = _select = 0;
    else
      _target = _select = Math::Modulo((int)_select + 1, (int)_items.size() + 1);
  }

  // Move up in menu with keyboard or joystick
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Up) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Z) == true ||
    Game::Window::Instance().mouse().wheel() > 0 ||
    (Game::Window::Instance().joystick().relative(0, sf::Joystick::Axis::PovY) > +0.9f && Game::Window::Instance().joystick().position(0, sf::Joystick::Axis::PovY) > +0.1f)) {
    if (_select == -1)
      _target = _select = 0;
    else
      _target = _select = Math::Modulo((int)_select - 1, (int)_items.size() + 1);
  }

  // Select menu if mouse cursor is over when moving
  if ((Game::Window::Instance().mouse().relative().x != 0 || Game::Window::Instance().mouse().relative().y != 0))
  {
    // Default to unselected
    _select = -1;

    sf::Vector2f  mouse = {
      (float)Game::Window::Instance().mouse().position().x,
      (float)Game::Window::Instance().mouse().position().y
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
      bound.height /= 1.28f;

    if (bound.top < Game::Window::Instance().window().getSize().y / 3.f)
      _scroll += elapsed.asSeconds() * 10.f * (Game::Window::Instance().window().getSize().y / 3.f - bound.top);
    else if (bound.top + bound.height > Game::Window::Instance().window().getSize().y * 2.f / 3.f)
      _scroll -= elapsed.asSeconds() * 10.f * (bound.top + bound.height - Game::Window::Instance().window().getSize().y * 2.f / 3.f);
  }

  // Select menu with mouse, keyboard or joystick
  if ((Game::Window::Instance().mouse().buttonReleased(sf::Mouse::Button::Left) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true ||
    Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Return) == true ||
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

  sf::Image   image;
  sf::Texture texture;
  sf::Sprite  sprite;

  image.create(1, 12);
  image.setPixel(0, 0, sf::Color(0, 0, 0, 255));
  image.setPixel(0, 1, sf::Color(0, 0, 0, 255));
  image.setPixel(0, 2, sf::Color(0, 0, 0, 255));
  image.setPixel(0, 3, sf::Color(0, 0, 0, 0));
  image.setPixel(0, 4, sf::Color(0, 0, 0, 0));
  image.setPixel(0, 5, sf::Color(0, 0, 0, 0));
  image.setPixel(0, 6, sf::Color(0, 0, 0, 0));
  image.setPixel(0, 7, sf::Color(0, 0, 0, 0));
  image.setPixel(0, 8, sf::Color(0, 0, 0, 0));
  image.setPixel(0, 9, sf::Color(0, 0, 0, 255));
  image.setPixel(0, 10, sf::Color(0, 0, 0, 255));
  image.setPixel(0, 11, sf::Color(0, 0, 0, 255));
  texture.loadFromImage(image);
  texture.setSmooth(true);
  sprite.setTexture(texture);
  sprite.setScale(
    (float)Game::Window::Instance().window().getSize().x / (float)image.getSize().x,
    (float)Game::Window::Instance().window().getSize().y / (float)image.getSize().y
  );
  Game::Window::Instance().window().draw(sprite);

  drawTitle();
  drawFooter();
}

void  Game::AbstractMenuScene::drawTitle()
{
  float	scale = std::min(
    Game::Window::Instance().window().getSize().x / (2.f * _title.getLocalBounds().width),
    Game::Window::Instance().window().getSize().y / (8.f * _title.getLocalBounds().height)
  );

  // Scale title
  _title.setScale({ scale, scale });

  // Position title
  _title.setPosition(
    Game::Window::Instance().window().getSize().x / 2.f - _title.getGlobalBounds().width / 2.f,
    Game::Window::Instance().window().getSize().y / 8.f - _title.getGlobalBounds().height / 2.f
  );

  // Draw title
  Game::Window::Instance().window().draw(_title);
}

void  Game::AbstractMenuScene::drawItems()
{
  float width = 0.f;
  float height = 0.f;

  // Get biggest item
  for (const auto& item : _items) {
    width = std::max(width, item.getLocalBounds().width);
    height = std::max(height, item.getLocalBounds().height);
  }

  // Scale of items
  float scale = std::min(
    Game::Window::Instance().window().getSize().x * 2.f / (3.f * width),
    Game::Window::Instance().window().getSize().y / (16.f * height)
  );

  // Set position and draw menu items
  for (unsigned int index = 0; index < (int)_items.size(); index++)
  {
    // Set scale of each item
    _items[index].setScale({ scale, scale });

    // Bigger selected item
    if (index == _select)
      _items[index].scale({ 1.28f, 1.28f });

    // Position item
    _items[index].setPosition(
      (Game::Window::Instance().window().getSize().x - _items[index].getGlobalBounds().width) / 2.f,
      Game::Window::Instance().window().getSize().y / 3.f
      + height * scale / 2.f
      + _scroll
      + height * scale * 2.f * index
    );

    // Draw item
    Game::Window::Instance().window().draw(_items[index]);
  }
}

void  Game::AbstractMenuScene::drawFooter()
{
  float	scale = std::min(
    Game::Window::Instance().window().getSize().x * 2.f / (3.f * _footer.getLocalBounds().width),
    Game::Window::Instance().window().getSize().y / (16.f * _footer.getLocalBounds().height)
  );

  // Set footer scale
  _footer.setScale({ scale, scale });
  if (_select == _items.size())
    _footer.scale({ 1.28f, 1.28f });

  // Position footer
  _footer.setPosition(
    (Game::Window::Instance().window().getSize().x - _footer.getGlobalBounds().width) / 2.f,
    Game::Window::Instance().window().getSize().y * 7.f / 8.f - _footer.getGlobalBounds().height / 2.f
  );

  // Draw item
  Game::Window::Instance().window().draw(_footer);
}