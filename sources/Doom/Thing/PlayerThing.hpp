#pragma once

#include "Doom/Automap.hpp"
#include "Doom/Camera.hpp"
#include "Doom/Statusbar.hpp"

namespace DOOM
{
  class PlayerThing : public DOOM::AbstractThing
  {
  private:
    static const float        VerticalLimit;    // Maximum vertical angle of camera
    static const float        TurningSpeed;     // Turning speed (rad/s)
    static const float        WalkingSpeed;     // Walking speed (units per frame)
    static const float        RunningSpeed;     // Running speed (units per frame)
    static const float        WeaponDispersion; // Default dispersion of fire
    static const unsigned int SectorSpeed;      // Player take damage every 32 tics when standing in damaging sectors

    enum WeaponState {
      State_None,
      State_LIGHTDONE,
      State_PUNCH, State_PUNCHDOWN, State_PUNCHUP, State_PUNCH1, State_PUNCH2, State_PUNCH3, State_PUNCH4, State_PUNCH5,
      State_PISTOL, State_PISTOLDOWN, State_PISTOLUP, State_PISTOL1, State_PISTOL2, State_PISTOL3, State_PISTOL4, State_PISTOLFLASH,
      State_SGUN, State_SGUNDOWN, State_SGUNUP, State_SGUN1, State_SGUN2, State_SGUN3, State_SGUN4, State_SGUN5, State_SGUN6, State_SGUN7, State_SGUN8, State_SGUN9, State_SGUNFLASH1, State_SGUNFLASH2,
      State_DSGUN, State_DSGUNDOWN, State_DSGUNUP, State_DSGUN1, State_DSGUN2, State_DSGUN3, State_DSGUN4, State_DSGUN5, State_DSGUN6, State_DSGUN7, State_DSGUN8, State_DSGUN9, State_DSGUN10, State_DSNR1, State_DSNR2, State_DSGUNFLASH1, State_DSGUNFLASH2,
      State_CHAIN, State_CHAINDOWN, State_CHAINUP, State_CHAIN1, State_CHAIN2, State_CHAIN3, State_CHAINFLASH1, State_CHAINFLASH2,
      State_MISSILE, State_MISSILEDOWN, State_MISSILEUP, State_MISSILE1, State_MISSILE2, State_MISSILE3, State_MISSILEFLASH1, State_MISSILEFLASH2, State_MISSILEFLASH3, State_MISSILEFLASH4,
      State_SAW, State_SAWB, State_SAWDOWN, State_SAWUP, State_SAW1, State_SAW2, State_SAW3,
      State_PLASMA, State_PLASMADOWN, State_PLASMAUP, State_PLASMA1, State_PLASMA2, State_PLASMAFLASH1, State_PLASMAFLASH2,
      State_BFG, State_BFGDOWN, State_BFGUP, State_BFG1, State_BFG2, State_BFG3, State_BFG4, State_BFGFLASH1, State_BFGFLASH2,
      State_Number
    };

    class State
    {
    public:
      using Action = void(DOOM::PlayerThing::*)(DOOM::Doom&);

      DOOM::AbstractThing::ThingSprite  sprite;     // Sprite sequence to use
      int                               frame;      // Frame of sprite to display
      bool                              brightness; // True if thing should be displayed at full brightness
      int                               duration;   // Duration of the state
      Action                            action;     // Action to be taken at the start of the state (nullptr if none)
      DOOM::PlayerThing::WeaponState    next;       // Index of the next state

      State() = default;
      ~State() = default;
    };

    class Attributs
    {
    public:
      DOOM::Enum::Ammo                ammo;   // Ammo type
      unsigned int                    count;  // Ammo count to fire
      DOOM::PlayerThing::WeaponState  up;     // Up state
      DOOM::PlayerThing::WeaponState  down;   // Down state
      DOOM::PlayerThing::WeaponState  ready;  // Ready state
      DOOM::PlayerThing::WeaponState  attack; // Attack state
      DOOM::PlayerThing::WeaponState  flash;  // Flash state

      Attributs() = default;
      ~Attributs() = default;
    };

    static const std::array<DOOM::PlayerThing::State, DOOM::PlayerThing::WeaponState::State_Number> _states;      // Table of weapon states
    static const std::array<DOOM::PlayerThing::Attributs, DOOM::Enum::Weapon::WeaponCount>          _attributs;   // Table of weapon attributes

  private:
    bool              _running;         // True if player is currently running
    bool              _automap;         // True if automap is enabled
    DOOM::Enum::Armor _armor;           // Player armor type
    sf::Time          _invulnerability; // Remaining time of invulnerability
    sf::Time          _invisibility;    // Remaining time of invisibility
    sf::Time          _light;           // Remaining time of light amplification visor
    sf::Time          _radiation;       // Remaining time of radiation suit
    sf::Time          _sector;          // Time spent in damaging sector
    bool              _berserk;         // Berserker enabled flag

    DOOM::Enum::Weapon              _weapon;          // Current type of weapon
    DOOM::Enum::Weapon              _weaponNext;      // Next weapon to use
    DOOM::PlayerThing::WeaponState  _weaponState;     // Current state of weapon
    sf::Time                        _weaponElapsed;   // Elapsed time since beginning of weapon state
    sf::Time                        _weaponRampage;   // Elapsed time attack key is down
    Game::Sound::Reference          _weaponSound;     // Sound of weapon
    Math::Vector<2>                 _weaponPosition;  // Position of weapon on screen
    bool                            _weaponRefire;    // True if shot is a refire (less accurate)
    bool                            _weaponFire;      // Fire trigger for BFG and rocket launcher

    void  setWeaponState(DOOM::Doom& doom, DOOM::PlayerThing::WeaponState state); //
    void  updateWeapon(DOOM::Doom& doom, sf::Time elapsed);                       //
    bool  setWeapon(DOOM::Enum::Weapon weapon);                                   // Attempt to equip weapon

    int                             _flash;         // Extra light level
    DOOM::PlayerThing::WeaponState  _flashState;    // Current state of light
    sf::Time                        _flashElapsed;  // Elapsed time since beginning of light state

    void  setFlashState(DOOM::Doom& doom, DOOM::PlayerThing::WeaponState state);
    void  updateFlash(DOOM::Doom& doom, sf::Time elapsed);

    sf::Time  _palettePickup;
    sf::Time  _paletteDamage;
    sf::Time  _paletteBerserk;

    void  updateKeyboard(DOOM::Doom& doom, sf::Time elapsed);         // Update using keyboard
    void  updateKeyboardTurn(DOOM::Doom& doom, sf::Time elapsed);     // Update using keyboard
    void  updateKeyboardMove(DOOM::Doom& doom, sf::Time elapsed);     // Update using keyboard
    void  updateKeyboardWeapon(DOOM::Doom& doom, sf::Time elapsed);   // Update using keyboard
    void  updateController(DOOM::Doom& doom, sf::Time elapsed);       // Update using game pad
    void  updateControllerTurn(DOOM::Doom& doom, sf::Time elapsed);   // Update using game pad
    void  updateControllerMove(DOOM::Doom& doom, sf::Time elapsed);   // Update using game pad

    void  updateTurn(DOOM::Doom& doom, sf::Time elapsed, float horizontal, float vertical); // Update player angle
    void  updateMove(DOOM::Doom& doom, sf::Time elapsed, Math::Vector<2> movement);         // Update player position
    void  updateUse(DOOM::Doom& doom, sf::Time elapsed);                                    // Perform use action
    void  updateAutomap(DOOM::Doom& doom, sf::Time elapsed);                                // Update player automap
    void  updateWeapon(DOOM::Doom& doom, sf::Time elapsed, int inc);                        // Switch to previous/next weapon
    
    void  updateInvulnerability(DOOM::Doom& doom, sf::Time elapsed);          // Update invulnerability timer
    void  updateInvisibility(DOOM::Doom& doom, sf::Time elapsed);             // Update invisibility timer
    void  updateLightAmplificationVisor(DOOM::Doom& doom, sf::Time elapsed);  // Update light amplification visor timer
    void  updateRadiationSuit(DOOM::Doom& doom, sf::Time elapsed);            // Update radation suit timer
    void  updatePalette(DOOM::Doom& doom, sf::Time elapsed);                  // Update palette timers

    bool  pickup(DOOM::Doom& doom, DOOM::AbstractThing& item) override;
    bool  pickupArmor(DOOM::Enum::Armor type);
    bool  pickupArmor(DOOM::Enum::Armor type, float quantity);
    bool  pickupHealth(unsigned int quantity, float maximum);
    bool  pickupKey(DOOM::Enum::KeyColor color, DOOM::Enum::KeyType type, bool multiplayer);
    bool  pickupBackpack();
    bool  pickupAmmo(const DOOM::Doom& doom, DOOM::Enum::Ammo type, unsigned int quantity);
    bool  pickupWeapon(DOOM::Enum::Weapon type);
    bool  pickupInvulnerability();
    bool  pickupInvisibility();
    bool  pickupLightAmplificationVisor();
    bool  pickupRadiationSuit();
    bool  pickupBerserk();
    bool  pickupComputerMap();

    void  damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, DOOM::AbstractThing& origin, float damage) override;  // Damage player, taking shield into account
    void  damageSector(DOOM::Doom& doom, sf::Time elapsed, float damage, bool end = false);                             // Receive damage from damaging sector

    DOOM::Camera::Special cameraMode() const;     // Return current camera mode
    int16_t               cameraPalette() const;  // Return current color palette

    void  A_WeaponReady(DOOM::Doom& doom);    // The player can fire the weapon or change to another weapon at this time. Follows after getting weapon up, or after previous attack/fire sequence.
    void  A_Lower(DOOM::Doom& doom);          // Lowers current weapon, and changes weapon at bottom.
    void  A_Raise(DOOM::Doom& doom);          // Raise weapon
    void  A_Punch(DOOM::Doom& doom);          // Hit monster at melee
    void  A_ReFire(DOOM::Doom& doom);         // Let the player refire before lowering weapon
    void  A_FirePistol(DOOM::Doom& doom);     // Fire with pistol
    void  A_FireShotgun(DOOM::Doom& doom);    // Fire with shotgun
    void  A_FireShotgun2(DOOM::Doom& doom);   // Fire with super shotgun
    void  A_CheckReload(DOOM::Doom& doom);    // Check for remaining ammos
    void  A_OpenShotgun2(DOOM::Doom& doom);   // Reload sound
    void  A_LoadShotgun2(DOOM::Doom& doom);   // Reload sound
    void  A_CloseShotgun2(DOOM::Doom& doom);  // Sound and reshoot
    void  A_FireCGun(DOOM::Doom& doom);       // Fire with chaingun
    void  A_FireMissile(DOOM::Doom& doom);    // Fire with rocket launcher
    void  A_Saw(DOOM::Doom& doom);            // Hit monsters with saw
    void  A_FirePlasma(DOOM::Doom& doom);     // Fire with plasma gun
    void  A_BFGsound(DOOM::Doom& doom);       // Big Fucking Gun sound
    void  A_FireBFG(DOOM::Doom& doom);        // Fire with BFG 9000
    void  A_GunFlash(DOOM::Doom& doom);       // Set attack state and flash
    void  A_Light0(DOOM::Doom& doom);         // Flash level to 0
    void  A_Light1(DOOM::Doom& doom);         // Flash level to 1
    void  A_Light2(DOOM::Doom& doom);         // Flash level to 2

    void  P_FireWeapon(DOOM::Doom& doom);                                                                             // Attempt to fire with current weapon
    bool  P_CheckAmmo(DOOM::Doom& doom);                                                                              // Returns true if there is enough ammo to shoot. If not, selects the next weapon to use.
    void  P_BringUpWeapon(DOOM::Doom& doom);                                                                          // Starts bringing the pending weapon up from the bottom of the screen.
    void  P_GunShot(DOOM::Doom& doom, float dispersion, float damage);                                                // Simple gun shot
    void  P_NoiseAlert(DOOM::Doom& doom, int16_t sector_index, int limit = 2);                                        // Make player sound target of adjacent sectors
    void  P_NoiseAlert(DOOM::Doom& doom, int16_t sector_index, int limit, std::unordered_map<int16_t, int>& sectors); // Make player sound target of adjacent sectors

    enum Control
    {
      ControlAttack,    // Attack/fire button
      ControlUse,       // Use button
      ControlRun,       // Sprint button
      ControlNext,      // Next weapon
      ControlPrevious,  // Previous weapon
      ControlAutomap,   // Toogle automap
      ControlMode,      // Change automap mode
      ControlGrid,      // Toogle automap grid
      ControlZoom,      // Zoom automap
      ControlUnzoom,    // Unzoom automap

      ControlCount
    };

    bool  control(DOOM::PlayerThing::Control action, bool pressed = false);

    void  drawCamera(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette);     // Render player camera
    void  drawWeapon(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette);     // Render player weapon and muzzle flash
    void  drawStatusbar(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette);  // Render player statusbar
    void  drawAutomap(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette);    // Render player automap

  public:
    const int         id;         // Player ID
    const int         controller; // Controller used by player
    DOOM::Camera      camera;     // Player camera
    DOOM::Statusbar   statusbar;  // Player status bar
    DOOM::Automap     automap;    // Player automap

    PlayerThing(DOOM::Doom& doom, int id, int controller);
    ~PlayerThing() = default;

    void  reset(DOOM::Doom& doom, bool hard = false);           // Reset player to begin level

    bool  update(DOOM::Doom& doom, sf::Time elapsed) override;  // Update player using controller, alway return false as a player thing is never deleted
    bool  key(DOOM::Enum::KeyColor color) const override;       // Return true if player has the key

    void  draw(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale);  // Render player on target
  };
}