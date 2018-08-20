#ifndef _PLAYER_THING_HPP_
#define _PLAYER_THING_HPP_

#include "Doom/Camera.hpp"
#include "Doom/Thing/AbstractThing.hpp"

namespace DOOM
{
  class PlayerThing : public DOOM::AbstractThing
  {
  private:
    static const float	TurningSpeed;	// Turning speed (rad/s)
    static const float	WalkingSpeed;	// Walking speed (unit/s)
    static const float	RunningSpeed;	// Running speed (unit/s)

  private:
    
    bool		_running;	// True if player is currently running
    
    void	updateKeyboard(DOOM::Doom & doom, sf::Time elapsed);		// Update using keyboard
    void	updateKeyboardTurn(DOOM::Doom & doom, sf::Time elapsed);	// Update using keyboard
    void	updateKeyboardMove(DOOM::Doom & doom, sf::Time elapsed);	// Update using keyboard
    void	updateController(DOOM::Doom & doom, sf::Time elapsed);		// Update using game pad
    void	updateControllerTurn(DOOM::Doom & doom, sf::Time elapsed);	// Update using game pad
    void	updateControllerMove(DOOM::Doom & doom, sf::Time elapsed);	// Update using game pad

    void	updateTurn(DOOM::Doom & doom, sf::Time elapsed, float turn);			// Update player angle
    void	updateMove(DOOM::Doom & doom, sf::Time elapsed, Math::Vector<2> movement);	// Update player position
    

  public:
    const int		id;		// Player ID
    const int		controller;	// Controller used by player
    DOOM::Camera	camera;		// Player camera

    int		health, armor;			// Player's current health & armor
    int		bullet, shell, rocket, cell;	// Player's ammo

    PlayerThing(DOOM::Doom & doom, int id, int controller);
    ~PlayerThing() = default;

    bool											update(DOOM::Doom & doom, sf::Time elapsed) override;	// Update player using controller, alway return false as a player thing is never deleted
    const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	sprite(float angle) const;				// Return sprite to be displayed
  };
};

#endif