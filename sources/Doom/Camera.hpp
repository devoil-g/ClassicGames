#pragma once

#include <vector>
#include <utility>

#include <SFML/Graphics/Image.hpp>

#include "Doom/Doom.hpp"
#include "Math/Vector.hpp"

namespace DOOM
{
  class Camera
  {
  public:
    static const unsigned int LightFade = 84; // Light distance diminishing factor

    Math::Vector<3> position;     // Camera position
    float           angle;        // Camera angle [rad]
    float           orientation;  // Camera looking up/down angle [rad]
    float           fov;          // Camera field of view [rad]

    enum Special
    {
      Normal,
      LightAmplificationVisor,
      Invulnerability
    };

  private:
    static const std::array<int, 50>  _fuzztable; // Vertical offset for specter effect

    struct Pixel
    {
      int16_t segment;  // Index of segment 
      float   height;   // Height of floor/ceiling
      int16_t colormap; // Colormap index
      int16_t color;    // Index of color in colormap
    };

    std::vector<DOOM::Camera::Pixel>  _buffer;                              // Store information associated to each pixel
    std::vector<std::pair<int, int>>  _vertical;                            // Completion of columns (optimization)
    std::pair<int, int>               _horizontal;                          // Completion of rendering (optimization)
    float                             _fov2_tan;                            // Pre-computed cosinus/sinus/tangent
    float                             _horizon, _factor;                    // Pre-computed projection variables
    Math::Vector<2>                   _screen, _screen_start, _screen_end;  // Pre-computed screen space
    int                               _fuzz;                                // Current offset in fuzz table

    bool    renderNode(const DOOM::Doom& doom, sf::Rect<int16_t> rect, int extralight, DOOM::Camera::Special special, int16_t index);                                                                                                                                                 // Render level BSP tree recursively from its root node
    bool    renderSubsector(const DOOM::Doom& doom, sf::Rect<int16_t> rect, int extralight, DOOM::Camera::Special special, int16_t index);                                                                                                                                            // Iterate through seg of subsector
    bool    renderSeg(const DOOM::Doom& doom, sf::Rect<int16_t> rect, int extralight, DOOM::Camera::Special special, int16_t index);                                                                                                                                                  // Projection of segment on screen
    int16_t renderLight(const DOOM::Doom& doom, sf::Rect<int16_t> rect, DOOM::Camera::Special special, int16_t light, float distance);                                                                                                                                // Compute light level from light and distance
    void    renderTexture(const DOOM::Doom& doom, sf::Rect<int16_t> rect, int extralight, DOOM::Camera::Special special, const DOOM::Doom::Resources::Texture& texture, int column, float top, float bottom, float height, int offset_x, float offset_y, int16_t light, int16_t seg); // Draw a column from a texture
    void    renderFlat(const DOOM::Doom& doom, sf::Rect<int16_t> rect, DOOM::Camera::Special special, const DOOM::AbstractFlat& flat, int column, int start, int end, float altitude, int16_t light, int16_t seg);                                                    // Draw a column from a flat
    void    renderSky(const DOOM::Doom& doom, sf::Rect<int16_t> rect, DOOM::Camera::Special special, int column, int start, int end, float altitude, int16_t seg);                                                                                                    // Draw a column from a sky texture
    void    renderThings(const DOOM::Doom& doom, sf::Rect<int16_t> rect, DOOM::Camera::Special special);                                                                                                                                                              // Draw things of current level

  public:
    Camera();
    ~Camera() = default;

    void  render(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int extralight = 0, DOOM::Camera::Special special = DOOM::Camera::Special::Normal, int16_t palette = 0);  // Render level using resources from camera point of view in image rect
  };
}