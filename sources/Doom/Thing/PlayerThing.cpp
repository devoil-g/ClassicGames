#include <iostream>
#include <functional>
#include <unordered_map>

#include "Doom/Thing/PlayerThing.hpp"
#include "System/Window.hpp"

const float         DOOM::PlayerThing::VerticalLimit = Math::DegToRad(22.5f); // 
const float         DOOM::PlayerThing::TurningSpeed = 2.f / 3.f * Math::Pi;   // 
const float         DOOM::PlayerThing::WalkingSpeed = 24.f;                   // 24 unit per frame (3 tics)
const float         DOOM::PlayerThing::RunningSpeed = 50.f;                   // 50 unit per frame (3 tics)
const float         DOOM::PlayerThing::WeaponDispersion = 32.f;               // Weapon dispersion (180° / dispersion)
const unsigned int  DOOM::PlayerThing::SectorSpeed = 32;                      // Damage every 32 tics

const std::array<DOOM::PlayerThing::State, DOOM::PlayerThing::WeaponState::State_Number>  DOOM::PlayerThing::_states =
{
  DOOM::PlayerThing::State{ Sprite_TROO, 0, false, -1, nullptr, State_None },			// State_None
  DOOM::PlayerThing::State{ Sprite_SHTG, 4, false, 0, &A_Light0, State_None },			// State_LIGHTDONE
  DOOM::PlayerThing::State{ Sprite_PUNG, 0, false, 1, &A_WeaponReady, State_PUNCH },		// State_PUNCH
  DOOM::PlayerThing::State{ Sprite_PUNG, 0, false, 1, &A_Lower, State_PUNCHDOWN },		// State_PUNCHDOWN
  DOOM::PlayerThing::State{ Sprite_PUNG, 0, false, 1, &A_Raise, State_PUNCHUP },		// State_PUNCHUP
  DOOM::PlayerThing::State{ Sprite_PUNG, 1, false, 4, nullptr, State_PUNCH2 },			// State_PUNCH1
  DOOM::PlayerThing::State{ Sprite_PUNG, 2, false, 4, &A_Punch, State_PUNCH3 },			// State_PUNCH2
  DOOM::PlayerThing::State{ Sprite_PUNG, 3, false, 5, nullptr, State_PUNCH4 },			// State_PUNCH3
  DOOM::PlayerThing::State{ Sprite_PUNG, 2, false, 4, nullptr, State_PUNCH5 },			// State_PUNCH4
  DOOM::PlayerThing::State{ Sprite_PUNG, 1, false, 5, &A_ReFire, State_PUNCH },			// State_PUNCH5
  DOOM::PlayerThing::State{ Sprite_PISG, 0, false, 1, &A_WeaponReady, State_PISTOL },		// State_PISTOL
  DOOM::PlayerThing::State{ Sprite_PISG, 0, false, 1, &A_Lower, State_PISTOLDOWN },		// State_PISTOLDOWN
  DOOM::PlayerThing::State{ Sprite_PISG, 0, false, 1, &A_Raise, State_PISTOLUP },		// State_PISTOLUP
  DOOM::PlayerThing::State{ Sprite_PISG, 0, false, 4, nullptr, State_PISTOL2 },			// State_PISTOL1
  DOOM::PlayerThing::State{ Sprite_PISG, 1, false, 6, &A_FirePistol, State_PISTOL3 },		// State_PISTOL2
  DOOM::PlayerThing::State{ Sprite_PISG, 2, false, 4, nullptr, State_PISTOL4 },			// State_PISTOL3
  DOOM::PlayerThing::State{ Sprite_PISG, 1, false, 5, &A_ReFire, State_PISTOL },		// State_PISTOL4
  DOOM::PlayerThing::State{ Sprite_PISF, 0, true, 7, &A_Light1, State_LIGHTDONE },		// State_PISTOLFLASH
  DOOM::PlayerThing::State{ Sprite_SHTG, 0, false, 1, &A_WeaponReady, State_SGUN },		// State_SGUN
  DOOM::PlayerThing::State{ Sprite_SHTG, 0, false, 1, &A_Lower, State_SGUNDOWN },		// State_SGUNDOWN
  DOOM::PlayerThing::State{ Sprite_SHTG, 0, false, 1, &A_Raise, State_SGUNUP },			// State_SGUNUP
  DOOM::PlayerThing::State{ Sprite_SHTG, 0, false, 3, nullptr, State_SGUN2 },			// State_SGUN1
  DOOM::PlayerThing::State{ Sprite_SHTG, 0, false, 7, &A_FireShotgun, State_SGUN3 },		// State_SGUN2
  DOOM::PlayerThing::State{ Sprite_SHTG, 1, false, 5, nullptr, State_SGUN4 },			// State_SGUN3
  DOOM::PlayerThing::State{ Sprite_SHTG, 2, false, 5, nullptr, State_SGUN5 },			// State_SGUN4
  DOOM::PlayerThing::State{ Sprite_SHTG, 3, false, 4, nullptr, State_SGUN6 },			// State_SGUN5
  DOOM::PlayerThing::State{ Sprite_SHTG, 2, false, 5, nullptr, State_SGUN7 },			// State_SGUN6
  DOOM::PlayerThing::State{ Sprite_SHTG, 1, false, 5, nullptr, State_SGUN8 },			// State_SGUN7
  DOOM::PlayerThing::State{ Sprite_SHTG, 0, false, 3, nullptr, State_SGUN9 },			// State_SGUN8
  DOOM::PlayerThing::State{ Sprite_SHTG, 0, false, 7, &A_ReFire, State_SGUN },			// State_SGUN9
  DOOM::PlayerThing::State{ Sprite_SHTF, 0, true, 4, &A_Light1, State_SGUNFLASH2 },		// State_SGUNFLASH1
  DOOM::PlayerThing::State{ Sprite_SHTF, 1, true, 3, &A_Light2, State_LIGHTDONE },		// State_SGUNFLASH2
  DOOM::PlayerThing::State{ Sprite_SHT2, 0, false, 1, &A_WeaponReady, State_DSGUN },		// State_DSGUN
  DOOM::PlayerThing::State{ Sprite_SHT2, 0, false, 1, &A_Lower, State_DSGUNDOWN },		// State_DSGUNDOWN
  DOOM::PlayerThing::State{ Sprite_SHT2, 0, false, 1, &A_Raise, State_DSGUNUP },		// State_DSGUNUP
  DOOM::PlayerThing::State{ Sprite_SHT2, 0, false, 3, nullptr, State_DSGUN2 },			// State_DSGUN1
  DOOM::PlayerThing::State{ Sprite_SHT2, 0, false, 7, &A_FireShotgun2, State_DSGUN3 },		// State_DSGUN2
  DOOM::PlayerThing::State{ Sprite_SHT2, 1, false, 7, nullptr, State_DSGUN4 },			// State_DSGUN3
  DOOM::PlayerThing::State{ Sprite_SHT2, 2, false, 7, &A_CheckReload, State_DSGUN5 },		// State_DSGUN4
  DOOM::PlayerThing::State{ Sprite_SHT2, 3, false, 7, &A_OpenShotgun2, State_DSGUN6 },		// State_DSGUN5
  DOOM::PlayerThing::State{ Sprite_SHT2, 4, false, 7, nullptr, State_DSGUN7 },			// State_DSGUN6
  DOOM::PlayerThing::State{ Sprite_SHT2, 5, false, 7, &A_LoadShotgun2, State_DSGUN8 },		// State_DSGUN7
  DOOM::PlayerThing::State{ Sprite_SHT2, 6, false, 6, nullptr, State_DSGUN9 },			// State_DSGUN8
  DOOM::PlayerThing::State{ Sprite_SHT2, 7, false, 6, &A_CloseShotgun2, State_DSGUN10 },	// State_DSGUN9
  DOOM::PlayerThing::State{ Sprite_SHT2, 0, false, 5, &A_ReFire, State_DSGUN },			// State_DSGUN10
  DOOM::PlayerThing::State{ Sprite_SHT2, 1, false, 7, nullptr, State_DSNR2 },			// State_DSNR1
  DOOM::PlayerThing::State{ Sprite_SHT2, 0, false, 3, nullptr, State_DSGUNDOWN },		// State_DSNR2
  DOOM::PlayerThing::State{ Sprite_SHT2, 8, true, 5, &A_Light1, State_DSGUNFLASH2 },		// State_DSGUNFLASH1
  DOOM::PlayerThing::State{ Sprite_SHT2, 9, true, 4, &A_Light2, State_LIGHTDONE },		// State_DSGUNFLASH2
  DOOM::PlayerThing::State{ Sprite_CHGG, 0, false, 1, &A_WeaponReady, State_CHAIN },		// State_CHAIN
  DOOM::PlayerThing::State{ Sprite_CHGG, 0, false, 1, &A_Lower, State_CHAINDOWN },		// State_CHAINDOWN
  DOOM::PlayerThing::State{ Sprite_CHGG, 0, false, 1, &A_Raise, State_CHAINUP },		// State_CHAINUP
  DOOM::PlayerThing::State{ Sprite_CHGG, 0, false, 4, &A_FireCGun, State_CHAIN2 },		// State_CHAIN1
  DOOM::PlayerThing::State{ Sprite_CHGG, 1, false, 4, &A_FireCGun, State_CHAIN3 },		// State_CHAIN2
  DOOM::PlayerThing::State{ Sprite_CHGG, 1, false, 0, &A_ReFire, State_CHAIN },			// State_CHAIN3
  DOOM::PlayerThing::State{ Sprite_CHGF, 0, true, 5, &A_Light1, State_LIGHTDONE },		// State_CHAINFLASH1
  DOOM::PlayerThing::State{ Sprite_CHGF, 1, true, 5, &A_Light2, State_LIGHTDONE },		// State_CHAINFLASH2
  DOOM::PlayerThing::State{ Sprite_MISG, 0, false, 1, &A_WeaponReady, State_MISSILE },		// State_MISSILE
  DOOM::PlayerThing::State{ Sprite_MISG, 0, false, 1, &A_Lower, State_MISSILEDOWN },		// State_MISSILEDOWN
  DOOM::PlayerThing::State{ Sprite_MISG, 0, false, 1, &A_Raise, State_MISSILEUP },		// State_MISSILEUP
  DOOM::PlayerThing::State{ Sprite_MISG, 1, false, 8, &A_GunFlash, State_MISSILE2 },		// State_MISSILE1
  DOOM::PlayerThing::State{ Sprite_MISG, 1, false, 12, &A_FireMissile, State_MISSILE3 },	// State_MISSILE2
  DOOM::PlayerThing::State{ Sprite_MISG, 1, false, 0, &A_ReFire, State_MISSILE },		// State_MISSILE3
  DOOM::PlayerThing::State{ Sprite_MISF, 0, true, 3, &A_Light1, State_MISSILEFLASH2 },		// State_MISSILEFLASH1
  DOOM::PlayerThing::State{ Sprite_MISF, 1, true, 4, nullptr, State_MISSILEFLASH3 },		// State_MISSILEFLASH2
  DOOM::PlayerThing::State{ Sprite_MISF, 2, true, 4, &A_Light2, State_MISSILEFLASH4 },		// State_MISSILEFLASH3
  DOOM::PlayerThing::State{ Sprite_MISF, 3, true, 4, &A_Light2, State_LIGHTDONE },		// State_MISSILEFLASH4
  DOOM::PlayerThing::State{ Sprite_SAWG, 2, false, 4, &A_WeaponReady, State_SAWB },		// State_SAW
  DOOM::PlayerThing::State{ Sprite_SAWG, 3, false, 4, &A_WeaponReady, State_SAW },		// State_SAWB
  DOOM::PlayerThing::State{ Sprite_SAWG, 2, false, 1, &A_Lower, State_SAWDOWN },		// State_SAWDOWN
  DOOM::PlayerThing::State{ Sprite_SAWG, 2, false, 1, &A_Raise, State_SAWUP },			// State_SAWUP
  DOOM::PlayerThing::State{ Sprite_SAWG, 0, false, 4, &A_Saw, State_SAW2 },			// State_SAW1
  DOOM::PlayerThing::State{ Sprite_SAWG, 1, false, 4, &A_Saw, State_SAW3 },			// State_SAW2
  DOOM::PlayerThing::State{ Sprite_SAWG, 1, false, 0, &A_ReFire, State_SAW },			// State_SAW3
  DOOM::PlayerThing::State{ Sprite_PLSG, 0, false, 1, &A_WeaponReady, State_PLASMA },		// State_PLASMA
  DOOM::PlayerThing::State{ Sprite_PLSG, 0, false, 1, &A_Lower, State_PLASMADOWN },		// State_PLASMADOWN
  DOOM::PlayerThing::State{ Sprite_PLSG, 0, false, 1, &A_Raise, State_PLASMAUP },		// State_PLASMAUP
  DOOM::PlayerThing::State{ Sprite_PLSG, 0, false, 3, &A_FirePlasma, State_PLASMA2 },		// State_PLASMA1
  DOOM::PlayerThing::State{ Sprite_PLSG, 1, false, 20, &A_ReFire, State_PLASMA },		// State_PLASMA2
  DOOM::PlayerThing::State{ Sprite_PLSF, 0, true, 4, &A_Light1, State_LIGHTDONE },		// State_PLASMAFLASH1
  DOOM::PlayerThing::State{ Sprite_PLSF, 1, true, 4, &A_Light1, State_LIGHTDONE },		// State_PLASMAFLASH2
  DOOM::PlayerThing::State{ Sprite_BFGG, 0, false, 1, &A_WeaponReady, State_BFG },		// State_BFG
  DOOM::PlayerThing::State{ Sprite_BFGG, 0, false, 1, &A_Lower, State_BFGDOWN },		// State_BFGDOWN
  DOOM::PlayerThing::State{ Sprite_BFGG, 0, false, 1, &A_Raise, State_BFGUP },			// State_BFGUP
  DOOM::PlayerThing::State{ Sprite_BFGG, 0, false, 20, &A_BFGsound, State_BFG2 },		// State_BFG1
  DOOM::PlayerThing::State{ Sprite_BFGG, 1, false, 10, &A_GunFlash, State_BFG3 },		// State_BFG2
  DOOM::PlayerThing::State{ Sprite_BFGG, 1, false, 10, &A_FireBFG, State_BFG4 },		// State_BFG3
  DOOM::PlayerThing::State{ Sprite_BFGG, 1, false, 20, &A_ReFire, State_BFG },			// State_BFG4
  DOOM::PlayerThing::State{ Sprite_BFGF, 0, true, 11, &A_Light1, State_BFGFLASH2 },		// State_BFGFLASH1
  DOOM::PlayerThing::State{ Sprite_BFGF, 1, true, 6, &A_Light2, State_LIGHTDONE }		// State_BFGFLASH2
};

const std::array<DOOM::PlayerThing::Attributs, DOOM::Enum::Weapon::WeaponCount> DOOM::PlayerThing::_attributs = {
  DOOM::PlayerThing::Attributs{ // Fist
    DOOM::Enum::Ammo::AmmoNone,
    0,
    DOOM::PlayerThing::WeaponState::State_PUNCHUP,
    DOOM::PlayerThing::WeaponState::State_PUNCHDOWN,
    DOOM::PlayerThing::WeaponState::State_PUNCH,
    DOOM::PlayerThing::WeaponState::State_PUNCH1,
    DOOM::PlayerThing::WeaponState::State_None
  },
  DOOM::PlayerThing::Attributs{ // Pistol
    DOOM::Enum::Ammo::AmmoBullet,
    1,
    DOOM::PlayerThing::WeaponState::State_PISTOLUP,
    DOOM::PlayerThing::WeaponState::State_PISTOLDOWN,
    DOOM::PlayerThing::WeaponState::State_PISTOL,
    DOOM::PlayerThing::WeaponState::State_PISTOL1,
    DOOM::PlayerThing::WeaponState::State_PISTOLFLASH
  },
  DOOM::PlayerThing::Attributs{ // Shootgun
    DOOM::Enum::Ammo::AmmoShell,
    1,
    DOOM::PlayerThing::WeaponState::State_SGUNUP,
    DOOM::PlayerThing::WeaponState::State_SGUNDOWN,
    DOOM::PlayerThing::WeaponState::State_SGUN,
    DOOM::PlayerThing::WeaponState::State_SGUN1,
    DOOM::PlayerThing::WeaponState::State_SGUNFLASH1
  },
  DOOM::PlayerThing::Attributs{ // Chaingun
    DOOM::Enum::Ammo::AmmoBullet,
    1,
    DOOM::PlayerThing::WeaponState::State_CHAINUP,
    DOOM::PlayerThing::WeaponState::State_CHAINDOWN,
    DOOM::PlayerThing::WeaponState::State_CHAIN,
    DOOM::PlayerThing::WeaponState::State_CHAIN1,
    DOOM::PlayerThing::WeaponState::State_CHAINFLASH1
  },
  DOOM::PlayerThing::Attributs{ // Missile launcher
    DOOM::Enum::Ammo::AmmoRocket,
    1,
    DOOM::PlayerThing::WeaponState::State_MISSILEUP,
    DOOM::PlayerThing::WeaponState::State_MISSILEDOWN,
    DOOM::PlayerThing::WeaponState::State_MISSILE,
    DOOM::PlayerThing::WeaponState::State_MISSILE1,
    DOOM::PlayerThing::WeaponState::State_MISSILEFLASH1
  },
  DOOM::PlayerThing::Attributs{ // Plasma rifle
    DOOM::Enum::Ammo::AmmoCell,
    1,
    DOOM::PlayerThing::WeaponState::State_PLASMAUP,
    DOOM::PlayerThing::WeaponState::State_PLASMADOWN,
    DOOM::PlayerThing::WeaponState::State_PLASMA,
    DOOM::PlayerThing::WeaponState::State_PLASMA1,
    DOOM::PlayerThing::WeaponState::State_PLASMAFLASH1
  },
  DOOM::PlayerThing::Attributs{ // BFG 9000
    DOOM::Enum::Ammo::AmmoCell,
    40,
    DOOM::PlayerThing::WeaponState::State_BFGUP,
    DOOM::PlayerThing::WeaponState::State_BFGDOWN,
    DOOM::PlayerThing::WeaponState::State_BFG,
    DOOM::PlayerThing::WeaponState::State_BFG1,
    DOOM::PlayerThing::WeaponState::State_BFGFLASH1
  },
  DOOM::PlayerThing::Attributs{ // Chainsaw
    DOOM::Enum::Ammo::AmmoNone,
    1,
    DOOM::PlayerThing::WeaponState::State_SAWUP,
    DOOM::PlayerThing::WeaponState::State_SAWDOWN,
    DOOM::PlayerThing::WeaponState::State_SAW,
    DOOM::PlayerThing::WeaponState::State_SAW1,
    DOOM::PlayerThing::WeaponState::State_None
  },
  DOOM::PlayerThing::Attributs{ // Super shotgun
    DOOM::Enum::Ammo::AmmoShell,
    2,
    DOOM::PlayerThing::WeaponState::State_DSGUNUP,
    DOOM::PlayerThing::WeaponState::State_DSGUNDOWN,
    DOOM::PlayerThing::WeaponState::State_DSGUN,
    DOOM::PlayerThing::WeaponState::State_DSGUN1,
    DOOM::PlayerThing::WeaponState::State_DSGUNFLASH1
  }
};

DOOM::PlayerThing::PlayerThing(DOOM::Doom& doom, int id, int controller) :
  DOOM::AbstractThing(doom, DOOM::Enum::ThingType::ThingType_PLAYER, DOOM::Enum::ThingFlag::FlagNone, 0.f, 0.f, 0.f),
  _running(false),
  _automap(false),
  _armor(DOOM::Enum::Armor::ArmorNone),
  _invulnerability(sf::Time::Zero),
  _invisibility(sf::Time::Zero),
  _light(sf::Time::Zero),
  _radiation(sf::Time::Zero),
  _sector(sf::Time::Zero),
  _berserk(false),
  id(id),
  _weapon(DOOM::Enum::Weapon::WeaponPistol), _weaponNext(DOOM::Enum::Weapon::WeaponPistol), _weaponState(_attributs[_weapon].up), _weaponElapsed(sf::Time::Zero), _weaponSound(Game::Sound::Instance().get()), _weaponPosition(), _weaponRefire(false), _weaponFire(false),
  _flash(0), _flashState(DOOM::PlayerThing::WeaponState::State_None), _flashElapsed(sf::Time::Zero),
  _palettePickup(sf::Time::Zero), _paletteDamage(sf::Time::Zero), _paletteBerserk(sf::Time::Zero),
  controller(controller),
  camera(),
  statusbar(doom, id),
  automap()
{
  // Check controller ID
  if (controller < 0)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Register player in DOOM
  doom.level.players.push_back(std::reference_wrapper<DOOM::PlayerThing>(*this));

  // Set player initial position
  for (const std::unique_ptr<DOOM::AbstractThing> & thing : doom.level.things)
    if (thing->attributs.id == id) {
      doom.level.blockmap.moveThing(*this, position.convert<2>(), thing->position.convert<2>());
      position = thing->position;
      angle = thing->angle;
      break;
    }

  // Update camera position
  camera.position = position;
  camera.position.z() += height * 0.73f;

  // Initial ammo
  statusbar.ammos[DOOM::Enum::Ammo::AmmoBullet] = 50;

  // Initial weapon
  _weaponNext = DOOM::Enum::Weapon::WeaponPistol;
  P_BringUpWeapon(doom);
}

void  DOOM::PlayerThing::updateRadiationSuit(DOOM::Doom& doom, sf::Time elapsed)
{
  // Decrement timer
  _radiation = std::max(sf::seconds(0.f), _radiation - elapsed);
}

void  DOOM::PlayerThing::updatePalette(DOOM::Doom& doom, sf::Time elapsed)
{
  // Decrement palette timers
  _palettePickup = std::max(sf::seconds(0.f), _palettePickup - elapsed);
  _paletteDamage = std::max(sf::seconds(0.f), _paletteDamage - elapsed);
  _paletteBerserk = std::max(sf::seconds(0.f), _paletteBerserk - elapsed);
}

void  DOOM::PlayerThing::updateInvulnerability(DOOM::Doom& doom, sf::Time elapsed)
{
  // Decrement timer
  _invulnerability = std::max(sf::seconds(0.f), _invulnerability - elapsed);
}

void  DOOM::PlayerThing::updateInvisibility(DOOM::Doom& doom, sf::Time elapsed)
{
  // Decrement timer
  _invisibility = std::max(sf::seconds(0.f), _invisibility - elapsed);

  // Set shadow flag
  if (_invisibility > sf::seconds(0.f))
    flags = (DOOM::Enum::ThingProperty)(flags | DOOM::Enum::ThingProperty::ThingProperty_Shadow);
  else
    flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_Shadow);
}

void  DOOM::PlayerThing::updateLightAmplificationVisor(DOOM::Doom& doom, sf::Time elapsed)
{
  // Decrement timer
  _light = std::max(sf::seconds(0.f), _light - elapsed);
}

bool  DOOM::PlayerThing::update(DOOM::Doom& doom, sf::Time elapsed)
{
  // Update player using controller
  if (controller == 0)
    updateKeyboard(doom, elapsed);
  else if (controller > 0)
    updateController(doom, elapsed);

  // Update BFG and rocket fire
  if (control(DOOM::PlayerThing::Control::ControlAttack, true) == true)
    _weaponFire = true;
  
  // Update timers
  updateInvulnerability(doom, elapsed);
  updateInvisibility(doom, elapsed);
  updateLightAmplificationVisor(doom, elapsed);
  updateRadiationSuit(doom, elapsed);
  updatePalette(doom, elapsed);

  // Update core components and physics
  DOOM::AbstractThing::update(doom, elapsed);

  // Update automap
  updateAutomap(doom, elapsed);

  // Update weapon states
  updateWeapon(doom, elapsed);
  updateFlash(doom, elapsed);

  // Update camera position
  camera.position = position;
  camera.position.z() += height * 0.73f;

  // Update status bar (TODO: complete this)
  statusbar.health = health;
  statusbar.ammo = (_attributs[_weapon].ammo == DOOM::Enum::Ammo::AmmoNone) ? 0 : statusbar.ammos[_attributs[_weapon].ammo];

  DOOM::Doom::Level::Sector& sector = doom.level.sectors[doom.level.getSector(position.convert<2>()).first];

  // Special sectors
  if (position.z() <= sector.floor_current) {
    switch (sector.special) {
    case DOOM::Doom::Level::Sector::Special::Secret:  // Player entering this sector gets credit for finding a secret
      // NOTE: original DOOM doesn't prompt message or play sound
      doom.level.statistics.players[id].secrets += 1;
      sector.special = DOOM::Doom::Level::Sector::Special::Normal;
      break;

    case DOOM::Doom::Level::Sector::Special::Damage5: // 5 % damage every 32 tics
      damageSector(doom, elapsed, 5.f);
      break;

    case DOOM::Doom::Level::Sector::Special::Damage10:  // 10 % damage every 32 tics
      damageSector(doom, elapsed, 10.f);
      break;

    case DOOM::Doom::Level::Sector::Special::End:             // Cancel God mode if active, 20 % damage every 32 tics, when player dies, level ends
      _invulnerability = sf::Time::Zero;
    case DOOM::Doom::Level::Sector::Special::Damage20:        // 20 % damage per second
    case DOOM::Doom::Level::Sector::Special::Damage20Blink05: // 20 % damage every 32 tics plus light blink 0.5 second
      damageSector(doom, elapsed, 20.f, sector.special == DOOM::Doom::Level::Sector::Special::End);
      break;

    default:  // Reset sector timer
      _sector = sf::Time::Zero;
      break;
    }
  }

  // Always return false as a player is never deleted
  return false;
}

void  DOOM::PlayerThing::updateAutomap(DOOM::Doom& doom, sf::Time elapsed)
{
  // Toogle automap
  if (control(DOOM::PlayerThing::Control::ControlAutomap, true) == true)
    _automap = !_automap;

  // Control automap when enabled
  if (_automap == true) {
    if (control(DOOM::PlayerThing::Control::ControlMode, true) == true) // Change mode
      automap.mode = (DOOM::Automap::Mode)((automap.mode + 1) % DOOM::Automap::Mode::ModeCount);
    if (control(DOOM::PlayerThing::Control::ControlGrid, true) == true) // Toggle grid
      automap.grid = !automap.grid;
    if (control(DOOM::PlayerThing::Control::ControlZoom) == true)       // Zoom
      automap.zoom = std::clamp(automap.zoom * std::pow(2.f, elapsed.asSeconds()), 0.015625f, 1.f);
    if (control(DOOM::PlayerThing::Control::ControlUnzoom) == true)     // Unzoom
      automap.zoom = std::clamp(automap.zoom / std::pow(2.f, elapsed.asSeconds()), 0.015625f, 1.f);
  }

  if (automap.mode == DOOM::Automap::Mode::ModeFollow) {
    automap.position = position.convert<2>();
    automap.angle = Math::DegToRad(90.f);
  }
  else if (automap.mode == DOOM::Automap::Mode::ModeRotation) {
    automap.position = position.convert<2>();
    automap.angle = angle;
  }
}

void  DOOM::PlayerThing::updateKeyboard(DOOM::Doom& doom, sf::Time elapsed)
{
  updateKeyboardTurn(doom, elapsed);
  updateKeyboardMove(doom, elapsed);
  updateKeyboardWeapon(doom, elapsed);

  // Perform a use action
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::E) == true)
    updateUse(doom, elapsed);

  // Perform a fire action
  if (Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Space) == true)
    updateFire(doom, elapsed);
}

void  DOOM::PlayerThing::updateKeyboardTurn(DOOM::Doom& doom, sf::Time elapsed)
{
  // Turn player
  float	horizontal = 0.f;
  float	vertical = 0.f;

  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Left) == true)	// Turn left
    horizontal += 1.f;
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Right) == true)	// Turn right
    horizontal -= 1.f;

  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Up) == true)	// Turn up
    vertical += 1.f;
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Down) == true)	// Turn down
    vertical -= 1.f;

  // Apply rotation to player
  updateTurn(doom, elapsed, horizontal, vertical);
}

void  DOOM::PlayerThing::updateKeyboardMove(DOOM::Doom & doom, sf::Time elapsed)
{
  // Move player
  Math::Vector<2> movement(0.f, 0.f);

  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Z) == true)	// Move forward
    movement += Math::Vector<2>(+1.f, 0.f);
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::S) == true)	// Move backward
    movement += Math::Vector<2>(-1.f, 0.f);
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Q) == true)	// Strafe left
    movement += Math::Vector<2>(0.f, -1.f);
  if (Game::Window::Instance().keyboard().keyDown(sf::Keyboard::D) == true)	// Strafe right
    movement += Math::Vector<2>(0.f, +1.f);

  // Handle running
  _running = Game::Window::Instance().keyboard().keyDown(sf::Keyboard::LShift);

  // Apply movement to player position
  updateMove(doom, elapsed, movement);
}

void  DOOM::PlayerThing::updateKeyboardWeapon(DOOM::Doom& doom, sf::Time elapsed)
{
  // Weapon binding
  static const std::list<std::pair<sf::Keyboard::Key, std::list<DOOM::Enum::Weapon>>> bindings = {
    { sf::Keyboard::Num1, { DOOM::Enum::Weapon::WeaponChainsaw, DOOM::Enum::Weapon::WeaponFist } },
    { sf::Keyboard::Num2, { DOOM::Enum::Weapon::WeaponPistol } },
    { sf::Keyboard::Num3, { DOOM::Enum::Weapon::WeaponSuperShotgun, DOOM::Enum::Weapon::WeaponShotgun } },
    { sf::Keyboard::Num4, { DOOM::Enum::Weapon::WeaponChaingun } },
    { sf::Keyboard::Num5, { DOOM::Enum::Weapon::WeaponRocketLauncher } },
    { sf::Keyboard::Num6, { DOOM::Enum::Weapon::WeaponPlasmaGun } },
    { sf::Keyboard::Num7, { DOOM::Enum::Weapon::WeaponBFG9000 } },
  };

  // Attempt every bind
  for (auto& bind : bindings) {
    if (Game::Window::Instance().keyboard().keyPressed(bind.first) == true) {
      for (auto weapon : bind.second) {
        if (setWeapon(weapon) == true)
          return;
      }
    }
  }
}

void  DOOM::PlayerThing::updateController(DOOM::Doom & doom, sf::Time elapsed)
{
  updateControllerTurn(doom, elapsed);
  updateControllerMove(doom, elapsed);

  // Perform a use action
  if (Game::Window::Instance().joystick().buttonPressed(controller - 1, 1) == true)
    updateUse(doom, elapsed);

  // Perform a fire action
  if (Game::Window::Instance().joystick().buttonPressed(controller - 1, 0) == true)
    updateFire(doom, elapsed);
}

void  DOOM::PlayerThing::updateControllerTurn(DOOM::Doom & doom, sf::Time elapsed)
{
  // Apply rotation to player
  updateTurn(doom, elapsed,
    std::fabsf(Game::Window::Instance().joystick().position(controller - 1, 4)) / 100.f > 0.2f ? -Game::Window::Instance().joystick().position(controller - 1, 4) / 100.f : 0.f,
    std::fabsf(Game::Window::Instance().joystick().position(controller - 1, 5)) / 100.f > 0.2f ? -Game::Window::Instance().joystick().position(controller - 1, 5) / 100.f : 0.f
  );
}

void  DOOM::PlayerThing::updateControllerMove(DOOM::Doom & doom, sf::Time elapsed)
{
  // Move player
  Math::Vector<2> movement(
    std::fabsf(Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::Y)) / 100.f > 0.2f ? -Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::Y) / 100.f : 0.f,
    std::fabs(Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::X)) / 100.f > 0.2f ? +Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::X) / 100.f : 0.f
  );
  
  // Handle running (left stick click)
  if (movement.length() < 0.72f)
    _running = false;
  if (Game::Window::Instance().joystick().buttonDown(controller - 1, 8) == true)
    _running = true;

  // Apply movement to player position
  updateMove(doom, elapsed, movement);
}

void  DOOM::PlayerThing::updateTurn(DOOM::Doom & doom, sf::Time elapsed, float horizontal, float vertical)
{
  // Automap control
  if (_automap == true && automap.mode == DOOM::Automap::Mode::ModeScroll) {
    automap.angle += horizontal * DOOM::PlayerThing::TurningSpeed * elapsed.asSeconds();
    return;
  }

  // The dead can't move
  if (health <= 0.f)
    return;

  // Apply sprinting to turning
  horizontal *= (_running == true ? DOOM::PlayerThing::RunningSpeed / DOOM::PlayerThing::WalkingSpeed : 1.f);

  // Apply turning angle and time to player
  angle += horizontal * DOOM::PlayerThing::TurningSpeed * elapsed.asSeconds();

  // Handle vertical angle
  camera.orientation += (DOOM::PlayerThing::VerticalLimit * vertical - camera.orientation) * (1.f - std::pow(0.1f, elapsed.asSeconds()));
  
  // Update camera
  camera.angle = angle;
}

void  DOOM::PlayerThing::updateMove(DOOM::Doom & doom, sf::Time elapsed, Math::Vector<2> movement)
{
  // Automap control
  if (_automap == true && automap.mode == DOOM::Automap::Mode::ModeScroll)
  {
    // Limit
    movement.x() = std::clamp(movement.x(), -1.f, +1.f);
    movement.y() = std::clamp(movement.y(), -1.f, +1.f);

    // Rotate
    movement = Math::Vector<2>(
      movement.x() * std::cos(automap.angle) + movement.y() * std::sin(automap.angle),
      movement.x() * std::sin(automap.angle) - movement.y() * std::cos(automap.angle));

    // Apply movement
    automap.position += movement * elapsed.asSeconds() / automap.zoom * DOOM::PlayerThing::RunningSpeed * 2.f;
    return;
  }

  // The dead can't move
  if (health <= 0.f)
    return;

  // Limit movement
  movement.x() = std::clamp(movement.x(), -1.f, +1.f);
  movement.y() = std::clamp(movement.y(), -1.f, +1.f);

  // Apply speed to movement
  movement.x() *= (_running == true ? DOOM::PlayerThing::RunningSpeed : DOOM::PlayerThing::WalkingSpeed) * 1.f;
  movement.y() *= (_running == true ? DOOM::PlayerThing::RunningSpeed : DOOM::PlayerThing::WalkingSpeed) * 0.8f;

  // Apply rotation to movement
  movement = Math::Vector<2>(
    movement.x() * std::cos(angle) + movement.y() * std::sin(angle),
    movement.x() * std::sin(angle) - movement.y() * std::cos(angle));

  // TODO: do not move when not touching ground

  // Apply movement to current position
  thrust(Math::Vector<3>(movement.x(), movement.y(), 0.f) * elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds());
}

void  DOOM::PlayerThing::updateUse(DOOM::Doom & doom, sf::Time elapsed)
{
  P_LineSwitch(doom, 64.f, position + Math::Vector<3>(0.f, 0.f, height / 2.f), Math::Vector<3>(std::cos(angle), std::sin(angle), std::tan(camera.orientation)));
}

void  DOOM::PlayerThing::updateFire(DOOM::Doom & doom, sf::Time elapsed)
{}

void  DOOM::PlayerThing::draw(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale)
{
  int16_t palette = cameraPalette();

  // Draw automap
  if (_automap == true)
    drawAutomap(doom, target, rect, scale, palette);

  // Draw player view
  else {
    drawCamera(doom, target, rect, scale, palette);
    drawWeapon(doom, target, rect, scale, palette);
  }
  
  // Draw statusbar
  drawStatusbar(doom, target, rect, scale, palette);  
}

void  DOOM::PlayerThing::drawCamera(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette)
{
  // Compute head bobing
  float bob = std::min(8.f, (Math::Pow<2>(_thrust.x()) + Math::Pow<2>(_thrust.y())) / 8.f)
    * std::sinf(Math::Pi * 2.f * Math::Modulo(doom.level.statistics.time.asSeconds() / DOOM::Doom::Tic.asSeconds() / 20.f, 1.f));

  // Apply position to camera
  camera.position.x() = position.x();
  camera.position.y() = position.y();
  camera.position.z() = position.z() + 41.f + bob;

  // Render 3D view
  camera.render(doom, target, sf::Rect<int16_t>(rect.left, rect.top, rect.width, rect.height - 32 * scale), _flash, cameraMode(), palette);
}

void  DOOM::PlayerThing::drawWeapon(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette)
{
  // Simulate bobing of weapon
  int angle_x = (int)(doom.level.statistics.time.asSeconds() / DOOM::Doom::Tic.asSeconds() * 128) % 8192;
  int angle_y = angle_x % 4096;
  float bob = (_states[_weaponState].action == &DOOM::PlayerThing::A_WeaponReady) ?
    (std::min(16.f, (Math::Pow<2>(_thrust.x()) + Math::Pow<2>(_thrust.y())) / 4.f)) :
    (0.f);

  // Render muzzle flash
  if (_flashState != DOOM::PlayerThing::WeaponState::State_None) {
    auto  iterator = doom.resources.animations.find(DOOM::str_to_key(_sprites[_states[_flashState].sprite]));

    if (iterator != doom.resources.animations.cend() && _states[_flashState].frame < iterator->second.size()) {
      iterator->second[_states[_flashState].frame][0].first.get().draw(
        doom,
        target,
        sf::Rect<int16_t>(rect.left, rect.top, rect.width, rect.height - 32 * scale),
        sf::Vector2i(
          rect.left + (int)((_weaponPosition.x() + bob * std::cosf(angle_x / 8192.f * Math::Pi * 2.f)) * scale),
          rect.top + (int)((_weaponPosition.y() - 16 + bob * std::sinf(angle_y / 8192.f * Math::Pi * 2.f)) * scale)
        ),
        sf::Vector2i(scale, scale),
        palette);
    }
  }

  // Render weapon
  if (_weaponState != DOOM::PlayerThing::WeaponState::State_None) {
    auto  iterator = doom.resources.animations.find(DOOM::str_to_key(_sprites[_states[_weaponState].sprite]));

    if (iterator != doom.resources.animations.cend() && _states[_weaponState].frame < iterator->second.size()) {
      iterator->second[_states[_weaponState].frame][0].first.get().draw(
        doom,
        target,
        sf::Rect<int16_t>(rect.left, rect.top, rect.width, rect.height - 32 * scale),
        sf::Vector2i(
          rect.left + (int)((_weaponPosition.x() + bob * std::cosf(angle_x / 8192.f * Math::Pi * 2.f)) * scale),
          rect.top + (int)((_weaponPosition.y() - 16 + bob * std::sinf(angle_y / 8192.f * Math::Pi * 2.f)) * scale)
        ),
        sf::Vector2i(scale, scale),
        palette);
    }
  }
}

void  DOOM::PlayerThing::drawStatusbar(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette)
{
  statusbar.render(doom, target, sf::Rect<int16_t>(rect.left, rect.top + (DOOM::Doom::RenderHeight - 32) * scale, rect.width, 32 * scale), palette);
}

void  DOOM::PlayerThing::drawAutomap(DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette)
{
  // Render automap
  automap.render(doom, target, sf::Rect<int16_t>(rect.left, rect.top, rect.width, rect.height - 32 * scale), scale, palette);
}

bool  DOOM::PlayerThing::pickup(DOOM::Doom& doom, DOOM::AbstractThing& item)
{
  // Can't pick-up when dead
  if (health <= 0.f)
    return false;

  // Map of pick-upable items
  static const std::unordered_map<DOOM::Enum::ThingType, std::function<bool(DOOM::Doom&, DOOM::PlayerThing&, DOOM::AbstractThing&)>> items = {
    { DOOM::Enum::ThingType::ThingType_MISC0, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupArmor(DOOM::Enum::Armor::ArmorSecurity) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                                   // Green armor
    { DOOM::Enum::ThingType::ThingType_MISC1, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupArmor(DOOM::Enum::Armor::ArmorMega) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                                       // Blue armor
    { DOOM::Enum::ThingType::ThingType_MISC2, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupHealth(1, 200), doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true; } },                                                                                                                                                                                                                                                // Health potion
    { DOOM::Enum::ThingType::ThingType_MISC3, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupArmor(DOOM::Enum::Armor::ArmorSecurity, 1), doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true; } },                                                                                                                                                                                                                    // Spiritual armor
    { DOOM::Enum::ThingType::ThingType_MISC4, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorBlue, DOOM::Enum::KeyType::KeyTypeKeycard, doom.level.players.size() > 1) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                               // Blue keycard
    { DOOM::Enum::ThingType::ThingType_MISC5, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorRed, DOOM::Enum::KeyType::KeyTypeKeycard, doom.level.players.size() > 1) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                // Red keycard
    { DOOM::Enum::ThingType::ThingType_MISC6, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorYellow, DOOM::Enum::KeyType::KeyTypeKeycard, doom.level.players.size() > 1) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                             // Yellow keycard
    { DOOM::Enum::ThingType::ThingType_MISC7, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorYellow, DOOM::Enum::KeyType::KeyTypeSkullkey, doom.level.players.size() > 1) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                            // Yellow skull key
    { DOOM::Enum::ThingType::ThingType_MISC8, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorRed, DOOM::Enum::KeyType::KeyTypeSkullkey, doom.level.players.size() > 1) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                               // Red skull key
    { DOOM::Enum::ThingType::ThingType_MISC9, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupKey(DOOM::Enum::KeyColor::KeyColorBlue, DOOM::Enum::KeyType::KeyTypeSkullkey, doom.level.players.size() > 1) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                              // Blue skull key
    { DOOM::Enum::ThingType::ThingType_MISC10, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupHealth(10, 100) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                                                          // Stimpack
    { DOOM::Enum::ThingType::ThingType_MISC11, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupHealth(25, 100) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                                                          // Medikit
    { DOOM::Enum::ThingType::ThingType_MISC12, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupHealth(100, 200) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_getpow), true); } },                                                                                                                                                                                                                                         // Soul sphere
    { DOOM::Enum::ThingType::ThingType_MEGA, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return (player.pickupHealth(200, 200) | player.pickupArmor(DOOM::Enum::Armor::ArmorMega)) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_getpow), true); } },                                                                                                                                                                                      // Megasphere
    { DOOM::Enum::ThingType::ThingType_INV, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupInvulnerability() && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_getpow), true); } },                                                                                                                                                                                                                                           // Invulnerability
    { DOOM::Enum::ThingType::ThingType_MISC13, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return (player.pickupBerserk() | player.pickupHealth(100, 100)) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_getpow), true); } },                                                                                                                                                                                                              // Berserk
    { DOOM::Enum::ThingType::ThingType_INS, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupInvisibility() && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_getpow), true); } },                                                                                                                                                                                                                                              // Invisibility
    { DOOM::Enum::ThingType::ThingType_MISC14, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupRadiationSuit() && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_getpow), true); } },                                                                                                                                                                                                                                          // Radiation suit
    { DOOM::Enum::ThingType::ThingType_CLIP, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoBullet, (item.flags & DOOM::Enum::ThingProperty::ThingProperty_Dropped) ? 5 : 10) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                         // Ammo clip
    { DOOM::Enum::ThingType::ThingType_MISC15, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupComputerMap() && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_getpow), true); } },                                                                                                                                                                                                                                            // Computer map
    { DOOM::Enum::ThingType::ThingType_MISC16, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupLightAmplificationVisor() && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_getpow), true); } },                                                                                                                                                                                                                                // Light amplification visor
    { DOOM::Enum::ThingType::ThingType_MISC17, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoBullet, 50) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                             // Box of ammo
    { DOOM::Enum::ThingType::ThingType_MISC18, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoRocket, 1) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                              // Rocket
    { DOOM::Enum::ThingType::ThingType_MISC19, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoRocket, 1) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                              // Box of rockets
    { DOOM::Enum::ThingType::ThingType_MISC20, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoCell, 20) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                               // Cell charge
    { DOOM::Enum::ThingType::ThingType_MISC21, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoCell, 100) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                              // Cell charge pack
    { DOOM::Enum::ThingType::ThingType_MISC22, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoShell, 4) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                               // Shotgun shells
    { DOOM::Enum::ThingType::ThingType_MISC23, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoShell, 20) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },                                                                                                                                                                                                              // Box of shells
    { DOOM::Enum::ThingType::ThingType_MISC24, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return (player.pickupBackpack() | player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoBullet, 10) | player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoShell, 4) | player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoRocket, 1) | player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoCell, 20)) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_itemup), true); } },  // Backpack
    { DOOM::Enum::ThingType::ThingType_MISC25, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return (player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoCell, 40) | player.pickupWeapon(DOOM::Enum::Weapon::WeaponBFG9000)) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_wpnup), true); } },                                                                                                                                                     // BFG 9000
    { DOOM::Enum::ThingType::ThingType_CHAINGUN, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return (player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoBullet, (item.flags & DOOM::Enum::ThingProperty::ThingProperty_Dropped) ? 10 : 20) | player.pickupWeapon(DOOM::Enum::Weapon::WeaponChaingun)) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_wpnup), true); } },                                                                         // Chaingun
    { DOOM::Enum::ThingType::ThingType_MISC26, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return player.pickupWeapon(DOOM::Enum::Weapon::WeaponChainsaw) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_wpnup), true); } },                                                                                                                                                                                                                // Chainsaw
    { DOOM::Enum::ThingType::ThingType_MISC27, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return (player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoRocket, 2) | player.pickupWeapon(DOOM::Enum::Weapon::WeaponRocketLauncher)) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_wpnup), true); } },                                                                                                                                             // Rocket launcher
    { DOOM::Enum::ThingType::ThingType_MISC28, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return (player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoCell, 40) | player.pickupWeapon(DOOM::Enum::Weapon::WeaponPlasmaGun)) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_wpnup), true); } },                                                                                                                                                   // Plasma rifle
    { DOOM::Enum::ThingType::ThingType_SHOTGUN, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return (player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoShell, (item.flags & DOOM::Enum::ThingProperty::ThingProperty_Dropped) ? 4 : 8) | player.pickupWeapon(DOOM::Enum::Weapon::WeaponShotgun)) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_wpnup), true); } },                                                                              // Shotgun
    { DOOM::Enum::ThingType::ThingType_SUPERSHOTGUN, [](DOOM::Doom& doom, DOOM::PlayerThing& player, DOOM::AbstractThing& item) { return (player.pickupAmmo(doom, DOOM::Enum::Ammo::AmmoShell, (item.flags & DOOM::Enum::ThingProperty::ThingProperty_Dropped) ? 4 : 8) | player.pickupWeapon(DOOM::Enum::Weapon::WeaponSuperShotgun)) && (doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_wpnup), true); } },                                                                    // Super shotgun
  };

  auto  iterator = items.find(item.type);

  // Handle errors
  if (iterator == items.end()) {
    std::cerr << "[DOOM::PlayerThing] Warning, pickup type '" << item.type << "' not supported." << std::endl;
    return false;
  }

  // Try to pick-up
  if (iterator->second(doom, *this, item) == false)
    return false;

  // Count pickup in statistics
  if (item.flags & DOOM::Enum::ThingProperty::ThingProperty_CountItem)
    doom.level.statistics.players[id].items += 1;

  // Pick-up flash
  _palettePickup += DOOM::Doom::Tic * 6.f;

  // Remove picked-up item
  return true;
}

bool  DOOM::PlayerThing::pickupComputerMap()
{
  // Do not pickup if already enabled
  if (automap.reveal == true)
    return false;

  // Enable map
  automap.reveal = true;

  // TODO: stuff here

  return true;
}

bool  DOOM::PlayerThing::pickupBerserk()
{
  // Do not pickup if already enabled
  if (_berserk == true)
    return false;

  // Become berserk
  _berserk = true;

  // Raise fists
  _weaponNext = DOOM::Enum::Weapon::WeaponFist;

  // Berserk flash
  _paletteBerserk += DOOM::Doom::Tic * 64.f * 12.f;

  return true;
}

bool  DOOM::PlayerThing::pickupRadiationSuit()
{
  // Do not pickup if already full
  if (_radiation >= sf::seconds(60.f))
    return false;

  // Set radiation suit for 60 seconds
  _radiation = sf::seconds(60.f);

  return true;
}

bool  DOOM::PlayerThing::pickupInvulnerability()
{
  // Do not pickup if already full
  if (_invulnerability >= sf::seconds(30.f))
    return false;

  // Set invulnerability for 30 seconds
  _invulnerability = sf::seconds(30.f);

  return true;
}

bool  DOOM::PlayerThing::pickupInvisibility()
{
  // Do not pickup if already full
  if (_invisibility >= sf::seconds(60.f))
    return false;

  // Set invisibility for one minute
  _invisibility = sf::seconds(60.f);
  flags = (DOOM::Enum::ThingProperty)(flags | DOOM::Enum::ThingProperty::ThingProperty_Shadow);

  return true;
}

bool  DOOM::PlayerThing::pickupLightAmplificationVisor()
{
  // Do not pickup if already full
  if (_light >= sf::seconds(120.f))
    return false;

  // Set light amplification for two minutes
  _light = sf::seconds(120.f);

  return true;
}

// TODO: remove these pragma, due to a MSVC optimization bug, setWeapon is not called properly
#pragma optimize("", off)
bool  DOOM::PlayerThing::pickupWeapon(DOOM::Enum::Weapon type)
{
  // Cancel if weapon already in inventory
  if (statusbar.weapons[type] == true)
    return false;

  // Pickup gun
  statusbar.weapons[type] = true;
  setWeapon(type);

  // Increase pick-up flash
  _palettePickup += DOOM::Doom::Tic * 6.f;

  return true;
}
#pragma optimize("", on)

bool  DOOM::PlayerThing::pickupAmmo(const DOOM::Doom& doom, DOOM::Enum::Ammo type, unsigned int quantity)
{
  // Invalid type
  if (type == DOOM::Enum::Ammo::AmmoNone)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Don't pickup if already full
  if (statusbar.ammos[type] >= statusbar.maximum[type])
    return false;

  // Pickup double in "I'm Too Young To Die" and "Nightmare!" skill levels
  statusbar.ammos[type] = std::min(statusbar.maximum[type], (statusbar.ammos[type] + quantity) * ((doom.skill == DOOM::Enum::Skill::SkillBaby || doom.skill == DOOM::Enum::Skill::SkillNightmare) ? 2 : 1));

  return true;
}

bool  DOOM::PlayerThing::pickupBackpack()
{
  // Backpack already picked-up
  if (statusbar.maximum[DOOM::Enum::Ammo::AmmoBullet] == 400 &&
    statusbar.maximum[DOOM::Enum::Ammo::AmmoShell] == 100 &&
    statusbar.maximum[DOOM::Enum::Ammo::AmmoRocket] == 100 &&
    statusbar.maximum[DOOM::Enum::Ammo::AmmoCell] == 600)
    return false;

  // Expand inventory
  statusbar.maximum[DOOM::Enum::Ammo::AmmoBullet] = 400;
  statusbar.maximum[DOOM::Enum::Ammo::AmmoShell] = 100;
  statusbar.maximum[DOOM::Enum::Ammo::AmmoRocket] = 100;
  statusbar.maximum[DOOM::Enum::Ammo::AmmoCell] = 600;
  return true;
}

bool  DOOM::PlayerThing::pickupKey(DOOM::Enum::KeyColor color, DOOM::Enum::KeyType type, bool multiplayer)
{
  // Add key to player
  statusbar.keys[color] = type;

  // Don't remove card if multiplayer
  if (multiplayer == true)
    return false;

  return true;
}

bool  DOOM::PlayerThing::pickupHealth(unsigned int quantity, float maximum)
{
  // Does not pickup if already full
  if (health >= (int)maximum)
    return false;

  // Add health
  health = std::min(health + quantity, maximum);

  return true;
}

bool  DOOM::PlayerThing::pickupArmor(DOOM::Enum::Armor type, float quantity)
{
  // Does not pickup if already full
  if (statusbar.armor >= DOOM::Enum::Armor::ArmorMega)
    return false;

  // Add armor
  statusbar.armor = std::min(statusbar.armor + quantity, (float)DOOM::Enum::Armor::ArmorMega);

  // Apply new armor type if better
  if (_armor < type)
    _armor = type;

  return true;
}

bool  DOOM::PlayerThing::pickupArmor(DOOM::Enum::Armor type)
{
  // Does not pickup if already full
  if (statusbar.armor >= (float)type)
    return false;

  // Set new armor
  _armor = type;
  statusbar.armor = (float)type;

  // Remove item
  return true;
}

void  DOOM::PlayerThing::damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, DOOM::AbstractThing& origin, float damage)
{
  // Take half damage in baby mode
  if (doom.skill == DOOM::Enum::Skill::SkillBaby)
    damage /= 2;

  float protection = 0;

  // Compute protection
  switch (_armor) {
  case DOOM::Enum::Armor::ArmorSecurity:
    protection = std::min(statusbar.armor, damage / 3.f);
    break;
  case DOOM::Enum::Armor::ArmorMega:
    protection = std::min(statusbar.armor, damage / 2.f);
    break;
  default:
    protection = 0;
    break;
  }

  // Remove protection from armor
  statusbar.armor -= protection;
  if (statusbar.armor == 0)
    _armor = DOOM::Enum::Armor::ArmorNone;

  auto& sector = doom.level.sectors[doom.level.getSector(position.convert<2>()).first];

  // Limit damage when standing in end sector
  if (position.z() <= sector.floor_current && sector.special == DOOM::Doom::Level::Sector::Special::End)
    damage = std::min(health - 1, damage);

  // Apply remaining damage
  DOOM::AbstractThing::damage(doom, attacker, origin, damage - protection);

  // Red flash
  _paletteDamage += DOOM::Doom::Tic * (damage - protection);

  // End game
  if (position.z() <= sector.floor_current && sector.special == DOOM::Doom::Level::Sector::Special::End && health < 11.f)
    doom.level.end = DOOM::Enum::End::EndNormal;
}

void  DOOM::PlayerThing::damageSector(DOOM::Doom& doom, sf::Time elapsed, float damage, bool end)
{
  // Add elapsed time to sector timer
  _sector += elapsed;

  // Damage player every 32 tics
  while (_sector >= DOOM::Doom::Tic * (float)DOOM::PlayerThing::SectorSpeed) {
    float dmg = damage;
    float protection = 0;

    // Radiation suit
    if (_radiation.asSeconds() > 0.f && (dmg < 20.f || std::rand() % 256 < 250))
      continue;

    // 1/2 armor when mega armor, 1/3 otherwise
    if (_armor == DOOM::Enum::Armor::ArmorMega || statusbar.armor >= 100.f)
      protection = std::min(statusbar.armor, dmg / 2.f);
    else if (_armor == DOOM::Enum::Armor::ArmorSecurity)
      protection = std::min(statusbar.armor, dmg / 3.f);

    // Remove protection from armor
    statusbar.armor -= protection;
    if (statusbar.armor == 0)
      _armor = DOOM::Enum::Armor::ArmorNone;

    // Keep player alive when ending level
    dmg -= protection;
    if (end == true)
      dmg = std::min(health - 1.f, dmg);

    // Apply remaining damage
    DOOM::AbstractThing::damage(doom, *this, dmg);

    // Red flash
    _paletteDamage += DOOM::Doom::Tic * dmg;

    // End game
    if (end == true && health < 11.f)
      doom.level.end = DOOM::Enum::End::EndNormal;

    _sector -= DOOM::Doom::Tic * (float)DOOM::PlayerThing::SectorSpeed;
  }
}

bool  DOOM::PlayerThing::key(DOOM::Enum::KeyColor color) const
{
  // Check if player has a keycard or a skullkey of given color
  return statusbar.keys[color] != DOOM::Enum::KeyType::KeyTypeNone;
}

DOOM::Camera::Special DOOM::PlayerThing::cameraMode() const
{
  // Invulnerability
  if (_invulnerability > sf::seconds(0.f))
    return DOOM::Camera::Special::Invulnerability;

  // Light amplification visor
  else if (_light > sf::seconds(0.f))
    return DOOM::Camera::Special::LightAmplificationVisor;

  // Default mode
  else
    return DOOM::Camera::Special::Normal;
}

int16_t DOOM::PlayerThing::cameraPalette() const
{
  int16_t palette = 0;

  // Damage palette
  if (_paletteDamage > sf::Time::Zero || _paletteBerserk > sf::Time::Zero)
    palette = std::clamp((int)(std::max(_paletteDamage.asSeconds() / DOOM::Doom::Tic.asSeconds() / 8.f, _paletteBerserk.asSeconds() / DOOM::Doom::Tic.asSeconds() / 64.f)), 0, 7) + 1;
  // Pick-up palette
  else if (_palettePickup > sf::Time::Zero)
    palette = std::clamp((int)(_palettePickup.asSeconds() / DOOM::Doom::Tic.asSeconds() / 8.f), 0, 3) + 9;
  // Radiation suit
  else if (_radiation > sf::seconds(0.f))
    palette = 13;

  return palette;
}

void  DOOM::PlayerThing::setWeaponState(DOOM::Doom& doom, DOOM::PlayerThing::WeaponState state)
{
  // Change state
  _weaponState = state;

  // Call state callback
  if (_states[_weaponState].action != nullptr)
    std::invoke(_states[_weaponState].action, this, doom);
}

void  DOOM::PlayerThing::updateWeapon(DOOM::Doom& doom, sf::Time elapsed)
{
  // Update internal timer
  _weaponElapsed += elapsed;

  while (_weaponElapsed >= DOOM::Doom::Tic * (sf::Int64)_states[_weaponState].duration)
  {
    // Stop when no duration
    if (_states[_weaponState].duration == -1) {
      _weaponElapsed = sf::Time::Zero;
      break;
    }

    // Skip to next state
    _weaponElapsed -= DOOM::Doom::Tic * (sf::Int64)_states[_weaponState].duration;
    setWeaponState(doom, _states[_weaponState].next);
  }
}

bool  DOOM::PlayerThing::setWeapon(DOOM::Enum::Weapon weapon)
{
  // Try to equip weapon
  if (_weapon != weapon &&
    statusbar.weapons[weapon] == true &&
    (_attributs[weapon].ammo == DOOM::Enum::Ammo::AmmoNone || statusbar.ammos[_attributs[weapon].ammo] >= _attributs[weapon].count)) {
    _weaponNext = weapon;
    return true;
  }
  else {
    return false;
  }
}

void  DOOM::PlayerThing::setFlashState(DOOM::Doom& doom, DOOM::PlayerThing::WeaponState state)
{
  // Change state
  _flashState = state;

  // Call state callback
  if (_states[_flashState].action != nullptr)
    std::invoke(_states[_flashState].action, this, doom);
}

void  DOOM::PlayerThing::updateFlash(DOOM::Doom& doom, sf::Time elapsed)
{
  // Update internal timer
  _flashElapsed += elapsed;

  while (_flashElapsed >= DOOM::Doom::Tic * (sf::Int64)_states[_flashState].duration)
  {
    // Stop when no duration
    if (_states[_flashState].duration == -1) {
      _flashElapsed = sf::Time::Zero;
      break;
    }

    // Skip to next state
    _flashElapsed -= DOOM::Doom::Tic * (sf::Int64)_states[_flashState].duration;
    setFlashState(doom, _states[_flashState].next);
  }
}

bool  DOOM::PlayerThing::control(DOOM::PlayerThing::Control action, bool pressed)
{
  // Keyboard
  if (controller == 0) {
    if (action == DOOM::PlayerThing::Control::ControlAttack)
      return (pressed == true) ? Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Key::Space) : Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::Space);
    else if (action == DOOM::PlayerThing::Control::ControlUse)
      return (pressed == true) ? Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Key::E) : Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::E);
    else if (action == DOOM::PlayerThing::Control::ControlRun)
      return (pressed == true) ? Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Key::LShift) : Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::LShift);
    else if (action == DOOM::PlayerThing::Control::ControlAutomap)
      return (pressed == true) ? Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Key::Tab) : Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::Tab);
    else if (action == DOOM::PlayerThing::Control::ControlMode)
      return (pressed == true) ? Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Key::F) : Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::F);
    else if (action == DOOM::PlayerThing::Control::ControlGrid)
      return (pressed == true) ? Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Key::G) : Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::G);
    else if (action == DOOM::PlayerThing::Control::ControlZoom)
      return (pressed == true) ? Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Key::Add) : Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::Add);
    else if (action == DOOM::PlayerThing::Control::ControlUnzoom)
      return (pressed == true) ? Game::Window::Instance().keyboard().keyPressed(sf::Keyboard::Key::Subtract) : Game::Window::Instance().keyboard().keyDown(sf::Keyboard::Key::Subtract);
    else
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // Joystick
  else {
    if (action == DOOM::PlayerThing::Control::ControlAttack)
      return (pressed == true) ? Game::Window::Instance().joystick().buttonPressed(controller - 1, 0) : Game::Window::Instance().joystick().buttonDown(controller - 1, 0);
    else if (action == DOOM::PlayerThing::Control::ControlUse)
      return (pressed == true) ? Game::Window::Instance().joystick().buttonPressed(controller - 1, 1) : Game::Window::Instance().joystick().buttonDown(controller - 1, 1);
    else if (action == DOOM::PlayerThing::Control::ControlRun)
      return (pressed == true) ? Game::Window::Instance().joystick().buttonPressed(controller - 1, 8) : Game::Window::Instance().joystick().buttonDown(controller - 1, 8);
    else if (action == DOOM::PlayerThing::Control::ControlAutomap)
      return (pressed == true) ? Game::Window::Instance().joystick().buttonPressed(controller - 1, 6) : Game::Window::Instance().joystick().buttonDown(controller - 1, 8);
    else if (action == DOOM::PlayerThing::Control::ControlMode)
      return (pressed == true) ? Game::Window::Instance().joystick().buttonPressed(controller - 1, 3) : Game::Window::Instance().joystick().buttonDown(controller - 1, 3);
    else if (action == DOOM::PlayerThing::Control::ControlGrid)
      return (pressed == true) ? Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovX) != 0.f : Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovX) != 0.f;
    else if (action == DOOM::PlayerThing::Control::ControlZoom)
      return (pressed == true) ? Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovY) > 0.f : Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovY) > 0.f;
    else if (action == DOOM::PlayerThing::Control::ControlUnzoom)
      return (pressed == true) ? Game::Window::Instance().joystick().relative(controller - 1, sf::Joystick::Axis::PovY) < 0.f : Game::Window::Instance().joystick().position(controller - 1, sf::Joystick::Axis::PovY) < 0.f;
    else
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }
}

void  DOOM::PlayerThing::A_WeaponReady(DOOM::Doom& doom)
{
  // Get out of attack state
  if (_state == DOOM::AbstractThing::ThingState::State_PLAY_ATK1 ||
    _state == DOOM::AbstractThing::ThingState::State_PLAY_ATK2) {
    setState(doom, DOOM::AbstractThing::ThingState::State_PLAY);
  }

  // Play chainsaw idle sound
  if (_weapon == DOOM::Enum::Weapon::WeaponChainsaw &&
    _weaponState == DOOM::PlayerThing::WeaponState::State_SAW) {
    doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_sawidl, false);
  }

  // Check for change or player death, put the weapon away
  if (_weapon != _weaponNext || health <= 0.f) {
    setWeaponState(doom, _attributs[_weapon].down);
    return;
  }

  // The missile launcher and BFG do not auto fire
  if ((_weapon == DOOM::Enum::Weapon::WeaponRocketLauncher || _weapon == DOOM::Enum::Weapon::WeaponBFG9000) ?
    (_weaponFire == true) :
    (control(DOOM::PlayerThing::Control::ControlAttack, false) == true)) {
    P_FireWeapon(doom);
    return;
  }
}

void  DOOM::PlayerThing::A_Lower(DOOM::Doom& doom)
{
  // Lower weapon
  _weaponPosition.y() = std::min(_weaponPosition.y() + 6.f, 128.f);

  // Not already down
  if (_weaponPosition.y() < 128.f)
    return;

  // Player is dead, don't bring weapon back up
  if (health <= 0.f)
    return;

  // The old weapon has been lowered off the screen, so change the weapon and start raising it
  _weapon = _weaponNext;

  // Change weapon
  P_BringUpWeapon(doom);
}

void  DOOM::PlayerThing::A_Raise(DOOM::Doom& doom)
{
  // Raise weapon
  _weaponPosition.y() = std::max(_weaponPosition.y() - 6.f, 32.f);

  // Not already up
  if (_weaponPosition.y() > 32.f)
    return;

  // The weapon has been raised all the way, so change to the ready state.
  setWeaponState(doom, _attributs[_weapon].ready);
}

void  DOOM::PlayerThing::A_ReFire(DOOM::Doom& doom)
{
  // Check for fire, if a weaponchange is pending, let it go through instead
  if (control(DOOM::PlayerThing::Control::ControlAttack) == true &&
    _weapon == _weaponNext &&
    health > 0.f) {
    _weaponRefire = true;
    P_FireWeapon(doom);
  }
  else {
    _weaponRefire = false;
    P_CheckAmmo(doom);
  }
}

void  DOOM::PlayerThing::A_Punch(DOOM::Doom& doom)
{
  float atk_slope = camera.orientation + (Math::Random() - Math::Random()) * Math::Pi / 8.f;
  float atk_angle = angle + (Math::Random() - Math::Random()) * Math::Pi / 8.f;
  float atk_damage = (std::rand() % 10 + 1) / 2 * (_berserk == true ? 10.f : 1.f);

  // Attack nearby things
  bool success = P_LineAttack(
    doom,
    DOOM::AbstractThing::MeleeRange,
    Math::Vector<3>(position.x(), position.y(), position.z() + height / 2.f),
    Math::Vector<3>(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope)),
    atk_damage
  );

  // Target shot
  if (success == true) {
    doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_punch);
    // NOTE: we are suppose to turn to face target
  }
}

void  DOOM::PlayerThing::A_FirePistol(DOOM::Doom& doom)
{
  // Play pistol sound
  doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol);

  // Change player to attack state
  setState(doom, DOOM::AbstractThing::ThingState::State_PLAY_ATK2);

  // Consum ammos
  statusbar.ammos[_attributs[DOOM::Enum::Weapon::WeaponPistol].ammo] -= _attributs[DOOM::Enum::Weapon::WeaponPistol].count;

  // Muzzle flash
  setFlashState(doom, _attributs[DOOM::Enum::Weapon::WeaponPistol].flash);

  // Shot!
  P_GunShot(doom, (_weaponRefire == true) ? DOOM::PlayerThing::WeaponDispersion : 0.f, 5.f * (std::rand() % 3 + 1));
}

void  DOOM::PlayerThing::A_FireShotgun(DOOM::Doom& doom)
{
  // Play pistol sound
  doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_shotgn);

  // Change player to attack state
  setState(doom, DOOM::AbstractThing::ThingState::State_PLAY_ATK2);

  // Consum ammos
  statusbar.ammos[_attributs[DOOM::Enum::Weapon::WeaponShotgun].ammo] -= _attributs[DOOM::Enum::Weapon::WeaponShotgun].count;

  // Muzzle flash
  setFlashState(doom, _attributs[DOOM::Enum::Weapon::WeaponShotgun].flash);

  // Fire 7 inaccurate pellets
  for (int i = 0; i < 7; i++) {
    P_GunShot(doom, DOOM::PlayerThing::WeaponDispersion, 5.f * (std::rand() % 3 + 1));
  }
}

void  DOOM::PlayerThing::A_FireShotgun2(DOOM::Doom& doom)
{
  // Play pistol sound
  doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_dshtgn);

  // Change player to attack state
  setState(doom, DOOM::AbstractThing::ThingState::State_PLAY_ATK2);

  // Consum ammos
  statusbar.ammos[_attributs[DOOM::Enum::Weapon::WeaponSuperShotgun].ammo] -= _attributs[DOOM::Enum::Weapon::WeaponSuperShotgun].count;

  // Muzzle flash
  setFlashState(doom, _attributs[DOOM::Enum::Weapon::WeaponSuperShotgun].flash);

  // Fire 20 very inaccurate pellets
  for (int i = 0; i < 20; i++) {
    P_GunShot(doom, DOOM::PlayerThing::WeaponDispersion / 2.f, 5.f * (std::rand() % 3 + 1));
  }
}

void  DOOM::PlayerThing::A_CheckReload(DOOM::Doom& doom)
{
  P_CheckAmmo(doom);
}

void  DOOM::PlayerThing::A_OpenShotgun2(DOOM::Doom& doom)
{
  doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_dbopn);
}

void  DOOM::PlayerThing::A_LoadShotgun2(DOOM::Doom& doom)
{
  doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_dbload);
}

void  DOOM::PlayerThing::A_CloseShotgun2(DOOM::Doom& doom)
{
  doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_dbcls);
  A_ReFire(doom);
}

void  DOOM::PlayerThing::A_FireCGun(DOOM::Doom& doom)
{
  // Play pistol sound
  doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol);

  // Change player to attack state
  setState(doom, DOOM::AbstractThing::ThingState::State_PLAY_ATK2);

  // Check for ammos
  if (statusbar.ammos[_attributs[DOOM::Enum::Weapon::WeaponChaingun].ammo] < _attributs[DOOM::Enum::Weapon::WeaponChaingun].count)
    return;

  // Consum ammos
  statusbar.ammos[_attributs[DOOM::Enum::Weapon::WeaponChaingun].ammo] -= _attributs[DOOM::Enum::Weapon::WeaponChaingun].count;

  // Muzzle flash
  setFlashState(doom, _attributs[DOOM::Enum::Weapon::WeaponChaingun].flash);

  // NOTE: we're supposed to do something stange with flash state
  // P_SetPsprite(player,
  //  ps_flash,
  //  weaponinfo[player->readyweapon].flashstate
  //  + psp->state
  //  - &states[S_CHAIN1]);

  // Fire bullet
  P_GunShot(doom, (_weaponRefire == true) ? DOOM::PlayerThing::WeaponDispersion : 0.f, 5.f * (std::rand() % 3 + 1));
}

void  DOOM::PlayerThing::A_FireMissile(DOOM::Doom& doom)
{
  // Consum ammos
  statusbar.ammos[_attributs[DOOM::Enum::Weapon::WeaponRocketLauncher].ammo] -= _attributs[DOOM::Enum::Weapon::WeaponRocketLauncher].count;

  // Spawn missile
  P_SpawnPlayerMissile(doom, DOOM::Enum::ThingType::ThingType_ROCKET, camera.orientation);
}

void  DOOM::PlayerThing::A_Saw(DOOM::Doom& doom)
{
  float atk_slope = camera.orientation + (Math::Random() - Math::Random()) * Math::Pi / 8.f;
  float atk_angle = angle + (Math::Random() - Math::Random()) * Math::Pi / 8.f;

  // Attack with chainsaw
  bool success = P_LineAttack(
    doom,
    DOOM::AbstractThing::MeleeRange + 1.f,
    Math::Vector<3>(position.x(), position.y(), position.z() + height / 2.f),
    Math::Vector<3>(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope)),
    2.f * (std::rand() % 10 + 1));

  // Missed
  if (success == false) {
    doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_sawful);
    return;
  }

  // Chainsaw goes brrrrr!
  doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_sawhit);

  // NOTE: were are supposed to face target

  // Add JustAttacked flag to player
  flags = (DOOM::Enum::ThingProperty)(flags | DOOM::Enum::ThingProperty::ThingProperty_JustAttacked);
}

void  DOOM::PlayerThing::A_FirePlasma(DOOM::Doom& doom)
{
  // Consum ammos
  statusbar.ammos[_attributs[DOOM::Enum::Weapon::WeaponPlasmaGun].ammo] -= _attributs[DOOM::Enum::Weapon::WeaponPlasmaGun].count;

  // Muzzle flash
  setFlashState(doom, (DOOM::PlayerThing::WeaponState)(_attributs[DOOM::Enum::Weapon::WeaponPlasmaGun].flash + std::rand() % 2));

  // Spawn missile
  P_SpawnPlayerMissile(doom, DOOM::Enum::ThingType::ThingType_PLASMA, camera.orientation);
}

void  DOOM::PlayerThing::A_BFGsound(DOOM::Doom& doom)
{
  doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_bfg);
}

void  DOOM::PlayerThing::A_FireBFG(DOOM::Doom& doom)
{
  // Consum ammos
  statusbar.ammos[_attributs[DOOM::Enum::Weapon::WeaponBFG9000].ammo] -= _attributs[DOOM::Enum::Weapon::WeaponBFG9000].count;

  // Spawn missile
  P_SpawnPlayerMissile(doom, DOOM::Enum::ThingType::ThingType_BFG, camera.orientation);
}

void  DOOM::PlayerThing::A_GunFlash(DOOM::Doom& doom)
{
  setState(doom, DOOM::PlayerThing::ThingState::State_PLAY_ATK2);
  setFlashState(doom, _attributs[_weapon].flash);
}

void  DOOM::PlayerThing::A_Light0(DOOM::Doom& doom)
{
  _flash = 0;
}

void  DOOM::PlayerThing::A_Light1(DOOM::Doom& doom)
{
  _flash = 1;
}

void  DOOM::PlayerThing::A_Light2(DOOM::Doom& doom)
{
  _flash = 2;
}

void  DOOM::PlayerThing::P_FireWeapon(DOOM::Doom& doom)
{
  // Remove fire flag
  _weaponFire = false;

  // Check enough ammo to fire
  if (P_CheckAmmo(doom) == false)
    return;

  setState(doom, DOOM::AbstractThing::ThingState::State_PLAY_ATK1);
  setWeaponState(doom, _attributs[_weapon].attack);
  
  // Alert nearby monsters
  P_NoiseAlert(doom, doom.level.getSector(position.convert<2>()).first);
}

void  DOOM::PlayerThing::P_NoiseAlert(DOOM::Doom& doom, int16_t sector_index, int limit, std::unordered_map<int16_t, int>& sectors)
{
  // Limit reached
  if (limit <= 0)
    return;

  // Check if sector has already been traversed
  if (sectors.find(sector_index) == sectors.end())
    sectors[sector_index] = limit;
  else if (sectors[sector_index] < limit)
    sectors[sector_index] = limit;
  else
    return;

  auto& sector = doom.level.sectors[sector_index];

  // Set current sector target
  sector.sound_target = this;

  for (const auto& linedef : doom.level.linedefs)
  {
    // Need sector on both sides
    if (linedef->back == -1 || linedef->front == -1)
      continue;

    int16_t neighbor_index;

    // Get adjacent sector index
    if (doom.level.sidedefs[linedef->back].sector == sector_index)
      neighbor_index = doom.level.sidedefs[linedef->front].sector;
    else if (doom.level.sidedefs[linedef->front].sector == sector_index)
      neighbor_index = doom.level.sidedefs[linedef->back].sector;
    else
      continue;

    auto& neighbor = doom.level.sectors[neighbor_index];

    // Door is closed
    if (std::min(sector.ceiling_current, neighbor.ceiling_current) <= std::max(sector.floor_current, neighbor.floor_current))
      continue;

    // Propagate sound in neighbor sector
    P_NoiseAlert(doom, neighbor_index, (linedef->flag & DOOM::AbstractLinedef::Flag::BlockSound) ? (limit - 1) : (limit), sectors);
  }
}

bool  DOOM::PlayerThing::P_CheckAmmo(DOOM::Doom& doom)
{
  // Check for number of ammo
  if (_attributs[_weapon].ammo == DOOM::Enum::Ammo::AmmoNone ||
    statusbar.ammos[_attributs[_weapon].ammo] >= _attributs[_weapon].count) {
    return true;
  }

  // Sorted prefered weapons
  static const std::array<DOOM::Enum::Weapon, DOOM::Enum::Weapon::WeaponCount>  preferred = {
    DOOM::Enum::Weapon::WeaponPlasmaGun,
    DOOM::Enum::Weapon::WeaponSuperShotgun,
    DOOM::Enum::Weapon::WeaponChaingun,
    DOOM::Enum::Weapon::WeaponShotgun,
    DOOM::Enum::Weapon::WeaponPistol,
    DOOM::Enum::Weapon::WeaponChainsaw,
    DOOM::Enum::Weapon::WeaponRocketLauncher,
    DOOM::Enum::Weapon::WeaponBFG9000,
    DOOM::Enum::Weapon::WeaponFist
  };

  // Select preferred weapon with enough ammo
  for (auto weapon : preferred) {
    if (statusbar.weapons[weapon] == true &&
      (_attributs[weapon].ammo == DOOM::Enum::Ammo::AmmoNone ||
      statusbar.ammos[_attributs[weapon].ammo] >= _attributs[weapon].count)) {
      _weaponNext = weapon;
      setWeaponState(doom, _attributs[_weapon].down);
      return false;
    }
  }

  // Fist should always be possible
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

void  DOOM::PlayerThing::P_BringUpWeapon(DOOM::Doom& doom)
{
  // Play chainsaw start sound
  if (_weaponNext == DOOM::Enum::Weapon::WeaponChainsaw) {
    doom.sound(_weaponSound, DOOM::Doom::Resources::Sound::EnumSound::Sound_sawup, false);
  }

  // Change weapon
  _weapon = _weaponNext;
  _weaponPosition.y() = 128.f;
  setWeaponState(doom, _attributs[_weapon].up);
}

void  DOOM::PlayerThing::P_GunShot(DOOM::Doom& doom, float dispersion, float damage)
{
  float atk_slope = camera.orientation
    + ((dispersion == 0.f) ? (0) : ((Math::Random() - Math::Random()) * Math::Pi / dispersion));
  float atk_angle = angle
    + ((dispersion == 0.f) ? (0) : ((Math::Random() - Math::Random()) * Math::Pi / dispersion));

  // Attack
  P_LineAttack(
    doom,
    AbstractThing::MissileRange,
    Math::Vector<3>(position.x(), position.y(), position.z() + height / 2.f),
    Math::Vector<3>(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope)),
    damage);
}