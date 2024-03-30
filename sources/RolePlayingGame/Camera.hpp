#pragma once

#include <SFML/Graphics/View.hpp>

#include "Math/Vector.hpp"

namespace RPG
{
  class Camera
  {
  private:
    static const unsigned int ScreenWidth = 128;  // World X resolution at zoom level 1.0
    static const unsigned int ScreenHeight = 72; // World Y resolution at zoom level 1.0

    Math::Vector<2> _targetPosition;  // Target camera position
    Math::Vector<2> _currentPosition; // Current camera position
    float           _dragPosition;    // Move drag factor, used for smooth movement (time to move half the way)

    float _targetZoom;  // Target camera zoom
    float _currentZoom; // Current camera zoom
    float _dragZoom;    // Zoom drag factor, used for smooth zoom

  public:
    Camera();
    Camera(const Camera&) = default;
    Camera(Camera&&) = default;
    ~Camera() = default;

    Camera& operator=(const Camera&) = default;
    Camera& operator=(Camera&&) = default;

    Math::Vector<2> coordsToPixel(const Math::Vector<2>& coords) const; // Get pixel coordinates from world position
    Math::Vector<2> pixelToCoords(const Math::Vector<2>& pixel) const;  // Get world position from pixel coordinates
    
    void  setPosition(const Math::Vector<2>& position);       // Set current and target camera center
    void  setPositionTarget(const Math::Vector<2>& position); // Set target camera center
    void  setPositionDrag(float drag);                        // Set move drag factor

    Math::Vector<2> getPosition() const;        // Get current position of the center of the screen
    Math::Vector<2> getPositionTarget() const;  // Get target position of the center of the screen
    float           getPositionDrag() const;    // Get position drag factor

    void  setZoom(float scale);         // Set current and target zoom value
    void  setZoomTarget(float scale);   // Set target zoom value
    void  setZoomDrag(float drag);      // Set zoom drag factor

    float getZoom() const;        // Get current zoom value
    float getZoomTarget() const;  // Get target zoom value
    float getZoomDrag() const;    // Get zoom drag factor

    sf::View  view() const;

    void  set() const;    // Set window viewport to camera
    void  reset() const;  // Reset window viewport

    void  update(float elapsed);  // Compute new camera position
  };
}