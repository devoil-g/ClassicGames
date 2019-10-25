#ifndef _PLAYER_THING_HPP_
#define _PLAYER_THING_HPP_

#include "Doom/Camera.hpp"
#include "Doom/Statusbar.hpp"

namespace DOOM
{
  class PlayerThing : public DOOM::AbstractThing
  {
  private:
    static const float	VerticalLimit;	// Maximum vertical angle of camera
    static const float	TurningSpeed;	// Turning speed (rad/s)
    static const float	WalkingSpeed;	// Walking speed (units per frame)
    static const float	RunningSpeed;	// Running speed (units per frame)

  private:
    bool	_running;	// True if player is currently running
    
    void	updateKeyboard(DOOM::Doom & doom, sf::Time elapsed);		// Update using keyboard
    void	updateKeyboardTurn(DOOM::Doom & doom, sf::Time elapsed);	// Update using keyboard
    void	updateKeyboardMove(DOOM::Doom & doom, sf::Time elapsed);	// Update using keyboard
    void	updateController(DOOM::Doom & doom, sf::Time elapsed);		// Update using game pad
    void	updateControllerTurn(DOOM::Doom & doom, sf::Time elapsed);	// Update using game pad
    void	updateControllerMove(DOOM::Doom & doom, sf::Time elapsed);	// Update using game pad

    void	updateTurn(DOOM::Doom & doom, sf::Time elapsed, float horizontal, float vertical);	// Update player angle
    void	updateMove(DOOM::Doom & doom, sf::Time elapsed, Math::Vector<2> movement);		// Update player position
    void	updateUse(DOOM::Doom & doom, sf::Time elapsed);						// Perform use action
    void	updateFire(DOOM::Doom & doom, sf::Time elapsed);					// Perform gunfire action

  public:
    const int		id;		// Player ID
    const int		controller;	// Controller used by player
    DOOM::Camera	camera;		// Player camera
    DOOM::Statusbar	statusbar;	// Player status bar
    DOOM::Enum::Armor	armor;		// Player armor type

    PlayerThing(DOOM::Doom & doom, int id, int controller);
    ~PlayerThing() = default;

    bool	update(DOOM::Doom & doom, sf::Time elapsed) override;	// Update player using controller, alway return false as a player thing is never deleted
  };
};

#endif