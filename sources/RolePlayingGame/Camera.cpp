#include <SFML/Graphics.hpp>

#include "RolePlayingGame/Camera.hpp"
#include "System/Window.hpp"

RPG::Camera::Camera() :
  _targetPosition(0.f, 0.f),
  _currentPosition(0.f, 0.f),
  _dragPosition(0.f),
  _targetZoom(1.f),
  _currentZoom(1.f),
  _dragZoom(0.f),
  _coordinatesZoom(0.f, 0.f)
{}

Math::Vector<2> RPG::Camera::coordsToPixel(const Math::Vector<2>& coords) const
{
  auto pixel = Game::Window::Instance().coordsToPixels({ coords.x(), coords.y() }, view());

  return { (float)pixel.x(), (float)pixel.y() };
}

Math::Vector<2> RPG::Camera::pixelToCoords(const Math::Vector<2>& pixel) const
{
  return Game::Window::Instance().pixelToCoords({ (int)pixel.x(), (int)pixel.y() }, view());
}

Math::Vector<2> RPG::Camera::coordsToPixelTarget(const Math::Vector<2>& coords) const
{
  auto pixel = Game::Window::Instance().coordsToPixels({ coords.x(), coords.y() }, viewTarget());

  return { (float)pixel.x(), (float)pixel.y() };
}

Math::Vector<2> RPG::Camera::pixelToCoordsTarget(const Math::Vector<2>& pixel) const
{
  return Game::Window::Instance().pixelToCoords({ (int)pixel.x(), (int)pixel.y() }, viewTarget());
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

  auto  size = Game::Window::Instance().getSize();

  // Set zoom to center of the screen
  _coordinatesZoom = { size.x() / 2.f, size.y() / 2.f };
}

void  RPG::Camera::setZoomTarget(float scale, const Math::Vector<2>& coordinates)
{
  // Set zoom target
  setZoomTarget(scale);

  // Override coordinates
  _coordinatesZoom = coordinates;
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

Game::Window::View  RPG::Camera::view() const
{
  auto  size = Game::Window::Instance().getSize();
  float targetX = ScreenWidth / _currentZoom;
  float targetY = ScreenHeight / _currentZoom;
  float zoomX = targetX / size.x();
  float zoomY = targetY / size.y();
  float zoom = std::max(zoomX, zoomY);

  // Compute current view
  return Game::Window::View(
    { _currentPosition.x() - size.x() * zoom / 2.f, _currentPosition.y() - size.y() * zoom / 2.f },
    { size.x() * zoom, size.y() * zoom }
  );
}

Game::Window::View  RPG::Camera::viewTarget() const
{
  auto  size = Game::Window::Instance().getSize();
  float targetX = ScreenWidth / _targetZoom;
  float targetY = ScreenHeight / _targetZoom;
  float zoomX = targetX / size.x();
  float zoomY = targetY / size.y();
  float zoom = std::max(zoomX, zoomY);

  // Compute current view
  return Game::Window::View(
    { _targetPosition.x() - size.x() * zoom / 2.f, _targetPosition.y() - size.y() * zoom / 2.f },
    { size.x() * zoom, size.y() * zoom }
  );
}

void  RPG::Camera::set() const
{
  // Setup camera
  Game::Window::Instance().setView(view());
}

void  RPG::Camera::reset() const
{
  auto& window = Game::Window::Instance();
  auto  size = window.getSize();

  // Reset window view
  window.setView(Game::Window::View({ 0.f, 0.f }, { (float)size.x(), (float)size.y() }));
}

void  RPG::Camera::update(float elapsed)
{
  float positionCompleted = (_dragPosition > 0.f) ?
    (1.f - (1.f / std::pow(2.f, elapsed / _dragPosition))) :
    (1.f);

  // Move camera
  _currentPosition += (_targetPosition - _currentPosition) * positionCompleted;
  
  auto oldCursor = pixelToCoords(_coordinatesZoom);
  float zoomCompleted = (_dragZoom > 0.f) ?
    (1.f - (1.f / std::pow(2.f, elapsed / _dragZoom))) :
    (1.f);
  
  // Zoom camera
  _currentZoom += (_targetZoom - _currentZoom) * zoomCompleted;

  auto newCursor = pixelToCoords(_coordinatesZoom);
  auto offsetZoom = oldCursor - newCursor;

  // Zoom move camera to mouse coordinates
  _currentPosition += offsetZoom;
  _targetPosition += offsetZoom;
}