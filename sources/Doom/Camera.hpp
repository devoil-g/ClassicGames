#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include <vector>
#include <utility>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "Doom/Doom.hpp"
#include "Math/Vector.hpp"

namespace Game
{
  class Camera
  {
  public:
    static const int	DefaultWidth = 320;	// Default rendering width size
    static const int	DefaultHeight = 180;	// Default rendering height size

    Math::Vector<2>	position;	// Camera position
    float		height;		// Camera Z position
    float		angle;		// Camera angle [rad]
    float		orientation;	// Camera looking up/down angle [rad]
    float		fov;		// Camera field of view [rad]

  private:
    sf::Image	_image;		// Rendering engine target
    sf::Texture	_texture;	// Image buffer on graphic card
    sf::Sprite	_sprite;	// Sprite covering all texture on graphic card

    std::vector<std::pair<int16_t, float>>	_sbuffer;				// Store segment / height index associated with each pixel (for sprite rendering), stored in vertical order
    std::vector<std::pair<int, int>>		_vertical;				// Completion of columns (optimization)
    std::pair<int, int>				_horizontal;				// Completion of rendering (optimization)
    float					_fov2_tan;				// Pre-computed cosinus/sinus/tangent
    float					_horizon, _factor;			// Pre-computed projection variables
    Math::Vector<2>				_screen, _screen_start, _screen_end;	// Pre-computed screen space

    bool	renderNode(const Game::Doom &, int16_t);												// Render level BSP tree recursively from its root node
    bool	renderSubsector(const Game::Doom &, int16_t);												// Iterate through seg of subsector
    bool	renderSeg(const Game::Doom &, int16_t);													// Projection of segment on screen
    void	renderTexture(const Game::Doom &, const Game::Doom::Resources::Texture &, int, float, float, float, int, int, int16_t, int16_t);	// Draw a column from a texture
    void	renderFlat(const Game::Doom &, const Game::AbstractFlat &, int, int, int, float, int16_t, int16_t);					// Draw a column from a flat
    void	renderSky(const Game::Doom &, int, int, int, float, int16_t);											// Draw a column from a sky texture
    void	renderThings(const Game::Doom &);													// Draw things of current level

  public:
    Camera();
    ~Camera();

    void	render(Game::Doom const &);	// Render level using resources from camera point of view

    // Getter/setter
    inline sf::Image &		image() { return _image; }
    inline sf::Texture &	texture() { return _texture; }
    inline sf::Sprite &		sprite() { return _sprite; }

    // Const getter
    inline const sf::Image &	image() const { return _image; }
    inline const sf::Texture &	texture() const { return _texture; }
    inline const sf::Sprite &	sprite() const { return _sprite; }
  };
};

#endif