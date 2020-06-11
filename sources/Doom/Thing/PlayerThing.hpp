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
    bool              _running;         // True if player is currently running
    DOOM::Enum::Armor _armor;           // Player armor type
    sf::Time          _invulnerability; // Remaining time of invulnerability
    sf::Time          _invisibility;    // Remaining time of invisibility
    sf::Time          _light;           // Remaining time of light amplification visor
    sf::Time          _radiation;       // Remaining time of radiation suit
    bool              _berserk;         // Berserker enabled flag
    bool              _map;             // Computer map enabled flag

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

    void  updateInvulnerability(DOOM::Doom& doom, sf::Time elapsed);          // Update invulnerability timer
    void  updateInvisibility(DOOM::Doom& doom, sf::Time elapsed);             // Update invisibility timer
    void  updateLightAmplificationVisor(DOOM::Doom& doom, sf::Time elapsed);  // Update light amplification visor timer
    void  updateRadiationSuit(DOOM::Doom& doom, sf::Time elapsed);            // Update radation suit timer

    bool  pickup(DOOM::Doom& doom, DOOM::AbstractThing& item) override;

    bool  pickupArmor(DOOM::Enum::Armor type);
    bool  pickupArmor(DOOM::Enum::Armor type, unsigned int quantity);
    bool  pickupHealth(unsigned int quantity, unsigned int maximum);
    bool  pickupKey(DOOM::Enum::KeyColor color, DOOM::Enum::KeyType type, bool multiplayer);
    bool  pickupBackpack();
    bool  pickupAmmo(DOOM::Enum::Ammo type, unsigned int quantity);
    bool  pickupWeapon(DOOM::Enum::Weapon type);
    bool  pickupInvulnerability();
    bool  pickupInvisibility();
    bool  pickupLightAmplificationVisor();
    bool  pickupRadiationSuit();
    bool  pickupBerserk();
    bool  pickupComputerMap();

    void  damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, DOOM::AbstractThing& origin, int damage) override;  // Damage player, taking shield into account

  public:
    const int         id;         // Player ID
    const int         controller; // Controller used by player
    DOOM::Camera      camera;     // Player camera
    DOOM::Statusbar   statusbar;  // Player status bar
    
    PlayerThing(DOOM::Doom& doom, int id, int controller);
    ~PlayerThing() = default;

    bool  update(DOOM::Doom& doom, sf::Time elapsed) override;  // Update player using controller, alway return false as a player thing is never deleted
    bool  key(DOOM::Enum::KeyColor color) const override;       // Return true if player has the key

    DOOM::Camera::Special cameraMode() const;     // Return current camera mode
    int16_t               cameraPalette() const;  // Return current color palette
  };
}