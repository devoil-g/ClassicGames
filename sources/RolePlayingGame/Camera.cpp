#include <SFML/Graphics.hpp>

#include "RolePlayingGame/Camera.hpp"
#include "System/Window.hpp"

RPG::Camera::Camera() :
  _targetPosition(0.f, 0.f),
  _currentPosition(0.f, 0.f),
  _dragPosition(0.f),
  _targetZoom(1.f),
  _currentZoom(1.f),
  _dragZoom(0.f)
{}

Math::Vector<2> RPG::Camera::coordsToPixel(const Math::Vector<2>& coords) const
{
  auto pixel = Game::Window::Instance().window().mapCoordsToPixel({ coords.x(), coords.y() }, view());

  return { (float)pixel.x, (float)pixel.y };
}

Math::Vector<2> RPG::Camera::pixelToCoords(const Math::Vector<2>& pixel) const
{
  auto coords = Game::Window::Instance().window().mapPixelToCoords({ (int)pixel.x(), (int)pixel.y() }, view());

  return { coords.x, coords.y };
}

Math::Vector<2> RPG::Camera::coordsToPixelTarget(const Math::Vector<2>& coords) const
{
  auto pixel = Game::Window::Instance().window().mapCoordsToPixel({ coords.x(), coords.y() }, viewTarget());

  return { (float)pixel.x, (float)pixel.y };
}

Math::Vector<2> RPG::Camera::pixelToCoordsTarget(const Math::Vector<2>& pixel) const
{
  auto coords = Game::Window::Instance().window().mapPixelToCoords({ (int)pixel.x(), (int)pixel.y() }, viewTarget());

  return { coords.x, coords.y };
}

void  RPG::Camera::setPosition(const Math::Vector<2>& position)
{
  // Handle errors
  if (std::isnan(position.x()) == true || std::isnan(position.y()) == true)
    return;

  // Set current and target
  _targetPosition = position;
  _currentPosition = _targetPosition;
}

void  RPG::Camera::setPositionTarget(const Math::Vector<2>& position)
{
  // Handle errors
  if (std::isnan(position.x()) == true || std::isnan(position.y()) == true)
    return;

  // Set target
  _targetPosition = position;

  // Instant move
  if (_dragPosition == 0.f)
    _currentPosition = _targetPosition;
}

void  RPG::Camera::setPositionDrag(float drag)
{
  // Handle errors
  if (std::isnan(drag) == true)
    return;

  // Change zoom drag factor
  _dragPosition = std::max(drag, 0.f);

  // Instant move
  if (_dragPosition == 0.f)
    _currentPosition = _targetPosition;
}

Math::Vector<2> RPG::Camera::getPosition() const { return _currentPosition; }
Math::Vector<2> RPG::Camera::getPositionTarget() const { return _targetPosition; }
float           RPG::Camera::getPositionDrag() const { return _dragPosition; }

void  RPG::Camera::setZoom(float scale)
{
  // Handle errors
  if (std::isnan(scale) == true)
    return;

  // Set current and target
  _targetZoom = std::clamp(scale, 1.f / 8.f, 8.f);
  _currentZoom = _targetZoom;
}

void  RPG::Camera::setZoomTarget(float scale)
{
  // Handle errors
  if (std::isnan(scale) == true)
    return;

  // Set target
  _targetZoom = std::clamp(scale, 1.f / 8.f, 8.f);

  // Instant zoom
  if (_dragZoom == 0.f)
    _currentZoom = _targetZoom;
}

void  RPG::Camera::setZoomDrag(float drag)
{
  // Handle errors
  if (std::isnan(drag) == true)
    return;

  // Change zoom drag factor
  _dragZoom = std::max(drag, 0.f);

  // Instant zoom
  if (_dragZoom == 0.f)
    _currentZoom = _targetZoom;
}

float RPG::Camera::getZoom() const { return _currentZoom; }
float RPG::Camera::getZoomTarget() const { return _targetZoom; }
float RPG::Camera::getZoomDrag() const { return _dragZoom; }

sf::View  RPG::Camera::view() const
{
  auto& window = Game::Window::Instance().window();
  auto size = window.getSize();

  float targetX = (float)ScreenWidth / _currentZoom;
  float targetY = (float)ScreenHeight / _currentZoom;
  float zoomX = targetX / (float)size.x;
  float zoomY = targetY / (float)size.y;
  float zoom = std::max(zoomX, zoomY);

  // Compute current view
  return sf::View(
    { _currentPosition.x(), _currentPosition.y() },
    { (float)size.x * zoom, (float)size.y * zoom }
  );
}

sf::View  RPG::Camera::viewTarget() const
{
  auto& window = Game::Window::Instance().window();
  auto size = window.getSize();

  float targetX = (float)ScreenWidth / _targetZoom;
  float targetY = (float)ScreenHeight / _targetZoom;
  float zoomX = targetX / (float)size.x;
  float zoomY = targetY / (float)size.y;
  float zoom = std::max(zoomX, zoomY);

  // Compute current view
  return sf::View(
    { _targetPosition.x(), _targetPosition.y() },
    { (float)size.x * zoom, (float)size.y * zoom }
  );
}

void  RPG::Camera::set() const
{
  auto& window = Game::Window::Instance().window();
  
  // Setup camera
  window.setView(view());
}

void  RPG::Camera::reset() const
{
  auto& window = Game::Window::Instance().window();
  auto size = window.getSize();

  // Reset window view
  window.setView(sf::View(sf::FloatRect(0, 0, (float)size.x, (float)size.y)));
}

void  RPG::Camera::update(float elapsed)
{
  float positionCompleted = (_dragPosition > 0.f) ?
    (1.f - (1.f / std::pow(2.f, elapsed / _dragPosition))) :
    (1.f);

  // Move camera
  _currentPosition += (_targetPosition - _currentPosition) * positionCompleted;
  
  float zoomCompleted = (_dragZoom > 0.f) ?
    (1.f - (1.f / std::pow(2.f, elapsed / _dragZoom))) :
    (1.f);

  // Zoom camera
  _currentZoom += (_targetZoom - _currentZoom) * zoomCompleted;
}