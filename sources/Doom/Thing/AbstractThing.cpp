#include <iostream>

#include "Doom/Doom.hpp"
#include "Doom/Thing/AbstractThing.hpp"
#include "Doom/Thing/PlayerThing.hpp"
#include "System/Sound.hpp"

const float	DOOM::AbstractThing::MeleeRange = 64.f;
const float	DOOM::AbstractThing::MissileRange = 32.f * 64.f;
const float	DOOM::AbstractThing::FloatSpeed = 4.f;
const float	DOOM::AbstractThing::SkullSpeed = 20.f;
const int	DOOM::AbstractThing::TargetThreshold = 100;
const int	DOOM::AbstractThing::MaxRadius = 32;
const float     DOOM::AbstractThing::FatSpread = Math::Pi / 16.f;

const std::array<std::string, DOOM::AbstractThing::ThingSprite::Sprite_Number>	DOOM::AbstractThing::_sprites =
{
  "TROO", "SHTG", "PUNG", "PISG", "PISF", "SHTF", "SHT2", "CHGG", "CHGF", "MISG",
  "MISF", "SAWG", "PLSG", "PLSF", "BFGG", "BFGF", "BLUD", "PUFF", "BAL1", "BAL2",
  "PLSS", "PLSE", "MISL", "BFS1", "BFE1", "BFE2", "TFOG", "IFOG", "PLAY", "POSS",
  "SPOS", "VILE", "FIRE", "FATB", "FBXP", "SKEL", "MANF", "FATT", "CPOS", "SARG",
  "HEAD", "BAL7", "BOSS", "BOS2", "SKUL", "SPID", "BSPI", "APLS", "APBX", "CYBR",
  "PAIN", "SSWV", "KEEN", "BBRN", "BOSF", "ARM1", "ARM2", "BAR1", "BEXP", "FCAN",
  "BON1", "BON2", "BKEY", "RKEY", "YKEY", "BSKU", "RSKU", "YSKU", "STIM", "MEDI",
  "SOUL", "PINV", "PSTR", "PINS", "MEGA", "SUIT", "PMAP", "PVIS", "CLIP", "AMMO",
  "ROCK", "BROK", "CELL", "CELP", "SHEL", "SBOX", "BPAK", "BFUG", "MGUN", "CSAW",
  "LAUN", "PLAS", "SHOT", "SGN2", "COLU", "SMT2", "GOR1", "POL2", "POL5", "POL4",
  "POL3", "POL1", "POL6", "GOR2", "GOR3", "GOR4", "GOR5", "SMIT", "COL1", "COL2",
  "COL3", "COL4", "CAND", "CBRA", "COL6", "TRE1", "TRE2", "ELEC", "CEYE", "FSKU",
  "COL5", "TBLU", "TGRN", "TRED", "SMBT", "SMGT", "SMRT", "HDB1", "HDB2", "HDB3",
  "HDB4", "HDB5", "HDB6", "POB1", "POB2", "BRS1", "TLMP", "TLP2"
};

const std::array<DOOM::AbstractThing::State, DOOM::AbstractThing::ThingState::State_Number>	DOOM::AbstractThing::_states =
{
  DOOM::AbstractThing::State{ Sprite_TROO, 0, false, -1, nullptr, State_None },			// State_None
  DOOM::AbstractThing::State{ Sprite_SHTG, 4, false, 0, &A_Light0, State_None },		// State_LIGHTDONE
  DOOM::AbstractThing::State{ Sprite_PUNG, 0, false, 1, &A_WeaponReady, State_PUNCH },		// State_PUNCH
  DOOM::AbstractThing::State{ Sprite_PUNG, 0, false, 1, &A_Lower, State_PUNCHDOWN },		// State_PUNCHDOWN
  DOOM::AbstractThing::State{ Sprite_PUNG, 0, false, 1, &A_Raise, State_PUNCHUP },		// State_PUNCHUP
  DOOM::AbstractThing::State{ Sprite_PUNG, 1, false, 4, nullptr, State_PUNCH2 },		// State_PUNCH1
  DOOM::AbstractThing::State{ Sprite_PUNG, 2, false, 4, &A_Punch, State_PUNCH3 },		// State_PUNCH2
  DOOM::AbstractThing::State{ Sprite_PUNG, 3, false, 5, nullptr, State_PUNCH4 },		// State_PUNCH3
  DOOM::AbstractThing::State{ Sprite_PUNG, 2, false, 4, nullptr, State_PUNCH5 },		// State_PUNCH4
  DOOM::AbstractThing::State{ Sprite_PUNG, 1, false, 5, &A_ReFire, State_PUNCH },		// State_PUNCH5
  DOOM::AbstractThing::State{ Sprite_PISG, 0, false, 1, &A_WeaponReady, State_PISTOL },		// State_PISTOL
  DOOM::AbstractThing::State{ Sprite_PISG, 0, false, 1, &A_Lower, State_PISTOLDOWN },		// State_PISTOLDOWN
  DOOM::AbstractThing::State{ Sprite_PISG, 0, false, 1, &A_Raise, State_PISTOLUP },		// State_PISTOLUP
  DOOM::AbstractThing::State{ Sprite_PISG, 0, false, 4, nullptr, State_PISTOL2 },		// State_PISTOL1
  DOOM::AbstractThing::State{ Sprite_PISG, 1, false, 6, &A_FirePistol, State_PISTOL3 },		// State_PISTOL2
  DOOM::AbstractThing::State{ Sprite_PISG, 2, false, 4, nullptr, State_PISTOL4 },		// State_PISTOL3
  DOOM::AbstractThing::State{ Sprite_PISG, 1, false, 5, &A_ReFire, State_PISTOL },		// State_PISTOL4
  DOOM::AbstractThing::State{ Sprite_PISF, 0, true, 7, &A_Light1, State_LIGHTDONE },		// State_PISTOLFLASH
  DOOM::AbstractThing::State{ Sprite_SHTG, 0, false, 1, &A_WeaponReady, State_SGUN },		// State_SGUN
  DOOM::AbstractThing::State{ Sprite_SHTG, 0, false, 1, &A_Lower, State_SGUNDOWN },		// State_SGUNDOWN
  DOOM::AbstractThing::State{ Sprite_SHTG, 0, false, 1, &A_Raise, State_SGUNUP },		// State_SGUNUP
  DOOM::AbstractThing::State{ Sprite_SHTG, 0, false, 3, nullptr, State_SGUN2 },			// State_SGUN1
  DOOM::AbstractThing::State{ Sprite_SHTG, 0, false, 7, &A_FireShotgun, State_SGUN3 },		// State_SGUN2
  DOOM::AbstractThing::State{ Sprite_SHTG, 1, false, 5, nullptr, State_SGUN4 },			// State_SGUN3
  DOOM::AbstractThing::State{ Sprite_SHTG, 2, false, 5, nullptr, State_SGUN5 },			// State_SGUN4
  DOOM::AbstractThing::State{ Sprite_SHTG, 3, false, 4, nullptr, State_SGUN6 },			// State_SGUN5
  DOOM::AbstractThing::State{ Sprite_SHTG, 2, false, 5, nullptr, State_SGUN7 },			// State_SGUN6
  DOOM::AbstractThing::State{ Sprite_SHTG, 1, false, 5, nullptr, State_SGUN8 },			// State_SGUN7
  DOOM::AbstractThing::State{ Sprite_SHTG, 0, false, 3, nullptr, State_SGUN9 },			// State_SGUN8
  DOOM::AbstractThing::State{ Sprite_SHTG, 0, false, 7, &A_ReFire, State_SGUN },		// State_SGUN9
  DOOM::AbstractThing::State{ Sprite_SHTF, 0, true, 4, &A_Light1, State_SGUNFLASH2 },		// State_SGUNFLASH1
  DOOM::AbstractThing::State{ Sprite_SHTF, 1, true, 3, &A_Light2, State_LIGHTDONE },		// State_SGUNFLASH2
  DOOM::AbstractThing::State{ Sprite_SHT2, 0, false, 1, &A_WeaponReady, State_DSGUN },		// State_DSGUN
  DOOM::AbstractThing::State{ Sprite_SHT2, 0, false, 1, &A_Lower, State_DSGUNDOWN },		// State_DSGUNDOWN
  DOOM::AbstractThing::State{ Sprite_SHT2, 0, false, 1, &A_Raise, State_DSGUNUP },		// State_DSGUNUP
  DOOM::AbstractThing::State{ Sprite_SHT2, 0, false, 3, nullptr, State_DSGUN2 },		// State_DSGUN1
  DOOM::AbstractThing::State{ Sprite_SHT2, 0, false, 7, &A_FireShotgun2, State_DSGUN3 },	// State_DSGUN2
  DOOM::AbstractThing::State{ Sprite_SHT2, 1, false, 7, nullptr, State_DSGUN4 },		// State_DSGUN3
  DOOM::AbstractThing::State{ Sprite_SHT2, 2, false, 7, &A_CheckReload, State_DSGUN5 },		// State_DSGUN4
  DOOM::AbstractThing::State{ Sprite_SHT2, 3, false, 7, &A_OpenShotgun2, State_DSGUN6 },	// State_DSGUN5
  DOOM::AbstractThing::State{ Sprite_SHT2, 4, false, 7, nullptr, State_DSGUN7 },		// State_DSGUN6
  DOOM::AbstractThing::State{ Sprite_SHT2, 5, false, 7, &A_LoadShotgun2, State_DSGUN8 },	// State_DSGUN7
  DOOM::AbstractThing::State{ Sprite_SHT2, 6, false, 6, nullptr, State_DSGUN9 },		// State_DSGUN8
  DOOM::AbstractThing::State{ Sprite_SHT2, 7, false, 6, &A_CloseShotgun2, State_DSGUN10 },	// State_DSGUN9
  DOOM::AbstractThing::State{ Sprite_SHT2, 0, false, 5, &A_ReFire, State_DSGUN },		// State_DSGUN10
  DOOM::AbstractThing::State{ Sprite_SHT2, 1, false, 7, nullptr, State_DSNR2 },			// State_DSNR1
  DOOM::AbstractThing::State{ Sprite_SHT2, 0, false, 3, nullptr, State_DSGUNDOWN },		// State_DSNR2
  DOOM::AbstractThing::State{ Sprite_SHT2, 8, true, 5, &A_Light1, State_DSGUNFLASH2 },		// State_DSGUNFLASH1
  DOOM::AbstractThing::State{ Sprite_SHT2, 9, true, 4, &A_Light2, State_LIGHTDONE },		// State_DSGUNFLASH2
  DOOM::AbstractThing::State{ Sprite_CHGG, 0, false, 1, &A_WeaponReady, State_CHAIN },		// State_CHAIN
  DOOM::AbstractThing::State{ Sprite_CHGG, 0, false, 1, &A_Lower, State_CHAINDOWN },		// State_CHAINDOWN
  DOOM::AbstractThing::State{ Sprite_CHGG, 0, false, 1, &A_Raise, State_CHAINUP },		// State_CHAINUP
  DOOM::AbstractThing::State{ Sprite_CHGG, 0, false, 4, &A_FireCGun, State_CHAIN2 },		// State_CHAIN1
  DOOM::AbstractThing::State{ Sprite_CHGG, 1, false, 4, &A_FireCGun, State_CHAIN3 },		// State_CHAIN2
  DOOM::AbstractThing::State{ Sprite_CHGG, 1, false, 0, &A_ReFire, State_CHAIN },		// State_CHAIN3
  DOOM::AbstractThing::State{ Sprite_CHGF, 0, true, 5, &A_Light1, State_LIGHTDONE },		// State_CHAINFLASH1
  DOOM::AbstractThing::State{ Sprite_CHGF, 1, true, 5, &A_Light2, State_LIGHTDONE },		// State_CHAINFLASH2
  DOOM::AbstractThing::State{ Sprite_MISG, 0, false, 1, &A_WeaponReady, State_MISSILE },	// State_MISSILE
  DOOM::AbstractThing::State{ Sprite_MISG, 0, false, 1, &A_Lower, State_MISSILEDOWN },		// State_MISSILEDOWN
  DOOM::AbstractThing::State{ Sprite_MISG, 0, false, 1, &A_Raise, State_MISSILEUP },		// State_MISSILEUP
  DOOM::AbstractThing::State{ Sprite_MISG, 1, false, 8, &A_GunFlash, State_MISSILE2 },		// State_MISSILE1
  DOOM::AbstractThing::State{ Sprite_MISG, 1, false, 12, &A_FireMissile, State_MISSILE3 },	// State_MISSILE2
  DOOM::AbstractThing::State{ Sprite_MISG, 1, false, 0, &A_ReFire, State_MISSILE },		// State_MISSILE3
  DOOM::AbstractThing::State{ Sprite_MISF, 0, true, 3, &A_Light1, State_MISSILEFLASH2 },	// State_MISSILEFLASH1
  DOOM::AbstractThing::State{ Sprite_MISF, 1, true, 4, nullptr, State_MISSILEFLASH3 },		// State_MISSILEFLASH2
  DOOM::AbstractThing::State{ Sprite_MISF, 2, true, 4, &A_Light2, State_MISSILEFLASH4 },	// State_MISSILEFLASH3
  DOOM::AbstractThing::State{ Sprite_MISF, 3, true, 4, &A_Light2, State_LIGHTDONE },		// State_MISSILEFLASH4
  DOOM::AbstractThing::State{ Sprite_SAWG, 2, false, 4, &A_WeaponReady, State_SAWB },		// State_SAW
  DOOM::AbstractThing::State{ Sprite_SAWG, 3, false, 4, &A_WeaponReady, State_SAW },		// State_SAWB
  DOOM::AbstractThing::State{ Sprite_SAWG, 2, false, 1, &A_Lower, State_SAWDOWN },		// State_SAWDOWN
  DOOM::AbstractThing::State{ Sprite_SAWG, 2, false, 1, &A_Raise, State_SAWUP },		// State_SAWUP
  DOOM::AbstractThing::State{ Sprite_SAWG, 0, false, 4, &A_Saw, State_SAW2 },			// State_SAW1
  DOOM::AbstractThing::State{ Sprite_SAWG, 1, false, 4, &A_Saw, State_SAW3 },			// State_SAW2
  DOOM::AbstractThing::State{ Sprite_SAWG, 1, false, 0, &A_ReFire, State_SAW },			// State_SAW3
  DOOM::AbstractThing::State{ Sprite_PLSG, 0, false, 1, &A_WeaponReady, State_PLASMA },		// State_PLASMA
  DOOM::AbstractThing::State{ Sprite_PLSG, 0, false, 1, &A_Lower, State_PLASMADOWN },		// State_PLASMADOWN
  DOOM::AbstractThing::State{ Sprite_PLSG, 0, false, 1, &A_Raise, State_PLASMAUP },		// State_PLASMAUP
  DOOM::AbstractThing::State{ Sprite_PLSG, 0, false, 3, &A_FirePlasma, State_PLASMA2 },		// State_PLASMA1
  DOOM::AbstractThing::State{ Sprite_PLSG, 1, false, 20, &A_ReFire, State_PLASMA },		// State_PLASMA2
  DOOM::AbstractThing::State{ Sprite_PLSF, 0, true, 4, &A_Light1, State_LIGHTDONE },		// State_PLASMAFLASH1
  DOOM::AbstractThing::State{ Sprite_PLSF, 1, true, 4, &A_Light1, State_LIGHTDONE },		// State_PLASMAFLASH2
  DOOM::AbstractThing::State{ Sprite_BFGG, 0, false, 1, &A_WeaponReady, State_BFG },		// State_BFG
  DOOM::AbstractThing::State{ Sprite_BFGG, 0, false, 1, &A_Lower, State_BFGDOWN },		// State_BFGDOWN
  DOOM::AbstractThing::State{ Sprite_BFGG, 0, false, 1, &A_Raise, State_BFGUP },		// State_BFGUP
  DOOM::AbstractThing::State{ Sprite_BFGG, 0, false, 20, &A_BFGsound, State_BFG2 },		// State_BFG1
  DOOM::AbstractThing::State{ Sprite_BFGG, 1, false, 10, &A_GunFlash, State_BFG3 },		// State_BFG2
  DOOM::AbstractThing::State{ Sprite_BFGG, 1, false, 10, &A_FireBFG, State_BFG4 },		// State_BFG3
  DOOM::AbstractThing::State{ Sprite_BFGG, 1, false, 20, &A_ReFire, State_BFG },		// State_BFG4
  DOOM::AbstractThing::State{ Sprite_BFGF, 0, true, 11, &A_Light1, State_BFGFLASH2 },		// State_BFGFLASH1
  DOOM::AbstractThing::State{ Sprite_BFGF, 1, true, 6, &A_Light2, State_LIGHTDONE },		// State_BFGFLASH2
  DOOM::AbstractThing::State{ Sprite_BLUD, 2, false, 8, nullptr, State_BLOOD2 },		// State_BLOOD1
  DOOM::AbstractThing::State{ Sprite_BLUD, 1, false, 8, nullptr, State_BLOOD3 },		// State_BLOOD2
  DOOM::AbstractThing::State{ Sprite_BLUD, 0, false, 8, nullptr, State_None },			// State_BLOOD3
  DOOM::AbstractThing::State{ Sprite_PUFF, 0, true, 4, nullptr, State_PUFF2 },			// State_PUFF1
  DOOM::AbstractThing::State{ Sprite_PUFF, 1, false, 4, nullptr, State_PUFF3 },			// State_PUFF2
  DOOM::AbstractThing::State{ Sprite_PUFF, 2, false, 4, nullptr, State_PUFF4 },			// State_PUFF3
  DOOM::AbstractThing::State{ Sprite_PUFF, 3, false, 4, nullptr, State_None },			// State_PUFF4
  DOOM::AbstractThing::State{ Sprite_BAL1, 0, true, 4, nullptr, State_TBALL2 },			// State_TBALL1
  DOOM::AbstractThing::State{ Sprite_BAL1, 1, true, 4, nullptr, State_TBALL1 },			// State_TBALL2
  DOOM::AbstractThing::State{ Sprite_BAL1, 2, true, 6, nullptr, State_TBALLX2 },		// State_TBALLX1
  DOOM::AbstractThing::State{ Sprite_BAL1, 3, true, 6, nullptr, State_TBALLX3 },		// State_TBALLX2
  DOOM::AbstractThing::State{ Sprite_BAL1, 4, true, 6, nullptr, State_None },			// State_TBALLX3
  DOOM::AbstractThing::State{ Sprite_BAL2, 0, true, 4, nullptr, State_RBALL2 },			// State_RBALL1
  DOOM::AbstractThing::State{ Sprite_BAL2, 1, true, 4, nullptr, State_RBALL1 },			// State_RBALL2
  DOOM::AbstractThing::State{ Sprite_BAL2, 2, true, 6, nullptr, State_RBALLX2 },		// State_RBALLX1
  DOOM::AbstractThing::State{ Sprite_BAL2, 3, true, 6, nullptr, State_RBALLX3 },		// State_RBALLX2
  DOOM::AbstractThing::State{ Sprite_BAL2, 4, true, 6, nullptr, State_None },			// State_RBALLX3
  DOOM::AbstractThing::State{ Sprite_PLSS, 0, true, 6, nullptr, State_PLASBALL2 },		// State_PLASBALL
  DOOM::AbstractThing::State{ Sprite_PLSS, 1, true, 6, nullptr, State_PLASBALL },		// State_PLASBALL2
  DOOM::AbstractThing::State{ Sprite_PLSE, 0, true, 4, nullptr, State_PLASEXP2 },		// State_PLASEXP
  DOOM::AbstractThing::State{ Sprite_PLSE, 1, true, 4, nullptr, State_PLASEXP3 },		// State_PLASEXP2
  DOOM::AbstractThing::State{ Sprite_PLSE, 2, true, 4, nullptr, State_PLASEXP4 },		// State_PLASEXP3
  DOOM::AbstractThing::State{ Sprite_PLSE, 3, true, 4, nullptr, State_PLASEXP5 },		// State_PLASEXP4
  DOOM::AbstractThing::State{ Sprite_PLSE, 4, true, 4, nullptr, State_None },			// State_PLASEXP5
  DOOM::AbstractThing::State{ Sprite_MISL, 0, true, 1, nullptr, State_ROCKET },			// State_ROCKET
  DOOM::AbstractThing::State{ Sprite_BFS1, 0, true, 4, nullptr, State_BFGSHOT2 },		// State_BFGSHOT
  DOOM::AbstractThing::State{ Sprite_BFS1, 1, true, 4, nullptr, State_BFGSHOT },		// State_BFGSHOT2
  DOOM::AbstractThing::State{ Sprite_BFE1, 0, true, 8, nullptr, State_BFGLAND2 },		// State_BFGLAND
  DOOM::AbstractThing::State{ Sprite_BFE1, 1, true, 8, nullptr, State_BFGLAND3 },		// State_BFGLAND2
  DOOM::AbstractThing::State{ Sprite_BFE1, 2, true, 8, &A_BFGSpray, State_BFGLAND4 },		// State_BFGLAND3
  DOOM::AbstractThing::State{ Sprite_BFE1, 3, true, 8, nullptr, State_BFGLAND5 },		// State_BFGLAND4
  DOOM::AbstractThing::State{ Sprite_BFE1, 4, true, 8, nullptr, State_BFGLAND6 },		// State_BFGLAND5
  DOOM::AbstractThing::State{ Sprite_BFE1, 5, true, 8, nullptr, State_None },			// State_BFGLAND6
  DOOM::AbstractThing::State{ Sprite_BFE2, 0, true, 8, nullptr, State_BFGEXP2 },		// State_BFGEXP
  DOOM::AbstractThing::State{ Sprite_BFE2, 1, true, 8, nullptr, State_BFGEXP3 },		// State_BFGEXP2
  DOOM::AbstractThing::State{ Sprite_BFE2, 2, true, 8, nullptr, State_BFGEXP4 },		// State_BFGEXP3
  DOOM::AbstractThing::State{ Sprite_BFE2, 3, true, 8, nullptr, State_None },			// State_BFGEXP4
  DOOM::AbstractThing::State{ Sprite_MISL, 1, true, 8, &A_Explode, State_EXPLODE2 },		// State_EXPLODE1
  DOOM::AbstractThing::State{ Sprite_MISL, 2, true, 6, nullptr, State_EXPLODE3 },		// State_EXPLODE2
  DOOM::AbstractThing::State{ Sprite_MISL, 3, true, 4, nullptr, State_None },			// State_EXPLODE3
  DOOM::AbstractThing::State{ Sprite_TFOG, 0, true, 6, nullptr, State_TFOG01 },			// State_TFOG
  DOOM::AbstractThing::State{ Sprite_TFOG, 1, true, 6, nullptr, State_TFOG02 },			// State_TFOG01
  DOOM::AbstractThing::State{ Sprite_TFOG, 0, true, 6, nullptr, State_TFOG2 },			// State_TFOG02
  DOOM::AbstractThing::State{ Sprite_TFOG, 1, true, 6, nullptr, State_TFOG3 },			// State_TFOG2
  DOOM::AbstractThing::State{ Sprite_TFOG, 2, true, 6, nullptr, State_TFOG4 },			// State_TFOG3
  DOOM::AbstractThing::State{ Sprite_TFOG, 3, true, 6, nullptr, State_TFOG5 },			// State_TFOG4
  DOOM::AbstractThing::State{ Sprite_TFOG, 4, true, 6, nullptr, State_TFOG6 },			// State_TFOG5
  DOOM::AbstractThing::State{ Sprite_TFOG, 5, true, 6, nullptr, State_TFOG7 },			// State_TFOG6
  DOOM::AbstractThing::State{ Sprite_TFOG, 6, true, 6, nullptr, State_TFOG8 },			// State_TFOG7
  DOOM::AbstractThing::State{ Sprite_TFOG, 7, true, 6, nullptr, State_TFOG9 },			// State_TFOG8
  DOOM::AbstractThing::State{ Sprite_TFOG, 8, true, 6, nullptr, State_TFOG10 },			// State_TFOG9
  DOOM::AbstractThing::State{ Sprite_TFOG, 9, true, 6, nullptr, State_None },			// State_TFOG10
  DOOM::AbstractThing::State{ Sprite_IFOG, 0, true, 6, nullptr, State_IFOG01 },			// State_IFOG
  DOOM::AbstractThing::State{ Sprite_IFOG, 1, true, 6, nullptr, State_IFOG02 },			// State_IFOG01
  DOOM::AbstractThing::State{ Sprite_IFOG, 0, true, 6, nullptr, State_IFOG2 },			// State_IFOG02
  DOOM::AbstractThing::State{ Sprite_IFOG, 1, true, 6, nullptr, State_IFOG3 },			// State_IFOG2
  DOOM::AbstractThing::State{ Sprite_IFOG, 2, true, 6, nullptr, State_IFOG4 },			// State_IFOG3
  DOOM::AbstractThing::State{ Sprite_IFOG, 3, true, 6, nullptr, State_IFOG5 },			// State_IFOG4
  DOOM::AbstractThing::State{ Sprite_IFOG, 4, true, 6, nullptr, State_None },			// State_IFOG5
  DOOM::AbstractThing::State{ Sprite_PLAY, 0, false, -1, nullptr, State_None },			// State_PLAY
  DOOM::AbstractThing::State{ Sprite_PLAY, 0, false, 4, nullptr, State_PLAY_RUN2 },		// State_PLAY_RUN1
  DOOM::AbstractThing::State{ Sprite_PLAY, 1, false, 4, nullptr, State_PLAY_RUN3 },		// State_PLAY_RUN2
  DOOM::AbstractThing::State{ Sprite_PLAY, 2, false, 4, nullptr, State_PLAY_RUN4 },		// State_PLAY_RUN3
  DOOM::AbstractThing::State{ Sprite_PLAY, 3, false, 4, nullptr, State_PLAY_RUN1 },		// State_PLAY_RUN4
  DOOM::AbstractThing::State{ Sprite_PLAY, 4, false, 12, nullptr, State_PLAY },			// State_PLAY_ATK1
  DOOM::AbstractThing::State{ Sprite_PLAY, 5, true, 6, nullptr, State_PLAY_ATK1 },		// State_PLAY_ATK2
  DOOM::AbstractThing::State{ Sprite_PLAY, 6, false, 4, nullptr, State_PLAY_PAIN2 },		// State_PLAY_PAIN
  DOOM::AbstractThing::State{ Sprite_PLAY, 6, false, 4, &A_Pain, State_PLAY },			// State_PLAY_PAIN2
  DOOM::AbstractThing::State{ Sprite_PLAY, 7, false, 10, nullptr, State_PLAY_DIE2 },		// State_PLAY_DIE1
  DOOM::AbstractThing::State{ Sprite_PLAY, 8, false, 10, &A_PlayerScream, State_PLAY_DIE3 },	// State_PLAY_DIE2
  DOOM::AbstractThing::State{ Sprite_PLAY, 9, false, 10, &A_Fall, State_PLAY_DIE4 },		// State_PLAY_DIE3
  DOOM::AbstractThing::State{ Sprite_PLAY, 10, false, 10, nullptr, State_PLAY_DIE5 },		// State_PLAY_DIE4
  DOOM::AbstractThing::State{ Sprite_PLAY, 11, false, 10, nullptr, State_PLAY_DIE6 },		// State_PLAY_DIE5
  DOOM::AbstractThing::State{ Sprite_PLAY, 12, false, 10, nullptr, State_PLAY_DIE7 },		// State_PLAY_DIE6
  DOOM::AbstractThing::State{ Sprite_PLAY, 13, false, -1, nullptr, State_None },		// State_PLAY_DIE7
  DOOM::AbstractThing::State{ Sprite_PLAY, 14, false, 5, nullptr, State_PLAY_XDIE2 },		// State_PLAY_XDIE1
  DOOM::AbstractThing::State{ Sprite_PLAY, 15, false, 5, &A_XScream, State_PLAY_XDIE3 },	// State_PLAY_XDIE2
  DOOM::AbstractThing::State{ Sprite_PLAY, 16, false, 5, &A_Fall, State_PLAY_XDIE4 },		// State_PLAY_XDIE3
  DOOM::AbstractThing::State{ Sprite_PLAY, 17, false, 5, nullptr, State_PLAY_XDIE5 },		// State_PLAY_XDIE4
  DOOM::AbstractThing::State{ Sprite_PLAY, 18, false, 5, nullptr, State_PLAY_XDIE6 },		// State_PLAY_XDIE5
  DOOM::AbstractThing::State{ Sprite_PLAY, 19, false, 5, nullptr, State_PLAY_XDIE7 },		// State_PLAY_XDIE6
  DOOM::AbstractThing::State{ Sprite_PLAY, 20, false, 5, nullptr, State_PLAY_XDIE8 },		// State_PLAY_XDIE7
  DOOM::AbstractThing::State{ Sprite_PLAY, 21, false, 5, nullptr, State_PLAY_XDIE9 },		// State_PLAY_XDIE8
  DOOM::AbstractThing::State{ Sprite_PLAY, 22, false, -1, nullptr, State_None },		// State_PLAY_XDIE9
  DOOM::AbstractThing::State{ Sprite_POSS, 0, false, 10, &A_Look, State_POSS_STND2 },		// State_POSS_STND
  DOOM::AbstractThing::State{ Sprite_POSS, 1, false, 10, &A_Look, State_POSS_STND },		// State_POSS_STND2
  DOOM::AbstractThing::State{ Sprite_POSS, 0, false, 4, &A_Chase, State_POSS_RUN2 },		// State_POSS_RUN1
  DOOM::AbstractThing::State{ Sprite_POSS, 0, false, 4, &A_Chase, State_POSS_RUN3 },		// State_POSS_RUN2
  DOOM::AbstractThing::State{ Sprite_POSS, 1, false, 4, &A_Chase, State_POSS_RUN4 },		// State_POSS_RUN3
  DOOM::AbstractThing::State{ Sprite_POSS, 1, false, 4, &A_Chase, State_POSS_RUN5 },		// State_POSS_RUN4
  DOOM::AbstractThing::State{ Sprite_POSS, 2, false, 4, &A_Chase, State_POSS_RUN6 },		// State_POSS_RUN5
  DOOM::AbstractThing::State{ Sprite_POSS, 2, false, 4, &A_Chase, State_POSS_RUN7 },		// State_POSS_RUN6
  DOOM::AbstractThing::State{ Sprite_POSS, 3, false, 4, &A_Chase, State_POSS_RUN8 },		// State_POSS_RUN7
  DOOM::AbstractThing::State{ Sprite_POSS, 3, false, 4, &A_Chase, State_POSS_RUN1 },		// State_POSS_RUN8
  DOOM::AbstractThing::State{ Sprite_POSS, 4, false, 10, &A_FaceTarget, State_POSS_ATK2 },	// State_POSS_ATK1
  DOOM::AbstractThing::State{ Sprite_POSS, 5, false, 8, &A_PosAttack, State_POSS_ATK3 },	// State_POSS_ATK2
  DOOM::AbstractThing::State{ Sprite_POSS, 4, false, 8, nullptr, State_POSS_RUN1 },		// State_POSS_ATK3
  DOOM::AbstractThing::State{ Sprite_POSS, 6, false, 3, nullptr, State_POSS_PAIN2 },		// State_POSS_PAIN
  DOOM::AbstractThing::State{ Sprite_POSS, 6, false, 3, &A_Pain, State_POSS_RUN1 },		// State_POSS_PAIN2
  DOOM::AbstractThing::State{ Sprite_POSS, 7, false, 5, nullptr, State_POSS_DIE2 },		// State_POSS_DIE1
  DOOM::AbstractThing::State{ Sprite_POSS, 8, false, 5, &A_Scream, State_POSS_DIE3 },		// State_POSS_DIE2
  DOOM::AbstractThing::State{ Sprite_POSS, 9, false, 5, &A_Fall, State_POSS_DIE4 },		// State_POSS_DIE3
  DOOM::AbstractThing::State{ Sprite_POSS, 10, false, 5, nullptr, State_POSS_DIE5 },		// State_POSS_DIE4
  DOOM::AbstractThing::State{ Sprite_POSS, 11, false, -1, nullptr, State_None },		// State_POSS_DIE5
  DOOM::AbstractThing::State{ Sprite_POSS, 12, false, 5, nullptr, State_POSS_XDIE2 },		// State_POSS_XDIE1
  DOOM::AbstractThing::State{ Sprite_POSS, 13, false, 5, &A_XScream, State_POSS_XDIE3 },	// State_POSS_XDIE2
  DOOM::AbstractThing::State{ Sprite_POSS, 14, false, 5, &A_Fall, State_POSS_XDIE4 },		// State_POSS_XDIE3
  DOOM::AbstractThing::State{ Sprite_POSS, 15, false, 5, nullptr, State_POSS_XDIE5 },		// State_POSS_XDIE4
  DOOM::AbstractThing::State{ Sprite_POSS, 16, false, 5, nullptr, State_POSS_XDIE6 },		// State_POSS_XDIE5
  DOOM::AbstractThing::State{ Sprite_POSS, 17, false, 5, nullptr, State_POSS_XDIE7 },		// State_POSS_XDIE6
  DOOM::AbstractThing::State{ Sprite_POSS, 18, false, 5, nullptr, State_POSS_XDIE8 },		// State_POSS_XDIE7
  DOOM::AbstractThing::State{ Sprite_POSS, 19, false, 5, nullptr, State_POSS_XDIE9 },		// State_POSS_XDIE8
  DOOM::AbstractThing::State{ Sprite_POSS, 20, false, -1, nullptr, State_None },		// State_POSS_XDIE9
  DOOM::AbstractThing::State{ Sprite_POSS, 10, false, 5, nullptr, State_POSS_RAISE2 },		// State_POSS_RAISE1
  DOOM::AbstractThing::State{ Sprite_POSS, 9, false, 5, nullptr, State_POSS_RAISE3 },		// State_POSS_RAISE2
  DOOM::AbstractThing::State{ Sprite_POSS, 8, false, 5, nullptr, State_POSS_RAISE4 },		// State_POSS_RAISE3
  DOOM::AbstractThing::State{ Sprite_POSS, 7, false, 5, nullptr, State_POSS_RUN1 },		// State_POSS_RAISE4
  DOOM::AbstractThing::State{ Sprite_SPOS, 0, false, 10, &A_Look, State_SPOS_STND2 },		// State_SPOS_STND
  DOOM::AbstractThing::State{ Sprite_SPOS, 1, false, 10, &A_Look, State_SPOS_STND },		// State_SPOS_STND2
  DOOM::AbstractThing::State{ Sprite_SPOS, 0, false, 3, &A_Chase, State_SPOS_RUN2 },		// State_SPOS_RUN1
  DOOM::AbstractThing::State{ Sprite_SPOS, 0, false, 3, &A_Chase, State_SPOS_RUN3 },		// State_SPOS_RUN2
  DOOM::AbstractThing::State{ Sprite_SPOS, 1, false, 3, &A_Chase, State_SPOS_RUN4 },		// State_SPOS_RUN3
  DOOM::AbstractThing::State{ Sprite_SPOS, 1, false, 3, &A_Chase, State_SPOS_RUN5 },		// State_SPOS_RUN4
  DOOM::AbstractThing::State{ Sprite_SPOS, 2, false, 3, &A_Chase, State_SPOS_RUN6 },		// State_SPOS_RUN5
  DOOM::AbstractThing::State{ Sprite_SPOS, 2, false, 3, &A_Chase, State_SPOS_RUN7 },		// State_SPOS_RUN6
  DOOM::AbstractThing::State{ Sprite_SPOS, 3, false, 3, &A_Chase, State_SPOS_RUN8 },		// State_SPOS_RUN7
  DOOM::AbstractThing::State{ Sprite_SPOS, 3, false, 3, &A_Chase, State_SPOS_RUN1 },		// State_SPOS_RUN8
  DOOM::AbstractThing::State{ Sprite_SPOS, 4, false, 10, &A_FaceTarget, State_SPOS_ATK2 },	// State_SPOS_ATK1
  DOOM::AbstractThing::State{ Sprite_SPOS, 5, true, 10, &A_SPosAttack, State_SPOS_ATK3 },	// State_SPOS_ATK2
  DOOM::AbstractThing::State{ Sprite_SPOS, 4, false, 10, nullptr, State_SPOS_RUN1 },		// State_SPOS_ATK3
  DOOM::AbstractThing::State{ Sprite_SPOS, 6, false, 3, nullptr, State_SPOS_PAIN2 },		// State_SPOS_PAIN
  DOOM::AbstractThing::State{ Sprite_SPOS, 6, false, 3, &A_Pain, State_SPOS_RUN1 },		// State_SPOS_PAIN2
  DOOM::AbstractThing::State{ Sprite_SPOS, 7, false, 5, nullptr, State_SPOS_DIE2 },		// State_SPOS_DIE1
  DOOM::AbstractThing::State{ Sprite_SPOS, 8, false, 5, &A_Scream, State_SPOS_DIE3 },		// State_SPOS_DIE2
  DOOM::AbstractThing::State{ Sprite_SPOS, 9, false, 5, &A_Fall, State_SPOS_DIE4 },		// State_SPOS_DIE3
  DOOM::AbstractThing::State{ Sprite_SPOS, 10, false, 5, nullptr, State_SPOS_DIE5 },		// State_SPOS_DIE4
  DOOM::AbstractThing::State{ Sprite_SPOS, 11, false, -1, nullptr, State_None },		// State_SPOS_DIE5
  DOOM::AbstractThing::State{ Sprite_SPOS, 12, false, 5, nullptr, State_SPOS_XDIE2 },		// State_SPOS_XDIE1
  DOOM::AbstractThing::State{ Sprite_SPOS, 13, false, 5, &A_XScream, State_SPOS_XDIE3 },	// State_SPOS_XDIE2
  DOOM::AbstractThing::State{ Sprite_SPOS, 14, false, 5, &A_Fall, State_SPOS_XDIE4 },		// State_SPOS_XDIE3
  DOOM::AbstractThing::State{ Sprite_SPOS, 15, false, 5, nullptr, State_SPOS_XDIE5 },		// State_SPOS_XDIE4
  DOOM::AbstractThing::State{ Sprite_SPOS, 16, false, 5, nullptr, State_SPOS_XDIE6 },		// State_SPOS_XDIE5
  DOOM::AbstractThing::State{ Sprite_SPOS, 17, false, 5, nullptr, State_SPOS_XDIE7 },		// State_SPOS_XDIE6
  DOOM::AbstractThing::State{ Sprite_SPOS, 18, false, 5, nullptr, State_SPOS_XDIE8 },		// State_SPOS_XDIE7
  DOOM::AbstractThing::State{ Sprite_SPOS, 19, false, 5, nullptr, State_SPOS_XDIE9 },		// State_SPOS_XDIE8
  DOOM::AbstractThing::State{ Sprite_SPOS, 20, false, -1, nullptr, State_None },		// State_SPOS_XDIE9
  DOOM::AbstractThing::State{ Sprite_SPOS, 11, false, 5, nullptr, State_SPOS_RAISE2 },		// State_SPOS_RAISE1
  DOOM::AbstractThing::State{ Sprite_SPOS, 10, false, 5, nullptr, State_SPOS_RAISE3 },		// State_SPOS_RAISE2
  DOOM::AbstractThing::State{ Sprite_SPOS, 9, false, 5, nullptr, State_SPOS_RAISE4 },		// State_SPOS_RAISE3
  DOOM::AbstractThing::State{ Sprite_SPOS, 8, false, 5, nullptr, State_SPOS_RAISE5 },		// State_SPOS_RAISE4
  DOOM::AbstractThing::State{ Sprite_SPOS, 7, false, 5, nullptr, State_SPOS_RUN1 },		// State_SPOState_RAISE5
  DOOM::AbstractThing::State{ Sprite_VILE, 0, false, 10, &A_Look, State_VILE_STND2 },		// State_VILE_STND
  DOOM::AbstractThing::State{ Sprite_VILE, 1, false, 10, &A_Look, State_VILE_STND },		// State_VILE_STND2
  DOOM::AbstractThing::State{ Sprite_VILE, 0, false, 2, &A_VileChase, State_VILE_RUN2 },	// State_VILE_RUN1
  DOOM::AbstractThing::State{ Sprite_VILE, 0, false, 2, &A_VileChase, State_VILE_RUN3 },	// State_VILE_RUN2
  DOOM::AbstractThing::State{ Sprite_VILE, 1, false, 2, &A_VileChase, State_VILE_RUN4 },	// State_VILE_RUN3
  DOOM::AbstractThing::State{ Sprite_VILE, 1, false, 2, &A_VileChase, State_VILE_RUN5 },	// State_VILE_RUN4
  DOOM::AbstractThing::State{ Sprite_VILE, 2, false, 2, &A_VileChase, State_VILE_RUN6 },	// State_VILE_RUN5
  DOOM::AbstractThing::State{ Sprite_VILE, 2, false, 2, &A_VileChase, State_VILE_RUN7 },	// State_VILE_RUN6
  DOOM::AbstractThing::State{ Sprite_VILE, 3, false, 2, &A_VileChase, State_VILE_RUN8 },	// State_VILE_RUN7
  DOOM::AbstractThing::State{ Sprite_VILE, 3, false, 2, &A_VileChase, State_VILE_RUN9 },	// State_VILE_RUN8
  DOOM::AbstractThing::State{ Sprite_VILE, 4, false, 2, &A_VileChase, State_VILE_RUN10 },	// State_VILE_RUN9
  DOOM::AbstractThing::State{ Sprite_VILE, 4, false, 2, &A_VileChase, State_VILE_RUN11 },	// State_VILE_RUN10
  DOOM::AbstractThing::State{ Sprite_VILE, 5, false, 2, &A_VileChase, State_VILE_RUN12 },	// State_VILE_RUN11
  DOOM::AbstractThing::State{ Sprite_VILE, 5, false, 2, &A_VileChase, State_VILE_RUN1 },	// State_VILE_RUN12
  DOOM::AbstractThing::State{ Sprite_VILE, 6, true, 0, &A_VileStart, State_VILE_ATK2 },		// State_VILE_ATK1
  DOOM::AbstractThing::State{ Sprite_VILE, 6, true, 10, &A_FaceTarget, State_VILE_ATK3 },	// State_VILE_ATK2
  DOOM::AbstractThing::State{ Sprite_VILE, 7, true, 8, &A_VileTarget, State_VILE_ATK4 },	// State_VILE_ATK3
  DOOM::AbstractThing::State{ Sprite_VILE, 8, true, 8, &A_FaceTarget, State_VILE_ATK5 },	// State_VILE_ATK4
  DOOM::AbstractThing::State{ Sprite_VILE, 9, true, 8, &A_FaceTarget, State_VILE_ATK6 },	// State_VILE_ATK5
  DOOM::AbstractThing::State{ Sprite_VILE, 10, true, 8, &A_FaceTarget, State_VILE_ATK7 },	// State_VILE_ATK6
  DOOM::AbstractThing::State{ Sprite_VILE, 11, true, 8, &A_FaceTarget, State_VILE_ATK8 },	// State_VILE_ATK7
  DOOM::AbstractThing::State{ Sprite_VILE, 12, true, 8, &A_FaceTarget, State_VILE_ATK9 },	// State_VILE_ATK8
  DOOM::AbstractThing::State{ Sprite_VILE, 13, true, 8, &A_FaceTarget, State_VILE_ATK10 },	// State_VILE_ATK9
  DOOM::AbstractThing::State{ Sprite_VILE, 14, true, 8, &A_VileAttack, State_VILE_ATK11 },	// State_VILE_ATK10
  DOOM::AbstractThing::State{ Sprite_VILE, 15, true, 20, nullptr, State_VILE_RUN1 },		// State_VILE_ATK11
  DOOM::AbstractThing::State{ Sprite_VILE, 36, true, 10, nullptr, State_VILE_HEAL2 },		// State_VILE_HEAL1
  DOOM::AbstractThing::State{ Sprite_VILE, 37, true, 10, nullptr, State_VILE_HEAL3 },		// State_VILE_HEAL2
  DOOM::AbstractThing::State{ Sprite_VILE, 38, true, 10, nullptr, State_VILE_RUN1 },		// State_VILE_HEAL3
  DOOM::AbstractThing::State{ Sprite_VILE, 16, false, 5, nullptr, State_VILE_PAIN2 },		// State_VILE_PAIN
  DOOM::AbstractThing::State{ Sprite_VILE, 16, false, 5, &A_Pain, State_VILE_RUN1 },		// State_VILE_PAIN2
  DOOM::AbstractThing::State{ Sprite_VILE, 16, false, 7, nullptr, State_VILE_DIE2 },		// State_VILE_DIE1
  DOOM::AbstractThing::State{ Sprite_VILE, 17, false, 7, &A_Scream, State_VILE_DIE3 },		// State_VILE_DIE2
  DOOM::AbstractThing::State{ Sprite_VILE, 18, false, 7, &A_Fall, State_VILE_DIE4 },		// State_VILE_DIE3
  DOOM::AbstractThing::State{ Sprite_VILE, 19, false, 7, nullptr, State_VILE_DIE5 },		// State_VILE_DIE4
  DOOM::AbstractThing::State{ Sprite_VILE, 20, false, 7, nullptr, State_VILE_DIE6 },		// State_VILE_DIE5
  DOOM::AbstractThing::State{ Sprite_VILE, 21, false, 7, nullptr, State_VILE_DIE7 },		// State_VILE_DIE6
  DOOM::AbstractThing::State{ Sprite_VILE, 22, false, 7, nullptr, State_VILE_DIE8 },		// State_VILE_DIE7
  DOOM::AbstractThing::State{ Sprite_VILE, 23, false, 5, nullptr, State_VILE_DIE9 },		// State_VILE_DIE8
  DOOM::AbstractThing::State{ Sprite_VILE, 24, false, 5, nullptr, State_VILE_DIE10 },		// State_VILE_DIE9
  DOOM::AbstractThing::State{ Sprite_VILE, 25, false, -1, nullptr, State_None },		// State_VILE_DIE10
  DOOM::AbstractThing::State{ Sprite_FIRE, 0, true, 2, &A_StartFire, State_FIRE2 },		// State_FIRE1
  DOOM::AbstractThing::State{ Sprite_FIRE, 1, true, 2, &A_Fire, State_FIRE3 },			// State_FIRE2
  DOOM::AbstractThing::State{ Sprite_FIRE, 0, true, 2, &A_Fire, State_FIRE4 },			// State_FIRE3
  DOOM::AbstractThing::State{ Sprite_FIRE, 1, true, 2, &A_Fire, State_FIRE5 },			// State_FIRE4
  DOOM::AbstractThing::State{ Sprite_FIRE, 2, true, 2, &A_FireCrackle, State_FIRE6 },		// State_FIRE5
  DOOM::AbstractThing::State{ Sprite_FIRE, 1, true, 2, &A_Fire, State_FIRE7 },			// State_FIRE6
  DOOM::AbstractThing::State{ Sprite_FIRE, 2, true, 2, &A_Fire, State_FIRE8 },			// State_FIRE7
  DOOM::AbstractThing::State{ Sprite_FIRE, 1, true, 2, &A_Fire, State_FIRE9 },			// State_FIRE8
  DOOM::AbstractThing::State{ Sprite_FIRE, 2, true, 2, &A_Fire, State_FIRE10 },			// State_FIRE9
  DOOM::AbstractThing::State{ Sprite_FIRE, 3, true, 2, &A_Fire, State_FIRE11 },			// State_FIRE10
  DOOM::AbstractThing::State{ Sprite_FIRE, 2, true, 2, &A_Fire, State_FIRE12 },			// State_FIRE11
  DOOM::AbstractThing::State{ Sprite_FIRE, 3, true, 2, &A_Fire, State_FIRE13 },			// State_FIRE12
  DOOM::AbstractThing::State{ Sprite_FIRE, 2, true, 2, &A_Fire, State_FIRE14 },			// State_FIRE13
  DOOM::AbstractThing::State{ Sprite_FIRE, 3, true, 2, &A_Fire, State_FIRE15 },			// State_FIRE14
  DOOM::AbstractThing::State{ Sprite_FIRE, 4, true, 2, &A_Fire, State_FIRE16 },			// State_FIRE15
  DOOM::AbstractThing::State{ Sprite_FIRE, 3, true, 2, &A_Fire, State_FIRE17 },			// State_FIRE16
  DOOM::AbstractThing::State{ Sprite_FIRE, 4, true, 2, &A_Fire, State_FIRE18 },			// State_FIRE17
  DOOM::AbstractThing::State{ Sprite_FIRE, 3, true, 2, &A_Fire, State_FIRE19 },			// State_FIRE18
  DOOM::AbstractThing::State{ Sprite_FIRE, 4, true, 2, &A_FireCrackle, State_FIRE20 },		// State_FIRE19
  DOOM::AbstractThing::State{ Sprite_FIRE, 5, true, 2, &A_Fire, State_FIRE21 },			// State_FIRE20
  DOOM::AbstractThing::State{ Sprite_FIRE, 4, true, 2, &A_Fire, State_FIRE22 },			// State_FIRE21
  DOOM::AbstractThing::State{ Sprite_FIRE, 5, true, 2, &A_Fire, State_FIRE23 },			// State_FIRE22
  DOOM::AbstractThing::State{ Sprite_FIRE, 4, true, 2, &A_Fire, State_FIRE24 },			// State_FIRE23
  DOOM::AbstractThing::State{ Sprite_FIRE, 5, true, 2, &A_Fire, State_FIRE25 },			// State_FIRE24
  DOOM::AbstractThing::State{ Sprite_FIRE, 6, true, 2, &A_Fire, State_FIRE26 },			// State_FIRE25
  DOOM::AbstractThing::State{ Sprite_FIRE, 7, true, 2, &A_Fire, State_FIRE27 },			// State_FIRE26
  DOOM::AbstractThing::State{ Sprite_FIRE, 6, true, 2, &A_Fire, State_FIRE28 },			// State_FIRE27
  DOOM::AbstractThing::State{ Sprite_FIRE, 7, true, 2, &A_Fire, State_FIRE29 },			// State_FIRE28
  DOOM::AbstractThing::State{ Sprite_FIRE, 6, true, 2, &A_Fire, State_FIRE30 },			// State_FIRE29
  DOOM::AbstractThing::State{ Sprite_FIRE, 7, true, 2, &A_Fire, State_None },			// State_FIRE30
  DOOM::AbstractThing::State{ Sprite_PUFF, 1, false, 4, nullptr, State_SMOKE2 },		// State_SMOKE1
  DOOM::AbstractThing::State{ Sprite_PUFF, 2, false, 4, nullptr, State_SMOKE3 },		// State_SMOKE2
  DOOM::AbstractThing::State{ Sprite_PUFF, 1, false, 4, nullptr, State_SMOKE4 },		// State_SMOKE3
  DOOM::AbstractThing::State{ Sprite_PUFF, 2, false, 4, nullptr, State_SMOKE5 },		// State_SMOKE4
  DOOM::AbstractThing::State{ Sprite_PUFF, 3, false, 4, nullptr, State_None },			// State_SMOKE5
  DOOM::AbstractThing::State{ Sprite_FATB, 0, true, 2, &A_Tracer, State_TRACER2 },		// State_TRACER
  DOOM::AbstractThing::State{ Sprite_FATB, 1, true, 2, &A_Tracer, State_TRACER },		// State_TRACER2
  DOOM::AbstractThing::State{ Sprite_FBXP, 0, true, 8, nullptr, State_TRACEEXP2 },		// State_TRACEEXP1
  DOOM::AbstractThing::State{ Sprite_FBXP, 1, true, 6, nullptr, State_TRACEEXP3 },		// State_TRACEEXP2
  DOOM::AbstractThing::State{ Sprite_FBXP, 2, true, 4, nullptr, State_None },			// State_TRACEEXP3
  DOOM::AbstractThing::State{ Sprite_SKEL, 0, false, 10, &A_Look, State_SKEL_STND2 },		// State_SKEL_STND
  DOOM::AbstractThing::State{ Sprite_SKEL, 1, false, 10, &A_Look, State_SKEL_STND },		// State_SKEL_STND2
  DOOM::AbstractThing::State{ Sprite_SKEL, 0, false, 2, &A_Chase, State_SKEL_RUN2 },		// State_SKEL_RUN1
  DOOM::AbstractThing::State{ Sprite_SKEL, 0, false, 2, &A_Chase, State_SKEL_RUN3 },		// State_SKEL_RUN2
  DOOM::AbstractThing::State{ Sprite_SKEL, 1, false, 2, &A_Chase, State_SKEL_RUN4 },		// State_SKEL_RUN3
  DOOM::AbstractThing::State{ Sprite_SKEL, 1, false, 2, &A_Chase, State_SKEL_RUN5 },		// State_SKEL_RUN4
  DOOM::AbstractThing::State{ Sprite_SKEL, 2, false, 2, &A_Chase, State_SKEL_RUN6 },		// State_SKEL_RUN5
  DOOM::AbstractThing::State{ Sprite_SKEL, 2, false, 2, &A_Chase, State_SKEL_RUN7 },		// State_SKEL_RUN6
  DOOM::AbstractThing::State{ Sprite_SKEL, 3, false, 2, &A_Chase, State_SKEL_RUN8 },		// State_SKEL_RUN7
  DOOM::AbstractThing::State{ Sprite_SKEL, 3, false, 2, &A_Chase, State_SKEL_RUN9 },		// State_SKEL_RUN8
  DOOM::AbstractThing::State{ Sprite_SKEL, 4, false, 2, &A_Chase, State_SKEL_RUN10 },		// State_SKEL_RUN9
  DOOM::AbstractThing::State{ Sprite_SKEL, 4, false, 2, &A_Chase, State_SKEL_RUN11 },		// State_SKEL_RUN10
  DOOM::AbstractThing::State{ Sprite_SKEL, 5, false, 2, &A_Chase, State_SKEL_RUN12 },		// State_SKEL_RUN11
  DOOM::AbstractThing::State{ Sprite_SKEL, 5, false, 2, &A_Chase, State_SKEL_RUN1 },		// State_SKEL_RUN12
  DOOM::AbstractThing::State{ Sprite_SKEL, 6, false, 0, &A_FaceTarget, State_SKEL_FIST2 },	// State_SKEL_FIST1
  DOOM::AbstractThing::State{ Sprite_SKEL, 6, false, 6, &A_SkelWhoosh, State_SKEL_FIST3 },	// State_SKEL_FIST2
  DOOM::AbstractThing::State{ Sprite_SKEL, 7, false, 6, &A_FaceTarget, State_SKEL_FIST4 },	// State_SKEL_FIST3
  DOOM::AbstractThing::State{ Sprite_SKEL, 8, false, 6, &A_SkelFist, State_SKEL_RUN1 },		// State_SKEL_FIST4
  DOOM::AbstractThing::State{ Sprite_SKEL, 9, true, 0, &A_FaceTarget, State_SKEL_MISS2 },	// State_SKEL_MISS1
  DOOM::AbstractThing::State{ Sprite_SKEL, 9, true, 10, &A_FaceTarget, State_SKEL_MISS3 },	// State_SKEL_MISS2
  DOOM::AbstractThing::State{ Sprite_SKEL, 10, false, 10, &A_SkelMissile, State_SKEL_MISS4 },	// State_SKEL_MISS3
  DOOM::AbstractThing::State{ Sprite_SKEL, 10, false, 10, &A_FaceTarget, State_SKEL_RUN1 },	// State_SKEL_MISS4
  DOOM::AbstractThing::State{ Sprite_SKEL, 11, false, 5, nullptr, State_SKEL_PAIN2 },		// State_SKEL_PAIN
  DOOM::AbstractThing::State{ Sprite_SKEL, 11, false, 5, &A_Pain, State_SKEL_RUN1 },		// State_SKEL_PAIN2
  DOOM::AbstractThing::State{ Sprite_SKEL, 11, false, 7, nullptr, State_SKEL_DIE2 },		// State_SKEL_DIE1
  DOOM::AbstractThing::State{ Sprite_SKEL, 12, false, 7, nullptr, State_SKEL_DIE3 },		// State_SKEL_DIE2
  DOOM::AbstractThing::State{ Sprite_SKEL, 13, false, 7, &A_Scream, State_SKEL_DIE4 },		// State_SKEL_DIE3
  DOOM::AbstractThing::State{ Sprite_SKEL, 14, false, 7, &A_Fall, State_SKEL_DIE5 },		// State_SKEL_DIE4
  DOOM::AbstractThing::State{ Sprite_SKEL, 15, false, 7, nullptr, State_SKEL_DIE6 },		// State_SKEL_DIE5
  DOOM::AbstractThing::State{ Sprite_SKEL, 16, false, -1, nullptr, State_None },		// State_SKEL_DIE6
  DOOM::AbstractThing::State{ Sprite_SKEL, 16, false, 5, nullptr, State_SKEL_RAISE2 },		// State_SKEL_RAISE1
  DOOM::AbstractThing::State{ Sprite_SKEL, 15, false, 5, nullptr, State_SKEL_RAISE3 },		// State_SKEL_RAISE2
  DOOM::AbstractThing::State{ Sprite_SKEL, 14, false, 5, nullptr, State_SKEL_RAISE4 },		// State_SKEL_RAISE3
  DOOM::AbstractThing::State{ Sprite_SKEL, 13, false, 5, nullptr, State_SKEL_RAISE5 },		// State_SKEL_RAISE4
  DOOM::AbstractThing::State{ Sprite_SKEL, 12, false, 5, nullptr, State_SKEL_RAISE6 },		// State_SKEL_RAISE5
  DOOM::AbstractThing::State{ Sprite_SKEL, 11, false, 5, nullptr, State_SKEL_RUN1 },		// State_SKEL_RAISE6
  DOOM::AbstractThing::State{ Sprite_MANF, 0, true, 4, nullptr, State_FATSHOT2 },		// State_FATSHOT1
  DOOM::AbstractThing::State{ Sprite_MANF, 1, true, 4, nullptr, State_FATSHOT1 },		// State_FATSHOT2
  DOOM::AbstractThing::State{ Sprite_MISL, 1, true, 8, nullptr, State_FATSHOTX2 },		// State_FATSHOTX1
  DOOM::AbstractThing::State{ Sprite_MISL, 2, true, 6, nullptr, State_FATSHOTX3 },		// State_FATSHOTX2
  DOOM::AbstractThing::State{ Sprite_MISL, 3, true, 4, nullptr, State_None },			// State_FATSHOTX3
  DOOM::AbstractThing::State{ Sprite_FATT, 0, false, 15, &A_Look, State_FATT_STND2 },		// State_FATT_STND
  DOOM::AbstractThing::State{ Sprite_FATT, 1, false, 15, &A_Look, State_FATT_STND },		// State_FATT_STND2
  DOOM::AbstractThing::State{ Sprite_FATT, 0, false, 4, &A_Chase, State_FATT_RUN2 },		// State_FATT_RUN1
  DOOM::AbstractThing::State{ Sprite_FATT, 0, false, 4, &A_Chase, State_FATT_RUN3 },		// State_FATT_RUN2
  DOOM::AbstractThing::State{ Sprite_FATT, 1, false, 4, &A_Chase, State_FATT_RUN4 },		// State_FATT_RUN3
  DOOM::AbstractThing::State{ Sprite_FATT, 1, false, 4, &A_Chase, State_FATT_RUN5 },		// State_FATT_RUN4
  DOOM::AbstractThing::State{ Sprite_FATT, 2, false, 4, &A_Chase, State_FATT_RUN6 },		// State_FATT_RUN5
  DOOM::AbstractThing::State{ Sprite_FATT, 2, false, 4, &A_Chase, State_FATT_RUN7 },		// State_FATT_RUN6
  DOOM::AbstractThing::State{ Sprite_FATT, 3, false, 4, &A_Chase, State_FATT_RUN8 },		// State_FATT_RUN7
  DOOM::AbstractThing::State{ Sprite_FATT, 3, false, 4, &A_Chase, State_FATT_RUN9 },		// State_FATT_RUN8
  DOOM::AbstractThing::State{ Sprite_FATT, 4, false, 4, &A_Chase, State_FATT_RUN10 },		// State_FATT_RUN9
  DOOM::AbstractThing::State{ Sprite_FATT, 4, false, 4, &A_Chase, State_FATT_RUN11 },		// State_FATT_RUN10
  DOOM::AbstractThing::State{ Sprite_FATT, 5, false, 4, &A_Chase, State_FATT_RUN12 },		// State_FATT_RUN11
  DOOM::AbstractThing::State{ Sprite_FATT, 5, false, 4, &A_Chase, State_FATT_RUN1 },		// State_FATT_RUN12
  DOOM::AbstractThing::State{ Sprite_FATT, 6, false, 20, &A_FatRaise, State_FATT_ATK2 },	// State_FATT_ATK1
  DOOM::AbstractThing::State{ Sprite_FATT, 7, true, 10, &A_FatAttack1, State_FATT_ATK3 },	// State_FATT_ATK2
  DOOM::AbstractThing::State{ Sprite_FATT, 8, false, 5, &A_FaceTarget, State_FATT_ATK4 },	// State_FATT_ATK3
  DOOM::AbstractThing::State{ Sprite_FATT, 6, false, 5, &A_FaceTarget, State_FATT_ATK5 },	// State_FATT_ATK4
  DOOM::AbstractThing::State{ Sprite_FATT, 7, true, 10, &A_FatAttack2, State_FATT_ATK6 },	// State_FATT_ATK5
  DOOM::AbstractThing::State{ Sprite_FATT, 8, false, 5, &A_FaceTarget, State_FATT_ATK7 },	// State_FATT_ATK6
  DOOM::AbstractThing::State{ Sprite_FATT, 6, false, 5, &A_FaceTarget, State_FATT_ATK8 },	// State_FATT_ATK7
  DOOM::AbstractThing::State{ Sprite_FATT, 7, true, 10, &A_FatAttack3, State_FATT_ATK9 },	// State_FATT_ATK8
  DOOM::AbstractThing::State{ Sprite_FATT, 8, false, 5, &A_FaceTarget, State_FATT_ATK10 },	// State_FATT_ATK9
  DOOM::AbstractThing::State{ Sprite_FATT, 6, false, 5, &A_FaceTarget, State_FATT_RUN1 },	// State_FATT_ATK10
  DOOM::AbstractThing::State{ Sprite_FATT, 9, false, 3, nullptr, State_FATT_PAIN2 },		// State_FATT_PAIN
  DOOM::AbstractThing::State{ Sprite_FATT, 9, false, 3, &A_Pain, State_FATT_RUN1 },		// State_FATT_PAIN2
  DOOM::AbstractThing::State{ Sprite_FATT, 10, false, 6, nullptr, State_FATT_DIE2 },		// State_FATT_DIE1
  DOOM::AbstractThing::State{ Sprite_FATT, 11, false, 6, &A_Scream, State_FATT_DIE3 },		// State_FATT_DIE2
  DOOM::AbstractThing::State{ Sprite_FATT, 12, false, 6, &A_Fall, State_FATT_DIE4 },		// State_FATT_DIE3
  DOOM::AbstractThing::State{ Sprite_FATT, 13, false, 6, nullptr, State_FATT_DIE5 },		// State_FATT_DIE4
  DOOM::AbstractThing::State{ Sprite_FATT, 14, false, 6, nullptr, State_FATT_DIE6 },		// State_FATT_DIE5
  DOOM::AbstractThing::State{ Sprite_FATT, 15, false, 6, nullptr, State_FATT_DIE7 },		// State_FATT_DIE6
  DOOM::AbstractThing::State{ Sprite_FATT, 16, false, 6, nullptr, State_FATT_DIE8 },		// State_FATT_DIE7
  DOOM::AbstractThing::State{ Sprite_FATT, 17, false, 6, nullptr, State_FATT_DIE9 },		// State_FATT_DIE8
  DOOM::AbstractThing::State{ Sprite_FATT, 18, false, 6, nullptr, State_FATT_DIE10 },		// State_FATT_DIE9
  DOOM::AbstractThing::State{ Sprite_FATT, 19, false, -1, &A_BossDeath, State_None },		// State_FATT_DIE10
  DOOM::AbstractThing::State{ Sprite_FATT, 17, false, 5, nullptr, State_FATT_RAISE2 },		// State_FATT_RAISE1
  DOOM::AbstractThing::State{ Sprite_FATT, 16, false, 5, nullptr, State_FATT_RAISE3 },		// State_FATT_RAISE2
  DOOM::AbstractThing::State{ Sprite_FATT, 15, false, 5, nullptr, State_FATT_RAISE4 },		// State_FATT_RAISE3
  DOOM::AbstractThing::State{ Sprite_FATT, 14, false, 5, nullptr, State_FATT_RAISE5 },		// State_FATT_RAISE4
  DOOM::AbstractThing::State{ Sprite_FATT, 13, false, 5, nullptr, State_FATT_RAISE6 },		// State_FATT_RAISE5
  DOOM::AbstractThing::State{ Sprite_FATT, 12, false, 5, nullptr, State_FATT_RAISE7 },		// State_FATT_RAISE6
  DOOM::AbstractThing::State{ Sprite_FATT, 11, false, 5, nullptr, State_FATT_RAISE8 },		// State_FATT_RAISE7
  DOOM::AbstractThing::State{ Sprite_FATT, 10, false, 5, nullptr, State_FATT_RUN1 },		// State_FATT_RAISE8
  DOOM::AbstractThing::State{ Sprite_CPOS, 0, false, 10, &A_Look, State_CPOS_STND2 },		// State_CPOS_STND
  DOOM::AbstractThing::State{ Sprite_CPOS, 1, false, 10, &A_Look, State_CPOS_STND },		// State_CPOS_STND2
  DOOM::AbstractThing::State{ Sprite_CPOS, 0, false, 3, &A_Chase, State_CPOS_RUN2 },		// State_CPOS_RUN1
  DOOM::AbstractThing::State{ Sprite_CPOS, 0, false, 3, &A_Chase, State_CPOS_RUN3 },		// State_CPOS_RUN2
  DOOM::AbstractThing::State{ Sprite_CPOS, 1, false, 3, &A_Chase, State_CPOS_RUN4 },		// State_CPOS_RUN3
  DOOM::AbstractThing::State{ Sprite_CPOS, 1, false, 3, &A_Chase, State_CPOS_RUN5 },		// State_CPOS_RUN4
  DOOM::AbstractThing::State{ Sprite_CPOS, 2, false, 3, &A_Chase, State_CPOS_RUN6 },		// State_CPOS_RUN5
  DOOM::AbstractThing::State{ Sprite_CPOS, 2, false, 3, &A_Chase, State_CPOS_RUN7 },		// State_CPOS_RUN6
  DOOM::AbstractThing::State{ Sprite_CPOS, 3, false, 3, &A_Chase, State_CPOS_RUN8 },		// State_CPOS_RUN7
  DOOM::AbstractThing::State{ Sprite_CPOS, 3, false, 3, &A_Chase, State_CPOS_RUN1 },		// State_CPOS_RUN8
  DOOM::AbstractThing::State{ Sprite_CPOS, 4, false, 10, &A_FaceTarget, State_CPOS_ATK2 },	// State_CPOS_ATK1
  DOOM::AbstractThing::State{ Sprite_CPOS, 5, true, 4, &A_CPosAttack, State_CPOS_ATK3 },	// State_CPOS_ATK2
  DOOM::AbstractThing::State{ Sprite_CPOS, 4, true, 4, &A_CPosAttack, State_CPOS_ATK4 },	// State_CPOS_ATK3
  DOOM::AbstractThing::State{ Sprite_CPOS, 5, false, 1, &A_CPosRefire, State_CPOS_ATK2 },	// State_CPOS_ATK4
  DOOM::AbstractThing::State{ Sprite_CPOS, 6, false, 3, nullptr, State_CPOS_PAIN2 },		// State_CPOS_PAIN
  DOOM::AbstractThing::State{ Sprite_CPOS, 6, false, 3, &A_Pain, State_CPOS_RUN1 },		// State_CPOS_PAIN2
  DOOM::AbstractThing::State{ Sprite_CPOS, 7, false, 5, nullptr, State_CPOS_DIE2 },		// State_CPOS_DIE1
  DOOM::AbstractThing::State{ Sprite_CPOS, 8, false, 5, &A_Scream, State_CPOS_DIE3 },		// State_CPOS_DIE2
  DOOM::AbstractThing::State{ Sprite_CPOS, 9, false, 5, &A_Fall, State_CPOS_DIE4 },		// State_CPOS_DIE3
  DOOM::AbstractThing::State{ Sprite_CPOS, 10, false, 5, nullptr, State_CPOS_DIE5 },		// State_CPOS_DIE4
  DOOM::AbstractThing::State{ Sprite_CPOS, 11, false, 5, nullptr, State_CPOS_DIE6 },		// State_CPOS_DIE5
  DOOM::AbstractThing::State{ Sprite_CPOS, 12, false, 5, nullptr, State_CPOS_DIE7 },		// State_CPOS_DIE6
  DOOM::AbstractThing::State{ Sprite_CPOS, 13, false, -1, nullptr, State_None },		// State_CPOS_DIE7
  DOOM::AbstractThing::State{ Sprite_CPOS, 14, false, 5, nullptr, State_CPOS_XDIE2 },		// State_CPOS_XDIE1
  DOOM::AbstractThing::State{ Sprite_CPOS, 15, false, 5, &A_XScream, State_CPOS_XDIE3 },	// State_CPOS_XDIE2
  DOOM::AbstractThing::State{ Sprite_CPOS, 16, false, 5, &A_Fall, State_CPOS_XDIE4 },		// State_CPOS_XDIE3
  DOOM::AbstractThing::State{ Sprite_CPOS, 17, false, 5, nullptr, State_CPOS_XDIE5 },		// State_CPOS_XDIE4
  DOOM::AbstractThing::State{ Sprite_CPOS, 18, false, 5, nullptr, State_CPOS_XDIE6 },		// State_CPOS_XDIE5
  DOOM::AbstractThing::State{ Sprite_CPOS, 19, false, -1, nullptr, State_None },		// State_CPOS_XDIE6
  DOOM::AbstractThing::State{ Sprite_CPOS, 13, false, 5, nullptr, State_CPOS_RAISE2 },		// State_CPOS_RAISE1
  DOOM::AbstractThing::State{ Sprite_CPOS, 12, false, 5, nullptr, State_CPOS_RAISE3 },		// State_CPOS_RAISE2
  DOOM::AbstractThing::State{ Sprite_CPOS, 11, false, 5, nullptr, State_CPOS_RAISE4 },		// State_CPOS_RAISE3
  DOOM::AbstractThing::State{ Sprite_CPOS, 10, false, 5, nullptr, State_CPOS_RAISE5 },		// State_CPOS_RAISE4
  DOOM::AbstractThing::State{ Sprite_CPOS, 9, false, 5, nullptr, State_CPOS_RAISE6 },		// State_CPOS_RAISE5
  DOOM::AbstractThing::State{ Sprite_CPOS, 8, false, 5, nullptr, State_CPOS_RAISE7 },		// State_CPOS_RAISE6
  DOOM::AbstractThing::State{ Sprite_CPOS, 7, false, 5, nullptr, State_CPOS_RUN1 },		// State_CPOState_RAISE7
  DOOM::AbstractThing::State{ Sprite_TROO, 0, false, 10, &A_Look, State_TROO_STND2 },		// State_TROO_STND
  DOOM::AbstractThing::State{ Sprite_TROO, 1, false, 10, &A_Look, State_TROO_STND },		// State_TROO_STND2
  DOOM::AbstractThing::State{ Sprite_TROO, 0, false, 3, &A_Chase, State_TROO_RUN2 },		// State_TROO_RUN1
  DOOM::AbstractThing::State{ Sprite_TROO, 0, false, 3, &A_Chase, State_TROO_RUN3 },		// State_TROO_RUN2
  DOOM::AbstractThing::State{ Sprite_TROO, 1, false, 3, &A_Chase, State_TROO_RUN4 },		// State_TROO_RUN3
  DOOM::AbstractThing::State{ Sprite_TROO, 1, false, 3, &A_Chase, State_TROO_RUN5 },		// State_TROO_RUN4
  DOOM::AbstractThing::State{ Sprite_TROO, 2, false, 3, &A_Chase, State_TROO_RUN6 },		// State_TROO_RUN5
  DOOM::AbstractThing::State{ Sprite_TROO, 2, false, 3, &A_Chase, State_TROO_RUN7 },		// State_TROO_RUN6
  DOOM::AbstractThing::State{ Sprite_TROO, 3, false, 3, &A_Chase, State_TROO_RUN8 },		// State_TROO_RUN7
  DOOM::AbstractThing::State{ Sprite_TROO, 3, false, 3, &A_Chase, State_TROO_RUN1 },		// State_TROO_RUN8
  DOOM::AbstractThing::State{ Sprite_TROO, 4, false, 8, &A_FaceTarget, State_TROO_ATK2 },	// State_TROO_ATK1
  DOOM::AbstractThing::State{ Sprite_TROO, 5, false, 8, &A_FaceTarget, State_TROO_ATK3 },	// State_TROO_ATK2
  DOOM::AbstractThing::State{ Sprite_TROO, 6, false, 6, &A_TroopAttack, State_TROO_RUN1 },	// State_TROO_ATK3
  DOOM::AbstractThing::State{ Sprite_TROO, 7, false, 2, nullptr, State_TROO_PAIN2 },		// State_TROO_PAIN
  DOOM::AbstractThing::State{ Sprite_TROO, 7, false, 2, &A_Pain, State_TROO_RUN1 },		// State_TROO_PAIN2
  DOOM::AbstractThing::State{ Sprite_TROO, 8, false, 8, nullptr, State_TROO_DIE2 },		// State_TROO_DIE1
  DOOM::AbstractThing::State{ Sprite_TROO, 9, false, 8, &A_Scream, State_TROO_DIE3 },		// State_TROO_DIE2
  DOOM::AbstractThing::State{ Sprite_TROO, 10, false, 6, nullptr, State_TROO_DIE4 },		// State_TROO_DIE3
  DOOM::AbstractThing::State{ Sprite_TROO, 11, false, 6, &A_Fall, State_TROO_DIE5 },		// State_TROO_DIE4
  DOOM::AbstractThing::State{ Sprite_TROO, 12, false, -1, nullptr, State_None },		// State_TROO_DIE5
  DOOM::AbstractThing::State{ Sprite_TROO, 13, false, 5, nullptr, State_TROO_XDIE2 },		// State_TROO_XDIE1
  DOOM::AbstractThing::State{ Sprite_TROO, 14, false, 5, &A_XScream, State_TROO_XDIE3 },	// State_TROO_XDIE2
  DOOM::AbstractThing::State{ Sprite_TROO, 15, false, 5, nullptr, State_TROO_XDIE4 },		// State_TROO_XDIE3
  DOOM::AbstractThing::State{ Sprite_TROO, 16, false, 5, &A_Fall, State_TROO_XDIE5 },		// State_TROO_XDIE4
  DOOM::AbstractThing::State{ Sprite_TROO, 17, false, 5, nullptr, State_TROO_XDIE6 },		// State_TROO_XDIE5
  DOOM::AbstractThing::State{ Sprite_TROO, 18, false, 5, nullptr, State_TROO_XDIE7 },		// State_TROO_XDIE6
  DOOM::AbstractThing::State{ Sprite_TROO, 19, false, 5, nullptr, State_TROO_XDIE8 },		// State_TROO_XDIE7
  DOOM::AbstractThing::State{ Sprite_TROO, 20, false, -1, nullptr, State_None },		// State_TROO_XDIE8
  DOOM::AbstractThing::State{ Sprite_TROO, 12, false, 8, nullptr, State_TROO_RAISE2 },		// State_TROO_RAISE1
  DOOM::AbstractThing::State{ Sprite_TROO, 11, false, 8, nullptr, State_TROO_RAISE3 },		// State_TROO_RAISE2
  DOOM::AbstractThing::State{ Sprite_TROO, 10, false, 6, nullptr, State_TROO_RAISE4 },		// State_TROO_RAISE3
  DOOM::AbstractThing::State{ Sprite_TROO, 9, false, 6, nullptr, State_TROO_RAISE5 },		// State_TROO_RAISE4
  DOOM::AbstractThing::State{ Sprite_TROO, 8, false, 6, nullptr, State_TROO_RUN1 },		// State_TROO_RAISE5
  DOOM::AbstractThing::State{ Sprite_SARG, 0, false, 10, &A_Look, State_SARG_STND2 },		// State_SARG_STND
  DOOM::AbstractThing::State{ Sprite_SARG, 1, false, 10, &A_Look, State_SARG_STND },		// State_SARG_STND2
  DOOM::AbstractThing::State{ Sprite_SARG, 0, false, 2, &A_Chase, State_SARG_RUN2 },		// State_SARG_RUN1
  DOOM::AbstractThing::State{ Sprite_SARG, 0, false, 2, &A_Chase, State_SARG_RUN3 },		// State_SARG_RUN2
  DOOM::AbstractThing::State{ Sprite_SARG, 1, false, 2, &A_Chase, State_SARG_RUN4 },		// State_SARG_RUN3
  DOOM::AbstractThing::State{ Sprite_SARG, 1, false, 2, &A_Chase, State_SARG_RUN5 },		// State_SARG_RUN4
  DOOM::AbstractThing::State{ Sprite_SARG, 2, false, 2, &A_Chase, State_SARG_RUN6 },		// State_SARG_RUN5
  DOOM::AbstractThing::State{ Sprite_SARG, 2, false, 2, &A_Chase, State_SARG_RUN7 },		// State_SARG_RUN6
  DOOM::AbstractThing::State{ Sprite_SARG, 3, false, 2, &A_Chase, State_SARG_RUN8 },		// State_SARG_RUN7
  DOOM::AbstractThing::State{ Sprite_SARG, 3, false, 2, &A_Chase, State_SARG_RUN1 },		// State_SARG_RUN8
  DOOM::AbstractThing::State{ Sprite_SARG, 4, false, 8, &A_FaceTarget, State_SARG_ATK2 },	// State_SARG_ATK1
  DOOM::AbstractThing::State{ Sprite_SARG, 5, false, 8, &A_FaceTarget, State_SARG_ATK3 },	// State_SARG_ATK2
  DOOM::AbstractThing::State{ Sprite_SARG, 6, false, 8, &A_SargAttack, State_SARG_RUN1 },	// State_SARG_ATK3
  DOOM::AbstractThing::State{ Sprite_SARG, 7, false, 2, nullptr, State_SARG_PAIN2 },		// State_SARG_PAIN
  DOOM::AbstractThing::State{ Sprite_SARG, 7, false, 2, &A_Pain, State_SARG_RUN1 },		// State_SARG_PAIN2
  DOOM::AbstractThing::State{ Sprite_SARG, 8, false, 8, nullptr, State_SARG_DIE2 },		// State_SARG_DIE1
  DOOM::AbstractThing::State{ Sprite_SARG, 9, false, 8, &A_Scream, State_SARG_DIE3 },		// State_SARG_DIE2
  DOOM::AbstractThing::State{ Sprite_SARG, 10, false, 4, nullptr, State_SARG_DIE4 },		// State_SARG_DIE3
  DOOM::AbstractThing::State{ Sprite_SARG, 11, false, 4, &A_Fall, State_SARG_DIE5 },		// State_SARG_DIE4
  DOOM::AbstractThing::State{ Sprite_SARG, 12, false, 4, nullptr, State_SARG_DIE6 },		// State_SARG_DIE5
  DOOM::AbstractThing::State{ Sprite_SARG, 13, false, -1, nullptr, State_None },		// State_SARG_DIE6
  DOOM::AbstractThing::State{ Sprite_SARG, 13, false, 5, nullptr, State_SARG_RAISE2 },		// State_SARG_RAISE1
  DOOM::AbstractThing::State{ Sprite_SARG, 12, false, 5, nullptr, State_SARG_RAISE3 },		// State_SARG_RAISE2
  DOOM::AbstractThing::State{ Sprite_SARG, 11, false, 5, nullptr, State_SARG_RAISE4 },		// State_SARG_RAISE3
  DOOM::AbstractThing::State{ Sprite_SARG, 10, false, 5, nullptr, State_SARG_RAISE5 },		// State_SARG_RAISE4
  DOOM::AbstractThing::State{ Sprite_SARG, 9, false, 5, nullptr, State_SARG_RAISE6 },		// State_SARG_RAISE5
  DOOM::AbstractThing::State{ Sprite_SARG, 8, false, 5, nullptr, State_SARG_RUN1 },		// State_SARG_RAISE6
  DOOM::AbstractThing::State{ Sprite_HEAD, 0, false, 10, &A_Look, State_HEAD_STND },		// State_HEAD_STND
  DOOM::AbstractThing::State{ Sprite_HEAD, 0, false, 3, &A_Chase, State_HEAD_RUN1 },		// State_HEAD_RUN1
  DOOM::AbstractThing::State{ Sprite_HEAD, 1, false, 5, &A_FaceTarget, State_HEAD_ATK2 },	// State_HEAD_ATK1
  DOOM::AbstractThing::State{ Sprite_HEAD, 2, false, 5, &A_FaceTarget, State_HEAD_ATK3 },	// State_HEAD_ATK2
  DOOM::AbstractThing::State{ Sprite_HEAD, 3, true, 5, &A_HeadAttack, State_HEAD_RUN1 },	// State_HEAD_ATK3
  DOOM::AbstractThing::State{ Sprite_HEAD, 4, false, 3, nullptr, State_HEAD_PAIN2 },		// State_HEAD_PAIN
  DOOM::AbstractThing::State{ Sprite_HEAD, 4, false, 3, &A_Pain, State_HEAD_PAIN3 },		// State_HEAD_PAIN2
  DOOM::AbstractThing::State{ Sprite_HEAD, 5, false, 6, nullptr, State_HEAD_RUN1 },		// State_HEAD_PAIN3
  DOOM::AbstractThing::State{ Sprite_HEAD, 6, false, 8, nullptr, State_HEAD_DIE2 },		// State_HEAD_DIE1
  DOOM::AbstractThing::State{ Sprite_HEAD, 7, false, 8, &A_Scream, State_HEAD_DIE3 },		// State_HEAD_DIE2
  DOOM::AbstractThing::State{ Sprite_HEAD, 8, false, 8, nullptr, State_HEAD_DIE4 },		// State_HEAD_DIE3
  DOOM::AbstractThing::State{ Sprite_HEAD, 9, false, 8, nullptr, State_HEAD_DIE5 },		// State_HEAD_DIE4
  DOOM::AbstractThing::State{ Sprite_HEAD, 10, false, 8, &A_Fall, State_HEAD_DIE6 },		// State_HEAD_DIE5
  DOOM::AbstractThing::State{ Sprite_HEAD, 11, false, -1, nullptr, State_None },		// State_HEAD_DIE6
  DOOM::AbstractThing::State{ Sprite_HEAD, 11, false, 8, nullptr, State_HEAD_RAISE2 },		// State_HEAD_RAISE1
  DOOM::AbstractThing::State{ Sprite_HEAD, 10, false, 8, nullptr, State_HEAD_RAISE3 },		// State_HEAD_RAISE2
  DOOM::AbstractThing::State{ Sprite_HEAD, 9, false, 8, nullptr, State_HEAD_RAISE4 },		// State_HEAD_RAISE3
  DOOM::AbstractThing::State{ Sprite_HEAD, 8, false, 8, nullptr, State_HEAD_RAISE5 },		// State_HEAD_RAISE4
  DOOM::AbstractThing::State{ Sprite_HEAD, 7, false, 8, nullptr, State_HEAD_RAISE6 },		// State_HEAD_RAISE5
  DOOM::AbstractThing::State{ Sprite_HEAD, 6, false, 8, nullptr, State_HEAD_RUN1 },		// State_HEAD_RAISE6
  DOOM::AbstractThing::State{ Sprite_BAL7, 0, true, 4, nullptr, State_BRBALL2 },		// State_BRBALL1
  DOOM::AbstractThing::State{ Sprite_BAL7, 1, true, 4, nullptr, State_BRBALL1 },		// State_BRBALL2
  DOOM::AbstractThing::State{ Sprite_BAL7, 2, true, 6, nullptr, State_BRBALLX2 },		// State_BRBALLX1
  DOOM::AbstractThing::State{ Sprite_BAL7, 3, true, 6, nullptr, State_BRBALLX3 },		// State_BRBALLX2
  DOOM::AbstractThing::State{ Sprite_BAL7, 4, true, 6, nullptr, State_None },			// State_BRBALLX3
  DOOM::AbstractThing::State{ Sprite_BOSS, 0, false, 10, &A_Look, State_BOSS_STND2 },		// State_BOSS_STND
  DOOM::AbstractThing::State{ Sprite_BOSS, 1, false, 10, &A_Look, State_BOSS_STND },		// State_BOSS_STND2
  DOOM::AbstractThing::State{ Sprite_BOSS, 0, false, 3, &A_Chase, State_BOSS_RUN2 },		// State_BOSS_RUN1
  DOOM::AbstractThing::State{ Sprite_BOSS, 0, false, 3, &A_Chase, State_BOSS_RUN3 },		// State_BOSS_RUN2
  DOOM::AbstractThing::State{ Sprite_BOSS, 1, false, 3, &A_Chase, State_BOSS_RUN4 },		// State_BOSS_RUN3
  DOOM::AbstractThing::State{ Sprite_BOSS, 1, false, 3, &A_Chase, State_BOSS_RUN5 },		// State_BOSS_RUN4
  DOOM::AbstractThing::State{ Sprite_BOSS, 2, false, 3, &A_Chase, State_BOSS_RUN6 },		// State_BOSS_RUN5
  DOOM::AbstractThing::State{ Sprite_BOSS, 2, false, 3, &A_Chase, State_BOSS_RUN7 },		// State_BOSS_RUN6
  DOOM::AbstractThing::State{ Sprite_BOSS, 3, false, 3, &A_Chase, State_BOSS_RUN8 },		// State_BOSS_RUN7
  DOOM::AbstractThing::State{ Sprite_BOSS, 3, false, 3, &A_Chase, State_BOSS_RUN1 },		// State_BOSS_RUN8
  DOOM::AbstractThing::State{ Sprite_BOSS, 4, false, 8, &A_FaceTarget, State_BOSS_ATK2 },	// State_BOSS_ATK1
  DOOM::AbstractThing::State{ Sprite_BOSS, 5, false, 8, &A_FaceTarget, State_BOSS_ATK3 },	// State_BOSS_ATK2
  DOOM::AbstractThing::State{ Sprite_BOSS, 6, false, 8, &A_BruisAttack, State_BOSS_RUN1 },	// State_BOSS_ATK3
  DOOM::AbstractThing::State{ Sprite_BOSS, 7, false, 2, nullptr, State_BOSS_PAIN2 },		// State_BOSS_PAIN
  DOOM::AbstractThing::State{ Sprite_BOSS, 7, false, 2, &A_Pain, State_BOSS_RUN1 },		// State_BOSS_PAIN2
  DOOM::AbstractThing::State{ Sprite_BOSS, 8, false, 8, nullptr, State_BOSS_DIE2 },		// State_BOSS_DIE1
  DOOM::AbstractThing::State{ Sprite_BOSS, 9, false, 8, &A_Scream, State_BOSS_DIE3 },		// State_BOSS_DIE2
  DOOM::AbstractThing::State{ Sprite_BOSS, 10, false, 8, nullptr, State_BOSS_DIE4 },		// State_BOSS_DIE3
  DOOM::AbstractThing::State{ Sprite_BOSS, 11, false, 8, &A_Fall, State_BOSS_DIE5 },		// State_BOSS_DIE4
  DOOM::AbstractThing::State{ Sprite_BOSS, 12, false, 8, nullptr, State_BOSS_DIE6 },		// State_BOSS_DIE5
  DOOM::AbstractThing::State{ Sprite_BOSS, 13, false, 8, nullptr, State_BOSS_DIE7 },		// State_BOSS_DIE6
  DOOM::AbstractThing::State{ Sprite_BOSS, 14, false, -1, &A_BossDeath, State_None },		// State_BOSS_DIE7
  DOOM::AbstractThing::State{ Sprite_BOSS, 14, false, 8, nullptr, State_BOSS_RAISE2 },		// State_BOSS_RAISE1
  DOOM::AbstractThing::State{ Sprite_BOSS, 13, false, 8, nullptr, State_BOSS_RAISE3 },		// State_BOSS_RAISE2
  DOOM::AbstractThing::State{ Sprite_BOSS, 12, false, 8, nullptr, State_BOSS_RAISE4 },		// State_BOSS_RAISE3
  DOOM::AbstractThing::State{ Sprite_BOSS, 11, false, 8, nullptr, State_BOSS_RAISE5 },		// State_BOSS_RAISE4
  DOOM::AbstractThing::State{ Sprite_BOSS, 10, false, 8, nullptr, State_BOSS_RAISE6 },		// State_BOSS_RAISE5
  DOOM::AbstractThing::State{ Sprite_BOSS, 9, false, 8, nullptr, State_BOSS_RAISE7 },		// State_BOSS_RAISE6
  DOOM::AbstractThing::State{ Sprite_BOSS, 8, false, 8, nullptr, State_BOSS_RUN1 },		// State_BOSS_RAISE7
  DOOM::AbstractThing::State{ Sprite_BOS2, 0, false, 10, &A_Look, State_BOS2_STND2 },		// State_BOS2_STND
  DOOM::AbstractThing::State{ Sprite_BOS2, 1, false, 10, &A_Look, State_BOS2_STND },		// State_BOS2_STND2
  DOOM::AbstractThing::State{ Sprite_BOS2, 0, false, 3, &A_Chase, State_BOS2_RUN2 },		// State_BOS2_RUN1
  DOOM::AbstractThing::State{ Sprite_BOS2, 0, false, 3, &A_Chase, State_BOS2_RUN3 },		// State_BOS2_RUN2
  DOOM::AbstractThing::State{ Sprite_BOS2, 1, false, 3, &A_Chase, State_BOS2_RUN4 },		// State_BOS2_RUN3
  DOOM::AbstractThing::State{ Sprite_BOS2, 1, false, 3, &A_Chase, State_BOS2_RUN5 },		// State_BOS2_RUN4
  DOOM::AbstractThing::State{ Sprite_BOS2, 2, false, 3, &A_Chase, State_BOS2_RUN6 },		// State_BOS2_RUN5
  DOOM::AbstractThing::State{ Sprite_BOS2, 2, false, 3, &A_Chase, State_BOS2_RUN7 },		// State_BOS2_RUN6
  DOOM::AbstractThing::State{ Sprite_BOS2, 3, false, 3, &A_Chase, State_BOS2_RUN8 },		// State_BOS2_RUN7
  DOOM::AbstractThing::State{ Sprite_BOS2, 3, false, 3, &A_Chase, State_BOS2_RUN1 },		// State_BOS2_RUN8
  DOOM::AbstractThing::State{ Sprite_BOS2, 4, false, 8, &A_FaceTarget, State_BOS2_ATK2 },	// State_BOS2_ATK1
  DOOM::AbstractThing::State{ Sprite_BOS2, 5, false, 8, &A_FaceTarget, State_BOS2_ATK3 },	// State_BOS2_ATK2
  DOOM::AbstractThing::State{ Sprite_BOS2, 6, false, 8, &A_BruisAttack, State_BOS2_RUN1 },	// State_BOS2_ATK3
  DOOM::AbstractThing::State{ Sprite_BOS2, 7, false, 2, nullptr, State_BOS2_PAIN2 },		// State_BOS2_PAIN
  DOOM::AbstractThing::State{ Sprite_BOS2, 7, false, 2, &A_Pain, State_BOS2_RUN1 },		// State_BOS2_PAIN2
  DOOM::AbstractThing::State{ Sprite_BOS2, 8, false, 8, nullptr, State_BOS2_DIE2 },		// State_BOS2_DIE1
  DOOM::AbstractThing::State{ Sprite_BOS2, 9, false, 8, &A_Scream, State_BOS2_DIE3 },		// State_BOS2_DIE2
  DOOM::AbstractThing::State{ Sprite_BOS2, 10, false, 8, nullptr, State_BOS2_DIE4 },		// State_BOS2_DIE3
  DOOM::AbstractThing::State{ Sprite_BOS2, 11, false, 8, &A_Fall, State_BOS2_DIE5 },		// State_BOS2_DIE4
  DOOM::AbstractThing::State{ Sprite_BOS2, 12, false, 8, nullptr, State_BOS2_DIE6 },		// State_BOS2_DIE5
  DOOM::AbstractThing::State{ Sprite_BOS2, 13, false, 8, nullptr, State_BOS2_DIE7 },		// State_BOS2_DIE6
  DOOM::AbstractThing::State{ Sprite_BOS2, 14, false, -1, nullptr, State_None },		// State_BOS2_DIE7
  DOOM::AbstractThing::State{ Sprite_BOS2, 14, false, 8, nullptr, State_BOS2_RAISE2 },		// State_BOS2_RAISE1
  DOOM::AbstractThing::State{ Sprite_BOS2, 13, false, 8, nullptr, State_BOS2_RAISE3 },		// State_BOS2_RAISE2
  DOOM::AbstractThing::State{ Sprite_BOS2, 12, false, 8, nullptr, State_BOS2_RAISE4 },		// State_BOS2_RAISE3
  DOOM::AbstractThing::State{ Sprite_BOS2, 11, false, 8, nullptr, State_BOS2_RAISE5 },		// State_BOS2_RAISE4
  DOOM::AbstractThing::State{ Sprite_BOS2, 10, false, 8, nullptr, State_BOS2_RAISE6 },		// State_BOS2_RAISE5
  DOOM::AbstractThing::State{ Sprite_BOS2, 9, false, 8, nullptr, State_BOS2_RAISE7 },		// State_BOS2_RAISE6
  DOOM::AbstractThing::State{ Sprite_BOS2, 8, false, 8, nullptr, State_BOS2_RUN1 },		// State_BOS2_RAISE7
  DOOM::AbstractThing::State{ Sprite_SKUL, 0, true, 10, &A_Look, State_SKULL_STND2 },		// State_SKULL_STND
  DOOM::AbstractThing::State{ Sprite_SKUL, 1, true, 10, &A_Look, State_SKULL_STND },		// State_SKULL_STND2
  DOOM::AbstractThing::State{ Sprite_SKUL, 0, true, 6, &A_Chase, State_SKULL_RUN2 },		// State_SKULL_RUN1
  DOOM::AbstractThing::State{ Sprite_SKUL, 1, true, 6, &A_Chase, State_SKULL_RUN1 },		// State_SKULL_RUN2
  DOOM::AbstractThing::State{ Sprite_SKUL, 2, true, 10, &A_FaceTarget, State_SKULL_ATK2 },	// State_SKULL_ATK1
  DOOM::AbstractThing::State{ Sprite_SKUL, 3, true, 4, &A_SkullAttack, State_SKULL_ATK3 },	// State_SKULL_ATK2
  DOOM::AbstractThing::State{ Sprite_SKUL, 2, true, 4, nullptr, State_SKULL_ATK4 },		// State_SKULL_ATK3
  DOOM::AbstractThing::State{ Sprite_SKUL, 3, true, 4, nullptr, State_SKULL_ATK3 },		// State_SKULL_ATK4
  DOOM::AbstractThing::State{ Sprite_SKUL, 4, true, 3, nullptr, State_SKULL_PAIN2 },		// State_SKULL_PAIN
  DOOM::AbstractThing::State{ Sprite_SKUL, 4, true, 3, &A_Pain, State_SKULL_RUN1 },		// State_SKULL_PAIN2
  DOOM::AbstractThing::State{ Sprite_SKUL, 5, true, 6, nullptr, State_SKULL_DIE2 },		// State_SKULL_DIE1
  DOOM::AbstractThing::State{ Sprite_SKUL, 6, true, 6, &A_Scream, State_SKULL_DIE3 },		// State_SKULL_DIE2
  DOOM::AbstractThing::State{ Sprite_SKUL, 7, true, 6, nullptr, State_SKULL_DIE4 },		// State_SKULL_DIE3
  DOOM::AbstractThing::State{ Sprite_SKUL, 8, true, 6, &A_Fall, State_SKULL_DIE5 },		// State_SKULL_DIE4
  DOOM::AbstractThing::State{ Sprite_SKUL, 9, false, 6, nullptr, State_SKULL_DIE6 },		// State_SKULL_DIE5
  DOOM::AbstractThing::State{ Sprite_SKUL, 10, false, 6, nullptr, State_None },			// State_SKULL_DIE6
  DOOM::AbstractThing::State{ Sprite_SPID, 0, false, 10, &A_Look, State_SPID_STND2 },		// State_SPID_STND
  DOOM::AbstractThing::State{ Sprite_SPID, 1, false, 10, &A_Look, State_SPID_STND },		// State_SPID_STND2
  DOOM::AbstractThing::State{ Sprite_SPID, 0, false, 3, &A_Metal, State_SPID_RUN2 },		// State_SPID_RUN1
  DOOM::AbstractThing::State{ Sprite_SPID, 0, false, 3, &A_Chase, State_SPID_RUN3 },		// State_SPID_RUN2
  DOOM::AbstractThing::State{ Sprite_SPID, 1, false, 3, &A_Chase, State_SPID_RUN4 },		// State_SPID_RUN3
  DOOM::AbstractThing::State{ Sprite_SPID, 1, false, 3, &A_Chase, State_SPID_RUN5 },		// State_SPID_RUN4
  DOOM::AbstractThing::State{ Sprite_SPID, 2, false, 3, &A_Metal, State_SPID_RUN6 },		// State_SPID_RUN5
  DOOM::AbstractThing::State{ Sprite_SPID, 2, false, 3, &A_Chase, State_SPID_RUN7 },		// State_SPID_RUN6
  DOOM::AbstractThing::State{ Sprite_SPID, 3, false, 3, &A_Chase, State_SPID_RUN8 },		// State_SPID_RUN7
  DOOM::AbstractThing::State{ Sprite_SPID, 3, false, 3, &A_Chase, State_SPID_RUN9 },		// State_SPID_RUN8
  DOOM::AbstractThing::State{ Sprite_SPID, 4, false, 3, &A_Metal, State_SPID_RUN10 },		// State_SPID_RUN9
  DOOM::AbstractThing::State{ Sprite_SPID, 4, false, 3, &A_Chase, State_SPID_RUN11 },		// State_SPID_RUN10
  DOOM::AbstractThing::State{ Sprite_SPID, 5, false, 3, &A_Chase, State_SPID_RUN12 },		// State_SPID_RUN11
  DOOM::AbstractThing::State{ Sprite_SPID, 5, false, 3, &A_Chase, State_SPID_RUN1 },		// State_SPID_RUN12
  DOOM::AbstractThing::State{ Sprite_SPID, 0, true, 20, &A_FaceTarget, State_SPID_ATK2 },	// State_SPID_ATK1
  DOOM::AbstractThing::State{ Sprite_SPID, 6, true, 4, &A_SPosAttack, State_SPID_ATK3 },	// State_SPID_ATK2
  DOOM::AbstractThing::State{ Sprite_SPID, 7, true, 4, &A_SPosAttack, State_SPID_ATK4 },	// State_SPID_ATK3
  DOOM::AbstractThing::State{ Sprite_SPID, 7, true, 1, &A_SpidRefire, State_SPID_ATK2 },	// State_SPID_ATK4
  DOOM::AbstractThing::State{ Sprite_SPID, 8, false, 3, nullptr, State_SPID_PAIN2 },		// State_SPID_PAIN
  DOOM::AbstractThing::State{ Sprite_SPID, 8, false, 3, &A_Pain, State_SPID_RUN1 },		// State_SPID_PAIN2
  DOOM::AbstractThing::State{ Sprite_SPID, 9, false, 20, &A_Scream, State_SPID_DIE2 },		// State_SPID_DIE1
  DOOM::AbstractThing::State{ Sprite_SPID, 10, false, 10, &A_Fall, State_SPID_DIE3 },		// State_SPID_DIE2
  DOOM::AbstractThing::State{ Sprite_SPID, 11, false, 10, nullptr, State_SPID_DIE4 },		// State_SPID_DIE3
  DOOM::AbstractThing::State{ Sprite_SPID, 12, false, 10, nullptr, State_SPID_DIE5 },		// State_SPID_DIE4
  DOOM::AbstractThing::State{ Sprite_SPID, 13, false, 10, nullptr, State_SPID_DIE6 },		// State_SPID_DIE5
  DOOM::AbstractThing::State{ Sprite_SPID, 14, false, 10, nullptr, State_SPID_DIE7 },		// State_SPID_DIE6
  DOOM::AbstractThing::State{ Sprite_SPID, 15, false, 10, nullptr, State_SPID_DIE8 },		// State_SPID_DIE7
  DOOM::AbstractThing::State{ Sprite_SPID, 16, false, 10, nullptr, State_SPID_DIE9 },		// State_SPID_DIE8
  DOOM::AbstractThing::State{ Sprite_SPID, 17, false, 10, nullptr, State_SPID_DIE10 },		// State_SPID_DIE9
  DOOM::AbstractThing::State{ Sprite_SPID, 18, false, 30, nullptr, State_SPID_DIE11 },		// State_SPID_DIE10
  DOOM::AbstractThing::State{ Sprite_SPID, 18, false, -1, &A_BossDeath, State_None },		// State_SPID_DIE11
  DOOM::AbstractThing::State{ Sprite_BSPI, 0, false, 10, &A_Look, State_BSPI_STND2 },		// State_BSPI_STND
  DOOM::AbstractThing::State{ Sprite_BSPI, 1, false, 10, &A_Look, State_BSPI_STND },		// State_BSPI_STND2
  DOOM::AbstractThing::State{ Sprite_BSPI, 0, false, 20, nullptr, State_BSPI_RUN1 },		// State_BSPI_SIGHT
  DOOM::AbstractThing::State{ Sprite_BSPI, 0, false, 3, &A_BabyMetal, State_BSPI_RUN2 },	// State_BSPI_RUN1
  DOOM::AbstractThing::State{ Sprite_BSPI, 0, false, 3, &A_Chase, State_BSPI_RUN3 },		// State_BSPI_RUN2
  DOOM::AbstractThing::State{ Sprite_BSPI, 1, false, 3, &A_Chase, State_BSPI_RUN4 },		// State_BSPI_RUN3
  DOOM::AbstractThing::State{ Sprite_BSPI, 1, false, 3, &A_Chase, State_BSPI_RUN5 },		// State_BSPI_RUN4
  DOOM::AbstractThing::State{ Sprite_BSPI, 2, false, 3, &A_Chase, State_BSPI_RUN6 },		// State_BSPI_RUN5
  DOOM::AbstractThing::State{ Sprite_BSPI, 2, false, 3, &A_Chase, State_BSPI_RUN7 },		// State_BSPI_RUN6
  DOOM::AbstractThing::State{ Sprite_BSPI, 3, false, 3, &A_BabyMetal, State_BSPI_RUN8 },	// State_BSPI_RUN7
  DOOM::AbstractThing::State{ Sprite_BSPI, 3, false, 3, &A_Chase, State_BSPI_RUN9 },		// State_BSPI_RUN8
  DOOM::AbstractThing::State{ Sprite_BSPI, 4, false, 3, &A_Chase, State_BSPI_RUN10 },		// State_BSPI_RUN9
  DOOM::AbstractThing::State{ Sprite_BSPI, 4, false, 3, &A_Chase, State_BSPI_RUN11 },		// State_BSPI_RUN10
  DOOM::AbstractThing::State{ Sprite_BSPI, 5, false, 3, &A_Chase, State_BSPI_RUN12 },		// State_BSPI_RUN11
  DOOM::AbstractThing::State{ Sprite_BSPI, 5, false, 3, &A_Chase, State_BSPI_RUN1 },		// State_BSPI_RUN12
  DOOM::AbstractThing::State{ Sprite_BSPI, 0, true, 20, &A_FaceTarget, State_BSPI_ATK2 },	// State_BSPI_ATK1
  DOOM::AbstractThing::State{ Sprite_BSPI, 6, true, 4, &A_BspiAttack, State_BSPI_ATK3 },	// State_BSPI_ATK2
  DOOM::AbstractThing::State{ Sprite_BSPI, 7, true, 4, nullptr, State_BSPI_ATK4 },		// State_BSPI_ATK3
  DOOM::AbstractThing::State{ Sprite_BSPI, 7, true, 1, &A_SpidRefire, State_BSPI_ATK2 },	// State_BSPI_ATK4
  DOOM::AbstractThing::State{ Sprite_BSPI, 8, false, 3, nullptr, State_BSPI_PAIN2 },		// State_BSPI_PAIN
  DOOM::AbstractThing::State{ Sprite_BSPI, 8, false, 3, &A_Pain, State_BSPI_RUN1 },		// State_BSPI_PAIN2
  DOOM::AbstractThing::State{ Sprite_BSPI, 9, false, 20, &A_Scream, State_BSPI_DIE2 },		// State_BSPI_DIE1
  DOOM::AbstractThing::State{ Sprite_BSPI, 10, false, 7, &A_Fall, State_BSPI_DIE3 },		// State_BSPI_DIE2
  DOOM::AbstractThing::State{ Sprite_BSPI, 11, false, 7, nullptr, State_BSPI_DIE4 },		// State_BSPI_DIE3
  DOOM::AbstractThing::State{ Sprite_BSPI, 12, false, 7, nullptr, State_BSPI_DIE5 },		// State_BSPI_DIE4
  DOOM::AbstractThing::State{ Sprite_BSPI, 13, false, 7, nullptr, State_BSPI_DIE6 },		// State_BSPI_DIE5
  DOOM::AbstractThing::State{ Sprite_BSPI, 14, false, 7, nullptr, State_BSPI_DIE7 },		// State_BSPI_DIE6
  DOOM::AbstractThing::State{ Sprite_BSPI, 15, false, -1, &A_BossDeath, State_None },		// State_BSPI_DIE7
  DOOM::AbstractThing::State{ Sprite_BSPI, 15, false, 5, nullptr, State_BSPI_RAISE2 },		// State_BSPI_RAISE1
  DOOM::AbstractThing::State{ Sprite_BSPI, 14, false, 5, nullptr, State_BSPI_RAISE3 },		// State_BSPI_RAISE2
  DOOM::AbstractThing::State{ Sprite_BSPI, 13, false, 5, nullptr, State_BSPI_RAISE4 },		// State_BSPI_RAISE3
  DOOM::AbstractThing::State{ Sprite_BSPI, 12, false, 5, nullptr, State_BSPI_RAISE5 },		// State_BSPI_RAISE4
  DOOM::AbstractThing::State{ Sprite_BSPI, 11, false, 5, nullptr, State_BSPI_RAISE6 },		// State_BSPI_RAISE5
  DOOM::AbstractThing::State{ Sprite_BSPI, 10, false, 5, nullptr, State_BSPI_RAISE7 },		// State_BSPI_RAISE6
  DOOM::AbstractThing::State{ Sprite_BSPI, 9, false, 5, nullptr, State_BSPI_RUN1 },		// State_BSPI_RAISE7
  DOOM::AbstractThing::State{ Sprite_APLS, 0, true, 5, nullptr, State_ARACH_PLAZ2 },		// State_ARACH_PLAZ
  DOOM::AbstractThing::State{ Sprite_APLS, 1, true, 5, nullptr, State_ARACH_PLAZ },		// State_ARACH_PLAZ2
  DOOM::AbstractThing::State{ Sprite_APBX, 0, true, 5, nullptr, State_ARACH_PLEX2 },		// State_ARACH_PLEX
  DOOM::AbstractThing::State{ Sprite_APBX, 1, true, 5, nullptr, State_ARACH_PLEX3 },		// State_ARACH_PLEX2
  DOOM::AbstractThing::State{ Sprite_APBX, 2, true, 5, nullptr, State_ARACH_PLEX4 },		// State_ARACH_PLEX3
  DOOM::AbstractThing::State{ Sprite_APBX, 3, true, 5, nullptr, State_ARACH_PLEX5 },		// State_ARACH_PLEX4
  DOOM::AbstractThing::State{ Sprite_APBX, 4, true, 5, nullptr, State_None },			// State_ARACH_PLEX5
  DOOM::AbstractThing::State{ Sprite_CYBR, 0, false, 10, &A_Look, State_CYBER_STND2 },		// State_CYBER_STND
  DOOM::AbstractThing::State{ Sprite_CYBR, 1, false, 10, &A_Look, State_CYBER_STND },		// State_CYBER_STND2
  DOOM::AbstractThing::State{ Sprite_CYBR, 0, false, 3, &A_Hoof, State_CYBER_RUN2 },		// State_CYBER_RUN1
  DOOM::AbstractThing::State{ Sprite_CYBR, 0, false, 3, &A_Chase, State_CYBER_RUN3 },		// State_CYBER_RUN2
  DOOM::AbstractThing::State{ Sprite_CYBR, 1, false, 3, &A_Chase, State_CYBER_RUN4 },		// State_CYBER_RUN3
  DOOM::AbstractThing::State{ Sprite_CYBR, 1, false, 3, &A_Chase, State_CYBER_RUN5 },		// State_CYBER_RUN4
  DOOM::AbstractThing::State{ Sprite_CYBR, 2, false, 3, &A_Chase, State_CYBER_RUN6 },		// State_CYBER_RUN5
  DOOM::AbstractThing::State{ Sprite_CYBR, 2, false, 3, &A_Chase, State_CYBER_RUN7 },		// State_CYBER_RUN6
  DOOM::AbstractThing::State{ Sprite_CYBR, 3, false, 3, &A_Metal, State_CYBER_RUN8 },		// State_CYBER_RUN7
  DOOM::AbstractThing::State{ Sprite_CYBR, 3, false, 3, &A_Chase, State_CYBER_RUN1 },		// State_CYBER_RUN8
  DOOM::AbstractThing::State{ Sprite_CYBR, 4, false, 6, &A_FaceTarget, State_CYBER_ATK2 },	// State_CYBER_ATK1
  DOOM::AbstractThing::State{ Sprite_CYBR, 5, false, 12, &A_CyberAttack, State_CYBER_ATK3 },	// State_CYBER_ATK2
  DOOM::AbstractThing::State{ Sprite_CYBR, 4, false, 12, &A_FaceTarget, State_CYBER_ATK4 },	// State_CYBER_ATK3
  DOOM::AbstractThing::State{ Sprite_CYBR, 5, false, 12, &A_CyberAttack, State_CYBER_ATK5 },	// State_CYBER_ATK4
  DOOM::AbstractThing::State{ Sprite_CYBR, 4, false, 12, &A_FaceTarget, State_CYBER_ATK6 },	// State_CYBER_ATK5
  DOOM::AbstractThing::State{ Sprite_CYBR, 5, false, 12, &A_CyberAttack, State_CYBER_RUN1 },	// State_CYBER_ATK6
  DOOM::AbstractThing::State{ Sprite_CYBR, 6, false, 10, &A_Pain, State_CYBER_RUN1 },		// State_CYBER_PAIN
  DOOM::AbstractThing::State{ Sprite_CYBR, 7, false, 10, nullptr, State_CYBER_DIE2 },		// State_CYBER_DIE1
  DOOM::AbstractThing::State{ Sprite_CYBR, 8, false, 10, &A_Scream, State_CYBER_DIE3 },		// State_CYBER_DIE2
  DOOM::AbstractThing::State{ Sprite_CYBR, 9, false, 10, nullptr, State_CYBER_DIE4 },		// State_CYBER_DIE3
  DOOM::AbstractThing::State{ Sprite_CYBR, 10, false, 10, nullptr, State_CYBER_DIE5 },		// State_CYBER_DIE4
  DOOM::AbstractThing::State{ Sprite_CYBR, 11, false, 10, nullptr, State_CYBER_DIE6 },		// State_CYBER_DIE5
  DOOM::AbstractThing::State{ Sprite_CYBR, 12, false, 10, &A_Fall, State_CYBER_DIE7 },		// State_CYBER_DIE6
  DOOM::AbstractThing::State{ Sprite_CYBR, 13, false, 10, nullptr, State_CYBER_DIE8 },		// State_CYBER_DIE7
  DOOM::AbstractThing::State{ Sprite_CYBR, 14, false, 10, nullptr, State_CYBER_DIE9 },		// State_CYBER_DIE8
  DOOM::AbstractThing::State{ Sprite_CYBR, 15, false, 30, nullptr, State_CYBER_DIE10 },		// State_CYBER_DIE9
  DOOM::AbstractThing::State{ Sprite_CYBR, 15, false, -1, &A_BossDeath, State_None },		// State_CYBER_DIE10
  DOOM::AbstractThing::State{ Sprite_PAIN, 0, false, 10, &A_Look, State_PAIN_STND },		// State_PAIN_STND
  DOOM::AbstractThing::State{ Sprite_PAIN, 0, false, 3, &A_Chase, State_PAIN_RUN2 },		// State_PAIN_RUN1
  DOOM::AbstractThing::State{ Sprite_PAIN, 0, false, 3, &A_Chase, State_PAIN_RUN3 },		// State_PAIN_RUN2
  DOOM::AbstractThing::State{ Sprite_PAIN, 1, false, 3, &A_Chase, State_PAIN_RUN4 },		// State_PAIN_RUN3
  DOOM::AbstractThing::State{ Sprite_PAIN, 1, false, 3, &A_Chase, State_PAIN_RUN5 },		// State_PAIN_RUN4
  DOOM::AbstractThing::State{ Sprite_PAIN, 2, false, 3, &A_Chase, State_PAIN_RUN6 },		// State_PAIN_RUN5
  DOOM::AbstractThing::State{ Sprite_PAIN, 2, false, 3, &A_Chase, State_PAIN_RUN1 },		// State_PAIN_RUN6
  DOOM::AbstractThing::State{ Sprite_PAIN, 3, false, 5, &A_FaceTarget, State_PAIN_ATK2 },	// State_PAIN_ATK1
  DOOM::AbstractThing::State{ Sprite_PAIN, 4, false, 5, &A_FaceTarget, State_PAIN_ATK3 },	// State_PAIN_ATK2
  DOOM::AbstractThing::State{ Sprite_PAIN, 5, true, 5, &A_FaceTarget, State_PAIN_ATK4 },	// State_PAIN_ATK3
  DOOM::AbstractThing::State{ Sprite_PAIN, 5, true, 0, &A_PainAttack, State_PAIN_RUN1 },	// State_PAIN_ATK4
  DOOM::AbstractThing::State{ Sprite_PAIN, 6, false, 6, nullptr, State_PAIN_PAIN2 },		// State_PAIN_PAIN
  DOOM::AbstractThing::State{ Sprite_PAIN, 6, false, 6, &A_Pain, State_PAIN_RUN1 },		// State_PAIN_PAIN2
  DOOM::AbstractThing::State{ Sprite_PAIN, 7, true, 8, nullptr, State_PAIN_DIE2 },		// State_PAIN_DIE1
  DOOM::AbstractThing::State{ Sprite_PAIN, 8, true, 8, &A_Scream, State_PAIN_DIE3 },		// State_PAIN_DIE2
  DOOM::AbstractThing::State{ Sprite_PAIN, 9, true, 8, nullptr, State_PAIN_DIE4 },		// State_PAIN_DIE3
  DOOM::AbstractThing::State{ Sprite_PAIN, 10, true, 8, nullptr, State_PAIN_DIE5 },		// State_PAIN_DIE4
  DOOM::AbstractThing::State{ Sprite_PAIN, 11, true, 8, &A_PainDie, State_PAIN_DIE6 },		// State_PAIN_DIE5
  DOOM::AbstractThing::State{ Sprite_PAIN, 12, true, 8, nullptr, State_None },			// State_PAIN_DIE6
  DOOM::AbstractThing::State{ Sprite_PAIN, 12, false, 8, nullptr, State_PAIN_RAISE2 },		// State_PAIN_RAISE1
  DOOM::AbstractThing::State{ Sprite_PAIN, 11, false, 8, nullptr, State_PAIN_RAISE3 },		// State_PAIN_RAISE2
  DOOM::AbstractThing::State{ Sprite_PAIN, 10, false, 8, nullptr, State_PAIN_RAISE4 },		// State_PAIN_RAISE3
  DOOM::AbstractThing::State{ Sprite_PAIN, 9, false, 8, nullptr, State_PAIN_RAISE5 },		// State_PAIN_RAISE4
  DOOM::AbstractThing::State{ Sprite_PAIN, 8, false, 8, nullptr, State_PAIN_RAISE6 },		// State_PAIN_RAISE5
  DOOM::AbstractThing::State{ Sprite_PAIN, 7, false, 8, nullptr, State_PAIN_RUN1 },		// State_PAIN_RAISE6
  DOOM::AbstractThing::State{ Sprite_SSWV, 0, false, 10, &A_Look, State_SSWV_STND2 },		// State_SSWV_STND
  DOOM::AbstractThing::State{ Sprite_SSWV, 1, false, 10, &A_Look, State_SSWV_STND },		// State_SSWV_STND2
  DOOM::AbstractThing::State{ Sprite_SSWV, 0, false, 3, &A_Chase, State_SSWV_RUN2 },		// State_SSWV_RUN1
  DOOM::AbstractThing::State{ Sprite_SSWV, 0, false, 3, &A_Chase, State_SSWV_RUN3 },		// State_SSWV_RUN2
  DOOM::AbstractThing::State{ Sprite_SSWV, 1, false, 3, &A_Chase, State_SSWV_RUN4 },		// State_SSWV_RUN3
  DOOM::AbstractThing::State{ Sprite_SSWV, 1, false, 3, &A_Chase, State_SSWV_RUN5 },		// State_SSWV_RUN4
  DOOM::AbstractThing::State{ Sprite_SSWV, 2, false, 3, &A_Chase, State_SSWV_RUN6 },		// State_SSWV_RUN5
  DOOM::AbstractThing::State{ Sprite_SSWV, 2, false, 3, &A_Chase, State_SSWV_RUN7 },		// State_SSWV_RUN6
  DOOM::AbstractThing::State{ Sprite_SSWV, 3, false, 3, &A_Chase, State_SSWV_RUN8 },		// State_SSWV_RUN7
  DOOM::AbstractThing::State{ Sprite_SSWV, 3, false, 3, &A_Chase, State_SSWV_RUN1 },		// State_SSWV_RUN8
  DOOM::AbstractThing::State{ Sprite_SSWV, 4, false, 10, &A_FaceTarget, State_SSWV_ATK2 },	// State_SSWV_ATK1
  DOOM::AbstractThing::State{ Sprite_SSWV, 5, false, 10, &A_FaceTarget, State_SSWV_ATK3 },	// State_SSWV_ATK2
  DOOM::AbstractThing::State{ Sprite_SSWV, 6, true, 4, &A_CPosAttack, State_SSWV_ATK4 },	// State_SSWV_ATK3
  DOOM::AbstractThing::State{ Sprite_SSWV, 5, false, 6, &A_FaceTarget, State_SSWV_ATK5 },	// State_SSWV_ATK4
  DOOM::AbstractThing::State{ Sprite_SSWV, 6, true, 4, &A_CPosAttack, State_SSWV_ATK6 },	// State_SSWV_ATK5
  DOOM::AbstractThing::State{ Sprite_SSWV, 5, false, 1, &A_CPosRefire, State_SSWV_ATK2 },	// State_SSWV_ATK6
  DOOM::AbstractThing::State{ Sprite_SSWV, 7, false, 3, nullptr, State_SSWV_PAIN2 },		// State_SSWV_PAIN
  DOOM::AbstractThing::State{ Sprite_SSWV, 7, false, 3, &A_Pain, State_SSWV_RUN1 },		// State_SSWV_PAIN2
  DOOM::AbstractThing::State{ Sprite_SSWV, 8, false, 5, nullptr, State_SSWV_DIE2 },		// State_SSWV_DIE1
  DOOM::AbstractThing::State{ Sprite_SSWV, 9, false, 5, &A_Scream, State_SSWV_DIE3 },		// State_SSWV_DIE2
  DOOM::AbstractThing::State{ Sprite_SSWV, 10, false, 5, &A_Fall, State_SSWV_DIE4 },		// State_SSWV_DIE3
  DOOM::AbstractThing::State{ Sprite_SSWV, 11, false, 5, nullptr, State_SSWV_DIE5 },		// State_SSWV_DIE4
  DOOM::AbstractThing::State{ Sprite_SSWV, 12, false, -1, nullptr, State_None },		// State_SSWV_DIE5
  DOOM::AbstractThing::State{ Sprite_SSWV, 13, false, 5, nullptr, State_SSWV_XDIE2 },		// State_SSWV_XDIE1
  DOOM::AbstractThing::State{ Sprite_SSWV, 14, false, 5, &A_XScream, State_SSWV_XDIE3 },	// State_SSWV_XDIE2
  DOOM::AbstractThing::State{ Sprite_SSWV, 15, false, 5, &A_Fall, State_SSWV_XDIE4 },		// State_SSWV_XDIE3
  DOOM::AbstractThing::State{ Sprite_SSWV, 16, false, 5, nullptr, State_SSWV_XDIE5 },		// State_SSWV_XDIE4
  DOOM::AbstractThing::State{ Sprite_SSWV, 17, false, 5, nullptr, State_SSWV_XDIE6 },		// State_SSWV_XDIE5
  DOOM::AbstractThing::State{ Sprite_SSWV, 18, false, 5, nullptr, State_SSWV_XDIE7 },		// State_SSWV_XDIE6
  DOOM::AbstractThing::State{ Sprite_SSWV, 19, false, 5, nullptr, State_SSWV_XDIE8 },		// State_SSWV_XDIE7
  DOOM::AbstractThing::State{ Sprite_SSWV, 20, false, 5, nullptr, State_SSWV_XDIE9 },		// State_SSWV_XDIE8
  DOOM::AbstractThing::State{ Sprite_SSWV, 21, false, -1, nullptr, State_None },		// State_SSWV_XDIE9
  DOOM::AbstractThing::State{ Sprite_SSWV, 12, false, 5, nullptr, State_SSWV_RAISE2 },		// State_SSWV_RAISE1
  DOOM::AbstractThing::State{ Sprite_SSWV, 11, false, 5, nullptr, State_SSWV_RAISE3 },		// State_SSWV_RAISE2
  DOOM::AbstractThing::State{ Sprite_SSWV, 10, false, 5, nullptr, State_SSWV_RAISE4 },		// State_SSWV_RAISE3
  DOOM::AbstractThing::State{ Sprite_SSWV, 9, false, 5, nullptr, State_SSWV_RAISE5 },		// State_SSWV_RAISE4
  DOOM::AbstractThing::State{ Sprite_SSWV, 8, false, 5, nullptr, State_SSWV_RUN1 },		// State_SSWV_RAISE5
  DOOM::AbstractThing::State{ Sprite_KEEN, 0, false, -1, nullptr, State_KEENSTND },		// State_KEENSTND
  DOOM::AbstractThing::State{ Sprite_KEEN, 0, false, 6, nullptr, State_COMMKEEN2 },		// State_COMMKEEN
  DOOM::AbstractThing::State{ Sprite_KEEN, 1, false, 6, nullptr, State_COMMKEEN3 },		// State_COMMKEEN2
  DOOM::AbstractThing::State{ Sprite_KEEN, 2, false, 6, &A_Scream, State_COMMKEEN4 },		// State_COMMKEEN3
  DOOM::AbstractThing::State{ Sprite_KEEN, 3, false, 6, nullptr, State_COMMKEEN5 },		// State_COMMKEEN4
  DOOM::AbstractThing::State{ Sprite_KEEN, 4, false, 6, nullptr, State_COMMKEEN6 },		// State_COMMKEEN5
  DOOM::AbstractThing::State{ Sprite_KEEN, 5, false, 6, nullptr, State_COMMKEEN7 },		// State_COMMKEEN6
  DOOM::AbstractThing::State{ Sprite_KEEN, 6, false, 6, nullptr, State_COMMKEEN8 },		// State_COMMKEEN7
  DOOM::AbstractThing::State{ Sprite_KEEN, 7, false, 6, nullptr, State_COMMKEEN9 },		// State_COMMKEEN8
  DOOM::AbstractThing::State{ Sprite_KEEN, 8, false, 6, nullptr, State_COMMKEEN10 },		// State_COMMKEEN9
  DOOM::AbstractThing::State{ Sprite_KEEN, 9, false, 6, nullptr, State_COMMKEEN11 },		// State_COMMKEEN10
  DOOM::AbstractThing::State{ Sprite_KEEN, 10, false, 6, &A_KeenDie, State_COMMKEEN12 },	// State_COMMKEEN11
  DOOM::AbstractThing::State{ Sprite_KEEN, 11, false, -1, nullptr, State_None },		// State_COMMKEEN12
  DOOM::AbstractThing::State{ Sprite_KEEN, 12, false, 4, nullptr, State_KEENPAIN2 },		// State_KEENPAIN
  DOOM::AbstractThing::State{ Sprite_KEEN, 12, false, 8, &A_Pain, State_KEENSTND },		// State_KEENPAIN2
  DOOM::AbstractThing::State{ Sprite_BBRN, 0, false, -1, nullptr, State_None },			// State_BRAIN
  DOOM::AbstractThing::State{ Sprite_BBRN, 1, false, 36, &A_BrainPain, State_BRAIN },		// State_BRAIN_PAIN
  DOOM::AbstractThing::State{ Sprite_BBRN, 0, false, 100, &A_BrainScream, State_BRAIN_DIE2 },	// State_BRAIN_DIE1
  DOOM::AbstractThing::State{ Sprite_BBRN, 0, false, 10, nullptr, State_BRAIN_DIE3 },		// State_BRAIN_DIE2
  DOOM::AbstractThing::State{ Sprite_BBRN, 0, false, 10, nullptr, State_BRAIN_DIE4 },		// State_BRAIN_DIE3
  DOOM::AbstractThing::State{ Sprite_BBRN, 0, false, -1, &A_BrainDie, State_None },		// State_BRAIN_DIE4
  DOOM::AbstractThing::State{ Sprite_SSWV, 0, false, 10, &A_Look, State_BRAINEYE },		// State_BRAINEYE
  DOOM::AbstractThing::State{ Sprite_SSWV, 0, false, 181, &A_BrainAwake, State_BRAINEYE1 },	// State_BRAINEYESEE
  DOOM::AbstractThing::State{ Sprite_SSWV, 0, false, 150, &A_BrainSpit, State_BRAINEYE1 },	// State_BRAINEYE1
  DOOM::AbstractThing::State{ Sprite_BOSF, 0, true, 3, &A_SpawnSound, State_SPAWN2 },		// State_SPAWN1
  DOOM::AbstractThing::State{ Sprite_BOSF, 1, true, 3, &A_SpawnFly, State_SPAWN3 },		// State_SPAWN2
  DOOM::AbstractThing::State{ Sprite_BOSF, 2, true, 3, &A_SpawnFly, State_SPAWN4 },		// State_SPAWN3
  DOOM::AbstractThing::State{ Sprite_BOSF, 3, true, 3, &A_SpawnFly, State_SPAWN1 },		// State_SPAWN4
  DOOM::AbstractThing::State{ Sprite_FIRE, 0, true, 4, &A_Fire, State_SPAWNFIRE2 },		// State_SPAWNFIRE1
  DOOM::AbstractThing::State{ Sprite_FIRE, 1, true, 4, &A_Fire, State_SPAWNFIRE3 },		// State_SPAWNFIRE2
  DOOM::AbstractThing::State{ Sprite_FIRE, 2, true, 4, &A_Fire, State_SPAWNFIRE4 },		// State_SPAWNFIRE3
  DOOM::AbstractThing::State{ Sprite_FIRE, 3, true, 4, &A_Fire, State_SPAWNFIRE5 },		// State_SPAWNFIRE4
  DOOM::AbstractThing::State{ Sprite_FIRE, 4, true, 4, &A_Fire, State_SPAWNFIRE6 },		// State_SPAWNFIRE5
  DOOM::AbstractThing::State{ Sprite_FIRE, 5, true, 4, &A_Fire, State_SPAWNFIRE7 },		// State_SPAWNFIRE6
  DOOM::AbstractThing::State{ Sprite_FIRE, 6, true, 4, &A_Fire, State_SPAWNFIRE8 },		// State_SPAWNFIRE7
  DOOM::AbstractThing::State{ Sprite_FIRE, 7, true, 4, &A_Fire, State_None },			// State_SPAWNFIRE8
  DOOM::AbstractThing::State{ Sprite_MISL, 1, true, 10, nullptr, State_BRAINEXPLODE2 },		// State_BRAINEXPLODE1
  DOOM::AbstractThing::State{ Sprite_MISL, 2, true, 10, nullptr, State_BRAINEXPLODE3 },		// State_BRAINEXPLODE2
  DOOM::AbstractThing::State{ Sprite_MISL, 3, true, 10, &A_BrainExplode, State_None },		// State_BRAINEXPLODE3
  DOOM::AbstractThing::State{ Sprite_ARM1, 0, false, 6, nullptr, State_ARM1A },			// State_ARM1
  DOOM::AbstractThing::State{ Sprite_ARM1, 1, true, 7, nullptr, State_ARM1 },			// State_ARM1A
  DOOM::AbstractThing::State{ Sprite_ARM2, 0, false, 6, nullptr, State_ARM2A },			// State_ARM2
  DOOM::AbstractThing::State{ Sprite_ARM2, 1, true, 6, nullptr, State_ARM2 },			// State_ARM2A
  DOOM::AbstractThing::State{ Sprite_BAR1, 0, false, 6, nullptr, State_BAR2 },			// State_BAR1
  DOOM::AbstractThing::State{ Sprite_BAR1, 1, false, 6, nullptr, State_BAR1 },			// State_BAR2
  DOOM::AbstractThing::State{ Sprite_BEXP, 0, true, 5, nullptr, State_BEXP2 },			// State_BEXP
  DOOM::AbstractThing::State{ Sprite_BEXP, 1, true, 5, &A_Scream, State_BEXP3 },		// State_BEXP2
  DOOM::AbstractThing::State{ Sprite_BEXP, 2, true, 5, nullptr, State_BEXP4 },			// State_BEXP3
  DOOM::AbstractThing::State{ Sprite_BEXP, 3, true, 10, &A_Explode, State_BEXP5 },		// State_BEXP4
  DOOM::AbstractThing::State{ Sprite_BEXP, 4, true, 10, nullptr, State_None },			// State_BEXP5
  DOOM::AbstractThing::State{ Sprite_FCAN, 0, true, 4, nullptr, State_BBAR2 },			// State_BBAR1
  DOOM::AbstractThing::State{ Sprite_FCAN, 1, true, 4, nullptr, State_BBAR3 },			// State_BBAR2
  DOOM::AbstractThing::State{ Sprite_FCAN, 2, true, 4, nullptr, State_BBAR1 },			// State_BBAR3
  DOOM::AbstractThing::State{ Sprite_BON1, 0, false, 6, nullptr, State_BON1A },			// State_BON1
  DOOM::AbstractThing::State{ Sprite_BON1, 1, false, 6, nullptr, State_BON1B },			// State_BON1A
  DOOM::AbstractThing::State{ Sprite_BON1, 2, false, 6, nullptr, State_BON1C },			// State_BON1B
  DOOM::AbstractThing::State{ Sprite_BON1, 3, false, 6, nullptr, State_BON1D },			// State_BON1C
  DOOM::AbstractThing::State{ Sprite_BON1, 2, false, 6, nullptr, State_BON1E },			// State_BON1D
  DOOM::AbstractThing::State{ Sprite_BON1, 1, false, 6, nullptr, State_BON1 },			// State_BON1E
  DOOM::AbstractThing::State{ Sprite_BON2, 0, false, 6, nullptr, State_BON2A },			// State_BON2
  DOOM::AbstractThing::State{ Sprite_BON2, 1, false, 6, nullptr, State_BON2B },			// State_BON2A
  DOOM::AbstractThing::State{ Sprite_BON2, 2, false, 6, nullptr, State_BON2C },			// State_BON2B
  DOOM::AbstractThing::State{ Sprite_BON2, 3, false, 6, nullptr, State_BON2D },			// State_BON2C
  DOOM::AbstractThing::State{ Sprite_BON2, 2, false, 6, nullptr, State_BON2E },			// State_BON2D
  DOOM::AbstractThing::State{ Sprite_BON2, 1, false, 6, nullptr, State_BON2 },			// State_BON2E
  DOOM::AbstractThing::State{ Sprite_BKEY, 0, false, 10, nullptr, State_BKEY2 },		// State_BKEY
  DOOM::AbstractThing::State{ Sprite_BKEY, 1, true, 10, nullptr, State_BKEY },			// State_BKEY2
  DOOM::AbstractThing::State{ Sprite_RKEY, 0, false, 10, nullptr, State_RKEY2 },		// State_RKEY
  DOOM::AbstractThing::State{ Sprite_RKEY, 1, true, 10, nullptr, State_RKEY },			// State_RKEY2
  DOOM::AbstractThing::State{ Sprite_YKEY, 0, false, 10, nullptr, State_YKEY2 },		// State_YKEY
  DOOM::AbstractThing::State{ Sprite_YKEY, 1, true, 10, nullptr, State_YKEY },			// State_YKEY2
  DOOM::AbstractThing::State{ Sprite_BSKU, 0, false, 10, nullptr, State_BSKULL2 },		// State_BSKULL
  DOOM::AbstractThing::State{ Sprite_BSKU, 1, true, 10, nullptr, State_BSKULL },		// State_BSKULL2
  DOOM::AbstractThing::State{ Sprite_RSKU, 0, false, 10, nullptr, State_RSKULL2 },		// State_RSKULL
  DOOM::AbstractThing::State{ Sprite_RSKU, 1, true, 10, nullptr, State_RSKULL },		// State_RSKULL2
  DOOM::AbstractThing::State{ Sprite_YSKU, 0, false, 10, nullptr, State_YSKULL2 },		// State_YSKULL
  DOOM::AbstractThing::State{ Sprite_YSKU, 1, true, 10, nullptr, State_YSKULL },		// State_YSKULL2
  DOOM::AbstractThing::State{ Sprite_STIM, 0, false, -1, nullptr, State_None },			// State_STIM
  DOOM::AbstractThing::State{ Sprite_MEDI, 0, false, -1, nullptr, State_None },			// State_MEDI
  DOOM::AbstractThing::State{ Sprite_SOUL, 0, true, 6, nullptr, State_SOUL2 },			// State_SOUL
  DOOM::AbstractThing::State{ Sprite_SOUL, 1, true, 6, nullptr, State_SOUL3 },			// State_SOUL2
  DOOM::AbstractThing::State{ Sprite_SOUL, 2, true, 6, nullptr, State_SOUL4 },			// State_SOUL3
  DOOM::AbstractThing::State{ Sprite_SOUL, 3, true, 6, nullptr, State_SOUL5 },			// State_SOUL4
  DOOM::AbstractThing::State{ Sprite_SOUL, 2, true, 6, nullptr, State_SOUL6 },			// State_SOUL5
  DOOM::AbstractThing::State{ Sprite_SOUL, 1, true, 6, nullptr, State_SOUL },			// State_SOUL6
  DOOM::AbstractThing::State{ Sprite_PINV, 0, true, 6, nullptr, State_PINV2 },			// State_PINV
  DOOM::AbstractThing::State{ Sprite_PINV, 1, true, 6, nullptr, State_PINV3 },			// State_PINV2
  DOOM::AbstractThing::State{ Sprite_PINV, 2, true, 6, nullptr, State_PINV4 },			// State_PINV3
  DOOM::AbstractThing::State{ Sprite_PINV, 3, true, 6, nullptr, State_PINV },			// State_PINV4
  DOOM::AbstractThing::State{ Sprite_PSTR, 0, true, -1, nullptr, State_None },			// State_PSTR
  DOOM::AbstractThing::State{ Sprite_PINS, 0, true, 6, nullptr, State_PINS2 },			// State_PINS
  DOOM::AbstractThing::State{ Sprite_PINS, 1, true, 6, nullptr, State_PINS3 },			// State_PINS2
  DOOM::AbstractThing::State{ Sprite_PINS, 2, true, 6, nullptr, State_PINS4 },			// State_PINS3
  DOOM::AbstractThing::State{ Sprite_PINS, 3, true, 6, nullptr, State_PINS },			// State_PINS4
  DOOM::AbstractThing::State{ Sprite_MEGA, 0, true, 6, nullptr, State_MEGA2 },			// State_MEGA
  DOOM::AbstractThing::State{ Sprite_MEGA, 1, true, 6, nullptr, State_MEGA3 },			// State_MEGA2
  DOOM::AbstractThing::State{ Sprite_MEGA, 2, true, 6, nullptr, State_MEGA4 },			// State_MEGA3
  DOOM::AbstractThing::State{ Sprite_MEGA, 3, true, 6, nullptr, State_MEGA },			// State_MEGA4
  DOOM::AbstractThing::State{ Sprite_SUIT, 0, true, -1, nullptr, State_None },			// State_SUIT
  DOOM::AbstractThing::State{ Sprite_PMAP, 0, true, 6, nullptr, State_PMAP2 },			// State_PMAP
  DOOM::AbstractThing::State{ Sprite_PMAP, 1, true, 6, nullptr, State_PMAP3 },			// State_PMAP2
  DOOM::AbstractThing::State{ Sprite_PMAP, 2, true, 6, nullptr, State_PMAP4 },			// State_PMAP3
  DOOM::AbstractThing::State{ Sprite_PMAP, 3, true, 6, nullptr, State_PMAP5 },			// State_PMAP4
  DOOM::AbstractThing::State{ Sprite_PMAP, 2, true, 6, nullptr, State_PMAP6 },			// State_PMAP5
  DOOM::AbstractThing::State{ Sprite_PMAP, 1, true, 6, nullptr, State_PMAP },			// State_PMAP6
  DOOM::AbstractThing::State{ Sprite_PVIS, 0, true, 6, nullptr, State_PVIS2 },			// State_PVIS
  DOOM::AbstractThing::State{ Sprite_PVIS, 1, false, 6, nullptr, State_PVIS },			// State_PVIS2
  DOOM::AbstractThing::State{ Sprite_CLIP, 0, false, -1, nullptr, State_None },			// State_CLIP
  DOOM::AbstractThing::State{ Sprite_AMMO, 0, false, -1, nullptr, State_None },			// State_AMMO
  DOOM::AbstractThing::State{ Sprite_ROCK, 0, false, -1, nullptr, State_None },			// State_ROCK
  DOOM::AbstractThing::State{ Sprite_BROK, 0, false, -1, nullptr, State_None },			// State_BROK
  DOOM::AbstractThing::State{ Sprite_CELL, 0, false, -1, nullptr, State_None },			// State_CELL
  DOOM::AbstractThing::State{ Sprite_CELP, 0, false, -1, nullptr, State_None },			// State_CELP
  DOOM::AbstractThing::State{ Sprite_SHEL, 0, false, -1, nullptr, State_None },			// State_SHEL
  DOOM::AbstractThing::State{ Sprite_SBOX, 0, false, -1, nullptr, State_None },			// State_SBOX
  DOOM::AbstractThing::State{ Sprite_BPAK, 0, false, -1, nullptr, State_None },			// State_BPAK
  DOOM::AbstractThing::State{ Sprite_BFUG, 0, false, -1, nullptr, State_None },			// State_BFUG
  DOOM::AbstractThing::State{ Sprite_MGUN, 0, false, -1, nullptr, State_None },			// State_MGUN
  DOOM::AbstractThing::State{ Sprite_CSAW, 0, false, -1, nullptr, State_None },			// State_CSAW
  DOOM::AbstractThing::State{ Sprite_LAUN, 0, false, -1, nullptr, State_None },			// State_LAUN
  DOOM::AbstractThing::State{ Sprite_PLAS, 0, false, -1, nullptr, State_None },			// State_PLAS
  DOOM::AbstractThing::State{ Sprite_SHOT, 0, false, -1, nullptr, State_None },			// State_SHOT
  DOOM::AbstractThing::State{ Sprite_SGN2, 0, false, -1, nullptr, State_None },			// State_SHOT2
  DOOM::AbstractThing::State{ Sprite_COLU, 0, true, -1, nullptr, State_None },			// State_COLU
  DOOM::AbstractThing::State{ Sprite_SMT2, 0, false, -1, nullptr, State_None },			// State_STALAG
  DOOM::AbstractThing::State{ Sprite_GOR1, 0, false, 10, nullptr, State_BLOODYTWITCH2 },	// State_BLOODYTWITCH
  DOOM::AbstractThing::State{ Sprite_GOR1, 1, false, 15, nullptr, State_BLOODYTWITCH3 },	// State_BLOODYTWITCH2
  DOOM::AbstractThing::State{ Sprite_GOR1, 2, false, 8, nullptr, State_BLOODYTWITCH4 },		// State_BLOODYTWITCH3
  DOOM::AbstractThing::State{ Sprite_GOR1, 1, false, 6, nullptr, State_BLOODYTWITCH },		// State_BLOODYTWITCH4
  DOOM::AbstractThing::State{ Sprite_PLAY, 13, false, -1, nullptr, State_None },		// State_DEADTORSO
  DOOM::AbstractThing::State{ Sprite_PLAY, 18, false, -1, nullptr, State_None },		// State_DEADBOTTOM
  DOOM::AbstractThing::State{ Sprite_POL2, 0, false, -1, nullptr, State_None },			// State_HEADSONSTICK
  DOOM::AbstractThing::State{ Sprite_POL5, 0, false, -1, nullptr, State_None },			// State_GIBS
  DOOM::AbstractThing::State{ Sprite_POL4, 0, false, -1, nullptr, State_None },			// State_HEADONASTICK
  DOOM::AbstractThing::State{ Sprite_POL3, 0, true, 6, nullptr, State_HEADCANDLES2 },		// State_HEADCANDLES
  DOOM::AbstractThing::State{ Sprite_POL3, 1, true, 6, nullptr, State_HEADCANDLES },		// State_HEADCANDLES2
  DOOM::AbstractThing::State{ Sprite_POL1, 0, false, -1, nullptr, State_None },			// State_DEADSTICK
  DOOM::AbstractThing::State{ Sprite_POL6, 0, false, 6, nullptr, State_LIVESTICK2 },		// State_LIVESTICK
  DOOM::AbstractThing::State{ Sprite_POL6, 1, false, 8, nullptr, State_LIVESTICK },		// State_LIVESTICK2
  DOOM::AbstractThing::State{ Sprite_GOR2, 0, false, -1, nullptr, State_None },			// State_MEAT2
  DOOM::AbstractThing::State{ Sprite_GOR3, 0, false, -1, nullptr, State_None },			// State_MEAT3
  DOOM::AbstractThing::State{ Sprite_GOR4, 0, false, -1, nullptr, State_None },			// State_MEAT4
  DOOM::AbstractThing::State{ Sprite_GOR5, 0, false, -1, nullptr, State_None },			// State_MEAT5
  DOOM::AbstractThing::State{ Sprite_SMIT, 0, false, -1, nullptr, State_None },			// State_STALAGTITE
  DOOM::AbstractThing::State{ Sprite_COL1, 0, false, -1, nullptr, State_None },			// State_TALLGRNCOL
  DOOM::AbstractThing::State{ Sprite_COL2, 0, false, -1, nullptr, State_None },			// State_SHRTGRNCOL
  DOOM::AbstractThing::State{ Sprite_COL3, 0, false, -1, nullptr, State_None },			// State_TALLREDCOL
  DOOM::AbstractThing::State{ Sprite_COL4, 0, false, -1, nullptr, State_None },			// State_SHRTREDCOL
  DOOM::AbstractThing::State{ Sprite_CAND, 0, true, -1, nullptr, State_None },			// State_CANDLESTIK
  DOOM::AbstractThing::State{ Sprite_CBRA, 0, true, -1, nullptr, State_None },			// State_CANDELABRA
  DOOM::AbstractThing::State{ Sprite_COL6, 0, false, -1, nullptr, State_None },			// State_SKULLCOL
  DOOM::AbstractThing::State{ Sprite_TRE1, 0, false, -1, nullptr, State_None },			// State_TORCHTREE
  DOOM::AbstractThing::State{ Sprite_TRE2, 0, false, -1, nullptr, State_None },			// State_BIGTREE
  DOOM::AbstractThing::State{ Sprite_ELEC, 0, false, -1, nullptr, State_None },			// State_TECHPILLAR
  DOOM::AbstractThing::State{ Sprite_CEYE, 0, true, 6, nullptr, State_EVILEYE2 },		// State_EVILEYE
  DOOM::AbstractThing::State{ Sprite_CEYE, 1, true, 6, nullptr, State_EVILEYE3 },		// State_EVILEYE2
  DOOM::AbstractThing::State{ Sprite_CEYE, 2, true, 6, nullptr, State_EVILEYE4 },		// State_EVILEYE3
  DOOM::AbstractThing::State{ Sprite_CEYE, 1, true, 6, nullptr, State_EVILEYE },		// State_EVILEYE4
  DOOM::AbstractThing::State{ Sprite_FSKU, 0, true, 6, nullptr, State_FLOATSKULL2 },		// State_FLOATSKULL
  DOOM::AbstractThing::State{ Sprite_FSKU, 1, true, 6, nullptr, State_FLOATSKULL3 },		// State_FLOATSKULL2
  DOOM::AbstractThing::State{ Sprite_FSKU, 2, true, 6, nullptr, State_FLOATSKULL },		// State_FLOATSKULL3
  DOOM::AbstractThing::State{ Sprite_COL5, 0, false, 14, nullptr, State_HEARTCOL2 },		// State_HEARTCOL
  DOOM::AbstractThing::State{ Sprite_COL5, 1, false, 14, nullptr, State_HEARTCOL },		// State_HEARTCOL2
  DOOM::AbstractThing::State{ Sprite_TBLU, 0, true, 4, nullptr, State_BLUETORCH2 },		// State_BLUETORCH
  DOOM::AbstractThing::State{ Sprite_TBLU, 1, true, 4, nullptr, State_BLUETORCH3 },		// State_BLUETORCH2
  DOOM::AbstractThing::State{ Sprite_TBLU, 2, true, 4, nullptr, State_BLUETORCH4 },		// State_BLUETORCH3
  DOOM::AbstractThing::State{ Sprite_TBLU, 3, true, 4, nullptr, State_BLUETORCH },		// State_BLUETORCH4
  DOOM::AbstractThing::State{ Sprite_TGRN, 0, true, 4, nullptr, State_GREENTORCH2 },		// State_GREENTORCH
  DOOM::AbstractThing::State{ Sprite_TGRN, 1, true, 4, nullptr, State_GREENTORCH3 },		// State_GREENTORCH2
  DOOM::AbstractThing::State{ Sprite_TGRN, 2, true, 4, nullptr, State_GREENTORCH4 },		// State_GREENTORCH3
  DOOM::AbstractThing::State{ Sprite_TGRN, 3, true, 4, nullptr, State_GREENTORCH },		// State_GREENTORCH4
  DOOM::AbstractThing::State{ Sprite_TRED, 0, true, 4, nullptr, State_REDTORCH2 },		// State_REDTORCH
  DOOM::AbstractThing::State{ Sprite_TRED, 1, true, 4, nullptr, State_REDTORCH3 },		// State_REDTORCH2
  DOOM::AbstractThing::State{ Sprite_TRED, 2, true, 4, nullptr, State_REDTORCH4 },		// State_REDTORCH3
  DOOM::AbstractThing::State{ Sprite_TRED, 3, true, 4, nullptr, State_REDTORCH },		// State_REDTORCH4
  DOOM::AbstractThing::State{ Sprite_SMBT, 0, true, 4, nullptr, State_BTORCHSHRT2 },		// State_BTORCHSHRT
  DOOM::AbstractThing::State{ Sprite_SMBT, 1, true, 4, nullptr, State_BTORCHSHRT3 },		// State_BTORCHSHRT2
  DOOM::AbstractThing::State{ Sprite_SMBT, 2, true, 4, nullptr, State_BTORCHSHRT4 },		// State_BTORCHSHRT3
  DOOM::AbstractThing::State{ Sprite_SMBT, 3, true, 4, nullptr, State_BTORCHSHRT },		// State_BTORCHSHRT4
  DOOM::AbstractThing::State{ Sprite_SMGT, 0, true, 4, nullptr, State_GTORCHSHRT2 },		// State_GTORCHSHRT
  DOOM::AbstractThing::State{ Sprite_SMGT, 1, true, 4, nullptr, State_GTORCHSHRT3 },		// State_GTORCHSHRT2
  DOOM::AbstractThing::State{ Sprite_SMGT, 2, true, 4, nullptr, State_GTORCHSHRT4 },		// State_GTORCHSHRT3
  DOOM::AbstractThing::State{ Sprite_SMGT, 3, true, 4, nullptr, State_GTORCHSHRT },		// State_GTORCHSHRT4
  DOOM::AbstractThing::State{ Sprite_SMRT, 0, true, 4, nullptr, State_RTORCHSHRT2 },		// State_RTORCHSHRT
  DOOM::AbstractThing::State{ Sprite_SMRT, 1, true, 4, nullptr, State_RTORCHSHRT3 },		// State_RTORCHSHRT2
  DOOM::AbstractThing::State{ Sprite_SMRT, 2, true, 4, nullptr, State_RTORCHSHRT4 },		// State_RTORCHSHRT3
  DOOM::AbstractThing::State{ Sprite_SMRT, 3, true, 4, nullptr, State_RTORCHSHRT },		// State_RTORCHSHRT4
  DOOM::AbstractThing::State{ Sprite_HDB1, 0, false, -1, nullptr, State_None },			// State_HANGNOGUTS
  DOOM::AbstractThing::State{ Sprite_HDB2, 0, false, -1, nullptr, State_None },			// State_HANGBNOBRAIN
  DOOM::AbstractThing::State{ Sprite_HDB3, 0, false, -1, nullptr, State_None },			// State_HANGTLOOKDN
  DOOM::AbstractThing::State{ Sprite_HDB4, 0, false, -1, nullptr, State_None },			// State_HANGTSKULL
  DOOM::AbstractThing::State{ Sprite_HDB5, 0, false, -1, nullptr, State_None },			// State_HANGTLOOKUP
  DOOM::AbstractThing::State{ Sprite_HDB6, 0, false, -1, nullptr, State_None },			// State_HANGTNOBRAIN
  DOOM::AbstractThing::State{ Sprite_POB1, 0, false, -1, nullptr, State_None },			// State_COLONGIBS
  DOOM::AbstractThing::State{ Sprite_POB2, 0, false, -1, nullptr, State_None },			// State_SMALLPOOL
  DOOM::AbstractThing::State{ Sprite_BRS1, 0, false, -1, nullptr, State_None },			// State_BRAINSTEM
  DOOM::AbstractThing::State{ Sprite_TLMP, 0, true, 4, nullptr, State_TECHLAMP2 },		// State_TECHLAMP
  DOOM::AbstractThing::State{ Sprite_TLMP, 1, true, 4, nullptr, State_TECHLAMP3 },		// State_TECHLAMP2
  DOOM::AbstractThing::State{ Sprite_TLMP, 2, true, 4, nullptr, State_TECHLAMP4 },		// State_TECHLAMP3
  DOOM::AbstractThing::State{ Sprite_TLMP, 3, true, 4, nullptr, State_TECHLAMP },		// State_TECHLAMP4
  DOOM::AbstractThing::State{ Sprite_TLP2, 0, true, 4, nullptr, State_TECH2LAMP2 },		// State_TECH2LAMP
  DOOM::AbstractThing::State{ Sprite_TLP2, 1, true, 4, nullptr, State_TECH2LAMP3 },		// State_TECH2LAMP2
  DOOM::AbstractThing::State{ Sprite_TLP2, 2, true, 4, nullptr, State_TECH2LAMP4 },		// State_TECH2LAMP3
  DOOM::AbstractThing::State{ Sprite_TLP2, 3, true, 4, nullptr, State_TECH2LAMP }		// State_TECH2LAMP4
};

const std::array<DOOM::AbstractThing::Attributs, DOOM::Enum::ThingType::ThingType_Number>	DOOM::AbstractThing::_attributs =
{
  DOOM::AbstractThing::Attributs{	// Type_PLAYER
    -1,		// id
    100,	// spawnhealth
    0,		// reactiontime
    255,	// painchance
    0,		// speed
    16,		// radius
    56,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_PickUp | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_PLAY,		// state_spawn
    State_PLAY_RUN1,	// state_see
    State_PLAY_PAIN,	// state_pain
    State_None,		// state_melee
    State_PLAY_ATK1,	// state_missile
    State_PLAY_DIE1,	// state_death
    State_PLAY_XDIE1,	// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_plpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_pldeth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_POSSESSED
    3004,	// id
    20,		// spawnhealth
    8,		// reactiontime
    200,	// painchance
    8,		// speed
    20,		// radius
    56,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_POSS_STND,	// state_spawn
    State_POSS_RUN1,	// state_see
    State_POSS_PAIN,	// state_pain
    State_None,		// state_melee
    State_POSS_ATK1,	// state_missile
    State_POSS_DIE1,	// state_death
    State_POSS_XDIE1,	// state_xdeath
    State_POSS_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_posit1,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_podth1,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_posact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SHOTGUY
    9,		// id
    30,		// spawnhealth
    8,		// reactiontime
    170,	// painchance
    8,		// speed
    20,		// radius
    56,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_SPOS_STND,	// state_spawn
    State_SPOS_RUN1,	// state_see
    State_SPOS_PAIN,	// state_pain
    State_None,		// state_melee
    State_SPOS_ATK1,	// state_missile
    State_SPOS_DIE1,	// state_death
    State_SPOS_XDIE1,	// state_xdeath
    State_SPOS_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_posit2,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_podth2,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_posact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_VILE
    64,		// id
    700,	// spawnhealth
    8,		// reactiontime
    10,		// painchance
    15,		// speed
    20,		// radius
    56,		// height
    500,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_VILE_STND,	// state_spawn
    State_VILE_RUN1,	// state_see
    State_VILE_PAIN,	// state_pain
    State_None,		// state_melee
    State_VILE_ATK1,	// state_missile
    State_VILE_DIE1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_vilsit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_vipain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_vildth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_vilact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_FIRE
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_FIRE1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise
      
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_UNDEAD
    66,		// id
    300,	// spawnhealth
    8,		// reactiontime
    100,	// painchance
    10,		// speed
    20,		// radius
    66,		// height
    500,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_SKEL_STND,	// state_spawn
    State_SKEL_RUN1,	// state_see
    State_SKEL_PAIN,	// state_pain
    State_SKEL_FIST1,	// state_melee
    State_SKEL_MISS1,	// state_missile
    State_SKEL_DIE1,	// state_death
    State_None,		// state_xdeath
    State_SKEL_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_skesit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_skedth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_skeact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_TRACER
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    10,		// speed
    11,		// radius
    8,		// height
    100,	// mass
    10,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_TRACER,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_TRACEEXP1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_skeatk,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_barexp,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SMOKE
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    0,		// radius
    1,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_SMOKE1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_FATSO
    67,		// id
    600,	// spawnhealth
    8,		// reactiontime
    80,		// painchance
    8,		// speed
    48,		// radius
    64,		// height
    1000,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_FATT_STND,	// state_spawn
    State_FATT_RUN1,	// state_see
    State_FATT_PAIN,	// state_pain
    State_None,		// state_melee
    State_FATT_ATK1,	// state_missile
    State_FATT_DIE1,	// state_death
    State_None,		// state_xdeath
    State_FATT_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_mansit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_mnpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_mandth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_posact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_FATSHOT
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    20,		// speed
    6,		// radius
    8,		// height
    100,	// mass
    8,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_FATSHOT1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_FATSHOTX1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_firsht,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_CHAINGUY
    65,		// id
    70,		// spawnhealth
    8,		// reactiontime
    170,	// painchance
    8,		// speed
    20,		// radius
    56,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_CPOS_STND,	// state_spawn
    State_CPOS_RUN1,	// state_see
    State_CPOS_PAIN,	// state_pain
    State_None,		// state_melee
    State_CPOS_ATK1,	// state_missile
    State_CPOS_DIE1,	// state_death
    State_CPOS_XDIE1,	// state_xdeath
    State_CPOS_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_posit2,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_podth2,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_posact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_TROOP
    3001,	// id
    60,		// spawnhealth
    8,		// reactiontime
    200,	// painchance
    8,		// speed
    20,		// radius
    56,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_TROO_STND,	// state_spawn
    State_TROO_RUN1,	// state_see
    State_TROO_PAIN,	// state_pain
    State_TROO_ATK1,	// state_melee
    State_TROO_ATK1,	// state_missile
    State_TROO_DIE1,	// state_death
    State_TROO_XDIE1,	// state_xdeath
    State_TROO_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_bgsit1,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_bgdth1,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_bgact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SERGEANT
    3002,	// id
    150,	// spawnhealth
    8,		// reactiontime
    180,	// painchance
    10,		// speed
    30,		// radius
    56,		// height
    400,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_SARG_STND,	// state_spawn
    State_SARG_RUN1,	// state_see
    State_SARG_PAIN,	// state_pain
    State_SARG_ATK1,	// state_melee
    State_None,		// state_missile
    State_SARG_DIE1,	// state_death
    State_None,		// state_xdeath
    State_SARG_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtsit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtatk,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtdth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SHADOWS
    58,		// id
    150,	// spawnhealth
    8,		// reactiontime
    180,	// painchance
    10,		// speed
    30,		// radius
    56,		// height
    400,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Shadow | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_SARG_STND,	// state_spawn
    State_SARG_RUN1,	// state_see
    State_SARG_PAIN,	// state_pain
    State_SARG_ATK1,	// state_melee
    State_None,		// state_missile
    State_SARG_DIE1,	// state_death
    State_None,		// state_xdeath
    State_SARG_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtsit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtatk,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtdth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_HEAD
    3005,	// id
    400,	// spawnhealth
    8,		// reactiontime
    128,	// painchance
    8,		// speed
    31,		// radius
    56,		// height
    400,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_NoGravity | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_HEAD_STND,	// state_spawn
    State_HEAD_RUN1,	// state_see
    State_HEAD_PAIN,	// state_pain
    State_None,		// state_melee
    State_HEAD_ATK1,	// state_missile
    State_HEAD_DIE1,	// state_death
    State_None,		// state_xdeath
    State_HEAD_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_cacsit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_cacdth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_BRUISER
    3003,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    50,		// painchance
    8,		// speed
    24,		// radius
    64,		// height
    1000,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_BOSS_STND,	// state_spawn
    State_BOSS_RUN1,	// state_see
    State_BOSS_PAIN,	// state_pain
    State_BOSS_ATK1,	// state_melee
    State_BOSS_ATK1,	// state_missile
    State_BOSS_DIE1,	// state_death
    State_None,		// state_xdeath
    State_BOSS_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_brssit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_brsdth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_BRUISERSHOT
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    15,		// speed
    6,		// radius
    8,		// height
    100,	// mass
    8,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_BRBALL1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_BRBALLX1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_firsht,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_KNIGHT
    69,		// id
    500,	// spawnhealth
    8,		// reactiontime
    50,		// painchance
    8,		// speed
    24,		// radius
    64,		// height
    1000,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_BOS2_STND,	// state_spawn
    State_BOS2_RUN1,	// state_see
    State_BOS2_PAIN,	// state_pain
    State_BOS2_ATK1,	// state_melee
    State_BOS2_ATK1,	// state_missile
    State_BOS2_DIE1,	// state_death
    State_None,		// state_xdeath
    State_BOS2_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_kntsit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_kntdth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SKULL
    3006,	// id
    100,	// spawnhealth
    8,		// reactiontime
    256,	// painchance
    8,		// speed
    16,		// radius
    56,		// height
    50,		// mass
    3,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_SKULL_STND,	// state_spawn
    State_SKULL_RUN1,	// state_see
    State_SKULL_PAIN,	// state_pain
    State_None,		// state_melee
    State_SKULL_ATK1,	// state_missile
    State_SKULL_DIE1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_sklatk,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SPIDER
    7,		// id
    3000,	// spawnhealth
    8,		// reactiontime
    40,		// painchance
    12,		// speed
    128,	// radius
    100,	// height
    1000,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_SPID_STND,	// state_spawn
    State_SPID_RUN1,	// state_see
    State_SPID_PAIN,	// state_pain
    State_None,		// state_melee
    State_SPID_ATK1,	// state_missile
    State_SPID_DIE1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_spisit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_shotgn,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_spidth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_BABY
    68,		// id
    500,	// spawnhealth
    8,		// reactiontime
    128,	// painchance
    12,		// speed
    64,		// radius
    64,		// height
    600,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_BSPI_STND,	// state_spawn
    State_BSPI_SIGHT,	// state_see
    State_BSPI_PAIN,	// state_pain
    State_None,		// state_melee
    State_BSPI_ATK1,	// state_missile
    State_BSPI_DIE1,	// state_death
    State_None,		// state_xdeath
    State_BSPI_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_bspsit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_bspdth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_bspact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_CYBORG
    16,		// id
    4000,	// spawnhealth
    8,		// reactiontime
    20,		// painchance
    16,		// speed
    40,		// radius
    110,	// height
    1000,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_CYBER_STND,	// state_spawn
    State_CYBER_RUN1,	// state_see
    State_CYBER_PAIN,	// state_pain
    State_None,		// state_melee
    State_CYBER_ATK1,	// state_missile
    State_CYBER_DIE1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_cybsit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_cybdth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_PAIN
    71,		// id
    400,	// spawnhealth
    8,		// reactiontime
    128,	// painchance
    8,		// speed
    31,		// radius
    56,		// height
    400,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_NoGravity | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_PAIN_STND,	// state_spawn
    State_PAIN_RUN1,	// state_see
    State_PAIN_PAIN,	// state_pain
    State_None,		// state_melee
    State_PAIN_ATK1,	// state_missile
    State_PAIN_DIE1,	// state_death
    State_None,		// state_xdeath
    State_PAIN_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_pesit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_pepain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_pedth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_WOLFSS
    84,		// id
    50,		// spawnhealth
    8,		// reactiontime
    170,	// painchance
    8,		// speed
    20,		// radius
    56,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_SSWV_STND,	// state_spawn
    State_SSWV_RUN1,	// state_see
    State_SSWV_PAIN,	// state_pain
    State_None,		// state_melee
    State_SSWV_ATK1,	// state_missile
    State_SSWV_DIE1,	// state_death
    State_SSWV_XDIE1,	// state_xdeath
    State_SSWV_RAISE1,	// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_sssit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_ssdth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_posact	// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_KEEN
    72,		// id
    100,	// spawnhealth
    8,		// reactiontime
    256,	// painchance
    0,		// speed
    16,		// radius
    72,		// height
    10000000,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),	// properties

    State_KEENSTND,	// state_spawn
    State_None,		// state_see
    State_KEENPAIN,	// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_COMMKEEN,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_keenpn,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_keendt,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_BOSSBRAIN
    88,		// id
    250,	// spawnhealth
    8,		// reactiontime
    255,	// painchance
    0,		// speed
    16,		// radius
    16,		// height
    10000000,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable),	// properties

    State_BRAIN,	// state_spawn
    State_None,		// state_see
    State_BRAIN_PAIN,	// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_BRAIN_DIE1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_bospn,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_bosdth,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_BOSSSPIT
    89,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    32,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector),	// properties

    State_BRAINEYE,	// state_spawn
    State_BRAINEYESEE,	// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_BOSSTARGET
    87,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    32,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector),	// properties

    State_None,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SPAWNSHOT
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    10,		// speed
    6,		// radius
    32,		// height
    100,	// mass
    3,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity | DOOM::Enum::ThingProperty::ThingProperty_NoClip),	// properties

    State_SPAWN1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_bospit,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SPAWNFIRE
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_SPAWNFIRE1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_BARREL
    2035,	// id
    20,		// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    10,		// radius
    42,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_NoBlood),	// properties

    State_BAR1,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_BEXP,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_barexp,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_TROOPSHOT
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    10,		// speed
    6,		// radius
    8,		// height
    100,	// mass
    3,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_TBALL1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_TBALLX1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_firsht,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_HEADSHOT
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    10,		// speed
    6,		// radius
    8,		// height
    100,	// mass
    5,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_RBALL1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_RBALLX1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_firsht,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_ROCKET
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    20,		// speed
    11,		// radius
    8,		// height
    100,	// mass
    20,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_ROCKET,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_EXPLODE1,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_rlaunc,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_barexp,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_PLASMA
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    25,		// speed
    13,		// radius
    8,		// height
    100,	// mass
    5,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_PLASBALL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_PLASEXP,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_plasma,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_BFG
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    25,		// speed
    13,		// radius
    8,		// height
    100,	// mass
    100,	// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_BFGSHOT,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_BFGLAND,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_rxplod,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_ARACHPLAZ
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    25,		// speed
    13,		// radius
    8,		// height
    100,	// mass
    5,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_ARACH_PLAZ,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_ARACH_PLEX,	// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_plasma,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_PUFF
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_PUFF1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_BLOOD
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap),	// properties

    State_BLOOD1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_TFOG
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_TFOG,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_IFOG
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_IFOG,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_TELEPORTMAN
    14,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector),	// properties

    State_None,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_EXTRABFG
    -1,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_BFGEXP,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC0
    2018,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_ARM1,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC1
    2019,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_ARM2,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC2
    2014,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),	// properties

    State_BON1,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },  
  DOOM::AbstractThing::Attributs{	// Type_MISC3
    2015,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),	// properties

    State_BON2,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC4
    5,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_BKEY,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC5
    13,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_RKEY,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC6
    6,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_YKEY,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC7
    39,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_YSKULL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC8
    38,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_RSKULL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC9
    40,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_BSKULL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC10
    2011,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_STIM,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC11
    2012,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_MEDI,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC12
    2013,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),	// properties

    State_STIM,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_INV
    2022,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),	// properties

    State_PINV,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC13
    2023,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),	// properties

    State_PSTR,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_INS
    2024,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),	// properties

    State_PINS,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC14
    2025,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_SUIT,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC15
    2026,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),	// properties

    State_PMAP,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC16
    2045,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),	// properties

    State_PVIS,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MEGA
    83,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),	// properties

    State_MEGA,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_CLIP
    2007,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_CLIP,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC17
    2048,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_AMMO,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC18
    2010,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_ROCK,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC19
    2046,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_BROK,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC20
    2047,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_CELL,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC21
    17,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_CELP,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC22
    2008,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_SHEL,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC23
    2049,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_SBOX,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC24
    8,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_BPAK,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC25
    2006,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_BFUG,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_CHAINGUN
    2002,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_MGUN,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC26
    2005,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_CSAW,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC27
    2003,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_LAUN,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC28
    2004,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_PLAS,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SHOTGUN
    2001,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_SHOT,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_SUPERSHOTGUN
    82,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),	// properties

    State_SHOT2,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC29
    85,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_TECHLAMP,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC30
    86,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_TECH2LAMP,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC31
    2028,	// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_COLU,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC32
    30,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_TALLGRNCOL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC33
    31,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_SHRTGRNCOL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC34
    32,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_TALLREDCOL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC35
    33,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_SHRTREDCOL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC36
    37,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_SKULLCOL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC37
    36,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_HEARTCOL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC38
    41,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_EVILEYE,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC39
    42,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_FLOATSKULL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC40
    43,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_TORCHTREE,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC41
    44,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_BLUETORCH,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC42
    45,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_GREENTORCH,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC43
    46,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_REDTORCH,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC44
    55,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_BTORCHSHRT,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC45
    56,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_GTORCHSHRT,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC46
    57,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_RTORCHSHRT,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC47
    47,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_STALAGTITE,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC48
    48,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_TECHPILLAR,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC49
    34,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_CANDLESTIK,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC50
    35,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_CANDELABRA,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC51
    49,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    68,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_BLOODYTWITCH,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC52
    50,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    84,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_MEAT2,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC53
    51,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    84,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_MEAT3,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC54
    52,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    68,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_MEAT4,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC55
    53,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    52,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_MEAT5,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC56
    59,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    84,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_MEAT2,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC57
    60,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    68,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_MEAT4,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC58
    61,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    52,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_MEAT3,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC59
    62,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    52,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_MEAT5,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC60
    63,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    68,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_BLOODYTWITCH,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC61
    22,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_HEAD_DIE6,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC62
    15,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_PLAY_DIE7,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC63
    18,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_POSS_DIE5,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC64
    21,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_SARG_DIE6,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC65
    23,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_SKULL_DIE6,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC66
    20,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_TROO_DIE5,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC67
    19,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_SPOS_DIE5,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC68
    10,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_PLAY_XDIE9,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC69
    12,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_PLAY_XDIE9,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC70
    28,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_HEADSONSTICK,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC71
    24,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),	// properties

    State_GIBS,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC72
    27,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_HEADONASTICK,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC73
    29,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_HEADCANDLES,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC74
    25,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_DEADSTICK,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC75
    26,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_LIVESTICK,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC76
    54,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    32,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_BIGTREE,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC77
    70,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),	// properties

    State_BBAR1,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC78
    73,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    88,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_HANGNOGUTS,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC79
    74,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    88,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_HANGBNOBRAIN,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC80
    75,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    64,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_HANGTLOOKDN,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC81
    76,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    64,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_HANGTSKULL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC82
    77,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    64,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_HANGTLOOKUP,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC83
    78,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    16,		// radius
    64,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),	// properties

    State_HANGTNOBRAIN,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC84
    79,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap),	// properties

    State_COLONGIBS,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC85
    80,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap),	// properties

    State_SMALLPOOL,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_MISC86
    81,		// id
    1000,	// spawnhealth
    8,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap),	// properties

    State_BRAINSTEM,	// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_PLAYER_SPAWN1
    1,		// id
    1000,	// spawnhealth
    0,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_None,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_PLAYER_SPAWN2
    2,		// id
    1000,	// spawnhealth
    0,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_None,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_PLAYER_SPAWN3
    3,		// id
    1000,	// spawnhealth
    0,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_None,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_PLAYER_SPAWN4
    4,		// id
    1000,	// spawnhealth
    0,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_None,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  },
  DOOM::AbstractThing::Attributs{	// Type_PLAYER_SPAWNDM
    11,		// id
    1000,	// spawnhealth
    0,		// reactiontime
    0,		// painchance
    0,		// speed
    20,		// radius
    16,		// height
    100,	// mass
    0,		// damage
    (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),	// properties

    State_None,		// state_spawn
    State_None,		// state_see
    State_None,		// state_pain
    State_None,		// state_melee
    State_None,		// state_missile
    State_None,		// state_death
    State_None,		// state_xdeath
    State_None,		// state_raise

    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_see
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_attack
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_pain
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None,	// sound_death
    DOOM::Doom::Resources::Sound::EnumSound::Sound_None		// sound_active
  }
};

const std::array<Math::Vector<2>, DOOM::AbstractThing::Direction::DirectionNumber>	DOOM::AbstractThing::_directions =
{
  Math::Vector<2>(std::cos(Math::Pi * 0.f), std::sin(Math::Pi * 0.f)),
  Math::Vector<2>(std::cos(Math::Pi * 0.25f), std::sin(Math::Pi * 0.25f)),
  Math::Vector<2>(std::cos(Math::Pi * 0.5f), std::sin(Math::Pi * 0.5f)),
  Math::Vector<2>(std::cos(Math::Pi * 0.75f), std::sin(Math::Pi * 0.75f)),
  Math::Vector<2>(std::cos(Math::Pi * 1.f), std::sin(Math::Pi * 1.f)),
  Math::Vector<2>(std::cos(Math::Pi * 1.25f), std::sin(Math::Pi * 1.25f)),
  Math::Vector<2>(std::cos(Math::Pi * 1.5f), std::sin(Math::Pi * 1.5f)),
  Math::Vector<2>(std::cos(Math::Pi * 1.75f), std::sin(Math::Pi * 1.75f))
};

DOOM::AbstractThing::AbstractThing(DOOM::Doom& doom, DOOM::Wad::RawLevel::Thing const& thing) :
  DOOM::AbstractThing(
    doom,
    (DOOM::Enum::ThingType)std::distance(_attributs.cbegin(), std::find_if(_attributs.cbegin(), _attributs.cend(), [thing](const Attributs& attributs) { return attributs.id == thing.type; })),
    (DOOM::Enum::ThingFlag)thing.flag,
    (float)thing.x,
    (float)thing.y,
    thing.angle / 360.f * 2.f * Math::Pi)
{}

DOOM::AbstractThing::AbstractThing(DOOM::Doom& doom, DOOM::Enum::ThingType type, DOOM::Enum::ThingFlag flags, float x, float y, float angle) :
  position(x, y, 0.f),
  angle(angle),
  type(type),
  attributs(_attributs[type]),
  flags((DOOM::Enum::ThingProperty)(attributs.properties | ((flags & DOOM::Enum::ThingFlag::FlagAmbush) ? DOOM::Enum::ThingProperty::ThingProperty_Ambush : DOOM::Enum::ThingProperty::ThingProperty_None))),
  health((float)attributs.spawnhealth),
  height(attributs.height),
  reactiontime(attributs.reactiontime),
  move_direction(Direction::DirectionNone),
  move_count(0),
  _remove(false),
  _thrust(0.f, 0.f, 0.f),
  _gravity((this->flags & DOOM::Enum::ThingProperty::ThingProperty_NoGravity) ? 0.f : -1.f),
  _state(attributs.state_spawn),
  _elapsed(sf::Time::Zero),
  _target(nullptr),
  _tracer(nullptr),
  _target_threshold(0)
{
  // Cancel if type is invalid 
  if (type < 0 || type >= DOOM::Enum::ThingType::ThingType_Number)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Add thing to blockmap
  if (!(this->flags & DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap))
    doom.level.blockmap.addThing(*this, position.convert<2>());

  std::set<int16_t>	sectors = doom.level.getSectors(position.convert<2>(), attributs.radius / 2.f);
  float			floor = std::numeric_limits<int16_t>().min();
  float			ceiling = std::numeric_limits<int16_t>().max();

  // Get spawn floor and ceiling height
  if (sectors.empty() == true)
    floor = 0.f;
  else
    for (int16_t sector : sectors) {
      if (doom.level.sectors[sector].floor_current > floor)
	floor = doom.level.sectors[sector].floor_current;
      if (doom.level.sectors[sector].ceiling_current < ceiling)
	ceiling = doom.level.sectors[sector].ceiling_current;
    }

  // Set thing spawn Z position
  position.z() = (this->flags & (DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling) ? ceiling - height : floor);
}

bool	DOOM::AbstractThing::update(DOOM::Doom& doom, sf::Time elapsed)
{
  // Update internal timer
  _elapsed += elapsed;

  while (_states[_state].duration != -1 && _elapsed >= DOOM::Doom::Tic * (sf::Int64)_states[_state].duration) {
    // Skip to next state
    _elapsed -= DOOM::Doom::Tic * (sf::Int64)_states[_state].duration;
    setState(doom, _states[_state].next);
  }

  // Update physics of thing
  updatePhysics(doom, elapsed);

  // Return remove flag
  return _remove;
}

std::unique_ptr<DOOM::AbstractThing>	DOOM::AbstractThing::factory(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing)
{
  // Create a basic thing from WAD thing
  // TODO: AbstractThing isn't really abstract anymore
  return std::make_unique<DOOM::AbstractThing>(doom, thing);
}

void	DOOM::AbstractThing::teleport(DOOM::Doom & doom, const Math::Vector<2> & destination, float orientation)
{
  // Cancel thrust
  _thrust = Math::Vector<3>();

  // Update thing position and angle
  if (!(attributs.properties & DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap))
    doom.level.blockmap.moveThing(*this, position.convert<2>(), destination);
  position.convert<2>() = destination;
  angle = orientation;

  // Update thing vertical position
  // TODO: handle floating objects ?
  position.z() = doom.level.sectors[doom.level.getSector(destination).first].floor_current;

  // TODO: telefrag monster on landing position
}

void	DOOM::AbstractThing::thrust(const Math::Vector<3> & acceleration)
{
  // Apply thrust vector to player based on acceleration and elapsed time (is 3.125 a magic number ?)
  _thrust += acceleration / (float)attributs.mass * 3.125f;
}

DOOM::AbstractThing::Sprite	DOOM::AbstractThing::sprite(const DOOM::Doom & doom, float angle) const
{
  // Return a default empty texture if no state
  if (_state == DOOM::AbstractThing::ThingState::State_None)
    return { DOOM::Doom::Resources::Texture::Null, false, false };

  std::unordered_map<uint64_t, std::vector<std::array<std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>, 8>>>::const_iterator	iterator = doom.resources.animations.find(DOOM::str_to_key(_sprites[_states[_state].sprite]));

  // Cancel if sequence or frame not found
  if (iterator == doom.resources.animations.cend() || iterator->second.size() < _states[_state].frame)
    return { DOOM::Doom::Resources::Texture::Null, false, false };

  const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>& texture = iterator->second[_states[_state].frame][Math::Modulo<8>((int)((std::fmodf(angle, Math::Pi * 2.f) + Math::Pi * 2.f) * 4.f / Math::Pi + 16.5f))];

  return { texture.first.get(), texture.second, _states[_state].brightness };
}

void	DOOM::AbstractThing::A_SpawnFly(DOOM::Doom& doom)
{
  // Wait end of reaction time
  reactiontime += -1;
  if (reactiontime != 0)
    return;

  // Spawn teleport fog
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, DOOM::Enum::ThingType::ThingType_SPAWNFIRE, DOOM::Enum::ThingFlag::FlagNone, _target->position.x(), _target->position.y(), 0.f));
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_telept, _target->position);

  static const std::array<std::pair<int, DOOM::Enum::ThingType>, 11>	monsters =
  {
    std::pair<int, DOOM::Enum::ThingType>{ 50, DOOM::Enum::ThingType::ThingType_TROOP },
    std::pair<int, DOOM::Enum::ThingType>{ 90, DOOM::Enum::ThingType::ThingType_SERGEANT },
    std::pair<int, DOOM::Enum::ThingType>{ 120, DOOM::Enum::ThingType::ThingType_SHADOWS },
    std::pair<int, DOOM::Enum::ThingType>{ 130, DOOM::Enum::ThingType::ThingType_PAIN },
    std::pair<int, DOOM::Enum::ThingType>{ 160, DOOM::Enum::ThingType::ThingType_HEAD },
    std::pair<int, DOOM::Enum::ThingType>{ 162, DOOM::Enum::ThingType::ThingType_VILE },
    std::pair<int, DOOM::Enum::ThingType>{ 172, DOOM::Enum::ThingType::ThingType_UNDEAD },
    std::pair<int, DOOM::Enum::ThingType>{ 192, DOOM::Enum::ThingType::ThingType_BABY },
    std::pair<int, DOOM::Enum::ThingType>{ 222, DOOM::Enum::ThingType::ThingType_FATSO },
    std::pair<int, DOOM::Enum::ThingType>{ 246, DOOM::Enum::ThingType::ThingType_KNIGHT },
    std::pair<int, DOOM::Enum::ThingType>{ 256, DOOM::Enum::ThingType::ThingType_BRUISER }
  };

  // Randomly select monster to spawn
  int			r = std::rand() % 256;
  DOOM::Enum::ThingType	type = std::find_if(monsters.cbegin(), monsters.cend(), [r](const std::pair<int, DOOM::Enum::ThingType>& monster) { return (r % 256) < monster.first; })->second;

  // Spawn new monster
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, type, DOOM::Enum::ThingFlag::FlagNone, _target->position.x(), _target->position.y(), 0.f));
  if (doom.level.things.back()->P_LookForPlayers(doom, true) == true)
    doom.level.things.back()->setState(doom, doom.level.things.back()->attributs.state_see);

  // Telefrag anything in this spot
  doom.level.things.back()->teleport(doom, doom.level.things.back()->position.convert<2>(), doom.level.things.back()->angle);

  // Remove seft (spawn cube)
  setState(doom, State_None);
}

void	DOOM::AbstractThing::A_BruisAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Melee attack
  if (P_CheckMeleeRange(doom) == true) {
    doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_claw, position);
    _target->damage(doom, *this, (float)((std::rand() % 8 + 1) * 10));
  }

  // Range attack
  else
    P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_BRUISERSHOT);
}

void	DOOM::AbstractThing::A_CyberAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Range attack
  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_ROCKET);
}

void	DOOM::AbstractThing::A_HeadAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Melee attack
  if (P_CheckMeleeRange(doom) == true)
    _target->damage(doom, *this, (float)((std::rand() % 6 + 1) * 10));

  // Range attack
  else
    P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_HEADSHOT);
}

void	DOOM::AbstractThing::A_TroopAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Melee attack
  if (P_CheckMeleeRange(doom) == true) {
    doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_claw, position);
    _target->damage(doom, *this, (float)((std::rand() % 8 + 1) * 3));
  }

  // Range attack
  else
    P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_TROOPSHOT);
}

void	DOOM::AbstractThing::A_BspiAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);
    
  // Launch a missile
  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_ARACHPLAZ);
}

void	DOOM::AbstractThing::setState(DOOM::Doom & doom, DOOM::AbstractThing::ThingState state)
{
  // Remove thing
  if (state == DOOM::AbstractThing::ThingState::State_None) {
    _remove = true;
    return;
  }

  // Set new state
  _state = state;

  // Call state callback
  if (_states[_state].action != nullptr)
    std::invoke(_states[_state].action, this, doom);
}

void	DOOM::AbstractThing::updatePhysics(DOOM::Doom& doom, sf::Time elapsed)
{
  // Compute physics if minimal thrust
  if (std::fabsf(_thrust.x()) > 0.001f || std::fabsf(_thrust.y()) > 0.001f) {
    // Compute movement with collision
    updatePhysicsThrust(doom, elapsed);

    // Apply friction slowdown to thing (except missiles and attacking flying skulls) for next tic (hard coded drag factor of 0.90625)
    if (!(flags & (DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_SkullFly)))
      _thrust.convert<2>() *= std::powf(0.90625f, elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds());
  }

  // Special case for lost souls
  else if (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) {
    flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_SkullFly);
    setState(doom, attributs.state_see);
  }

  // Update gravity
  updatePhysicsGravity(doom, elapsed);
}

void DOOM::AbstractThing::updatePhysicsThrust(DOOM::Doom& doom, sf::Time elapsed, int depth, int16_t linedef_ignored, const DOOM::AbstractThing* thing_ignored)
{
  // NOTE: glitch might happen if radius > 128
  // TODO: recode this using square bounding box

  // Limit movement to 30 units per tics
  Math::Vector<2>	movement = ((_thrust.convert<2>().length() > 30.f) ? (_thrust.convert<2>() * 30.f / _thrust.convert<2>().length()) : _thrust.convert<2>())* elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds();

  // Stop if maximum recursion depth reach
  if (depth > 4 || (movement.x() == 0.f && movement.y() == 0.f)) {
    _thrust.convert<2>() = Math::Vector<2>(0.f, 0.f);
    return;
  }

  // Get intersectable linedefs and things
  std::pair<std::set<int16_t>, std::set<std::reference_wrapper<DOOM::AbstractThing>>>	linedefs_things = updatePhysicsThrustLinedefsThings(doom, movement);

  int16_t               closest_linedef = -1;
  DOOM::AbstractThing*  closest_thing = nullptr;
  float                 closest_distance = 1.f;
  Math::Vector<2>       closest_normal = Math::Vector<2>();

  // Check collision with linedefs
  for (int16_t linedef_index : linedefs_things.first) {
    std::pair<float, Math::Vector<2>>	intersection = updatePhysicsThrustLinedef(doom, movement, linedef_index, linedef_ignored);

    // Get nearest linedef
    if (intersection.first < closest_distance) {
      closest_linedef = linedef_index;
      closest_thing = nullptr;
      closest_distance = intersection.first;
      closest_normal = intersection.second;
    }
  }

  //   Check collision with things
  if ((flags & DOOM::Enum::ThingProperty::ThingProperty_Solid) || (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile))
    for (const std::reference_wrapper<DOOM::AbstractThing>& thing : linedefs_things.second) {
      // Thing has already been removed
      if (thing.get()._remove == true)
        continue;

      std::pair<float, Math::Vector<2>>	intersection = updatePhysicsThrustThing(doom, movement, thing.get(), thing_ignored);

      // Ignore missile emitter
      if ((flags & DOOM::Enum::ThingProperty::ThingProperty_Missile) && &thing.get() == _target)
        continue;

      // Get nearest thing
      if (intersection.first < closest_distance) {
	closest_linedef = -1;
	closest_thing = &thing.get();
	closest_distance = intersection.first;
	closest_normal = intersection.second;
      }
    }

  // Walkover linedefs
  for (int16_t linedef_index : linedefs_things.first) {
    // Ignore linedef if collided or ignored
    if (linedef_index == closest_linedef || linedef_index == linedef_ignored)
      continue;

    DOOM::AbstractLinedef &	linedef = *doom.level.linedefs[linedef_index].get();

    // Compute intersection of movement with linedef
    std::pair<float, float>	intersection = Math::intersection(
      doom.level.vertexes[linedef.start],
      doom.level.vertexes[linedef.end] - doom.level.vertexes[linedef.start],
      position.convert<2>(),
      movement * closest_distance);

    // Walkover linedef if intersected
    if (intersection.first >= 0.f && intersection.first <= 1.f &&
      intersection.second >= 0.f && intersection.second <= 1.f)
      linedef.walkover(doom, *this);
  }

  // Pickup things
  if ((flags & DOOM::Enum::ThingProperty::ThingProperty_PickUp) != 0)
    for (const std::reference_wrapper<DOOM::AbstractThing>& thing : linedefs_things.second) {
      // Ignore thing if collided or ignored or already removed
      if (&thing.get() == closest_thing || &thing.get() == thing_ignored || (thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Special) == 0 || thing.get()._remove == true)
        continue;

      // Pickup thing if destination is in thing area
      if ((position.convert<2>() + movement * closest_distance - thing.get().position.convert<2>()).length() < thing.get().attributs.radius + attributs.radius &&
        position.z() + height > thing.get().position.z() && position.z() < thing.get().position.z() + thing.get().height) {
        thing.get()._remove = pickup(doom, thing);
      }
    }

  // Move player to closest obstacle or full movement if none found
  if (closest_distance > 0.f) {
    Math::Vector<2>	destination = position.convert<2>() + movement * closest_distance;
    if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap))
      doom.level.blockmap.moveThing(*this, position.convert<2>(), destination);
    position.convert<2>() = destination;
  }

  // Re-compute movement if obstacle found
  if (closest_linedef != -1 || closest_thing != nullptr) {
    Math::Vector<2>	closest_direction = Math::Vector<2>(+closest_normal.y(), -closest_normal.x());

    // Slide against currently collisioned walls/things (change movement and thrust)
    _thrust.convert<2>() = closest_direction / closest_direction.length() * _thrust.convert<2>().length() * Math::Vector<2>::cos(_thrust.convert<2>(), closest_direction);

    // Reset lost soul when colliding something
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) {
      flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_SkullFly);
      setState(doom, attributs.state_see);
    }

    // Explode if thing is a missile
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile) {
      if (closest_thing != nullptr)
        P_ExplodeMissile(doom, *closest_thing);
      else
        P_ExplodeMissile(doom);
    }

    // Attempt new move, ignoring collided linedef/thing
    else
      updatePhysicsThrust(doom, elapsed * (1.f - closest_distance), depth + 1, closest_linedef, closest_thing);
  }
}

bool	DOOM::AbstractThing::updatePhysicsThrustSidedefs(DOOM::Doom& doom, int16_t sidedef_front_index, int16_t sidedef_back_index)
{
  // Force movement if thing outside of the map
  if (sidedef_front_index == -1)
    return true;

  // Can't move outside of the map
  if (sidedef_back_index == -1)
    return false;

  DOOM::Doom::Level::Sidedef& sidedef_front = doom.level.sidedefs[sidedef_front_index];
  DOOM::Doom::Level::Sidedef& sidedef_back = doom.level.sidedefs[sidedef_back_index];

  // Can't move if texture in middle section of sidedef
  if (sidedef_front.middle().width != 0 && sidedef_back.middle().width != 0)
    return false;

  DOOM::Doom::Level::Sector& sector_front = doom.level.sectors[sidedef_front.sector];
  DOOM::Doom::Level::Sector& sector_back = doom.level.sectors[sidedef_back.sector];

  // Can't take step over 24 units high
  if (sector_back.floor_current - position.z() > 24)
    return false;

  // Check if there is enough space between sector to move
  if (sector_back.ceiling_current - std::max(position.z(), sector_back.floor_current) >= height)
    return true;

  return false;
}

std::pair<float, Math::Vector<2>>	DOOM::AbstractThing::updatePhysicsThrustVertex(DOOM::Doom& doom, const Math::Vector<2>& movement, int16_t vertex_index, int16_t ignored_index)
{
  // Check if vertex should be ignored
  if (ignored_index != -1) {
    DOOM::AbstractLinedef& ignored = *doom.level.linedefs[ignored_index];

    if (ignored.start == vertex_index || ignored.end == vertex_index)
      return { 1.f, Math::Vector<2>() };
  }

  DOOM::Doom::Level::Vertex	vertex = doom.level.vertexes[vertex_index];
  Math::Vector<2>		normal = position.convert<2>() - vertex;

  // Ignore intersection if moving "outside" of vertex
  if (Math::Vector<2>::cos(movement, normal) > 0.f)
    return { 1.f, Math::Vector<2>() };

  // Check if vertex is already in bounding box
  if (normal.length() <= attributs.radius)
    return { 0.f, normal / normal.length() };

  // Intersect bounding circle with vertex
  float	a = std::pow(movement.x(), 2) + std::pow(movement.y(), 2);
  float	b = -2.f * ((vertex.x() - position.x()) * movement.x() + (vertex.y() - position.y()) * movement.y());
  float	c = std::pow(vertex.x() - position.x(), 2) + std::pow(vertex.y() - position.y(), 2) - std::pow((float)attributs.radius, 2);
  float	delta = std::pow(b, 2) - 4.f * a * c;

  // No intersection found
  if (delta <= 0.f)
    return { 1.f, Math::Vector<2>() };

  // Compute intersections
  float	s0 = (-b - std::sqrt(delta)) / (2.f * a);
  float	s1 = (-b + std::sqrt(delta)) / (2.f * a);

  // Return smaller solution
  if (s0 >= 0.f && s0 < 1.f)
    return { s0, normal / normal.length() };
  else if (s1 >= 0.f && s1 < 1.f)
    return { s1, normal / normal.length() };
  else
    return { 1.f, Math::Vector<2>() };
}

std::pair<float, Math::Vector<2>>	DOOM::AbstractThing::updatePhysicsThrustLinedef(DOOM::Doom& doom, const Math::Vector<2>& movement, int16_t linedef_index, int16_t ignored_index)
{
  // Check if linedef is ignored
  if (linedef_index == ignored_index)
    return { 1.f, Math::Vector<2>() };

  const DOOM::AbstractLinedef&		linedef = *doom.level.linedefs[linedef_index];
  const DOOM::Doom::Level::Vertex&	linedef_start = doom.level.vertexes[linedef.start];
  const DOOM::Doom::Level::Vertex&	linedef_end = doom.level.vertexes[linedef.end];

  Math::Vector<2>	linedef_direction = linedef_end - linedef_start;
  Math::Vector<2>	linedef_normal(+linedef_direction.y(), -linedef_direction.x());
  int16_t		sidedef_front_index = linedef.front;
  int16_t		sidedef_back_index = linedef.back;

  // Normalize normal
  linedef_normal /= linedef_normal.length();

  // Flip normal if on left side of linedef
  if (Math::Vector<2>::cos(position.convert<2>() - linedef_start, linedef_normal) < 0.f) {
    linedef_normal *= -1.f;
    std::swap(sidedef_front_index, sidedef_back_index);
  }

  // Ignore intersection if moving "outside" of linedef
  if (Math::Vector<2>::cos(movement, linedef_normal) > 0.f)
    return { 1.f, Math::Vector<2>() };

  // Check if intersection should be ignored using front and back sidedef
  if (!(linedef.flag & DOOM::AbstractLinedef::Flag::Impassible) &&
    !((linedef.flag & DOOM::AbstractLinedef::Flag::BlockMonsters) && (flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable) && type != DOOM::Enum::ThingType::ThingType_PLAYER) &&
    updatePhysicsThrustSidedefs(doom, sidedef_front_index, sidedef_back_index) == true)
    return { 1.f, Math::Vector<2>() };

  // Get intersection of thing bounding sphere and linedef line
  std::pair<float, float>		intersection = Math::intersection(
    Math::Vector<2>(position.x(), position.y()) - linedef_normal * (float)attributs.radius, movement,
    linedef_start, linedef_direction);

  // Handle parallele movement
  if (std::isnan(intersection.first) == true) {
    intersection = Math::intersection(position.convert<2>(), linedef_normal * -1.f, linedef_start, linedef_direction);
    intersection.first = 1.f;
  }

  // If linedef is already collided in initial collision, recompute intersection without movement
  if (intersection.first < 0.f) {
    intersection = Math::intersection(position.convert<2>(), linedef_normal * -1.f, linedef_start, linedef_direction);
    intersection.first = 0.f;
  }

  // Collide with start vertex, end vertex or linedef itself depending on intersection position
  if (intersection.second < 0.f)
    return updatePhysicsThrustVertex(doom, movement, linedef.start, ignored_index);
  else if (intersection.second > 1.f)
    return updatePhysicsThrustVertex(doom, movement, linedef.end, ignored_index);
  else
    return { intersection.first, linedef_normal };
}

std::pair<float, Math::Vector<2>>	DOOM::AbstractThing::updatePhysicsThrustThing(DOOM::Doom& doom, const Math::Vector<2>& movement, const DOOM::AbstractThing& thing, const DOOM::AbstractThing* ignored)
{
  // Check if linedef is ignored
  if (&thing == this || &thing == ignored || (thing.flags & DOOM::Enum::ThingProperty::ThingProperty_Solid) == 0)
    return { 1.f, Math::Vector<2>() };

  Math::Vector<2>	initial(position.convert<2>() - thing.position.convert<2>());

  // Check if thing is above or below
  if (thing.position.z() + thing.height <= position.z() || thing.position.z() > position.z() + attributs.height)
    return { 1.f, Math::Vector<2>() };

  // Check for initial collision
  if (initial.length() < (float)(attributs.radius + thing.attributs.radius)) {
    if (Math::Vector<2>::cos(movement, initial) < 0.f)
      return { 0.f, initial / initial.length() };
    else
      return { 1.f, Math::Vector<2>() };
  }

  float	a = std::pow(movement.x(), 2) + std::pow(movement.y(), 2);
  float	b = 2.f * (movement.x() * (position.x() - thing.position.x()) + movement.y() * (position.y() - thing.position.y()));
  float	c = std::pow(position.x() - thing.position.x(), 2) + std::pow(position.y() - thing.position.y(), 2) + std::pow((float)attributs.radius + (float)thing.attributs.radius, 2);
  float	delta = std::pow(b, 2) - 4.f * a * c;

  // No intersection found
  if (delta <= 0.f)
    return { 1.f, Math::Vector<2>() };

  // Compute intersections
  float	s0 = (-b - std::sqrt(delta)) / (2.f * a);
  float	s1 = (-b + std::sqrt(delta)) / (2.f * a);

  // Return smaller solution
  if (s0 >= 0.f && s0 < 1.f) {
    Math::Vector<2>	normal = position.convert<2>() + movement * s0 - thing.position.convert<2>();

    return { s0, normal / normal.length() };
  }
  else if (s1 >= 0.f && s1 < 1.f) {
    Math::Vector<2>	normal = position.convert<2>() + movement * s1 - thing.position.convert<2>();

    return { s1, normal / normal.length() };
  }
  else
    return { 1.f, Math::Vector<2>() };
}

std::pair<std::set<int16_t>, std::set<std::reference_wrapper<DOOM::AbstractThing>>>	DOOM::AbstractThing::updatePhysicsThrustLinedefsThings(DOOM::Doom& doom, const Math::Vector<2>& movement)
{
  std::set<int16_t>	blocks;

  // Get blockmap index at current, using the four corners
  blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() - (float)attributs.radius, position.y() - (float)attributs.radius)));
  blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() - (float)attributs.radius, position.y() + (float)attributs.radius)));
  blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + (float)attributs.radius, position.y() - (float)attributs.radius)));
  blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + (float)attributs.radius, position.y() + (float)attributs.radius)));

  // Get blockmap index at target position, using the four corners
  blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + movement.x() - (float)attributs.radius, position.y() + movement.y() - (float)attributs.radius)));
  blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + movement.x() - (float)attributs.radius, position.y() + movement.y() + (float)attributs.radius)));
  blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + movement.x() + (float)attributs.radius, position.y() + movement.y() - (float)attributs.radius)));
  blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + movement.x() + (float)attributs.radius, position.y() + movement.y() + (float)attributs.radius)));

  std::pair<std::set<int16_t>, std::set<std::reference_wrapper<DOOM::AbstractThing>>>	linedefs_things;

  // Get index of linedefs to test against position
  for (int16_t block_index : blocks)
    if (block_index != -1) {
      const DOOM::Doom::Level::Blockmap::Block& block = doom.level.blockmap.blocks[block_index];

      linedefs_things.first.insert(block.linedefs.begin(), block.linedefs.end());
      linedefs_things.second.insert(block.things.begin(), block.things.end());
    }

  // Return set of intersectable linedef
  return linedefs_things;
}

void	DOOM::AbstractThing::updatePhysicsGravity(DOOM::Doom& doom, sf::Time elapsed)
{
  std::set<int16_t>	sectors = doom.level.getSectors(position.convert<2>(), attributs.radius / 2.f);
  float			floor = std::numeric_limits<int16_t>().min();
  float			ceiling = std::numeric_limits<int16_t>().max();

  // Get target floor and ceiling height
  if (sectors.empty() == true) {
    floor = 0.f;
    ceiling = 0.f;
  }
  else
    for (int16_t sector : sectors) {
      floor = std::max(floor, doom.level.sectors[sector].floor_current);
      ceiling = std::min(ceiling, doom.level.sectors[sector].ceiling_current);
    }

  // Walk on things
  for (const DOOM::AbstractThing& thing : doom.level.getThings(position.convert<2>(), attributs.radius - 1.f)) {
    if (&thing != this && thing.flags & DOOM::Enum::ThingProperty::ThingProperty_Solid) {
      if (thing.position.z() + thing.height <= position.z())
	floor = std::max(floor, thing.position.z() + thing.height);
      if (thing.position.z() >= position.z() + height)
	ceiling = std::min(ceiling, position.z() + height);
    }
  }

  // Compute gravity
  _thrust.z() += _gravity / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds();

  // Apply friction to vertical thrust for non attacking fly skulls
  if (type == DOOM::Enum::ThingType::ThingType_SKULL && !(flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly))
    _thrust.z() *= std::powf(0.90625f, elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds());

  // TODO: delete missile if colliding with sky

  // Raise thing if below the floor
  if (position.z() < floor) {
    position.z() = std::min(floor, position.z() + std::max(_thrust.z(), (floor - position.z()) / 2.f + 3.2f) / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds());
    _thrust.z() = std::max(_thrust.z(), (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) ? +std::abs(_thrust.z()) : 0.f);

    // Explode missile if colliding with floor
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
      P_ExplodeMissile(doom);
  }
  // Lower thing is upper than the ceiling (limit to floor)
  else if (position.z() > ceiling - height) {
    position.z() = std::max(std::max(ceiling - height, floor), position.z() + std::min(_thrust.z(), ((ceiling - attributs.height) - position.z()) / 2.f + 2.f) / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds());
    _thrust.z() = std::min(_thrust.z(), (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) ? -std::abs(_thrust.z()) : 0.f);

    // Explode missile if colliding with ceiling
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
      P_ExplodeMissile(doom);
  }
  // Normal gravity
  else if (_thrust.z() < 0.f) {
    position.z() = std::max(floor, position.z() + _thrust.z() / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds());
    if (position.z() == floor) {
      _thrust.z() = (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) ? +std::abs(_thrust.z()) : 0.f;

      // Explode missile if colliding with floor
      if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
        P_ExplodeMissile(doom);
    }
  }
  // Reverse gravity
  else if (_thrust.z() > 0.f) {
    position.z() = std::min(std::max(ceiling - height, floor), position.z() + _thrust.z() / DOOM::Doom::Tic.asSeconds() * elapsed.asSeconds());
    if (position.z() == ceiling - height) {
      _thrust.z() = (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) ? -std::abs(_thrust.z()) : 0.f;

      // Explode missile if colliding with floor
      if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
        P_ExplodeMissile(doom);
    }
  }
}

bool  DOOM::AbstractThing::pickup(DOOM::Doom& doom, DOOM::AbstractThing& item)
{
  // Does nothing, AbstractThing don't pick-up items
  return false;
}

void  DOOM::AbstractThing::A_Look(DOOM::Doom& doom)
{
  // Reset target and threshold, any shot will wake up
  _target = nullptr;
  _target_threshold = 0;

  // Ambush mode, only look for players
  if (flags & DOOM::Enum::ThingProperty::ThingProperty_Ambush) {
    if (P_LookForPlayers(doom) == false)
      return;
  }

  // Target noise emitter
  else {
    DOOM::AbstractThing* sound_target = doom.level.sectors[doom.level.getSector(position.convert<2>()).first].sound_target;

    // Check valid target
    if (sound_target != nullptr && sound_target->flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable) {
      _target = sound_target;
      _target_threshold = DOOM::AbstractThing::TargetThreshold;
    }

    // Look for a player to target if no sound target
    else if (P_LookForPlayers(doom) == false)
      return;
  }

  // Select sound to play
  DOOM::Doom::Resources::Sound::EnumSound sound;

  switch (attributs.sound_see)
  {
  case DOOM::Doom::Resources::Sound::EnumSound::Sound_posit1:
  case DOOM::Doom::Resources::Sound::EnumSound::Sound_posit2:
  case DOOM::Doom::Resources::Sound::EnumSound::Sound_posit3:
    sound = (DOOM::Doom::Resources::Sound::EnumSound)(DOOM::Doom::Resources::Sound::EnumSound::Sound_posit1 + std::rand() % 3);
    break;

  case DOOM::Doom::Resources::Sound::EnumSound::Sound_bgsit1:
  case DOOM::Doom::Resources::Sound::EnumSound::Sound_bgsit2:
    sound = (DOOM::Doom::Resources::Sound::EnumSound)(DOOM::Doom::Resources::Sound::EnumSound::Sound_bgsit1 + std::rand() % 2);
    break;

  default:
    sound = attributs.sound_see;
    break;
  }

  // Full volume for bosses
  if (type == DOOM::Enum::ThingType::ThingType_SPIDER || type == DOOM::Enum::ThingType::ThingType_CYBORG)
    doom.sound(sound);
  else
    doom.sound(sound, position);

  // Set chase state
  setState(doom, attributs.state_see);
}

void	DOOM::AbstractThing::A_Chase(DOOM::Doom& doom)
{
    // Decrement reaction time
    if (reactiontime > 0)
      reactiontime += -1;

    // Update target threshold
    if (_target_threshold > 0) {
      if (_target == nullptr || _target->health <= 0)
	_target_threshold = 0;
      else
	_target_threshold += -1;
    }

    // Turn toward movement direction
    if (move_direction != Direction::DirectionNone) {
      angle = Math::Modulo(angle, 2.f * Math::Pi);

      int thing_direction = Math::Modulo<Direction::DirectionNumber>((int)(0.5f + angle / (2.f * Math::Pi / Direction::DirectionNumber)));

      if (thing_direction != move_direction) {
	if (Math::Modulo<Direction::DirectionNumber>(thing_direction - move_direction) < Direction::DirectionNumber / 2)
	  angle -= Math::Pi / 4.f;
	else
	  angle += Math::Pi / 4.f;
      }
    }

    // Look for new target if necessary
    if (_target == nullptr || !(_target->flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable)) {
      if (P_LookForPlayers(doom, true) == true)
	return;

      // Change to spawn state
      setState(doom, attributs.state_spawn);
      return;
    }

    // Do not attack twice in a row
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_JustAttacked) {
      flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_JustAttacked);

      if (doom.skill != DOOM::Enum::Skill::SkillNightmare)
	P_NewChaseDir(doom);

      return;
    }

    // Check for melee attack
    if (attributs.state_melee != DOOM::AbstractThing::ThingState::State_None && P_CheckMeleeRange(doom) == true) {
      doom.sound(attributs.sound_attack, position);

      // Change to melee state
      setState(doom, attributs.state_melee);
      return;
    }

    // Check for missile attack
    if (attributs.state_missile != DOOM::AbstractThing::ThingState::State_None &&
      true && // TODO: check if current game skill is Nightmare!
      move_count == 0 &&
      P_CheckMissileRange(doom) == true) {
      setState(doom, attributs.state_missile);
      flags = (DOOM::Enum::ThingProperty)(flags | DOOM::Enum::ThingProperty::ThingProperty_JustAttacked);
      return;
    }

    // Possibly choose another target
    if (doom.level.players.size() > 1 &&
      _target_threshold == 0 &&
      P_CheckSight(doom, *_target) == false &&
      P_LookForPlayers(doom, true) == true) {
      return;
    }

    // Chase towards player
    move_count += -1;
    if (move_count < 0 || P_Move(doom) == false)
      P_NewChaseDir(doom);

    // Make active sound
    if (std::rand() % 256 < 3)
      doom.sound(attributs.sound_active, position);
}

bool	DOOM::AbstractThing::P_LookForPlayers(DOOM::Doom& doom, bool full)
{
  // Reset target
  _target = nullptr;
  _target_threshold = 0;

  // Check every player in game
  for (DOOM::PlayerThing& player : doom.level.players) {
    // Player is dead
    if (player.health <= 0.f)
      continue;

    // Player is out of sight
    if (P_CheckSight(doom, player) == false)
      continue;

    // Player is behind and not too close
    if (full == false &&
      Math::Vector<2>::angle(Math::Vector<2>(std::cos(angle), std::sin(angle)), player.position.convert<2>() - position.convert<2>()) > Math::Pi / 2.f &&
      (position - player.position).length() > DOOM::AbstractThing::MeleeRange)
      continue;

    // Target found
    _target = &player;
    _target_threshold = DOOM::AbstractThing::TargetThreshold;
    return true;
  }

  // No target found
  return false;
}

bool	DOOM::AbstractThing::P_CheckSight(DOOM::Doom& doom, const DOOM::AbstractThing& target)
{
  // Test if an attack angle is available
  return !std::isnan(P_AimLineAttack(doom, target));
}

bool	DOOM::AbstractThing::P_CheckMeleeRange(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return false;

  // Cancel if not at range
  if ((position.convert<2>() - _target->position.convert<2>()).length() >= DOOM::AbstractThing::MeleeRange - 20.f + _target->attributs.radius)
    return false;

  // Cancel if target is not in sight
  if (P_CheckSight(doom, *_target) == false)
    return false;

  return true;
}

bool	DOOM::AbstractThing::P_CheckMissileRange(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return false;

  // Check is target is visible
  if (P_CheckSight(doom, *_target) == false)
    return false;

  // The target just hit the enemy, so fight back!
  if (flags & DOOM::Enum::ThingProperty::ThingProperty_JustHit) {
    flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_JustHit);
    return true;
  }

  // Wait reaction time
  if (reactiontime > 0)
    return false;

  // The probability of firing depends on the distance, the closer we are the higher the chance of firing.
  float	distance = (position.convert<2>() - _target->position.convert<2>()).length();

  // Higher chance if no melee attack
  if (attributs.state_melee == DOOM::AbstractThing::ThingState::State_None)
    distance += -128.f;

  // Vile can't attack from far
  if (type == DOOM::Enum::ThingType::ThingType_VILE && distance > 14 * 64.f)
    return false;

  // Undead close for fist attack
  if (type == DOOM::Enum::ThingType::ThingType_UNDEAD) {
    if (distance < 3 * 64.f)
      return false;
    distance /= 2.f;
  }

  // Higher chance for these monsters
  if (type == DOOM::Enum::ThingType::ThingType_CYBORG || type == DOOM::Enum::ThingType::ThingType_SPIDER || type == DOOM::Enum::ThingType::ThingType_SKULL)
    distance /= 2.f;

  // Limit the distance, higher max for Cyborg
  distance = std::min(distance, (type == DOOM::Enum::ThingType::ThingType_CYBORG) ? 160.f : 200.f);

  // Roll the dice
  return (std::rand() % 256) >= distance;
}

void	DOOM::AbstractThing::P_NewChaseDir(DOOM::Doom& doom)
{
  // Error if no target
  // TODO: we crashed once here
  if (_target == nullptr)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  DOOM::AbstractThing::Direction	move_old = move_direction;
  DOOM::AbstractThing::Direction	move_opposite = (DOOM::AbstractThing::Direction)(move_direction == DOOM::AbstractThing::Direction::DirectionNone ? DOOM::AbstractThing::Direction::DirectionNone : ((move_direction + DOOM::AbstractThing::Direction::DirectionNumber / 2) % DOOM::AbstractThing::Direction::DirectionNumber));
  Math::Vector<2>			delta = _target->position.convert<2>() - position.convert<2>();

  DOOM::AbstractThing::Direction	move_x = DOOM::AbstractThing::Direction::DirectionNone;
  DOOM::AbstractThing::Direction	move_y = DOOM::AbstractThing::Direction::DirectionNone;

  if (delta.x() > +10.f)
    move_x = Direction::DirectionEast;
  else if (delta.x() < -10.f)
    move_x = Direction::DirectionWest;

  if (delta.y() > +10.f)
    move_y = Direction::DirectionNorth;
  else if (delta.y() < -10.f)
    move_y = Direction::DirectionSouth;

  // Try direct route
  if (move_x != Direction::DirectionNone && move_y != Direction::DirectionNone) {
    move_direction = move_x == Direction::DirectionEast ?
      (move_y == Direction::DirectionNorth ? Direction::DirectionNorthEast : Direction::DirectionSouthEast) :
      (move_y == Direction::DirectionNorth ? Direction::DirectionNorthWest : Direction::DirectionSouthWest);

    if (move_direction != move_opposite && P_TryWalk(doom) == true) {
      return;
    }
  }

  // Try other direction
  if (std::rand() % 256 > 200 || std::abs(delta.x()) < std::abs(delta.y()))
    std::swap(move_x, move_y);

  if (move_x == move_opposite)
    move_x = Direction::DirectionNone;
  if (move_y == move_opposite)
    move_y = Direction::DirectionNone;

  // Either moved forward of attacked
  if (move_x != Direction::DirectionNone) {
    move_direction = move_x;
    if (P_TryWalk(doom) == true) {
      return;
    }
  }
  if (move_y != Direction::DirectionNone) {
    move_direction = move_y;
    if (P_TryWalk(doom) == true) {
      return;
    }
  }

  // There is no direct path to the player, so pick another direction
  if (move_old != Direction::DirectionNone) {
    move_direction = move_old;
    if (P_TryWalk(doom) == true) {
      return;
    }
  }

  // Randomly determine direction of search
  if (std::rand() % 2 == 0) {
    for (Direction dir = Direction::DirectionEast; dir <= Direction::DirectionSouthEast; dir = (Direction)(dir + 1)) {
      if (dir != move_opposite) {
	move_direction = dir;
	if (P_TryWalk(doom) == true) {
	  return;
	}
      }
    }
  }
  else {
    for (Direction dir = Direction::DirectionSouthEast; dir >= Direction::DirectionEast; dir = (Direction)(dir - 1)) {
      if (dir != move_opposite) {
	move_direction = dir;
	if (P_TryWalk(doom) == true) {
	  return;
	}
      }
    }
  }

  // Lastly, try the opposite direction
  if (move_opposite != Direction::DirectionNone) {
    move_direction = move_opposite;
    if (P_TryWalk(doom) == true) {
      return;
    }
  }

  // We cannot move
  move_direction = Direction::DirectionNone;
}

bool	DOOM::AbstractThing::P_TryWalk(DOOM::Doom& doom)
{
  // Attempt move
  if (P_Move(doom) == true) {
    move_count = std::rand() % 16;
    return true;
  }

  return false;
}

bool	DOOM::AbstractThing::P_TryMove(DOOM::Doom& doom, const Math::Vector<2>& position)
{
  // Solid wall or thing
  if (P_CheckPosition(doom, position) == false)
    return false;

  if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_NoClip)) {
    float target_floor = std::numeric_limits<float>::lowest();
    float target_ceiling = std::numeric_limits<float>::max();

    // Find target floor and ceiling height
    for (int16_t sector_index : doom.level.getSectors(position, attributs.radius / 2.f)) {
      target_floor = std::max(target_floor, doom.level.sectors[sector_index].floor_current);
      target_ceiling = std::min(target_ceiling, doom.level.sectors[sector_index].ceiling_current);
    }

    // Doesn't fit
    if (target_ceiling - target_floor < height)
      return false;

    // Must lower itself to fit
    if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_Teleport) &&
      target_ceiling - this->position.z() < height)
      return false;

    // Too big step up
    if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_Teleport) &&
      target_floor - this->position.z() > 24.f)
      return false;

    // Don't stand over a dropoff
    if (!(flags & (DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_Float)) &&
      ((target_floor - this->position.z() < -24.f) || (doom.level.sectors[doom.level.getSector(position).first].floor_current - doom.level.sectors[doom.level.getSector(this->position.convert<2>()).first].floor_current < -24.f)))
      return false;
  }

  if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap))
    doom.level.blockmap.moveThing(*this, this->position.convert<2>(), position);

  // Walkover linedef
  for (const std::pair<float, int16_t>& linedef_index : doom.level.getLinedefs(this->position.convert<2>(), position - this->position.convert<2>()))
    doom.level.linedefs[linedef_index.second]->walkover(doom, *this);

  // Move thing
  this->position.convert<2>() = position;

  return true;
}

bool	DOOM::AbstractThing::P_CheckPosition(DOOM::Doom& doom, const Math::Vector<2>& position)
{
  // Check collision with things
  for (const std::reference_wrapper<DOOM::AbstractThing> & thing : doom.level.getThings(position, (float)attributs.radius)) {
    if (&(thing.get()) != this &&
      (thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Solid) &&
      (thing.get().position.convert<2>() - position).length() < (float)(thing.get().attributs.radius + attributs.radius))
      return false;
  }

  // Check collision with linedefs
  for (int16_t linedef_index : doom.level.getLinedefs(position, (float)attributs.radius)) {
    const DOOM::AbstractLinedef&	linedef = *doom.level.linedefs[linedef_index];

    // One sided line
    if (linedef.back == -1)
      return false;

    if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)) {
      // Explicitely block monster
      if (linedef.flag & DOOM::AbstractLinedef::Flag::Impassible)
	return false;

      // Block monster only
      if (linedef.flag & DOOM::AbstractLinedef::Flag::BlockMonsters && (flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable) && type != DOOM::Enum::ThingType::ThingType_PLAYER)
	return false;
    }
  }

  return true;
}

bool	DOOM::AbstractThing::P_Move(DOOM::Doom& doom)
{
  // Cancel if no direction
  if (move_direction == Direction::DirectionNone) {
    return false;
  }

  Math::Vector<2> move_position = position.convert<2>() + (_directions[move_direction] * (float)attributs.speed);

  if (P_TryMove(doom, move_position) == false) {
    // Floating things
    if ((flags & DOOM::Enum::ThingProperty::ThingProperty_Float) && P_CheckPosition(doom, move_position) == true) {
      float target_floor = std::numeric_limits<float>::lowest();
      float target_ceiling = std::numeric_limits<float>::max();

      // Find target floor and ceiling height
      for (int16_t sector_index : doom.level.getSectors(position.convert<2>() + (_directions[move_direction] * (float)attributs.speed), attributs.radius / 2.f)) {
	target_floor = std::max(target_floor, doom.level.sectors[sector_index].floor_current);
	target_ceiling = std::min(target_ceiling, doom.level.sectors[sector_index].ceiling_current);
      }

      if (target_ceiling - target_floor >= height) {
	if (position.z() < target_floor)
	  position.z() += DOOM::AbstractThing::FloatSpeed;
	else
	  position.z() -= DOOM::AbstractThing::FloatSpeed;

	flags = (DOOM::Enum::ThingProperty)(flags | DOOM::Enum::ThingProperty::ThingProperty_InFloat);
	return true;
      }
    }

    move_direction = Direction::DirectionNone;
    
    // Try to open a door
    bool	switched = false;
    for (int16_t linedef_index : doom.level.getLinedefs(move_position, (float)attributs.radius))
      switched |= doom.level.linedefs[linedef_index]->switched(doom, *this);
    
    return switched;
  }
  else {
    flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_InFloat);
  }

  return true;
}

void	DOOM::AbstractThing::A_Hoof(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_hoof, position);
  A_Chase(doom);
}

void	DOOM::AbstractThing::A_BrainPain(DOOM::Doom& doom)
{
  // Emit pain sound
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_bospn);
}

void	DOOM::AbstractThing::A_Scream(DOOM::Doom& doom)
{
  DOOM::Doom::Resources::Sound::EnumSound	sound;

  switch (attributs.sound_death) {
    // No death sound
  case DOOM::Doom::Resources::Sound::EnumSound::Sound_None:
    return;

  case DOOM::Doom::Resources::Sound::EnumSound::Sound_podth1:
  case DOOM::Doom::Resources::Sound::EnumSound::Sound_podth2:
  case DOOM::Doom::Resources::Sound::EnumSound::Sound_podth3:
    sound = (DOOM::Doom::Resources::Sound::EnumSound)(DOOM::Doom::Resources::Sound::EnumSound::Sound_podth1 + std::rand() % 3);
    break;

  case DOOM::Doom::Resources::Sound::EnumSound::Sound_bgdth1:
  case DOOM::Doom::Resources::Sound::EnumSound::Sound_bgdth2:
    sound = (DOOM::Doom::Resources::Sound::EnumSound)(DOOM::Doom::Resources::Sound::EnumSound::Sound_bgdth1 + std::rand() % 2);
    break;

  default:
    sound = attributs.sound_death;
    break;
  }

  // Full volume for bosses
  if (type == DOOM::Enum::ThingType::ThingType_SPIDER || type == DOOM::Enum::ThingType::ThingType_CYBORG)
    doom.sound(sound);
  else
    doom.sound(sound, position);
}

void	DOOM::AbstractThing::A_VileStart(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_vilatk, position);
}

void	DOOM::AbstractThing::A_StartFire(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_flamst, position);
  A_Fire(doom);
}

void	DOOM::AbstractThing::A_FireCrackle(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_flame, position);
  A_Fire(doom);
}

void	DOOM::AbstractThing::A_CPosRefire(DOOM::Doom& doom)
{
  // Keep firing unless target got out of sight
  A_FaceTarget(doom);

  if (std::rand() % 256 < 40)
    return;

  if (_target == nullptr || _target->health <= 0 || P_CheckSight(doom, *_target) == false)
    setState(doom, attributs.state_see);
}

void	DOOM::AbstractThing::A_Metal(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_metal, position);
  A_Chase(doom);
}

void	DOOM::AbstractThing::A_SpidRefire(DOOM::Doom& doom)
{
  A_FaceTarget(doom);

  if (std::rand() % 256 < 10)
    return;

  if (_target == nullptr || _target->health <= 0 || P_CheckSight(doom, *_target) == false)
    setState(doom, attributs.state_see);
}

void	DOOM::AbstractThing::A_BabyMetal(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_bspwlk, position);
  A_Chase(doom);
}

void	DOOM::AbstractThing::A_FaceTarget(DOOM::Doom& doom)
{
  // Stop if no target
  if (_target == nullptr)
    return;

  // Remove ambush flag
  flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_Ambush);

  angle = Math::Vector<2>::angle(_target->position.convert<2>() - position.convert<2>());

  // If thing is invisible, randomize angle
  if (flags & DOOM::Enum::ThingProperty::ThingProperty_Shadow)
    angle += std::pow(Math::Random(), 2) * Math::Pi / 4.f;
}

void	DOOM::AbstractThing::A_SkelWhoosh(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Whoosh!
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_skeswg, position);
}

void	DOOM::AbstractThing::A_PlayerScream(DOOM::Doom& doom)
{
  // If the player dies with less than -50% without gibbing, special sound
  doom.sound(health < -50 ? DOOM::Doom::Resources::Sound::EnumSound::Sound_pdiehi : DOOM::Doom::Resources::Sound::EnumSound::Sound_pldeth, position);
}

void	DOOM::AbstractThing::A_SpawnSound(DOOM::Doom& doom)
{
  // Travelling cube sound
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_boscub, position);
  A_SpawnFly(doom);
}

void	DOOM::AbstractThing::A_Fall(DOOM::Doom& doom)
{
  // Thing is on ground, it can be walked over
  flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_Solid);
}

void	DOOM::AbstractThing::A_Pain(DOOM::Doom& doom)
{
  doom.sound(attributs.sound_pain, position);
}

void	DOOM::AbstractThing::A_XScream(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_slop, position);
}

void	DOOM::AbstractThing::A_PosAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Play pistol sound
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol, position);

  float target_height = P_AimLineAttack(doom, *_target);
  float atk_slope;

  if (std::isnan(target_height) == true)
    atk_slope = 0.f;
  else
    atk_slope = std::atan((target_height - (position.z() + height * 0.5f)) / (_target->position.convert<2>() - position.convert<2>()).length());

  atk_slope += std::pow(Math::Random() * 2.f - 1.f, 2) * Math::Pi / 8.f;

  float atk_angle = angle + std::pow(Math::Random() * 2.f - 1.f, 2) * Math::Pi / 8.f;
  float atk_damage = (float)((std::rand() % 5 + 1) * 3);

  // Attack
  P_LineAttack(doom, AbstractThing::MissileRange, Math::Vector<3>(position.x(), position.y(), position.z() + height / 2.f), Math::Vector<3>(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope)), atk_damage);
}

void	DOOM::AbstractThing::A_SPosAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Play pistol sound
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_shotgn, position);

  float target_height = P_AimLineAttack(doom, *_target);
  float slope;

  if (std::isnan(target_height) == true)
    slope = 0.f;
  else
    slope = std::atan((target_height - (position.z() + height * 0.5f)) / (_target->position.convert<2>() - position.convert<2>()).length());

  // 3 attacks
  // NOTE: should we randomize slope angle too ? the original game didn't
  for (int i = 0; i < 3; i++) {
    float               atk_angle = angle + std::pow(Math::Random() * 2.f - 1.f, 2) * Math::Pi / 8.f;
    float               atk_slope = slope + std::pow(Math::Random() * 2.f - 1.f, 2) * Math::Pi / 8.f;
    Math::Vector<3>     atk_shot(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope));
    float               atk_damage = (float)((std::rand() % 5 + 1) * 3);

    // Attack
    P_LineAttack(doom, AbstractThing::MissileRange, position + Math::Vector<3>(0.f, 0.f, height * 0.5f), atk_shot, atk_damage);
  }
}

void	DOOM::AbstractThing::A_CPosAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Play pistol sound
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_shotgn, position);

  float atk_angle = angle + std::pow(Math::Random(), 2) * Math::Pi / 8.f;
  float atk_damage = (float)((std::rand() % 5 + 1) * 3);
  float target_height = P_AimLineAttack(doom, *_target);
  float atk_slope = (std::isnan(target_height) == true ? 0.f : std::atan((target_height - (position.z() + height * 0.5f)) / (_target->position.convert<2>() - position.convert<2>()).length())) + std::pow(Math::Random() * 2.f - 1.f, 2) * Math::Pi / 8.f;

  // Attack
  P_LineAttack(doom, AbstractThing::MissileRange, Math::Vector<3>(position.x(), position.y(), position.z() + height / 2.f), Math::Vector<3>(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope)), atk_damage);
}

float	DOOM::AbstractThing::P_AimLineAttack(DOOM::Doom& doom, const DOOM::AbstractThing& target)
{
  float	target_bottom = target.position.z();
  float	target_top = target.position.z() + target.height;

  // Check every linedefs between thing and target
  for (const std::pair<float, int16_t>& linedef_index : doom.level.getLinedefs(position.convert<2>(), target.position.convert<2>() - position.convert<2>())) {
    DOOM::AbstractLinedef& linedef = *doom.level.linedefs[linedef_index.second];

    // Stop immediatly if linedef is impassible
    if (linedef.flag & DOOM::AbstractLinedef::Flag::Impassible)
      return std::numeric_limits<float>::quiet_NaN();

    // Can't see outside the map
    if (linedef.front == -1 || linedef.back == -1)
      return std::numeric_limits<float>::quiet_NaN();

    const float	sector_bottom = std::max(doom.level.sectors[doom.level.sidedefs[linedef.front].sector].floor_current, linedef.back != -1 ? doom.level.sectors[doom.level.sidedefs[linedef.back].sector].floor_current : std::numeric_limits<float>::lowest());
    const float	sector_top = std::min(doom.level.sectors[doom.level.sidedefs[linedef.front].sector].ceiling_current, linedef.back != -1 ? doom.level.sectors[doom.level.sidedefs[linedef.back].sector].ceiling_current : std::numeric_limits<float>::max());
    
    target_bottom = std::max(target_bottom, (sector_bottom - (position.z() + 0.75f * height)) / linedef_index.first + (position.z() + 0.75f * attributs.height));
    target_top = std::min(target_top, (sector_top - (position.z() + 0.75f * height)) / linedef_index.first + (position.z() + 0.75f * attributs.height));

    // Cancel if view is obstructed
    if (target_bottom >= target_top)
      return std::numeric_limits<float>::quiet_NaN();
  }

  // Line of sight
  return (target_bottom + target_top) / 2.f;
}

void	DOOM::AbstractThing::P_LineAttack(DOOM::Doom& doom, float atk_range, const Math::Vector<3>& atk_origin, const Math::Vector<3>& atk_direction, float atk_damage)
{
  std::list<std::pair<float, int16_t>>						linedefs_list = doom.level.getLinedefs(atk_origin.convert<2>(), atk_direction.convert<2>(), atk_range);
  std::list<std::pair<float, std::reference_wrapper<DOOM::AbstractThing>>>	things_list = doom.level.getThings(atk_origin.convert<2>(), atk_direction.convert<2>(), atk_range);
  float                                                                         sector = std::numeric_limits<float>::max();

  // Find first shootable thing
  while (things_list.empty() == false)
    // TODO: intersection only partially checked, check shoot from above or under
    if (&things_list.front().second.get() != this &&
      things_list.front().second.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable &&
      things_list.front().second.get().health > 0.f &&
      atk_origin.z() + atk_direction.z() * things_list.front().first >= things_list.front().second.get().position.z() &&
      atk_origin.z() + atk_direction.z() * things_list.front().first <= things_list.front().second.get().position.z() + things_list.front().second.get().height)
      break;
    else
      things_list.pop_front();

  // Find first shootable linedef
  while (linedefs_list.empty() == false) {
    DOOM::AbstractLinedef& linedef = *doom.level.linedefs[linedefs_list.front().second].get();
    const DOOM::Doom::Level::Vertex& linedef_start = doom.level.vertexes[linedef.start];
    const DOOM::Doom::Level::Vertex& linedef_end = doom.level.vertexes[linedef.end];

    Math::Vector<2>	linedef_direction = linedef_end - linedef_start;
    int16_t		sidedef_front_index = linedef.front;
    int16_t		sidedef_back_index = linedef.back;

    // Swap sidedef if on left side
    if (Math::Vector<2>::cos(atk_origin.convert<2>() - linedef_start, Math::Vector<2>(+linedef_direction.y(), -linedef_direction.x())) < 0.f) {
      std::swap(sidedef_front_index, sidedef_back_index);
    }

    // No intersection from outside the map
    if (sidedef_front_index == -1) {
      linedefs_list.pop_front();
      continue;
    }

    DOOM::Doom::Level::Sidedef& sidedef_front = doom.level.sidedefs[sidedef_front_index];
    DOOM::Doom::Level::Sector& sector_front = doom.level.sectors[sidedef_front.sector];

    float height = atk_origin.z() + atk_direction.z() * linedefs_list.front().first;

    if (atk_direction.z() != 0.f) {
      // Intersection with floor
      if (height < sector_front.floor_current) {
        sector = (sector_front.floor_current - atk_origin.z()) / atk_direction.z();
        break;
      }
      // Intersection with ceiling
      if (height > sector_front.ceiling_current) {
        sector = (sector_front.ceiling_current - atk_origin.z()) / atk_direction.z();
        break;
      }
    }

    // Impassible wall
    if (sidedef_back_index == -1) {
      break;
    }

    DOOM::Doom::Level::Sidedef& sidedef_back = doom.level.sidedefs[sidedef_back_index];
    DOOM::Doom::Level::Sector& sector_back = doom.level.sectors[sidedef_back.sector];

    // Intersection with lower of upper wall
    if (height < sector_back.floor_current || height > sector_back.ceiling_current) {
      break;
    }
    
    // Intersection with middle texture
    if (sidedef_front.middle().height != 0) {
      break;
    }

    // No intersection with linedef
    linedefs_list.pop_front();
  }

  // Shoot sector floor or ceiling
  if (sector != std::numeric_limits<float>::max() &&
    (things_list.empty() || things_list.front().first > sector) &&
    (linedefs_list.empty() || linedefs_list.front().first > sector)) {
    P_SpawnPuff(doom, atk_origin + atk_direction * sector);
  }

  // Shoot thing
  else if (things_list.empty() == false && (linedefs_list.empty() == true || things_list.front().first < linedefs_list.front().first)) {
    things_list.front().second.get().damage(doom, *this, atk_damage);

    // Spawn bullet puffs or blod spots, depending on target type.
    if (things_list.front().second.get().flags & DOOM::Enum::ThingProperty::ThingProperty_NoBlood)
      P_SpawnPuff(doom, atk_origin + atk_direction * things_list.front().first);
    else
      P_SpawnBlood(doom, atk_origin + atk_direction * things_list.front().first, atk_damage);
  }

  // Shoot linedef
  else if (linedefs_list.empty() == false && (things_list.empty() == true || linedefs_list.front().first < things_list.front().first)) {
    DOOM::AbstractLinedef& linedef = *doom.level.linedefs[linedefs_list.front().second].get();

    // Gunfire trigger
    linedef.gunfire(doom, *this);
    
    Math::Vector<2> linedef_direction = doom.level.vertexes[linedef.end] - doom.level.vertexes[linedef.start];
    Math::Vector<3> linedef_normal(Math::Vector<3>(+linedef_direction.y(), -linedef_direction.x(), 0.f) / linedef_direction.length());

    if (Math::Vector<2>::cos(linedef_normal.convert<2>(), atk_direction.convert<2>()) > 0.f)
      linedef_normal *= -1.f;

    // Spawn smoke puff
    P_SpawnPuff(doom, atk_origin + atk_direction * linedefs_list.front().first + linedef_normal);
  }
}

void    DOOM::AbstractThing::P_SpawnPuff(DOOM::Doom& doom, const Math::Vector<3>& coordinates) const
{
  // Spawn smoke puff and set its height
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, DOOM::Enum::ThingType::ThingType_SMOKE, DOOM::Enum::ThingFlag::FlagNone, coordinates.x(), coordinates.y(), 0.f));
  doom.level.things.back()->position.z() = coordinates.z();
  doom.level.things.back()->_thrust.z() = +1.f;
  doom.level.things.back()->_elapsed += DOOM::Doom::Tic * (float)std::min(std::rand() % 4, _states[doom.level.things.back()->_state].duration - 1);
}

void    DOOM::AbstractThing::P_SpawnBlood(DOOM::Doom& doom, const Math::Vector<3>& coordinates, float damage) const
{
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, DOOM::Enum::ThingType::ThingType_BLOOD, DOOM::Enum::ThingFlag::FlagNone, coordinates.x(), coordinates.y(), 0.f));
  doom.level.things.back()->position.z() = coordinates.z();
  doom.level.things.back()->_thrust.z() = +2.f;

  if (damage <= 12.f && damage >= 9.f)
    doom.level.things.back()->setState(doom, DOOM::AbstractThing::ThingState::State_BLOOD2);
  else if (damage < 9.f)
    doom.level.things.back()->setState(doom, DOOM::AbstractThing::ThingState::State_BLOOD3);
  else
    doom.level.things.back()->_elapsed += DOOM::Doom::Tic * (float)std::min(std::rand() % 4, _states[doom.level.things.back()->_state].duration - 1);
}

void	DOOM::AbstractThing::P_LineSwitch(DOOM::Doom& doom, float swc_range, const Math::Vector<3>& swc_origin, const Math::Vector<3>& swc_direction)
{
  std::list<std::pair<float, int16_t>>						linedefs_list = doom.level.getLinedefs(swc_origin.convert<2>(), swc_direction.convert<2>(), swc_range);
  std::list<std::pair<float, std::reference_wrapper<DOOM::AbstractThing>>>	things_list = doom.level.getThings(swc_origin.convert<2>(), swc_direction.convert<2>(), swc_range);
  float                                                                         sector = std::numeric_limits<float>::max();

  // Find first solid thing
  while (things_list.empty() == false)
    // TODO: intersection only partially checked, check shoot from above or under
    if (&things_list.front().second.get() != this &&
      things_list.front().second.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Solid &&
      swc_origin.z() + swc_direction.z() * things_list.front().first >= things_list.front().second.get().position.z() &&
      swc_origin.z() + swc_direction.z() * things_list.front().first <= things_list.front().second.get().position.z() + things_list.front().second.get().height)
      break;
    else
      things_list.pop_front();

  // Find first intersected linedef or floor/ceiling
  while (linedefs_list.empty() == false) {
    DOOM::AbstractLinedef& linedef = *doom.level.linedefs[linedefs_list.front().second].get();
    const DOOM::Doom::Level::Vertex& linedef_start = doom.level.vertexes[linedef.start];
    const DOOM::Doom::Level::Vertex& linedef_end = doom.level.vertexes[linedef.end];

    Math::Vector<2> linedef_direction = linedef_end - linedef_start;
    int16_t         sidedef_front_index = linedef.front;
    int16_t         sidedef_back_index = linedef.back;

    // Swap sidedef if on left side
    if (Math::Vector<2>::cos(swc_origin.convert<2>() - linedef_start, Math::Vector<2>(+linedef_direction.y(), -linedef_direction.x())) < 0.f) {
      std::swap(sidedef_front_index, sidedef_back_index);
    }

    // No intersection from outside the map
    if (sidedef_front_index == -1) {
      linedefs_list.pop_front();
      continue;
    }

    DOOM::Doom::Level::Sidedef& sidedef_front = doom.level.sidedefs[sidedef_front_index];
    DOOM::Doom::Level::Sector& sector_front = doom.level.sectors[sidedef_front.sector];

    float intersection = swc_origin.z() + swc_direction.z() * linedefs_list.front().first;

    if (swc_direction.z() != 0.f) {
      // Intersection with floor
      if (intersection < sector_front.floor_current) {
        sector = (sector_front.floor_current - swc_origin.z()) / swc_direction.z();
        break;
      }
      // Intersection with ceiling
      if (intersection > sector_front.ceiling_current) {
        sector = (sector_front.ceiling_current - swc_origin.z()) / swc_direction.z();
        break;
      }
    }

    // Impassible wall
    if (sidedef_back_index == -1) {
      break;
    }

    DOOM::Doom::Level::Sidedef& sidedef_back = doom.level.sidedefs[sidedef_back_index];
    DOOM::Doom::Level::Sector& sector_back = doom.level.sectors[sidedef_back.sector];

    // Intersection with lower of upper wall
    if (intersection < sector_back.floor_current || intersection > sector_back.ceiling_current) {
      break;
    }

    // Intersection with middle texture
    if (sidedef_front.middle().height != 0) {
      break;
    }

    // No intersection with linedef
    linedefs_list.pop_front();
  }

  // Does nothing if floor/ceiling intersected
  if (sector != std::numeric_limits<float>::max() &&
    (things_list.empty() || things_list.front().first > sector) &&
    (linedefs_list.empty() || linedefs_list.front().first > sector)) {
    return;
  }

  // Does nothing if thing intersected
  else if (things_list.empty() == false && (linedefs_list.empty() == true || things_list.front().first < linedefs_list.front().first)) {
    return;
  }

  // Shoot linedef
  else if (linedefs_list.empty() == false && (things_list.empty() == true || linedefs_list.front().first < things_list.front().first)) {
    DOOM::AbstractLinedef& linedef = *doom.level.linedefs[linedefs_list.front().second].get();
    Math::Vector<2> linedef_direction = doom.level.vertexes[linedef.end] - doom.level.vertexes[linedef.start];
    Math::Vector<2> linedef_normal(Math::Vector<2>(+linedef_direction.y(), -linedef_direction.x()) / linedef_direction.length());

    // Does nothing if on left side of linedef
    if (Math::Vector<2>::cos(linedef_normal, swc_direction.convert<2>()) > 0.f)
      return;

    // Switch linedef
    linedef.switched(doom, *this);
  }
}

void	DOOM::AbstractThing::A_SargAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Melee attack
  if (P_CheckMeleeRange(doom) == true)
    _target->damage(doom, *this, (float)((std::rand() % 10 + 1) * 4));
}

void	DOOM::AbstractThing::A_SkelFist(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Melee attack
  if (P_CheckMeleeRange(doom) == true) {
    doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_skepch, position);
    _target->damage(doom, *this, (float)((std::rand() % 10 + 1) * 6));
  }
}

void    DOOM::AbstractThing::P_SpawnMissile(DOOM::Doom& doom, DOOM::Enum::ThingType type)
{
  // Does nothing if no target
  if (_target == nullptr)
    return;

  float           target_height = P_AimLineAttack(doom, *_target);
  float           atk_slope = std::isnan(target_height) == true ? 0.f : std::atan((target_height - (position.z() + height * 0.5f)) / (_target->position.convert<2>() - position.convert<2>()).length()) + ((_target->flags & DOOM::Enum::ThingProperty::ThingProperty_Shadow) ? std::pow(Math::Random() * 2.f - 1.f, 2) * Math::Pi / 8.f : 0);
  float	          atk_angle = Math::Vector<2>::angle((_target->position.convert<2>() - position.convert<2>())) + ((_target->flags & DOOM::Enum::ThingProperty::ThingProperty_Shadow) ? std::pow(Math::Random() * 2.f - 1.f, 2) * Math::Pi / 8.f : 0);
  Math::Vector<3> direction(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope));

  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, type, DOOM::Enum::ThingFlag::FlagNone, position.x() + std::cos(atk_angle) * attributs.radius / 2.f, position.y() + std::sin(atk_angle) * attributs.radius / 2.f, atk_angle));
  doom.level.things.back()->position.z() = position.z() + 32.f;
  doom.level.things.back()->_thrust = direction * (doom.level.things.back()->attributs.speed / direction.length());
  doom.level.things.back()->_target = this;

  doom.sound(doom.level.things.back()->attributs.sound_see, doom.level.things.back()->position);
}

void	DOOM::AbstractThing::A_Explode(DOOM::Doom& doom)
{
  P_RadiusAttack(doom, _target != nullptr ? *_target : *this, 128);
}

void    DOOM::AbstractThing::P_ExplodeMissile(DOOM::Doom& doom)
{
  // Stop missile
  _thrust = Math::Vector<3>(0.f, 0.f, 0.f);

  // Disable missile flag
  flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_Missile);

  // Play death sound
  doom.sound(attributs.sound_death, position);

  // Change state to death
  setState(doom, attributs.state_death);

  // Randomly shorten first frame of animation
  _elapsed += DOOM::Doom::Tic * (float)(std::rand() % 4);
}

void    DOOM::AbstractThing::P_ExplodeMissile(DOOM::Doom& doom, DOOM::AbstractThing& target)
{
  // Explode missile
  P_ExplodeMissile(doom);

  // Ignore non-shootable things
  if (!(target.flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable))
    return;

  // Ignore missile emitter
  if (_target == &target)
    return;

  // Don't hit the same species as originator, except for player
  if (_target->type == target.type && _target->type != DOOM::Enum::ThingType::ThingType_PLAYER)
    return;

  // Knight and Bruiser are of the same species
  if ((_target->type == DOOM::Enum::ThingType::ThingType_KNIGHT && target.type == DOOM::Enum::ThingType::ThingType_BRUISER) ||
    (_target->type == DOOM::Enum::ThingType::ThingType_BRUISER && target.type == DOOM::Enum::ThingType::ThingType_KNIGHT))
    return;

  // Damage target
  target.damage(doom, *this, *_target, (float)((std::rand() % 8 + 1) * attributs.damage));
}

void	DOOM::AbstractThing::damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, DOOM::AbstractThing& origin, float damage)
{
  // Cancel if already dead
  if (health <= 0)
    return;

  // Take half damage in baby mode
  if (doom.skill == DOOM::Enum::Skill::SkillBaby)
    damage /= 2;

  // Deal damage
  health = std::max(0.f, health - damage);

  // Reset flying skull thrust
  if (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly)
    _thrust = Math::Vector<3>();

  // Ignore thrust if thing is cheating or is attacked by player chainsaw
  // TDOO: check player chainsaw
  if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_NoClip) &&
    &attacker != this &&
    (attacker.type != DOOM::Enum::ThingType::ThingType_PLAYER || true)) {
    float angle = Math::Vector<2>::angle(position.convert<2>() - attacker.position.convert<2>());
    float push = (float)(damage * 2);

    // Sometimes fall foward when killed
    if (damage < 40 && health <= 0 && position.z() - attacker.position.z() > 64 && (std::rand() % 2 == 1)) {
      angle += Math::Pi;
      push *= 4.f;
    }

    // Apply thrust
    thrust(Math::Vector<3>(std::cos(angle), std::sin(angle), 0.f) * push);
  }

  // Thing killed
  if (health <= 0) {
    // Remove some flags
    flags = (DOOM::Enum::ThingProperty)(flags & ~(DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_SkullFly));
    if (type != DOOM::Enum::ThingType::ThingType_SKULL)
      flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_NoGravity);

    // Add corpse flags
    flags = (DOOM::Enum::ThingProperty)(flags | DOOM::Enum::ThingProperty::ThingProperty_Corpse | DOOM::Enum::ThingProperty::ThingProperty_DropOff);

    // Reduce height
    height /= 4;

    DOOM::Enum::ThingType drop = (DOOM::Enum::ThingType)-1;

    // Drop stuff
    switch (type) {
    case DOOM::Enum::ThingType::ThingType_WOLFSS:
    case DOOM::Enum::ThingType::ThingType_POSSESSED:
      drop = DOOM::Enum::ThingType::ThingType_CLIP;
      break;

    case DOOM::Enum::ThingType::ThingType_SHOTGUY:
      drop = DOOM::Enum::ThingType::ThingType_SHOTGUN;
      break;

    case DOOM::Enum::ThingType::ThingType_CHAINGUY:
      drop = DOOM::Enum::ThingType::ThingType_CHAINGUN;
      break;

    default:
      break;
    }

    // Spawn drop, thrown away from thing
    if (drop != (DOOM::Enum::ThingType) - 1) {
      doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, drop, DOOM::Enum::ThingFlag::FlagNone, position.x(), position.y(), angle));
      doom.level.things.back()->position.z() = position.z() + 32.f;
      doom.level.things.back()->_thrust = _thrust * 3.2f + Math::Vector<3>(0.f, 0.f, 3.2f);
      doom.level.things.back()->_gravity = -0.5f;
      doom.level.things.back()->flags = (DOOM::Enum::ThingProperty)(doom.level.things.back()->flags | DOOM::Enum::ThingProperty::ThingProperty_Dropped);
    }
  }

  // Change target
  if (&origin != this && _target_threshold <= 0) {
    _target = &origin;
    _target_threshold = 100;
  }

  // Gibs monster
  if (attributs.state_xdeath != DOOM::AbstractThing::ThingState::State_None && health < -attributs.spawnhealth)
    setState(doom, attributs.state_xdeath);

  // Kill monster
  else if (attributs.state_death != DOOM::AbstractThing::ThingState::State_None && health <= 0)
    setState(doom, attributs.state_death);

  // Start pain state
  else if (attributs.state_pain != DOOM::AbstractThing::ThingState::State_None && std::rand() % 256 < attributs.painchance)
    setState(doom, attributs.state_pain);
}

void	DOOM::AbstractThing::damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, float dmg)
{
  // The attacker is also the origin
  damage(doom, attacker, attacker, dmg);
}

void	DOOM::AbstractThing::damage(DOOM::Doom& doom, float dmg)
{
  // The attacker is also the origin
  damage(doom, *this, dmg);
}

bool  DOOM::AbstractThing::key(DOOM::Enum::KeyColor color) const
{
  // Thing does have keys
  return false;
}

void  DOOM::AbstractThing::P_RadiusAttack(DOOM::Doom& doom, DOOM::AbstractThing& source, float damage)
{
  // NOTE: we are using true distance instead of X/Y distance to compute damages
  float distance = (float)(damage + DOOM::AbstractThing::MaxRadius);
  std::set<std::reference_wrapper<DOOM::AbstractThing>> things;
  
  // Get blocks of block in radius
  for (float x = position.x() - distance; x <= position.x() + distance; x += 128.f)
    for (float y = position.y() - distance; y <= position.y() + distance; y += 128.f) {
      int block_index = doom.level.blockmap.index(Math::Vector<2>(x, y));

      if (block_index == -1)
        continue;

      things.insert(doom.level.blockmap.blocks[block_index].things.begin(), doom.level.blockmap.blocks[block_index].things.end());
    }
  
  // Attempt to damage things
  for (const std::reference_wrapper<DOOM::AbstractThing>& thing : things) {
    // Ignore non shootable and Cybord/Spider boss
    if (!(thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable) ||
      thing.get().type == DOOM::Enum::ThingType::ThingType_CYBORG || 
      thing.get().type == DOOM::Enum::ThingType::ThingType_SPIDER)
      continue;

    float distance = std::max(0.f, (position.convert<2>() - thing.get().position.convert<2>()).length() - thing.get().attributs.radius);

    // Thing too far away or not visible
    if (distance >= damage || P_CheckSight(doom, thing.get()) == false)
      continue;

    // Damage thing
    thing.get().damage(doom, *this, source, damage - distance);
  }
}

void	DOOM::AbstractThing::A_SkelMissile(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Space missile higher
  position.z() += 16.f;
  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_TRACER);
  position.z() -= 16.f;
}

void	DOOM::AbstractThing::A_SkullAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Force skullfly flag
  flags = (DOOM::Enum::ThingProperty)(flags | DOOM::Enum::ThingProperty::ThingProperty_SkullFly);

  // Scream and face target
  doom.sound(attributs.sound_attack, position);
  A_FaceTarget(doom);

  // Rush to target
  _thrust = Math::Vector<3>();
  thrust(Math::Vector<3>(
    std::cos(angle) * DOOM::AbstractThing::SkullSpeed,
    std::sin(angle) * DOOM::AbstractThing::SkullSpeed,
    (_target->position.z() + (float)_target->height / 2.f - position.z()) / std::max(1.f, (_target->position.convert<2>() - position.convert<2>()).length()) * DOOM::AbstractThing::SkullSpeed));
}

void    DOOM::AbstractThing::A_PainAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Shoot skull to target
  A_FaceTarget(doom);
  A_PainShootSkull(doom, angle);
}

void    DOOM::AbstractThing::A_PainDie(DOOM::Doom& doom)
{
  // Transform to corpse
  A_Fall(doom);

  // Shoot skull in various directions
  A_PainShootSkull(doom, angle + 1.f * Math::Pi / 2.f);
  A_PainShootSkull(doom, angle + 2.f * Math::Pi / 2.f);
  A_PainShootSkull(doom, angle + 3.f * Math::Pi / 2.f);
}

void    DOOM::AbstractThing::A_PainShootSkull(DOOM::Doom& doom, float orientation)
{
  // Limit to 20 skull on the level
  if (std::count_if(doom.level.things.begin(), doom.level.things.end(), [](const std::unique_ptr<DOOM::AbstractThing>& thing) { return thing->type == DOOM::Enum::ThingType::ThingType_SKULL; }) > 20)
    return;

  float prestep = 4.f + 3.f * (attributs.radius + _attributs[DOOM::Enum::ThingType::ThingType_SKULL].radius) / 2.f;

  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom,
    DOOM::Enum::ThingType::ThingType_SKULL,
    DOOM::Enum::ThingFlag::FlagNone,
    position.x() + std::cos(orientation) * prestep,
    position.y() + std::sin(orientation) * prestep,
    0.f));
  doom.level.things.back()->position.z() = position.z() + 8.f;

  // Kill if no room for new skull
  if (doom.level.things.back()->P_TryMove(doom, doom.level.things.back()->position.convert<2>()) == false) {
    doom.level.things.back()->damage(doom, *this, 10000);
  }

  // Attack target
  else {
    doom.level.things.back()->_target = _target;
    doom.level.things.back()->A_SkullAttack(doom);
  }
  
}

void    DOOM::AbstractThing::A_KeenDie(DOOM::Doom& doom)
{
  A_Fall(doom);

  // Scan the remaining monsters to see if all Keens are dead
  for (const std::unique_ptr<DOOM::AbstractThing>& thing : doom.level.things)
    if (thing->type == type && thing->health > 0)
      return;

  // Lower floor to lowest neighbor
  for (DOOM::Doom::Level::Sector& sector : doom.level.sectors)
    if (sector.tag == 666)
      sector.action<DOOM::Doom::Level::Sector::Action::Leveling>(doom, 23);
}

void    DOOM::AbstractThing::A_BossDeath(DOOM::Doom& doom)
{
  // Check if boss level
  if (doom.mode == DOOM::Enum::Mode::ModeCommercial) {
    if (doom.level.episode.second != 7)
      return;
    if (type != DOOM::Enum::ThingType::ThingType_FATSO &&
      type != DOOM::Enum::ThingType::ThingType_BABY)
      return;
  }
  else {
    switch (doom.level.episode.first) {
    case 1:
      if (doom.level.episode.second != 8 || type != DOOM::Enum::ThingType::ThingType_BRUISER)
        return;
      break;
    case 2:
      if (doom.level.episode.second != 8 || type != DOOM::Enum::ThingType::ThingType_CYBORG)
        return;
      break;
    case 3:
      if (doom.level.episode.second != 8 || type != DOOM::Enum::ThingType::ThingType_SPIDER)
        return;
      break;
    case 4:
      switch (doom.level.episode.second) {
      case 6:
        if (type != DOOM::Enum::ThingType::ThingType_CYBORG)
          return;
        break;
      case 8:
        if (type != DOOM::Enum::ThingType::ThingType_SPIDER)
          return;
        break;
      default:
        return;
        break;
      }
      break;
    default:
      break;
    }
  }

  // Make sure there is a player alive for victory
  float max_health = 0.f;
  for (const DOOM::PlayerThing& player : doom.level.players)
    max_health = std::max(max_health, player.health);
  if (max_health <= 0.f)
    return;

  // Scan remaining things to see if all bosses are dead
  for (const std::unique_ptr<DOOM::AbstractThing>& thing : doom.level.things)
    if (thing->type == type && thing->health > 0.f)
      return;

  // Victory!
  if (doom.mode == DOOM::Enum::Mode::ModeCommercial) {
    if (type == DOOM::Enum::ThingType::ThingType_FATSO) {
      for (DOOM::Doom::Level::Sector& sector : doom.level.sectors)
        if (sector.tag == 666)
          sector.action<DOOM::Doom::Level::Sector::Action::Leveling>(doom, 23);
      return;
    }
    if (type == DOOM::Enum::ThingType::ThingType_BABY) {
      for (DOOM::Doom::Level::Sector& sector : doom.level.sectors)
        if (sector.tag == 667)
          sector.action<DOOM::Doom::Level::Sector::Action::Leveling>(doom, 30); // NOTE: is this the correct action ? source code says "raisetoTexture"
      return;
    }
  }
  else {
    if (doom.level.episode.first == 1) {
      for (DOOM::Doom::Level::Sector& sector : doom.level.sectors)
        if (sector.tag == 666)
          sector.action<DOOM::Doom::Level::Sector::Action::Leveling>(doom, 23);
      return;
    }
    if (doom.level.episode.first == 4) {
      if (doom.level.episode.first == 6) {
        for (DOOM::Doom::Level::Sector& sector : doom.level.sectors)
          if (sector.tag == 666)
            sector.action<DOOM::Doom::Level::Sector::Action::Leveling>(doom, 106); // NOTE: is this the correct action ? source code says "blazeOpen"
        return;
      }
      if (doom.level.episode.first == 8) {
        for (DOOM::Doom::Level::Sector& sector : doom.level.sectors)
          if (sector.tag == 666)
            sector.action<DOOM::Doom::Level::Sector::Action::Leveling>(doom, 23);
        return;
      }
    }
  }

  // TODO: Exit level, go to next level
}

void    DOOM::AbstractThing::A_BrainScream(DOOM::Doom& doom)
{
  // Spawn random explosion around brain
  for (float x = position.x() - 196.f; x < position.x() + 320.f; x += 8.f) {
    doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom,
      DOOM::Enum::ThingType::ThingType_ROCKET,
      DOOM::Enum::ThingFlag::FlagNone,
      x,
      position.y() - 320.f,
      0.f));
    doom.level.things.back()->position.z() = 128.f + (std::rand() % 256) * 2;
    doom.level.things.back()->thrust(Math::Vector<3>(0.f, 0.f, (std::rand() % 256) * 512.f));
    doom.level.things.back()->setState(doom, DOOM::AbstractThing::ThingState::State_BRAINEXPLODE1);
    doom.level.things.back()->_elapsed += sf::seconds(DOOM::Doom::Tic.asSeconds() * std::min(_states[DOOM::AbstractThing::ThingState::State_BRAINEXPLODE1].duration - 1, (std::rand() % 8)));
  }
  
  // Scream
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_bosdth);
}

void    DOOM::AbstractThing::A_BrainExplode(DOOM::Doom& doom)
{
  // Spawn an explosion
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom,
    DOOM::Enum::ThingType::ThingType_ROCKET,
    DOOM::Enum::ThingFlag::FlagNone,
    position.x() + (std::rand() % 256 - std::rand() % 256) / 32.f,
    position.y(),
    0.f));
  doom.level.things.back()->position.z() = 128.f + (std::rand() % 256) * 2;
  doom.level.things.back()->thrust(Math::Vector<3>(0.f, 0.f, (std::rand() % 256) * 512.f));
  doom.level.things.back()->setState(doom, DOOM::AbstractThing::ThingState::State_BRAINEXPLODE1);
  doom.level.things.back()->_elapsed += sf::seconds(DOOM::Doom::Tic.asSeconds() * std::min(_states[DOOM::AbstractThing::ThingState::State_BRAINEXPLODE1].duration - 1, (std::rand() % 8)));
}

void    DOOM::AbstractThing::A_BrainDie(DOOM::Doom& doom)
{
  // TODO: Exit level, go to next level
}

void    DOOM::AbstractThing::A_FatRaise(DOOM::Doom& doom)
{
  // Faace target and scream
  A_FaceTarget(doom);
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_manatk, position);
}

void    DOOM::AbstractThing::A_FatAttack1(DOOM::Doom& doom)
{
  // Does nothing if no target
  if (_target == nullptr)
    return;

  A_FaceTarget(doom);

  // Deviation
  angle += DOOM::AbstractThing::FatSpread;

  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_FATSHOT);
  doom.level.things.back()->angle += DOOM::AbstractThing::FatSpread;
  doom.level.things.back()->_thrust.x() = doom.level.things.back()->attributs.speed * std::cos(doom.level.things.back()->angle);
  doom.level.things.back()->_thrust.y() = doom.level.things.back()->attributs.speed * std::sin(doom.level.things.back()->angle);
}

void    DOOM::AbstractThing::A_FatAttack2(DOOM::Doom& doom)
{
  // Does nothing if no target
  if (_target == nullptr)
    return;

  A_FaceTarget(doom);

  // Opposite deviation
  angle -= DOOM::AbstractThing::FatSpread;

  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_FATSHOT);
  doom.level.things.back()->angle -= 2.f * DOOM::AbstractThing::FatSpread;
  doom.level.things.back()->_thrust.x() = doom.level.things.back()->attributs.speed * std::cos(doom.level.things.back()->angle);
  doom.level.things.back()->_thrust.y() = doom.level.things.back()->attributs.speed * std::sin(doom.level.things.back()->angle);
}

void    DOOM::AbstractThing::A_FatAttack3(DOOM::Doom& doom)
{
  // Does nothing if no target
  if (_target == nullptr)
    return;

  A_FaceTarget(doom);

  // Spawn first missile
  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_FATSHOT);
  doom.level.things.back()->angle -= DOOM::AbstractThing::FatSpread / 2.f;
  doom.level.things.back()->_thrust.x() = doom.level.things.back()->attributs.speed * std::cos(doom.level.things.back()->angle);
  doom.level.things.back()->_thrust.y() = doom.level.things.back()->attributs.speed * std::sin(doom.level.things.back()->angle);

  // Spawn second missile
  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_FATSHOT);
  doom.level.things.back()->angle += DOOM::AbstractThing::FatSpread / 2.f;
  doom.level.things.back()->_thrust.x() = doom.level.things.back()->attributs.speed * std::cos(doom.level.things.back()->angle);
  doom.level.things.back()->_thrust.y() = doom.level.things.back()->attributs.speed * std::sin(doom.level.things.back()->angle);
}

void    DOOM::AbstractThing::A_VileChase(DOOM::Doom& doom)
{
  // TODO: finish this!

  // Check for corpse to raise
  if (move_direction != DOOM::AbstractThing::Direction::DirectionNone)
  {
    // Iterator over things touching Vile
    for (const auto& thing : doom.level.getThings(position.convert<2>() + (_directions[move_direction] * (float)attributs.speed), attributs.radius)) {
      // Check if thing can be resurrected
      if (!(thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Corpse) ||
        _states[thing.get()._state].duration != -1 ||
        thing.get().attributs.state_raise == DOOM::AbstractThing::ThingState::State_None)
        continue;

      // Check that there is room for the thing
      if (thing.get().P_CheckPosition(doom, thing.get().position.convert<2>()) == false)
        continue;

      // Vile face resurrected thing
      auto tmp = _target;
      _target = &thing.get();
      A_FaceTarget(doom);
      _target = tmp;

      // Resurrect!
      setState(doom, DOOM::AbstractThing::ThingState::State_VILE_HEAL1);
      doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_slop, thing.get().position);
      
      // Reset properties
      thing.get().setState(doom, thing.get().attributs.state_raise);
      thing.get().height *= 4;
      thing.get().flags = thing.get().attributs.properties;
      thing.get().health = (float)thing.get().attributs.spawnhealth;
      thing.get()._target = nullptr;

      return;
    }
  }

  // Return to normal attack
  A_Chase(doom);
}

void    DOOM::AbstractThing::A_VileTarget(DOOM::Doom& doom)
{
  // Cancel when no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Spawn fire
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, DOOM::Enum::ThingType::ThingType_FIRE, DOOM::Enum::ThingFlag::FlagNone, _target->position.x(), _target->position.y(), 0.f));
  doom.level.things.back()->position.z() = _target->position.z();
  doom.level.things.back()->_target = _target;
  doom.level.things.back()->_tracer = this;
  doom.level.things.back()->A_Fire(doom);

  // Keep reference of current attack fire
  _tracer = doom.level.things.back().get();
}

void    DOOM::AbstractThing::A_Fire(DOOM::Doom& doom)
{
  // Cancel when no target or tracer
  if (_target == nullptr || _tracer == nullptr)
    return;

  // Ceck if vile still has player in sight
  if (_tracer->P_CheckSight(doom, *_target) == false)
    return;

  // Move fire in front of target sight
  Math::Vector<3> dest(
    _target->position.x() + 24.f * std::cos(_target->angle),
    _target->position.y() + 24.f * std::sin(_target->angle),
    _target->position.z()
  );

  if (flags & DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap)
    doom.level.blockmap.moveThing(*this, position.convert<2>(), dest.convert<2>());
  position = dest;
}

void    DOOM::AbstractThing::A_VileAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Cancel if target is not in sight
  if (P_CheckSight(doom, *_target) == false)
    return;

  // Direct attack target
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_barexp, position);
  _target->damage(doom, *this, 20.f);
  _target->thrust(Math::Vector<3>(0.f, 0.f, 1000.f / _target->attributs.mass));

  // Move fire between the vile and the player
  if (_tracer == nullptr)
    return;

  Math::Vector<3> dest(
    _target->position.x() - 24.f * std::cos(angle),
    _target->position.y() - 24.f * std::sin(angle),
    _target->position.z()
  );

  if (flags & DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap)
    doom.level.blockmap.moveThing(*_tracer, _tracer->position.convert<2>(), dest.convert<2>());
  position = dest;

  // Blast damage target
  _tracer->P_RadiusAttack(doom, *this, 70.f);
}
void    DOOM::AbstractThing::A_Tracer(DOOM::Doom& doom)
{
  /*
  angle_t     exact;
  fixed_t     dist;
  fixed_t     slope;
  mobj_t* dest;
  mobj_t* th;

  if (gametic & 3)
    return;

  // spawn a puff of smoke behind the rocket
  P_SpawnPuff(actor->x, actor->y, actor->z);

  th = P_SpawnMobj(actor->x - actor->momx,
    actor->y - actor->momy,
    actor->z, MT_SMOKE);

  th->momz = FRACUNIT;
  th->tics -= P_Random() & 3;
  if (th->tics < 1)
    th->tics = 1;

  // adjust direction
  dest = actor->tracer;

  if (!dest || dest->health <= 0)
    return;

  // change angle
  exact = R_PointToAngle2(actor->x,
    actor->y,
    dest->x,
    dest->y);

  if (exact != actor->angle)
  {
    if (exact - actor->angle > 0x80000000)
    {
      actor->angle -= TRACEANGLE;
      if (exact - actor->angle < 0x80000000)
        actor->angle = exact;
    }
    else
    {
      actor->angle += TRACEANGLE;
      if (exact - actor->angle > 0x80000000)
        actor->angle = exact;
    }
  }

  exact = actor->angle >> ANGLETOFINESHIFT;
  actor->momx = FixedMul(actor->info->speed, finecosine[exact]);
  actor->momy = FixedMul(actor->info->speed, finesine[exact]);

  // change slope
  dist = P_AproxDistance(dest->x - actor->x,
    dest->y - actor->y);

  dist = dist / actor->info->speed;

  if (dist < 1)
    dist = 1;
  slope = (dest->z + 40 * FRACUNIT - actor->z) / dist;

  if (slope < actor->momz)
    actor->momz -= FRACUNIT / 8;
  else
    actor->momz += FRACUNIT / 8;
    */
}

void    DOOM::AbstractThing::A_WeaponReady(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_Lower(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_Raise(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_Punch(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_ReFire(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_FirePistol(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_FireShotgun(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_FireShotgun2(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_CheckReload(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_OpenShotgun2(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_LoadShotgun2(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_CloseShotgun2(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_FireCGun(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_GunFlash(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_FireMissile(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_Saw(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_FirePlasma(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_BFGsound(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_FireBFG(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_BFGSpray(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_BrainAwake(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_BrainSpit(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_Light0(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_Light1(DOOM::Doom& doom) {}
void    DOOM::AbstractThing::A_Light2(DOOM::Doom& doom) {}