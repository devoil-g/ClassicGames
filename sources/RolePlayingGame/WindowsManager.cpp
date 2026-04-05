#include <stdexcept>
#include <SFML/Graphics/Sprite.hpp>

#include "RolePlayingGame/WindowsManager.hpp"

void  RPG::WindowsManager::update(float elapsed)
{
  auto& screen = Game::Window::Instance();
  auto  screenView = screen.getView();
  auto  screenMouse = screen.pixelToCoords(screen.mouse().position());
  bool  hover = true;
  bool  focus =
    screen.mouse().buttonPressed(Game::Window::MouseButton::Left) == true ||
    screen.mouse().buttonPressed(Game::Window::MouseButton::Right) == true;

  // Unfocus every window
  if (focus == true) {
    for (auto& window : _windows) {
      window->setFocus(false);
    }
  }

  // Check if mouse is hovering window
  for (auto& window : _windows)
  {
    bool contained = window->getBounds().contains(screenMouse);

    // Handle hover
    if (hover == true && contained == true)
    {
      // Hovering window
      window->setHover(true);
      hover = false;

      // Focus on hovered window
      if (focus == true) {
        window->setFocus(true);
        focus = false;
      }
    }
    else
      window->setHover(false);
  }

  // Put focused window on front
  for (auto iterator = _windows.begin(); iterator != _windows.end(); iterator++) {
    if (iterator->get()->getFocus() == true)
    {
      // Move window to front of the list
      if (iterator != _windows.begin())
        _windows.splice(_windows.begin(), _windows, iterator);
      break;
    }
  }

  // Update every window
  for (auto iterator = _windows.begin(); iterator != _windows.end();) {
    if (iterator->get()->update(elapsed) == true)
      iterator = _windows.erase(iterator);
    else
      iterator++;
  }
}

void  RPG::WindowsManager::render()
{
  // Draw each window back to front
  for (auto iterator = _windows.rbegin(); iterator != _windows.rend(); iterator++)
  {
    // Render window to target
    (*iterator)->render();
    (*iterator)->_texture.display();

    sf::Sprite  sprite((*iterator)->_texture.getTexture());

    // Set window position
    sprite.setPosition({ (*iterator)->getPosition().x(), (*iterator)->getPosition().y() });
    sprite.setOrigin({ (*iterator)->getOrigin().x(), (*iterator)->getOrigin().y() });
    sprite.setScale({ (*iterator)->getScale().x(), (*iterator)->getScale().y() });
    sprite.setRotation(sf::radians((*iterator)->getRotation().x()));

    // Draw texture to screen
    Game::Window::Instance().draw(sprite);
  }
}

sf::RenderTarget& RPG::WindowsManager::AbstractWindow::getTarget()
{
  // Return window render target
  return _texture;
}

Math::Vector<2, unsigned int> RPG::WindowsManager::AbstractWindow::getSize() const
{
  auto size = _texture.getSize();

  // Return window size
  return { size.x, size.y };
}

void  RPG::WindowsManager::AbstractWindow::setSize(const Math::Vector<2, unsigned int>& size)
{
  // Update render texture size
  if (_texture.resize({ size.x(), size.y() }) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

Math::Box<2>  RPG::WindowsManager::AbstractWindow::getBounds() const
{
  auto transformation = getTransform();

  // Get position and size from transformation matrix
  // NOTE: does not support rotation
  return {
    { transformation(2, 0), transformation(2, 1) },
    { transformation(0, 0) * getSize().x(), transformation(1, 1) * getSize().x() }
  };
}

void  RPG::WindowsManager::AbstractWindow::setFocus(bool focus)
{
  // Set focus flag
  _focus = focus;
}

void  RPG::WindowsManager::AbstractWindow::setHover(bool hover)
{
  // Set hover flag
  _hover = hover;
}

bool  RPG::WindowsManager::AbstractWindow::getFocus() const
{
  // Get focus flag
  return _focus;
}

bool  RPG::WindowsManager::AbstractWindow::getHover() const
{
  // Get hover flag
  return _hover;
}