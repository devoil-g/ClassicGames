#pragma once

#include "Math/Vector.hpp"
#include "System/Window.hpp"

namespace RPG
{
  class Camera
  {
  private:
    static const unsigned int ScreenWidth = 192;  // World X resolution at zoom level 1.0
    static const unsigned int ScreenHeight = 108; // World Y resolution at zoom level 1.0

    Math::Vector<2> _targetPosition;  // Target camera position
    Math::Vector<2> _currentPosition; // Current camera position
    float           _dragPosition;    // Move drag factor, used for smooth movement (time to move half the way)

    float           _targetZoom;      // Target camera zoom
    float           _currentZoom;     // Current camera zoom
    float           _dragZoom;        // Zoom drag factor, used for smooth zoom
    Math::Vector<2> _coordinatesZoom; // Coordinates of la mouse zoom on window

  public:
    Camera();
    Camera(const Camera&) = default;
    Camera(Camera&&) = default;
    ~Camera() = default;

    Camera& operator=(const Camera&) = default;
    Camera& operator=(Camera&&) = default;

    Math::Vector<2> coordsToPixel(const Math::Vector<2>& coords) const;       // Get pixel coordinates from current world position
    Math::Vector<2> pixelToCoords(const Math::Vector<2>& pixel) const;        // Get current world position from pixel coordinates
    Math::Vector<2> coordsToPixelTarget(const Math::Vector<2>& coords) const; // Get pixel coordinates from target world position
    Math::Vector<2> pixelToCoordsTarget(const Math::Vector<2>& pixel) const;  // Get target world position from pixel coordinates

    void  setPosition(const Math::Vector<2>& position);       // Set current and target camera center
    void  setPositionTarget(const Math::Vector<2>& position); // Set target camera center
    void  setPositionDrag(float drag);                        // Set move drag factor

    Math::Vector<2> getPosition() const;        // Get current position of the center of the screen
    Math::Vector<2> getPositionTarget() const;  // Get target position of the center of the screen
    float           getPositionDrag() const;    // Get position drag factor

    void  setZoom(float scale);                                           // Set current and target zoom value
    void  setZoomTarget(float scale);                                     // Set target zoom value to screen center
    void  setZoomTarget(float scale, const Math::Vector<2>& coordinates); // Set target zoom value to specific screen coordinates
    void  setZoomDrag(float drag);                                        // Set zoom drag factor

    float getZoom() const;        // Get current zoom value
    float getZoomTarget() const;  // Get target zoom value
    float getZoomDrag() const;    // Get zoom drag factor

    void                set() const;         // Set window viewport to current camera
    void                reset() const;       // Reset window viewport
    Game::Window::View  view() const;        // Get current view of camera
    Game::Window::View  viewTarget() const;  // Get target view of camera

    void  update(float elapsed);  // Compute new camera position
  };
}