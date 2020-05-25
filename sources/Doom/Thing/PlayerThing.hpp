#pragma once

#include "Doom/Camera.hpp"
#include "Doom/Statusbar.hpp"

namespace DOOM
{
  class PlayerThing : public DOOM::AbstractThing
  {
  private:
    static const float  VerticalLimit;  // Maximum vertical angle of camera
    static const float  TurningSpeed;   // Turning speed (rad/s)
    static const float  WalkingSpeed;   // Walking speed (units per frame)
    static const float  RunningSpeed;   // Running speed (units per frame)

  private:
    bool  _running; // True if player is currently running

    void  updateKeyboard(DOOM::Doom& doom, sf::Time elapsed);       // Update using keyboard
    void  updateKeyboardTurn(DOOM::Doom& doom, sf::Time elapsed);   // Update using keyboard
    void  updateKeyboardMove(DOOM::Doom& doom, sf::Time elapsed);   // Update using keyboard
    void  updateController(DOOM::Doom& doom, sf::Time elapsed);     // Update using game pad
    void  updateControllerTurn(DOOM::Doom& doom, sf::Time elapsed); // Update using game pad
    void  updateControllerMove(DOOM::Doom& doom, sf::Time elapsed); // Update using game pad

    void  updateTurn(DOOM::Doom& doom, sf::Time elapsed, float horizontal, float vertical); // Update player angle
    void  updateMove(DOOM::Doom& doom, sf::Time elapsed, Math::Vector<2> movement);         // Update player position
    void  updateUse(DOOM::Doom& doom, sf::Time elapsed);                                    // Perform use action
    void  updateFire(DOOM::Doom& doom, sf::Time elapsed);                                   // Perform gunfire action

    bool  pickup(DOOM::Doom& doom, DOOM::AbstractThing& item) override;

    bool  pickupArmor(DOOM::Enum::Armor type);
    bool  pickupArmor(DOOM::Enum::Armor type, unsigned int quantity);
    bool  pickupHealth(unsigned int quantity);
    bool  pickupKey(DOOM::Enum::KeyColor color, DOOM::Enum::KeyType type);

    void  damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, DOOM::AbstractThing& origin, int damage) override;  // Damage player, taking shield into account

  public:
    const int         id;         // Player ID
    const int         controller; // Controller used by player
    DOOM::Camera      camera;     // Player camera
    DOOM::Statusbar   statusbar;  // Player status bar
    DOOM::Enum::Armor armor;      // Player armor type

    PlayerThing(DOOM::Doom& doom, int id, int controller);
    ~PlayerThing() = default;

    bool  update(DOOM::Doom& doom, sf::Time elapsed) override;  // Update player using controller, alway return false as a player thing is never deleted

    bool  key(DOOM::Enum::KeyColor color) const override; // Return true if player has the key
  };
}