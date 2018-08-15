#ifndef _DOOM_STATE_HPP_
#define _DOOM_STATE_HPP_

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System.hpp>

#include "Doom/Camera.hpp"
#include "Doom/Doom.hpp"
#include "States/AbstractState.hpp"
#include "States/LoadingState.hpp"
#include "Math/Vector.hpp"
  
namespace Game
{
  class DoomState : public Game::AbstractState
  {
  private:
    sf::Time		_elapsed;	// Elapsed time since creation
    DOOM::Doom		_doom;		// Instance of current level
    DOOM::Camera	_camera;	// Camera to render level

    bool	updatePlayer(sf::Time);

    void	drawCamera();	// Render cameras
    void	drawImage();	// Draw rendered image to screen

  public:
    DoomState(std::array<int, 4> controllers);
    ~DoomState() override;

    bool	update(sf::Time) override;	// Update state
    void	draw() override;		// Draw state
  };
};

#endif
