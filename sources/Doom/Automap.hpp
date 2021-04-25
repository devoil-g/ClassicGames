#pragma once

#include <SFML/Graphics/Image.hpp>

#include "Doom/Doom.hpp"

namespace DOOM
{
  class Automap
  {
  public:
    enum Mode
    {
      ModeFollow,   // Camera follow the player
      ModeRotation, // Camera follow the player and rotate
      ModeScroll,   // Player can scroll the map using arrows

      ModeCount     // Number of camera mode
    };

    Math::Vector<2>     position; // Map camera position
    float               angle;    // Map camera orientation
    float               zoom;     // Map camera zoom factor
    DOOM::Automap::Mode mode;     // Current camera mode

  private:
    enum Color
    {
      ColorSolid = 176,   // Solid walls
      ColorCeiling = 231, // Ceiling changes
      ColorFloor = 64,    // Floor changes
      ColorMap = 96,      // Not yet seen (computer map)
      ColorPlayer = 4     // Players color
    };

    void            renderLine(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, Math::Vector<2> point_a, Math::Vector<2> point_b, DOOM::Automap::Color color, int16_t palette) const;
    Math::Vector<2> renderTransform(sf::Rect<int16_t> rect, unsigned int scale, const Math::Vector<2>& point) const;  // Transform point to map point of view

  public:
    Automap();
    Automap(const Automap&) = delete;
    ~Automap() = default;

    void  render(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette) const;  // Render automap to target image
  };
}