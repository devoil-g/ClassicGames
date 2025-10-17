#include <stdexcept>

#include "RolePlayingGame/WindowsManager.hpp"

/*
bool  RPG::Windows::MainWindow::update(float elapsed, const Math::Matrix<3, 3, float>& transform)
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

  return false;
}

void  RPG::Windows::render()
{
  auto& screen = Game::Window::Instance();
  auto  screenSize = screen.getSize();

  // Draw each window back to front
  for (auto iterator = _windows.rbegin(); iterator != _windows.rend(); iterator++)
  {
    auto& window = *iterator->get();
    auto windowSize = window.getSize();
    auto windowPosition = window.getPosition();
    
    // Window can't be bigger than screen
    if (windowSize.x() > screenSize.x() || windowSize.y() > screenSize.y())
      window.setSize({ 
        std::min(windowSize.x(), screenSize.x()),
        std::min(windowSize.y(), screenSize.y())
        });

    // Window can't be outside of the screen
    window.setPosition({
      std::clamp(windowPosition.x(), 0.f, (float)screenSize.x()),
      std::clamp(windowPosition.y(), 0.f, (float)screenSize.y())
      });

    // TODO: handle window scale
    
    // Render window to screen
    window.render();
  }
}

sf::RenderTarget& RPG::Windows::AbstractWindow::getTarget()
{
  // Return window render target
  return _texture;
}

const sf::Sprite& RPG::Windows::AbstractWindow::getSprite() const
{
  // Get window sprite
  return _sprite;
}

Math::Vector<2, float>  RPG::Windows::AbstractWindow::getPosition() const
{
  auto position = _sprite.getPosition();

  // Return window position
  return { position.x, position.y };
}

Math::Vector<2, unsigned int> RPG::Windows::AbstractWindow::getSize() const
{
  auto size = _texture.getSize();

  // Return window size
  return { size.x, size.y };
}

void  RPG::Windows::AbstractWindow::setSize(const Math::Vector<2, unsigned int>& size)
{
  // Update render texture size
  if (_texture.resize({ size.x(), size.y() }) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Bind texture to sprite
  _sprite.setTexture(_texture.getTexture(), true);
}

void  RPG::Windows::AbstractWindow::render()
{
  // Reset window content
  clear();



  // Draw content to render texture
  display();
}
*/