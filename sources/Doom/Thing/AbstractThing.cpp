#include <functional>
#include <iostream>
#include <set>

#include "Doom/Doom.hpp"
#include "Doom/Thing/AbstractThing.hpp"
#include "Doom/Thing/PlayerThing.hpp"
#include "System/Audio/Sound.hpp"

const float	DOOM::AbstractThing::MeleeRange = 64.f;
const float	DOOM::AbstractThing::MissileRange = 32.f * 64.f;
const float	DOOM::AbstractThing::FloatSpeed = 4.f;
const float	DOOM::AbstractThing::SkullSpeed = 20.f;
const int	DOOM::AbstractThing::TargetThreshold = 100;
const int	DOOM::AbstractThing::MaxRadius = 32;
const float     DOOM::AbstractThing::FatSpread = Math::Pi / 16.f;
const float     DOOM::AbstractThing::TracerAngle = 12.f / 255.f * 2.f * Math::Pi;

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

const std::array<DOOM::AbstractThing::State, DOOM::AbstractThing::ThingState::State_Number> DOOM::AbstractThing::_states =
{
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_None
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BLUD, .frame = 2, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BLOOD2 }, // State_BLOOD1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BLUD, .frame = 1, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BLOOD3 }, // State_BLOOD2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BLUD, .frame = 0, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_None }, // State_BLOOD3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PUFF, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_PUFF2 }, // State_PUFF1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PUFF, .frame = 1, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_PUFF3 }, // State_PUFF2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PUFF, .frame = 2, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_PUFF4 }, // State_PUFF3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PUFF, .frame = 3, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_None }, // State_PUFF4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL1, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TBALL2 }, // State_TBALL1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL1, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TBALL1 }, // State_TBALL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL1, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TBALLX2 }, // State_TBALLX1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL1, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TBALLX3 }, // State_TBALLX2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL1, .frame = 4, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_None }, // State_TBALLX3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL2, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_RBALL2 }, // State_RBALL1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL2, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_RBALL1 }, // State_RBALL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL2, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_RBALLX2 }, // State_RBALLX1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL2, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_RBALLX3 }, // State_RBALLX2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL2, .frame = 4, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_None }, // State_RBALLX3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLSS, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PLASBALL2 }, // State_PLASBALL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLSS, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PLASBALL }, // State_PLASBALL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLSE, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_PLASEXP2 }, // State_PLASEXP
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLSE, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_PLASEXP3 }, // State_PLASEXP2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLSE, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_PLASEXP4 }, // State_PLASEXP3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLSE, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_PLASEXP5 }, // State_PLASEXP4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLSE, .frame = 4, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_None }, // State_PLASEXP5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 0, .brightness = true, .duration = 1, .action = &DOOM::AbstractThing::A_MissileSmoke, .next = ThingState::State_ROCKET }, // State_ROCKET
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFS1, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BFGSHOT2 }, // State_BFGSHOT
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFS1, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BFGSHOT }, // State_BFGSHOT2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE1, .frame = 0, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_BFGLAND2 }, // State_BFGLAND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE1, .frame = 1, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_BFGLAND3 }, // State_BFGLAND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE1, .frame = 2, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_BFGSpray, .next = ThingState::State_BFGLAND4 }, // State_BFGLAND3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE1, .frame = 3, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_BFGLAND5 }, // State_BFGLAND4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE1, .frame = 4, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_BFGLAND6 }, // State_BFGLAND5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE1, .frame = 5, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_None }, // State_BFGLAND6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE2, .frame = 0, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_BFGEXP2 }, // State_BFGEXP
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE2, .frame = 1, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_BFGEXP3 }, // State_BFGEXP2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE2, .frame = 2, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_BFGEXP4 }, // State_BFGEXP3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFE2, .frame = 3, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_None }, // State_BFGEXP4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 1, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_Explode, .next = ThingState::State_EXPLODE2 }, // State_EXPLODE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_EXPLODE3 }, // State_EXPLODE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_None }, // State_EXPLODE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG01 }, // State_TFOG
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG02 }, // State_TFOG01
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG2 }, // State_TFOG02
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG3 }, // State_TFOG2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG4 }, // State_TFOG3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG5 }, // State_TFOG4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 4, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG6 }, // State_TFOG5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 5, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG7 }, // State_TFOG6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 6, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG8 }, // State_TFOG7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 7, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG9 }, // State_TFOG8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 8, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TFOG10 }, // State_TFOG9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TFOG, .frame = 9, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_None }, // State_TFOG10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_IFOG, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_IFOG01 }, // State_IFOG
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_IFOG, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_IFOG02 }, // State_IFOG01
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_IFOG, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_IFOG2 }, // State_IFOG02
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_IFOG, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_IFOG3 }, // State_IFOG2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_IFOG, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_IFOG4 }, // State_IFOG3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_IFOG, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_IFOG5 }, // State_IFOG4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_IFOG, .frame = 4, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_None }, // State_IFOG5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_PLAY
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 0, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_PLAY_RUN2 }, // State_PLAY_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 1, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_PLAY_RUN3 }, // State_PLAY_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 2, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_PLAY_RUN4 }, // State_PLAY_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 3, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_PLAY_RUN1 }, // State_PLAY_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 4, .brightness = false, .duration = 12, .action = nullptr, .next = ThingState::State_PLAY }, // State_PLAY_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 5, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PLAY_ATK1 }, // State_PLAY_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 6, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_PLAY_PAIN2 }, // State_PLAY_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 6, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_PLAY }, // State_PLAY_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 7, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_PLAY_DIE2 }, // State_PLAY_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 8, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_PlayerScream, .next = ThingState::State_PLAY_DIE3 }, // State_PLAY_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 9, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_PLAY_DIE4 }, // State_PLAY_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 10, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_PLAY_DIE5 }, // State_PLAY_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 11, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_PLAY_DIE6 }, // State_PLAY_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 12, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_PLAY_DIE7 }, // State_PLAY_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 13, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_PLAY_DIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 14, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_PLAY_XDIE2 }, // State_PLAY_XDIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 15, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_XScream, .next = ThingState::State_PLAY_XDIE3 }, // State_PLAY_XDIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 16, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_PLAY_XDIE4 }, // State_PLAY_XDIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 17, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_PLAY_XDIE5 }, // State_PLAY_XDIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 18, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_PLAY_XDIE6 }, // State_PLAY_XDIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 19, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_PLAY_XDIE7 }, // State_PLAY_XDIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 20, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_PLAY_XDIE8 }, // State_PLAY_XDIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 21, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_PLAY_XDIE9 }, // State_PLAY_XDIE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 22, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_PLAY_XDIE9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_POSS_STND2 }, // State_POSS_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_POSS_STND }, // State_POSS_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 0, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_POSS_RUN2 }, // State_POSS_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 0, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_POSS_RUN3 }, // State_POSS_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 1, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_POSS_RUN4 }, // State_POSS_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 1, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_POSS_RUN5 }, // State_POSS_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 2, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_POSS_RUN6 }, // State_POSS_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 2, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_POSS_RUN7 }, // State_POSS_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 3, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_POSS_RUN8 }, // State_POSS_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 3, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_POSS_RUN1 }, // State_POSS_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 4, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_POSS_ATK2 }, // State_POSS_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 5, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_PosAttack, .next = ThingState::State_POSS_ATK3 }, // State_POSS_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 4, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_POSS_RUN1 }, // State_POSS_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 6, .brightness = false, .duration = 3, .action = nullptr, .next = ThingState::State_POSS_PAIN2 }, // State_POSS_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 6, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_POSS_RUN1 }, // State_POSS_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 7, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_DIE2 }, // State_POSS_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 8, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_POSS_DIE3 }, // State_POSS_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 9, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_POSS_DIE4 }, // State_POSS_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_DIE5 }, // State_POSS_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 11, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_POSS_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 12, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_XDIE2 }, // State_POSS_XDIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 13, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_XScream, .next = ThingState::State_POSS_XDIE3 }, // State_POSS_XDIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 14, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_POSS_XDIE4 }, // State_POSS_XDIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 15, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_XDIE5 }, // State_POSS_XDIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 16, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_XDIE6 }, // State_POSS_XDIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 17, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_XDIE7 }, // State_POSS_XDIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 18, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_XDIE8 }, // State_POSS_XDIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 19, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_XDIE9 }, // State_POSS_XDIE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 20, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_POSS_XDIE9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_RAISE2 }, // State_POSS_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 9, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_RAISE3 }, // State_POSS_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 8, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_RAISE4 }, // State_POSS_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POSS, .frame = 7, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_POSS_RUN1 }, // State_POSS_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SPOS_STND2 }, // State_SPOS_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SPOS_STND }, // State_SPOS_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPOS_RUN2 }, // State_SPOS_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPOS_RUN3 }, // State_SPOS_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPOS_RUN4 }, // State_SPOS_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPOS_RUN5 }, // State_SPOS_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPOS_RUN6 }, // State_SPOS_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPOS_RUN7 }, // State_SPOS_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPOS_RUN8 }, // State_SPOS_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPOS_RUN1 }, // State_SPOS_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 4, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SPOS_ATK2 }, // State_SPOS_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 5, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_SPosAttack, .next = ThingState::State_SPOS_ATK3 }, // State_SPOS_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 4, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_SPOS_RUN1 }, // State_SPOS_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 6, .brightness = false, .duration = 3, .action = nullptr, .next = ThingState::State_SPOS_PAIN2 }, // State_SPOS_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 6, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_SPOS_RUN1 }, // State_SPOS_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 7, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_DIE2 }, // State_SPOS_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 8, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_SPOS_DIE3 }, // State_SPOS_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 9, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_SPOS_DIE4 }, // State_SPOS_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_DIE5 }, // State_SPOS_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 11, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SPOS_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 12, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_XDIE2 }, // State_SPOS_XDIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 13, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_XScream, .next = ThingState::State_SPOS_XDIE3 }, // State_SPOS_XDIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 14, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_SPOS_XDIE4 }, // State_SPOS_XDIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 15, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_XDIE5 }, // State_SPOS_XDIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 16, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_XDIE6 }, // State_SPOS_XDIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 17, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_XDIE7 }, // State_SPOS_XDIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 18, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_XDIE8 }, // State_SPOS_XDIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 19, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_XDIE9 }, // State_SPOS_XDIE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 20, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SPOS_XDIE9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_RAISE2 }, // State_SPOS_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_RAISE3 }, // State_SPOS_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 9, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_RAISE4 }, // State_SPOS_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 8, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_RAISE5 }, // State_SPOS_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPOS, .frame = 7, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SPOS_RUN1 }, // State_SPO.next = ThingState::State_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_VILE_STND2 }, // State_VILE_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_VILE_STND }, // State_VILE_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 0, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN2 }, // State_VILE_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 0, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN3 }, // State_VILE_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 1, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN4 }, // State_VILE_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 1, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN5 }, // State_VILE_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 2, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN6 }, // State_VILE_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 2, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN7 }, // State_VILE_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 3, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN8 }, // State_VILE_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 3, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN9 }, // State_VILE_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 4, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN10 }, // State_VILE_RUN9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 4, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN11 }, // State_VILE_RUN10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 5, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN12 }, // State_VILE_RUN11
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 5, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_VileChase, .next = ThingState::State_VILE_RUN1 }, // State_VILE_RUN12
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 6, .brightness = true, .duration = 0, .action = &DOOM::AbstractThing::A_VileStart, .next = ThingState::State_VILE_ATK2 }, // State_VILE_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 6, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_VILE_ATK3 }, // State_VILE_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 7, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_VileTarget, .next = ThingState::State_VILE_ATK4 }, // State_VILE_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 8, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_VILE_ATK5 }, // State_VILE_ATK4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 9, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_VILE_ATK6 }, // State_VILE_ATK5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 10, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_VILE_ATK7 }, // State_VILE_ATK6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 11, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_VILE_ATK8 }, // State_VILE_ATK7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 12, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_VILE_ATK9 }, // State_VILE_ATK8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 13, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_VILE_ATK10 }, // State_VILE_ATK9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 14, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_VileAttack, .next = ThingState::State_VILE_ATK11 }, // State_VILE_ATK10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 15, .brightness = true, .duration = 20, .action = nullptr, .next = ThingState::State_VILE_RUN1 }, // State_VILE_ATK11
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 26, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_VILE_HEAL2 }, // State_VILE_HEAL1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 27, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_VILE_HEAL3 }, // State_VILE_HEAL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 28, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_VILE_RUN1 }, // State_VILE_HEAL3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 16, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_VILE_PAIN2 }, // State_VILE_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 16, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_VILE_RUN1 }, // State_VILE_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 16, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_VILE_DIE2 }, // State_VILE_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 17, .brightness = false, .duration = 7, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_VILE_DIE3 }, // State_VILE_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 18, .brightness = false, .duration = 7, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_VILE_DIE4 }, // State_VILE_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 19, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_VILE_DIE5 }, // State_VILE_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 20, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_VILE_DIE6 }, // State_VILE_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 21, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_VILE_DIE7 }, // State_VILE_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 22, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_VILE_DIE8 }, // State_VILE_DIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 23, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_VILE_DIE9 }, // State_VILE_DIE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 24, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_VILE_DIE10 }, // State_VILE_DIE9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_VILE, .frame = 25, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_VILE_DIE10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 0, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_StartFire, .next = ThingState::State_FIRE2 }, // State_FIRE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 1, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE3 }, // State_FIRE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 0, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE4 }, // State_FIRE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 1, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE5 }, // State_FIRE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 2, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_FireCrackle, .next = ThingState::State_FIRE6 }, // State_FIRE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 1, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE7 }, // State_FIRE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 2, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE8 }, // State_FIRE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 1, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE9 }, // State_FIRE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 2, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE10 }, // State_FIRE9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 3, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE11 }, // State_FIRE10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 2, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE12 }, // State_FIRE11
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 3, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE13 }, // State_FIRE12
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 2, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE14 }, // State_FIRE13
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 3, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE15 }, // State_FIRE14
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 4, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE16 }, // State_FIRE15
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 3, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE17 }, // State_FIRE16
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 4, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE18 }, // State_FIRE17
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 3, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE19 }, // State_FIRE18
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 4, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_FireCrackle, .next = ThingState::State_FIRE20 }, // State_FIRE19
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 5, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE21 }, // State_FIRE20
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 4, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE22 }, // State_FIRE21
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 5, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE23 }, // State_FIRE22
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 4, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE24 }, // State_FIRE23
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 5, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE25 }, // State_FIRE24
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 6, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE26 }, // State_FIRE25
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 7, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE27 }, // State_FIRE26
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 6, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE28 }, // State_FIRE27
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 7, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE29 }, // State_FIRE28
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 6, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_FIRE30 }, // State_FIRE29
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 7, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_None }, // State_FIRE30
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PUFF, .frame = 1, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_SMOKE2 }, // State_SMOKE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PUFF, .frame = 2, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_SMOKE3 }, // State_SMOKE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PUFF, .frame = 1, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_SMOKE4 }, // State_SMOKE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PUFF, .frame = 2, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_SMOKE5 }, // State_SMOKE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PUFF, .frame = 3, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_None }, // State_SMOKE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATB, .frame = 0, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Tracer, .next = ThingState::State_TRACER2 }, // State_TRACER
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATB, .frame = 1, .brightness = true, .duration = 2, .action = &DOOM::AbstractThing::A_Tracer, .next = ThingState::State_TRACER }, // State_TRACER2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FBXP, .frame = 0, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_TRACEEXP2 }, // State_TRACEEXP1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FBXP, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_TRACEEXP3 }, // State_TRACEEXP2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FBXP, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_None }, // State_TRACEEXP3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SKEL_STND2 }, // State_SKEL_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SKEL_STND }, // State_SKEL_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 0, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN2 }, // State_SKEL_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 0, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN3 }, // State_SKEL_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 1, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN4 }, // State_SKEL_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 1, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN5 }, // State_SKEL_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 2, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN6 }, // State_SKEL_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 2, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN7 }, // State_SKEL_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 3, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN8 }, // State_SKEL_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 3, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN9 }, // State_SKEL_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 4, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN10 }, // State_SKEL_RUN9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 4, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN11 }, // State_SKEL_RUN10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 5, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN12 }, // State_SKEL_RUN11
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 5, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKEL_RUN1 }, // State_SKEL_RUN12
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 6, .brightness = false, .duration = 0, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SKEL_FIST2 }, // State_SKEL_FIST1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 6, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_SkelWhoosh, .next = ThingState::State_SKEL_FIST3 }, // State_SKEL_FIST2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 7, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SKEL_FIST4 }, // State_SKEL_FIST3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 8, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_SkelFist, .next = ThingState::State_SKEL_RUN1 }, // State_SKEL_FIST4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 9, .brightness = true, .duration = 0, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SKEL_MISS2 }, // State_SKEL_MISS1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 9, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SKEL_MISS3 }, // State_SKEL_MISS2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 10, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_SkelMissile, .next = ThingState::State_SKEL_MISS4 }, // State_SKEL_MISS3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 10, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SKEL_RUN1 }, // State_SKEL_MISS4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SKEL_PAIN2 }, // State_SKEL_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 11, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_SKEL_RUN1 }, // State_SKEL_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 11, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_SKEL_DIE2 }, // State_SKEL_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 12, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_SKEL_DIE3 }, // State_SKEL_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 13, .brightness = false, .duration = 7, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_SKEL_DIE4 }, // State_SKEL_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 14, .brightness = false, .duration = 7, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_SKEL_DIE5 }, // State_SKEL_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 15, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_SKEL_DIE6 }, // State_SKEL_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 16, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SKEL_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 16, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SKEL_RAISE2 }, // State_SKEL_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 15, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SKEL_RAISE3 }, // State_SKEL_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 14, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SKEL_RAISE4 }, // State_SKEL_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 13, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SKEL_RAISE5 }, // State_SKEL_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 12, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SKEL_RAISE6 }, // State_SKEL_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKEL, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SKEL_RUN1 }, // State_SKEL_RAISE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MANF, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_FATSHOT2 }, // State_FATSHOT1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MANF, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_FATSHOT1 }, // State_FATSHOT2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 1, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_FATSHOTX2 }, // State_FATSHOTX1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_FATSHOTX3 }, // State_FATSHOTX2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_None }, // State_FATSHOTX3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 0, .brightness = false, .duration = 15, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_FATT_STND2 }, // State_FATT_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 1, .brightness = false, .duration = 15, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_FATT_STND }, // State_FATT_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 0, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN2 }, // State_FATT_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 0, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN3 }, // State_FATT_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 1, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN4 }, // State_FATT_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 1, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN5 }, // State_FATT_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 2, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN6 }, // State_FATT_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 2, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN7 }, // State_FATT_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 3, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN8 }, // State_FATT_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 3, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN9 }, // State_FATT_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 4, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN10 }, // State_FATT_RUN9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 4, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN11 }, // State_FATT_RUN10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 5, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN12 }, // State_FATT_RUN11
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 5, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_FATT_RUN1 }, // State_FATT_RUN12
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 6, .brightness = false, .duration = 20, .action = &DOOM::AbstractThing::A_FatRaise, .next = ThingState::State_FATT_ATK2 }, // State_FATT_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 7, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_FatAttack1, .next = ThingState::State_FATT_ATK3 }, // State_FATT_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 8, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_FATT_ATK4 }, // State_FATT_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 6, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_FATT_ATK5 }, // State_FATT_ATK4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 7, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_FatAttack2, .next = ThingState::State_FATT_ATK6 }, // State_FATT_ATK5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 8, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_FATT_ATK7 }, // State_FATT_ATK6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 6, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_FATT_ATK8 }, // State_FATT_ATK7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 7, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_FatAttack3, .next = ThingState::State_FATT_ATK9 }, // State_FATT_ATK8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 8, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_FATT_ATK10 }, // State_FATT_ATK9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 6, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_FATT_RUN1 }, // State_FATT_ATK10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 9, .brightness = false, .duration = 3, .action = nullptr, .next = ThingState::State_FATT_PAIN2 }, // State_FATT_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 9, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_FATT_RUN1 }, // State_FATT_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 10, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_FATT_DIE2 }, // State_FATT_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 11, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_FATT_DIE3 }, // State_FATT_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 12, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_FATT_DIE4 }, // State_FATT_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 13, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_FATT_DIE5 }, // State_FATT_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 14, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_FATT_DIE6 }, // State_FATT_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 15, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_FATT_DIE7 }, // State_FATT_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 16, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_FATT_DIE8 }, // State_FATT_DIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 17, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_FATT_DIE9 }, // State_FATT_DIE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 18, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_FATT_DIE10 }, // State_FATT_DIE9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 19, .brightness = false, .duration = -1, .action = &DOOM::AbstractThing::A_BossDeath, .next = ThingState::State_None }, // State_FATT_DIE10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 17, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_FATT_RAISE2 }, // State_FATT_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 16, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_FATT_RAISE3 }, // State_FATT_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 15, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_FATT_RAISE4 }, // State_FATT_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 14, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_FATT_RAISE5 }, // State_FATT_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 13, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_FATT_RAISE6 }, // State_FATT_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 12, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_FATT_RAISE7 }, // State_FATT_RAISE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_FATT_RAISE8 }, // State_FATT_RAISE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FATT, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_FATT_RUN1 }, // State_FATT_RAISE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_CPOS_STND2 }, // State_CPOS_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_CPOS_STND }, // State_CPOS_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CPOS_RUN2 }, // State_CPOS_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CPOS_RUN3 }, // State_CPOS_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CPOS_RUN4 }, // State_CPOS_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CPOS_RUN5 }, // State_CPOS_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CPOS_RUN6 }, // State_CPOS_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CPOS_RUN7 }, // State_CPOS_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CPOS_RUN8 }, // State_CPOS_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CPOS_RUN1 }, // State_CPOS_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 4, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_CPOS_ATK2 }, // State_CPOS_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 5, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_CPosAttack, .next = ThingState::State_CPOS_ATK3 }, // State_CPOS_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 4, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_CPosAttack, .next = ThingState::State_CPOS_ATK4 }, // State_CPOS_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 5, .brightness = false, .duration = 1, .action = &DOOM::AbstractThing::A_CPosRefire, .next = ThingState::State_CPOS_ATK2 }, // State_CPOS_ATK4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 6, .brightness = false, .duration = 3, .action = nullptr, .next = ThingState::State_CPOS_PAIN2 }, // State_CPOS_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 6, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_CPOS_RUN1 }, // State_CPOS_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 7, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_DIE2 }, // State_CPOS_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 8, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_CPOS_DIE3 }, // State_CPOS_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 9, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_CPOS_DIE4 }, // State_CPOS_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_DIE5 }, // State_CPOS_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_DIE6 }, // State_CPOS_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 12, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_DIE7 }, // State_CPOS_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 13, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_CPOS_DIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 14, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_XDIE2 }, // State_CPOS_XDIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 15, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_XScream, .next = ThingState::State_CPOS_XDIE3 }, // State_CPOS_XDIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 16, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_CPOS_XDIE4 }, // State_CPOS_XDIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 17, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_XDIE5 }, // State_CPOS_XDIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 18, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_XDIE6 }, // State_CPOS_XDIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 19, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_CPOS_XDIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 13, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_RAISE2 }, // State_CPOS_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 12, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_RAISE3 }, // State_CPOS_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_RAISE4 }, // State_CPOS_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_RAISE5 }, // State_CPOS_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 9, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_RAISE6 }, // State_CPOS_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 8, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_RAISE7 }, // State_CPOS_RAISE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CPOS, .frame = 7, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_CPOS_RUN1 }, // State_CPO.next = ThingState::State_RAISE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_TROO_STND2 }, // State_TROO_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_TROO_STND }, // State_TROO_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_TROO_RUN2 }, // State_TROO_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_TROO_RUN3 }, // State_TROO_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_TROO_RUN4 }, // State_TROO_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_TROO_RUN5 }, // State_TROO_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_TROO_RUN6 }, // State_TROO_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_TROO_RUN7 }, // State_TROO_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_TROO_RUN8 }, // State_TROO_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_TROO_RUN1 }, // State_TROO_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 4, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_TROO_ATK2 }, // State_TROO_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 5, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_TROO_ATK3 }, // State_TROO_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 6, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_TroopAttack, .next = ThingState::State_TROO_RUN1 }, // State_TROO_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 7, .brightness = false, .duration = 2, .action = nullptr, .next = ThingState::State_TROO_PAIN2 }, // State_TROO_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 7, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_TROO_RUN1 }, // State_TROO_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 8, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_TROO_DIE2 }, // State_TROO_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 9, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_TROO_DIE3 }, // State_TROO_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 10, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_TROO_DIE4 }, // State_TROO_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 11, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_TROO_DIE5 }, // State_TROO_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 12, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_TROO_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 13, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_TROO_XDIE2 }, // State_TROO_XDIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 14, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_XScream, .next = ThingState::State_TROO_XDIE3 }, // State_TROO_XDIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 15, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_TROO_XDIE4 }, // State_TROO_XDIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 16, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_TROO_XDIE5 }, // State_TROO_XDIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 17, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_TROO_XDIE6 }, // State_TROO_XDIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 18, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_TROO_XDIE7 }, // State_TROO_XDIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 19, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_TROO_XDIE8 }, // State_TROO_XDIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 20, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_TROO_XDIE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 12, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_TROO_RAISE2 }, // State_TROO_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 11, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_TROO_RAISE3 }, // State_TROO_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 10, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_TROO_RAISE4 }, // State_TROO_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 9, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_TROO_RAISE5 }, // State_TROO_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TROO, .frame = 8, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_TROO_RUN1 }, // State_TROO_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SARG_STND2 }, // State_SARG_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SARG_STND }, // State_SARG_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 0, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SARG_RUN2 }, // State_SARG_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 0, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SARG_RUN3 }, // State_SARG_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 1, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SARG_RUN4 }, // State_SARG_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 1, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SARG_RUN5 }, // State_SARG_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 2, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SARG_RUN6 }, // State_SARG_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 2, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SARG_RUN7 }, // State_SARG_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 3, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SARG_RUN8 }, // State_SARG_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 3, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SARG_RUN1 }, // State_SARG_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 4, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SARG_ATK2 }, // State_SARG_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 5, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SARG_ATK3 }, // State_SARG_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 6, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_SargAttack, .next = ThingState::State_SARG_RUN1 }, // State_SARG_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 7, .brightness = false, .duration = 2, .action = nullptr, .next = ThingState::State_SARG_PAIN2 }, // State_SARG_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 7, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_SARG_RUN1 }, // State_SARG_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 8, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_SARG_DIE2 }, // State_SARG_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 9, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_SARG_DIE3 }, // State_SARG_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 10, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_SARG_DIE4 }, // State_SARG_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 11, .brightness = false, .duration = 4, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_SARG_DIE5 }, // State_SARG_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 12, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_SARG_DIE6 }, // State_SARG_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 13, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SARG_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 13, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SARG_RAISE2 }, // State_SARG_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 12, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SARG_RAISE3 }, // State_SARG_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SARG_RAISE4 }, // State_SARG_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SARG_RAISE5 }, // State_SARG_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 9, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SARG_RAISE6 }, // State_SARG_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SARG, .frame = 8, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SARG_RUN1 }, // State_SARG_RAISE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_HEAD_STND }, // State_HEAD_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_HEAD_RUN1 }, // State_HEAD_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 1, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_HEAD_ATK2 }, // State_HEAD_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 2, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_HEAD_ATK3 }, // State_HEAD_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 3, .brightness = true, .duration = 5, .action = &DOOM::AbstractThing::A_HeadAttack, .next = ThingState::State_HEAD_RUN1 }, // State_HEAD_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 4, .brightness = false, .duration = 3, .action = nullptr, .next = ThingState::State_HEAD_PAIN2 }, // State_HEAD_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 4, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_HEAD_PAIN3 }, // State_HEAD_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 5, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_HEAD_RUN1 }, // State_HEAD_PAIN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 6, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_HEAD_DIE2 }, // State_HEAD_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 7, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_HEAD_DIE3 }, // State_HEAD_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 8, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_HEAD_DIE4 }, // State_HEAD_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 9, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_HEAD_DIE5 }, // State_HEAD_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 10, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_HEAD_DIE6 }, // State_HEAD_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 11, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_HEAD_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 11, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_HEAD_RAISE2 }, // State_HEAD_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 10, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_HEAD_RAISE3 }, // State_HEAD_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 9, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_HEAD_RAISE4 }, // State_HEAD_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 8, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_HEAD_RAISE5 }, // State_HEAD_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 7, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_HEAD_RAISE6 }, // State_HEAD_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HEAD, .frame = 6, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_HEAD_RUN1 }, // State_HEAD_RAISE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL7, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BRBALL2 }, // State_BRBALL1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL7, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BRBALL1 }, // State_BRBALL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL7, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_BRBALLX2 }, // State_BRBALLX1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL7, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_BRBALLX3 }, // State_BRBALLX2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAL7, .frame = 4, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_None }, // State_BRBALLX3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_BOSS_STND2 }, // State_BOSS_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_BOSS_STND }, // State_BOSS_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOSS_RUN2 }, // State_BOSS_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOSS_RUN3 }, // State_BOSS_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOSS_RUN4 }, // State_BOSS_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOSS_RUN5 }, // State_BOSS_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOSS_RUN6 }, // State_BOSS_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOSS_RUN7 }, // State_BOSS_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOSS_RUN8 }, // State_BOSS_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOSS_RUN1 }, // State_BOSS_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 4, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_BOSS_ATK2 }, // State_BOSS_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 5, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_BOSS_ATK3 }, // State_BOSS_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 6, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_BruisAttack, .next = ThingState::State_BOSS_RUN1 }, // State_BOSS_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 7, .brightness = false, .duration = 2, .action = nullptr, .next = ThingState::State_BOSS_PAIN2 }, // State_BOSS_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 7, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_BOSS_RUN1 }, // State_BOSS_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 8, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_DIE2 }, // State_BOSS_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 9, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_BOSS_DIE3 }, // State_BOSS_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 10, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_DIE4 }, // State_BOSS_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 11, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_BOSS_DIE5 }, // State_BOSS_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 12, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_DIE6 }, // State_BOSS_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 13, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_DIE7 }, // State_BOSS_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 14, .brightness = false, .duration = -1, .action = &DOOM::AbstractThing::A_BossDeath, .next = ThingState::State_None }, // State_BOSS_DIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 14, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_RAISE2 }, // State_BOSS_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 13, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_RAISE3 }, // State_BOSS_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 12, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_RAISE4 }, // State_BOSS_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 11, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_RAISE5 }, // State_BOSS_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 10, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_RAISE6 }, // State_BOSS_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 9, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_RAISE7 }, // State_BOSS_RAISE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSS, .frame = 8, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOSS_RUN1 }, // State_BOSS_RAISE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_BOS2_STND2 }, // State_BOS2_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_BOS2_STND }, // State_BOS2_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOS2_RUN2 }, // State_BOS2_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOS2_RUN3 }, // State_BOS2_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOS2_RUN4 }, // State_BOS2_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOS2_RUN5 }, // State_BOS2_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOS2_RUN6 }, // State_BOS2_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOS2_RUN7 }, // State_BOS2_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOS2_RUN8 }, // State_BOS2_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BOS2_RUN1 }, // State_BOS2_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 4, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_BOS2_ATK2 }, // State_BOS2_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 5, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_BOS2_ATK3 }, // State_BOS2_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 6, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_BruisAttack, .next = ThingState::State_BOS2_RUN1 }, // State_BOS2_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 7, .brightness = false, .duration = 2, .action = nullptr, .next = ThingState::State_BOS2_PAIN2 }, // State_BOS2_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 7, .brightness = false, .duration = 2, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_BOS2_RUN1 }, // State_BOS2_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 8, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_DIE2 }, // State_BOS2_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 9, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_BOS2_DIE3 }, // State_BOS2_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 10, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_DIE4 }, // State_BOS2_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 11, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_BOS2_DIE5 }, // State_BOS2_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 12, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_DIE6 }, // State_BOS2_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 13, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_DIE7 }, // State_BOS2_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 14, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_BOS2_DIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 14, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_RAISE2 }, // State_BOS2_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 13, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_RAISE3 }, // State_BOS2_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 12, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_RAISE4 }, // State_BOS2_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 11, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_RAISE5 }, // State_BOS2_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 10, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_RAISE6 }, // State_BOS2_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 9, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_RAISE7 }, // State_BOS2_RAISE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOS2, .frame = 8, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BOS2_RUN1 }, // State_BOS2_RAISE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 0, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SKULL_STND2 }, // State_SKULL_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 1, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SKULL_STND }, // State_SKULL_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 0, .brightness = true, .duration = 6, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKULL_RUN2 }, // State_SKULL_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 1, .brightness = true, .duration = 6, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SKULL_RUN1 }, // State_SKULL_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 2, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SKULL_ATK2 }, // State_SKULL_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 3, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_SkullAttack, .next = ThingState::State_SKULL_ATK3 }, // State_SKULL_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_SKULL_ATK4 }, // State_SKULL_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_SKULL_ATK3 }, // State_SKULL_ATK4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 4, .brightness = true, .duration = 3, .action = nullptr, .next = ThingState::State_SKULL_PAIN2 }, // State_SKULL_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 4, .brightness = true, .duration = 3, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_SKULL_RUN1 }, // State_SKULL_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 5, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_SKULL_DIE2 }, // State_SKULL_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 6, .brightness = true, .duration = 6, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_SKULL_DIE3 }, // State_SKULL_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 7, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_SKULL_DIE4 }, // State_SKULL_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 8, .brightness = true, .duration = 6, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_SKULL_DIE5 }, // State_SKULL_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 9, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_SKULL_DIE6 }, // State_SKULL_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SKUL, .frame = 10, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_None }, // State_SKULL_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SPID_STND2 }, // State_SPID_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SPID_STND }, // State_SPID_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Metal, .next = ThingState::State_SPID_RUN2 }, // State_SPID_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPID_RUN3 }, // State_SPID_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPID_RUN4 }, // State_SPID_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPID_RUN5 }, // State_SPID_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Metal, .next = ThingState::State_SPID_RUN6 }, // State_SPID_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPID_RUN7 }, // State_SPID_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPID_RUN8 }, // State_SPID_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPID_RUN9 }, // State_SPID_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 4, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Metal, .next = ThingState::State_SPID_RUN10 }, // State_SPID_RUN9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 4, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPID_RUN11 }, // State_SPID_RUN10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 5, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPID_RUN12 }, // State_SPID_RUN11
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 5, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SPID_RUN1 }, // State_SPID_RUN12
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 0, .brightness = true, .duration = 20, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SPID_ATK2 }, // State_SPID_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 6, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_SPosAttack, .next = ThingState::State_SPID_ATK3 }, // State_SPID_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 7, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_SPosAttack, .next = ThingState::State_SPID_ATK4 }, // State_SPID_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 7, .brightness = true, .duration = 1, .action = &DOOM::AbstractThing::A_SpidRefire, .next = ThingState::State_SPID_ATK2 }, // State_SPID_ATK4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 8, .brightness = false, .duration = 3, .action = nullptr, .next = ThingState::State_SPID_PAIN2 }, // State_SPID_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 8, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_SPID_RUN1 }, // State_SPID_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 9, .brightness = false, .duration = 20, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_SPID_DIE2 }, // State_SPID_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 10, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_SPID_DIE3 }, // State_SPID_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 11, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_SPID_DIE4 }, // State_SPID_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 12, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_SPID_DIE5 }, // State_SPID_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 13, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_SPID_DIE6 }, // State_SPID_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 14, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_SPID_DIE7 }, // State_SPID_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 15, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_SPID_DIE8 }, // State_SPID_DIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 16, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_SPID_DIE9 }, // State_SPID_DIE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 17, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_SPID_DIE10 }, // State_SPID_DIE9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 18, .brightness = false, .duration = 30, .action = nullptr, .next = ThingState::State_SPID_DIE11 }, // State_SPID_DIE10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SPID, .frame = 18, .brightness = false, .duration = -1, .action = &DOOM::AbstractThing::A_BossDeath, .next = ThingState::State_None }, // State_SPID_DIE11
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_BSPI_STND2 }, // State_BSPI_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_BSPI_STND }, // State_BSPI_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 0, .brightness = false, .duration = 20, .action = nullptr, .next = ThingState::State_BSPI_RUN1 }, // State_BSPI_SIGHT
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_BabyMetal, .next = ThingState::State_BSPI_RUN2 }, // State_BSPI_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN3 }, // State_BSPI_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN4 }, // State_BSPI_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN5 }, // State_BSPI_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN6 }, // State_BSPI_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN7 }, // State_BSPI_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_BabyMetal, .next = ThingState::State_BSPI_RUN8 }, // State_BSPI_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN9 }, // State_BSPI_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 4, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN10 }, // State_BSPI_RUN9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 4, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN11 }, // State_BSPI_RUN10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 5, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN12 }, // State_BSPI_RUN11
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 5, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_BSPI_RUN1 }, // State_BSPI_RUN12
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 0, .brightness = true, .duration = 20, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_BSPI_ATK2 }, // State_BSPI_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 6, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_BspiAttack, .next = ThingState::State_BSPI_ATK3 }, // State_BSPI_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 7, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BSPI_ATK4 }, // State_BSPI_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 7, .brightness = true, .duration = 1, .action = &DOOM::AbstractThing::A_SpidRefire, .next = ThingState::State_BSPI_ATK2 }, // State_BSPI_ATK4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 8, .brightness = false, .duration = 3, .action = nullptr, .next = ThingState::State_BSPI_PAIN2 }, // State_BSPI_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 8, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_BSPI_RUN1 }, // State_BSPI_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 9, .brightness = false, .duration = 20, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_BSPI_DIE2 }, // State_BSPI_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 10, .brightness = false, .duration = 7, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_BSPI_DIE3 }, // State_BSPI_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 11, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_BSPI_DIE4 }, // State_BSPI_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 12, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_BSPI_DIE5 }, // State_BSPI_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 13, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_BSPI_DIE6 }, // State_BSPI_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 14, .brightness = false, .duration = 7, .action = nullptr, .next = ThingState::State_BSPI_DIE7 }, // State_BSPI_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 15, .brightness = false, .duration = -1, .action = &DOOM::AbstractThing::A_BossDeath, .next = ThingState::State_None }, // State_BSPI_DIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 15, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_BSPI_RAISE2 }, // State_BSPI_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 14, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_BSPI_RAISE3 }, // State_BSPI_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 13, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_BSPI_RAISE4 }, // State_BSPI_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 12, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_BSPI_RAISE5 }, // State_BSPI_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_BSPI_RAISE6 }, // State_BSPI_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_BSPI_RAISE7 }, // State_BSPI_RAISE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSPI, .frame = 9, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_BSPI_RUN1 }, // State_BSPI_RAISE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_APLS, .frame = 0, .brightness = true, .duration = 5, .action = nullptr, .next = ThingState::State_ARACH_PLAZ2 }, // State_ARACH_PLAZ
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_APLS, .frame = 1, .brightness = true, .duration = 5, .action = nullptr, .next = ThingState::State_ARACH_PLAZ }, // State_ARACH_PLAZ2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_APBX, .frame = 0, .brightness = true, .duration = 5, .action = nullptr, .next = ThingState::State_ARACH_PLEX2 }, // State_ARACH_PLEX
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_APBX, .frame = 1, .brightness = true, .duration = 5, .action = nullptr, .next = ThingState::State_ARACH_PLEX3 }, // State_ARACH_PLEX2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_APBX, .frame = 2, .brightness = true, .duration = 5, .action = nullptr, .next = ThingState::State_ARACH_PLEX4 }, // State_ARACH_PLEX3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_APBX, .frame = 3, .brightness = true, .duration = 5, .action = nullptr, .next = ThingState::State_ARACH_PLEX5 }, // State_ARACH_PLEX4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_APBX, .frame = 4, .brightness = true, .duration = 5, .action = nullptr, .next = ThingState::State_None }, // State_ARACH_PLEX5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_CYBER_STND2 }, // State_CYBER_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_CYBER_STND }, // State_CYBER_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Hoof, .next = ThingState::State_CYBER_RUN2 }, // State_CYBER_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CYBER_RUN3 }, // State_CYBER_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CYBER_RUN4 }, // State_CYBER_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CYBER_RUN5 }, // State_CYBER_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CYBER_RUN6 }, // State_CYBER_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CYBER_RUN7 }, // State_CYBER_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Metal, .next = ThingState::State_CYBER_RUN8 }, // State_CYBER_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_CYBER_RUN1 }, // State_CYBER_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 4, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_CYBER_ATK2 }, // State_CYBER_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 5, .brightness = false, .duration = 12, .action = &DOOM::AbstractThing::A_CyberAttack, .next = ThingState::State_CYBER_ATK3 }, // State_CYBER_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 4, .brightness = false, .duration = 12, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_CYBER_ATK4 }, // State_CYBER_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 5, .brightness = false, .duration = 12, .action = &DOOM::AbstractThing::A_CyberAttack, .next = ThingState::State_CYBER_ATK5 }, // State_CYBER_ATK4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 4, .brightness = false, .duration = 12, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_CYBER_ATK6 }, // State_CYBER_ATK5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 5, .brightness = false, .duration = 12, .action = &DOOM::AbstractThing::A_CyberAttack, .next = ThingState::State_CYBER_RUN1 }, // State_CYBER_ATK6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 6, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_CYBER_RUN1 }, // State_CYBER_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 7, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_CYBER_DIE2 }, // State_CYBER_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 8, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_CYBER_DIE3 }, // State_CYBER_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 9, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_CYBER_DIE4 }, // State_CYBER_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 10, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_CYBER_DIE5 }, // State_CYBER_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 11, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_CYBER_DIE6 }, // State_CYBER_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 12, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_CYBER_DIE7 }, // State_CYBER_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 13, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_CYBER_DIE8 }, // State_CYBER_DIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 14, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_CYBER_DIE9 }, // State_CYBER_DIE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 15, .brightness = false, .duration = 30, .action = nullptr, .next = ThingState::State_CYBER_DIE10 }, // State_CYBER_DIE9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CYBR, .frame = 15, .brightness = false, .duration = -1, .action = &DOOM::AbstractThing::A_BossDeath, .next = ThingState::State_None }, // State_CYBER_DIE10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_PAIN_STND }, // State_PAIN_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_PAIN_RUN2 }, // State_PAIN_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_PAIN_RUN3 }, // State_PAIN_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_PAIN_RUN4 }, // State_PAIN_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_PAIN_RUN5 }, // State_PAIN_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_PAIN_RUN6 }, // State_PAIN_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_PAIN_RUN1 }, // State_PAIN_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 3, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_PAIN_ATK2 }, // State_PAIN_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 4, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_PAIN_ATK3 }, // State_PAIN_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 5, .brightness = true, .duration = 5, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_PAIN_ATK4 }, // State_PAIN_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 5, .brightness = true, .duration = 0, .action = &DOOM::AbstractThing::A_PainAttack, .next = ThingState::State_PAIN_RUN1 }, // State_PAIN_ATK4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 6, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_PAIN_PAIN2 }, // State_PAIN_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 6, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_PAIN_RUN1 }, // State_PAIN_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 7, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_PAIN_DIE2 }, // State_PAIN_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 8, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_PAIN_DIE3 }, // State_PAIN_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 9, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_PAIN_DIE4 }, // State_PAIN_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 10, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_PAIN_DIE5 }, // State_PAIN_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 11, .brightness = true, .duration = 8, .action = &DOOM::AbstractThing::A_PainDie, .next = ThingState::State_PAIN_DIE6 }, // State_PAIN_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 12, .brightness = true, .duration = 8, .action = nullptr, .next = ThingState::State_None }, // State_PAIN_DIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 12, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_PAIN_RAISE2 }, // State_PAIN_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 11, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_PAIN_RAISE3 }, // State_PAIN_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 10, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_PAIN_RAISE4 }, // State_PAIN_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 9, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_PAIN_RAISE5 }, // State_PAIN_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 8, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_PAIN_RAISE6 }, // State_PAIN_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PAIN, .frame = 7, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_PAIN_RUN1 }, // State_PAIN_RAISE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SSWV_STND2 }, // State_SSWV_STND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 1, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_SSWV_STND }, // State_SSWV_STND2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SSWV_RUN2 }, // State_SSWV_RUN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 0, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SSWV_RUN3 }, // State_SSWV_RUN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SSWV_RUN4 }, // State_SSWV_RUN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 1, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SSWV_RUN5 }, // State_SSWV_RUN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SSWV_RUN6 }, // State_SSWV_RUN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 2, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SSWV_RUN7 }, // State_SSWV_RUN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SSWV_RUN8 }, // State_SSWV_RUN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 3, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Chase, .next = ThingState::State_SSWV_RUN1 }, // State_SSWV_RUN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 4, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SSWV_ATK2 }, // State_SSWV_ATK1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 5, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SSWV_ATK3 }, // State_SSWV_ATK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 6, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_CPosAttack, .next = ThingState::State_SSWV_ATK4 }, // State_SSWV_ATK3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 5, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_FaceTarget, .next = ThingState::State_SSWV_ATK5 }, // State_SSWV_ATK4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 6, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_CPosAttack, .next = ThingState::State_SSWV_ATK6 }, // State_SSWV_ATK5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 5, .brightness = false, .duration = 1, .action = &DOOM::AbstractThing::A_CPosRefire, .next = ThingState::State_SSWV_ATK2 }, // State_SSWV_ATK6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 7, .brightness = false, .duration = 3, .action = nullptr, .next = ThingState::State_SSWV_PAIN2 }, // State_SSWV_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 7, .brightness = false, .duration = 3, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_SSWV_RUN1 }, // State_SSWV_PAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 8, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_DIE2 }, // State_SSWV_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 9, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_SSWV_DIE3 }, // State_SSWV_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 10, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_SSWV_DIE4 }, // State_SSWV_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_DIE5 }, // State_SSWV_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 12, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SSWV_DIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 13, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_XDIE2 }, // State_SSWV_XDIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 14, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_XScream, .next = ThingState::State_SSWV_XDIE3 }, // State_SSWV_XDIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 15, .brightness = false, .duration = 5, .action = &DOOM::AbstractThing::A_Fall, .next = ThingState::State_SSWV_XDIE4 }, // State_SSWV_XDIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 16, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_XDIE5 }, // State_SSWV_XDIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 17, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_XDIE6 }, // State_SSWV_XDIE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 18, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_XDIE7 }, // State_SSWV_XDIE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 19, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_XDIE8 }, // State_SSWV_XDIE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 20, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_XDIE9 }, // State_SSWV_XDIE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 21, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SSWV_XDIE9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 12, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_RAISE2 }, // State_SSWV_RAISE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 11, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_RAISE3 }, // State_SSWV_RAISE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 10, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_RAISE4 }, // State_SSWV_RAISE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 9, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_RAISE5 }, // State_SSWV_RAISE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 8, .brightness = false, .duration = 5, .action = nullptr, .next = ThingState::State_SSWV_RUN1 }, // State_SSWV_RAISE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_KEENSTND }, // State_KEENSTND
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 0, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_COMMKEEN2 }, // State_COMMKEEN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 1, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_COMMKEEN3 }, // State_COMMKEEN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 2, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_COMMKEEN4 }, // State_COMMKEEN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 3, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_COMMKEEN5 }, // State_COMMKEEN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 4, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_COMMKEEN6 }, // State_COMMKEEN5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 5, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_COMMKEEN7 }, // State_COMMKEEN6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 6, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_COMMKEEN8 }, // State_COMMKEEN7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 7, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_COMMKEEN9 }, // State_COMMKEEN8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 8, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_COMMKEEN10 }, // State_COMMKEEN9
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 9, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_COMMKEEN11 }, // State_COMMKEEN10
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 10, .brightness = false, .duration = 6, .action = &DOOM::AbstractThing::A_KeenDie, .next = ThingState::State_COMMKEEN12 }, // State_COMMKEEN11
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 11, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_COMMKEEN12
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 12, .brightness = false, .duration = 4, .action = nullptr, .next = ThingState::State_KEENPAIN2 }, // State_KEENPAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_KEEN, .frame = 12, .brightness = false, .duration = 8, .action = &DOOM::AbstractThing::A_Pain, .next = ThingState::State_KEENSTND }, // State_KEENPAIN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BBRN, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_BRAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BBRN, .frame = 1, .brightness = false, .duration = 36, .action = &DOOM::AbstractThing::A_BrainPain, .next = ThingState::State_BRAIN }, // State_BRAIN_PAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BBRN, .frame = 0, .brightness = false, .duration = 100, .action = &DOOM::AbstractThing::A_BrainScream, .next = ThingState::State_BRAIN_DIE2 }, // State_BRAIN_DIE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BBRN, .frame = 0, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_BRAIN_DIE3 }, // State_BRAIN_DIE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BBRN, .frame = 0, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_BRAIN_DIE4 }, // State_BRAIN_DIE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BBRN, .frame = 0, .brightness = false, .duration = -1, .action = &DOOM::AbstractThing::A_BrainDie, .next = ThingState::State_None }, // State_BRAIN_DIE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 0, .brightness = false, .duration = 10, .action = &DOOM::AbstractThing::A_Look, .next = ThingState::State_BRAINEYE }, // State_BRAINEYE
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 0, .brightness = false, .duration = 181, .action = &DOOM::AbstractThing::A_BrainAwake, .next = ThingState::State_BRAINEYE1 }, // State_BRAINEYESEE
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SSWV, .frame = 0, .brightness = false, .duration = 150, .action = &DOOM::AbstractThing::A_BrainSpit, .next = ThingState::State_BRAINEYE1 }, // State_BRAINEYE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSF, .frame = 0, .brightness = true, .duration = 3, .action = &DOOM::AbstractThing::A_SpawnSound, .next = ThingState::State_SPAWN2 }, // State_SPAWN1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSF, .frame = 1, .brightness = true, .duration = 3, .action = &DOOM::AbstractThing::A_SpawnFly, .next = ThingState::State_SPAWN3 }, // State_SPAWN2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSF, .frame = 2, .brightness = true, .duration = 3, .action = &DOOM::AbstractThing::A_SpawnFly, .next = ThingState::State_SPAWN4 }, // State_SPAWN3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BOSF, .frame = 3, .brightness = true, .duration = 3, .action = &DOOM::AbstractThing::A_SpawnFly, .next = ThingState::State_SPAWN1 }, // State_SPAWN4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 0, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_SPAWNFIRE2 }, // State_SPAWNFIRE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 1, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_SPAWNFIRE3 }, // State_SPAWNFIRE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 2, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_SPAWNFIRE4 }, // State_SPAWNFIRE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 3, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_SPAWNFIRE5 }, // State_SPAWNFIRE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 4, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_SPAWNFIRE6 }, // State_SPAWNFIRE5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 5, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_SPAWNFIRE7 }, // State_SPAWNFIRE6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 6, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_SPAWNFIRE8 }, // State_SPAWNFIRE7
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FIRE, .frame = 7, .brightness = true, .duration = 4, .action = &DOOM::AbstractThing::A_Fire, .next = ThingState::State_None }, // State_SPAWNFIRE8
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 1, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_BRAINEXPLODE2 }, // State_BRAINEXPLODE1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 2, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_BRAINEXPLODE3 }, // State_BRAINEXPLODE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MISL, .frame = 3, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_BrainExplode, .next = ThingState::State_None }, // State_BRAINEXPLODE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_ARM1, .frame = 0, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_ARM1A }, // State_ARM1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_ARM1, .frame = 1, .brightness = true, .duration = 7, .action = nullptr, .next = ThingState::State_ARM1 }, // State_ARM1A
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_ARM2, .frame = 0, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_ARM2A }, // State_ARM2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_ARM2, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_ARM2 }, // State_ARM2A
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAR1, .frame = 0, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BAR2 }, // State_BAR1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BAR1, .frame = 1, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BAR1 }, // State_BAR2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BEXP, .frame = 0, .brightness = true, .duration = 5, .action = nullptr, .next = ThingState::State_BEXP2 }, // State_BEXP
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BEXP, .frame = 1, .brightness = true, .duration = 5, .action = &DOOM::AbstractThing::A_Scream, .next = ThingState::State_BEXP3 }, // State_BEXP2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BEXP, .frame = 2, .brightness = true, .duration = 5, .action = nullptr, .next = ThingState::State_BEXP4 }, // State_BEXP3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BEXP, .frame = 3, .brightness = true, .duration = 10, .action = &DOOM::AbstractThing::A_Explode, .next = ThingState::State_BEXP5 }, // State_BEXP4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BEXP, .frame = 4, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_None }, // State_BEXP5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FCAN, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BBAR2 }, // State_BBAR1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FCAN, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BBAR3 }, // State_BBAR2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FCAN, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BBAR1 }, // State_BBAR3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON1, .frame = 0, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON1A }, // State_BON1
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON1, .frame = 1, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON1B }, // State_BON1A
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON1, .frame = 2, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON1C }, // State_BON1B
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON1, .frame = 3, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON1D }, // State_BON1C
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON1, .frame = 2, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON1E }, // State_BON1D
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON1, .frame = 1, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON1 }, // State_BON1E
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON2, .frame = 0, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON2A }, // State_BON2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON2, .frame = 1, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON2B }, // State_BON2A
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON2, .frame = 2, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON2C }, // State_BON2B
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON2, .frame = 3, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON2D }, // State_BON2C
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON2, .frame = 2, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON2E }, // State_BON2D
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BON2, .frame = 1, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BON2 }, // State_BON2E
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BKEY, .frame = 0, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_BKEY2 }, // State_BKEY
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BKEY, .frame = 1, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_BKEY }, // State_BKEY2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_RKEY, .frame = 0, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_RKEY2 }, // State_RKEY
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_RKEY, .frame = 1, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_RKEY }, // State_RKEY2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_YKEY, .frame = 0, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_YKEY2 }, // State_YKEY
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_YKEY, .frame = 1, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_YKEY }, // State_YKEY2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSKU, .frame = 0, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_BSKULL2 }, // State_BSKULL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BSKU, .frame = 1, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_BSKULL }, // State_BSKULL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_RSKU, .frame = 0, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_RSKULL2 }, // State_RSKULL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_RSKU, .frame = 1, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_RSKULL }, // State_RSKULL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_YSKU, .frame = 0, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_YSKULL2 }, // State_YSKULL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_YSKU, .frame = 1, .brightness = true, .duration = 10, .action = nullptr, .next = ThingState::State_YSKULL }, // State_YSKULL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_STIM, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_STIM
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MEDI, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_MEDI
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SOUL, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_SOUL2 }, // State_SOUL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SOUL, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_SOUL3 }, // State_SOUL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SOUL, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_SOUL4 }, // State_SOUL3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SOUL, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_SOUL5 }, // State_SOUL4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SOUL, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_SOUL6 }, // State_SOUL5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SOUL, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_SOUL }, // State_SOUL6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PINV, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PINV2 }, // State_PINV
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PINV, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PINV3 }, // State_PINV2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PINV, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PINV4 }, // State_PINV3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PINV, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PINV }, // State_PINV4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PSTR, .frame = 0, .brightness = true, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_PSTR
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PINS, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PINS2 }, // State_PINS
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PINS, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PINS3 }, // State_PINS2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PINS, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PINS4 }, // State_PINS3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PINS, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PINS }, // State_PINS4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MEGA, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_MEGA2 }, // State_MEGA
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MEGA, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_MEGA3 }, // State_MEGA2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MEGA, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_MEGA4 }, // State_MEGA3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MEGA, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_MEGA }, // State_MEGA4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SUIT, .frame = 0, .brightness = true, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SUIT
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PMAP, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PMAP2 }, // State_PMAP
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PMAP, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PMAP3 }, // State_PMAP2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PMAP, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PMAP4 }, // State_PMAP3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PMAP, .frame = 3, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PMAP5 }, // State_PMAP4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PMAP, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PMAP6 }, // State_PMAP5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PMAP, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PMAP }, // State_PMAP6
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PVIS, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_PVIS2 }, // State_PVIS
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PVIS, .frame = 1, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_PVIS }, // State_PVIS2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CLIP, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_CLIP
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_AMMO, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_AMMO
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_ROCK, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_ROCK
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BROK, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_BROK
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CELL, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_CELL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CELP, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_CELP
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SHEL, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SHEL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SBOX, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SBOX
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BPAK, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_BPAK
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BFUG, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_BFUG
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_MGUN, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_MGUN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CSAW, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_CSAW
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_LAUN, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_LAUN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAS, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_PLAS
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SHOT, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SHOT
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SGN2, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SHOT2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_COLU, .frame = 0, .brightness = true, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_COLU
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMT2, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_STALAG
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_GOR1, .frame = 0, .brightness = false, .duration = 10, .action = nullptr, .next = ThingState::State_BLOODYTWITCH2 }, // State_BLOODYTWITCH
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_GOR1, .frame = 1, .brightness = false, .duration = 15, .action = nullptr, .next = ThingState::State_BLOODYTWITCH3 }, // State_BLOODYTWITCH2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_GOR1, .frame = 2, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_BLOODYTWITCH4 }, // State_BLOODYTWITCH3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_GOR1, .frame = 1, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_BLOODYTWITCH }, // State_BLOODYTWITCH4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 13, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_DEADTORSO
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_PLAY, .frame = 18, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_DEADBOTTOM
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POL2, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_HEADSONSTICK
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POL5, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_GIBS
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POL4, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_HEADONASTICK
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POL3, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_HEADCANDLES2 }, // State_HEADCANDLES
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POL3, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_HEADCANDLES }, // State_HEADCANDLES2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POL1, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_DEADSTICK
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POL6, .frame = 0, .brightness = false, .duration = 6, .action = nullptr, .next = ThingState::State_LIVESTICK2 }, // State_LIVESTICK
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POL6, .frame = 1, .brightness = false, .duration = 8, .action = nullptr, .next = ThingState::State_LIVESTICK }, // State_LIVESTICK2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_GOR2, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_MEAT2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_GOR3, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_MEAT3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_GOR4, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_MEAT4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_GOR5, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_MEAT5
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMIT, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_STALAGTITE
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_COL1, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_TALLGRNCOL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_COL2, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SHRTGRNCOL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_COL3, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_TALLREDCOL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_COL4, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SHRTREDCOL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CAND, .frame = 0, .brightness = true, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_CANDLESTIK
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CBRA, .frame = 0, .brightness = true, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_CANDELABRA
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_COL6, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SKULLCOL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TRE1, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_TORCHTREE
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TRE2, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_BIGTREE
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_ELEC, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_TECHPILLAR
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CEYE, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_EVILEYE2 }, // State_EVILEYE
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CEYE, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_EVILEYE3 }, // State_EVILEYE2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CEYE, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_EVILEYE4 }, // State_EVILEYE3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_CEYE, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_EVILEYE }, // State_EVILEYE4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FSKU, .frame = 0, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_FLOATSKULL2 }, // State_FLOATSKULL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FSKU, .frame = 1, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_FLOATSKULL3 }, // State_FLOATSKULL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_FSKU, .frame = 2, .brightness = true, .duration = 6, .action = nullptr, .next = ThingState::State_FLOATSKULL }, // State_FLOATSKULL3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_COL5, .frame = 0, .brightness = false, .duration = 14, .action = nullptr, .next = ThingState::State_HEARTCOL2 }, // State_HEARTCOL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_COL5, .frame = 1, .brightness = false, .duration = 14, .action = nullptr, .next = ThingState::State_HEARTCOL }, // State_HEARTCOL2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TBLU, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BLUETORCH2 }, // State_BLUETORCH
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TBLU, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BLUETORCH3 }, // State_BLUETORCH2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TBLU, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BLUETORCH4 }, // State_BLUETORCH3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TBLU, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BLUETORCH }, // State_BLUETORCH4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TGRN, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_GREENTORCH2 }, // State_GREENTORCH
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TGRN, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_GREENTORCH3 }, // State_GREENTORCH2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TGRN, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_GREENTORCH4 }, // State_GREENTORCH3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TGRN, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_GREENTORCH }, // State_GREENTORCH4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TRED, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_REDTORCH2 }, // State_REDTORCH
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TRED, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_REDTORCH3 }, // State_REDTORCH2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TRED, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_REDTORCH4 }, // State_REDTORCH3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TRED, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_REDTORCH }, // State_REDTORCH4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMBT, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BTORCHSHRT2 }, // State_BTORCHSHRT
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMBT, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BTORCHSHRT3 }, // State_BTORCHSHRT2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMBT, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BTORCHSHRT4 }, // State_BTORCHSHRT3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMBT, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_BTORCHSHRT }, // State_BTORCHSHRT4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMGT, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_GTORCHSHRT2 }, // State_GTORCHSHRT
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMGT, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_GTORCHSHRT3 }, // State_GTORCHSHRT2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMGT, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_GTORCHSHRT4 }, // State_GTORCHSHRT3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMGT, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_GTORCHSHRT }, // State_GTORCHSHRT4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMRT, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_RTORCHSHRT2 }, // State_RTORCHSHRT
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMRT, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_RTORCHSHRT3 }, // State_RTORCHSHRT2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMRT, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_RTORCHSHRT4 }, // State_RTORCHSHRT3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_SMRT, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_RTORCHSHRT }, // State_RTORCHSHRT4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HDB1, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_HANGNOGUTS
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HDB2, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_HANGBNOBRAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HDB3, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_HANGTLOOKDN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HDB4, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_HANGTSKULL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HDB5, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_HANGTLOOKUP
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_HDB6, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_HANGTNOBRAIN
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POB1, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_COLONGIBS
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_POB2, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_SMALLPOOL
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_BRS1, .frame = 0, .brightness = false, .duration = -1, .action = nullptr, .next = ThingState::State_None }, // State_BRAINSTEM
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TLMP, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TECHLAMP2 }, // State_TECHLAMP
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TLMP, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TECHLAMP3 }, // State_TECHLAMP2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TLMP, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TECHLAMP4 }, // State_TECHLAMP3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TLMP, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TECHLAMP }, // State_TECHLAMP4
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TLP2, .frame = 0, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TECH2LAMP2 }, // State_TECH2LAMP
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TLP2, .frame = 1, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TECH2LAMP3 }, // State_TECH2LAMP2
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TLP2, .frame = 2, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TECH2LAMP4 }, // State_TECH2LAMP3
  DOOM::AbstractThing::State{.sprite = ThingSprite::Sprite_TLP2, .frame = 3, .brightness = true, .duration = 4, .action = nullptr, .next = ThingState::State_TECH2LAMP } // State_TECH2LAMP4
};

const std::array<DOOM::AbstractThing::Attributs, DOOM::Enum::ThingType::ThingType_Number>DOOM::AbstractThing::_attributs =
{
  DOOM::AbstractThing::Attributs{ // Type_PLAYER
    .id = -1,
    .spawnhealth = 100,
    .reactiontime = 0,
    .painchance = 255,
    .speed = 0,
    .radius = 16,
    .height = 56,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_PickUp | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_PLAY,
    .state_see = ThingState::State_PLAY_RUN1,
    .state_pain = ThingState::State_PLAY_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_PLAY_ATK1,
    .state_death = ThingState::State_PLAY_DIE1,
    .state_xdeath = ThingState::State_PLAY_XDIE1,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_plpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_pldeth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_POSSESSED
    .id = 3004,
    .spawnhealth = 20,
    .reactiontime = 8,
    .painchance = 200,
    .speed = 8,
    .radius = 20,
    .height = 56,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_POSS_STND,
    .state_see = ThingState::State_POSS_RUN1,
    .state_pain = ThingState::State_POSS_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_POSS_ATK1,
    .state_death = ThingState::State_POSS_DIE1,
    .state_xdeath = ThingState::State_POSS_XDIE1,
    .state_raise = ThingState::State_POSS_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_posit1,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_podth1,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_posact
  },
  DOOM::AbstractThing::Attributs{ // Type_SHOTGUY
    .id = 9,
    .spawnhealth = 30,
    .reactiontime = 8,
    .painchance = 170,
    .speed = 8,
    .radius = 20,
    .height = 56,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_SPOS_STND,
    .state_see = ThingState::State_SPOS_RUN1,
    .state_pain = ThingState::State_SPOS_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_SPOS_ATK1,
    .state_death = ThingState::State_SPOS_DIE1,
    .state_xdeath = ThingState::State_SPOS_XDIE1,
    .state_raise = ThingState::State_SPOS_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_posit2,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_podth2,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_posact
  },
  DOOM::AbstractThing::Attributs{ // Type_VILE
    .id = 64,
    .spawnhealth = 700,
    .reactiontime = 8,
    .painchance = 10,
    .speed = 15,
    .radius = 20,
    .height = 56,
    .mass = 500,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_VILE_STND,
    .state_see = ThingState::State_VILE_RUN1,
    .state_pain = ThingState::State_VILE_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_VILE_ATK1,
    .state_death = ThingState::State_VILE_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_vilsit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_vipain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_vildth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_vilact
  },
  DOOM::AbstractThing::Attributs{ // Type_FIRE
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_FIRE1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_UNDEAD
    .id = 66,
    .spawnhealth = 300,
    .reactiontime = 8,
    .painchance = 100,
    .speed = 10,
    .radius = 20,
    .height = 66,
    .mass = 500,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_SKEL_STND,
    .state_see = ThingState::State_SKEL_RUN1,
    .state_pain = ThingState::State_SKEL_PAIN,
    .state_melee = ThingState::State_SKEL_FIST1,
    .state_missile = ThingState::State_SKEL_MISS1,
    .state_death = ThingState::State_SKEL_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_SKEL_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_skesit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_skedth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_skeact
  },
  DOOM::AbstractThing::Attributs{ // Type_TRACER
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 10,
    .radius = 11,
    .height = 8,
    .mass = 100,
    .damage = 10,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_TRACER,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_TRACEEXP1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_skeatk,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_barexp,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_SMOKE
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 0,
    .height = 1,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_SMOKE1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_FATSO
    .id = 67,
    .spawnhealth = 600,
    .reactiontime = 8,
    .painchance = 80,
    .speed = 8,
    .radius = 48,
    .height = 64,
    .mass = 1000,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_FATT_STND,
    .state_see = ThingState::State_FATT_RUN1,
    .state_pain = ThingState::State_FATT_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_FATT_ATK1,
    .state_death = ThingState::State_FATT_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_FATT_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_mansit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_mnpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_mandth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_posact
  },
  DOOM::AbstractThing::Attributs{ // Type_FATSHOT
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 20,
    .radius = 6,
    .height = 8,
    .mass = 100,
    .damage = 8,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_FATSHOT1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_FATSHOTX1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_firsht,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_CHAINGUY
    .id = 65,
    .spawnhealth = 70,
    .reactiontime = 8,
    .painchance = 170,
    .speed = 8,
    .radius = 20,
    .height = 56,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_CPOS_STND,
    .state_see = ThingState::State_CPOS_RUN1,
    .state_pain = ThingState::State_CPOS_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_CPOS_ATK1,
    .state_death = ThingState::State_CPOS_DIE1,
    .state_xdeath = ThingState::State_CPOS_XDIE1,
    .state_raise = ThingState::State_CPOS_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_posit2,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_podth2,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_posact
  },
  DOOM::AbstractThing::Attributs{ // Type_TROOP
    .id = 3001,
    .spawnhealth = 60,
    .reactiontime = 8,
    .painchance = 200,
    .speed = 8,
    .radius = 20,
    .height = 56,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_TROO_STND,
    .state_see = ThingState::State_TROO_RUN1,
    .state_pain = ThingState::State_TROO_PAIN,
    .state_melee = ThingState::State_TROO_ATK1,
    .state_missile = ThingState::State_TROO_ATK1,
    .state_death = ThingState::State_TROO_DIE1,
    .state_xdeath = ThingState::State_TROO_XDIE1,
    .state_raise = ThingState::State_TROO_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_bgsit1,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_bgdth1,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_bgact
  },
  DOOM::AbstractThing::Attributs{ // Type_SERGEANT
    .id = 3002,
    .spawnhealth = 150,
    .reactiontime = 8,
    .painchance = 180,
    .speed = 10,
    .radius = 30,
    .height = 56,
    .mass = 400,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_SARG_STND,
    .state_see = ThingState::State_SARG_RUN1,
    .state_pain = ThingState::State_SARG_PAIN,
    .state_melee = ThingState::State_SARG_ATK1,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_SARG_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_SARG_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtsit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtatk,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtdth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact
  },
  DOOM::AbstractThing::Attributs{ // Type_SHADOWS
    .id = 58,
    .spawnhealth = 150,
    .reactiontime = 8,
    .painchance = 180,
    .speed = 10,
    .radius = 30,
    .height = 56,
    .mass = 400,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Shadow | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_SARG_STND,
    .state_see = ThingState::State_SARG_RUN1,
    .state_pain = ThingState::State_SARG_PAIN,
    .state_melee = ThingState::State_SARG_ATK1,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_SARG_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_SARG_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtsit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtatk,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_sgtdth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact
  },
  DOOM::AbstractThing::Attributs{ // Type_HEAD
    .id = 3005,
    .spawnhealth = 400,
    .reactiontime = 8,
    .painchance = 128,
    .speed = 8,
    .radius = 31,
    .height = 56,
    .mass = 400,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_NoGravity | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_HEAD_STND,
    .state_see = ThingState::State_HEAD_RUN1,
    .state_pain = ThingState::State_HEAD_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_HEAD_ATK1,
    .state_death = ThingState::State_HEAD_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_HEAD_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_cacsit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_cacdth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact
  },
  DOOM::AbstractThing::Attributs{ // Type_BRUISER
    .id = 3003,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 50,
    .speed = 8,
    .radius = 24,
    .height = 64,
    .mass = 1000,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_BOSS_STND,
    .state_see = ThingState::State_BOSS_RUN1,
    .state_pain = ThingState::State_BOSS_PAIN,
    .state_melee = ThingState::State_BOSS_ATK1,
    .state_missile = ThingState::State_BOSS_ATK1,
    .state_death = ThingState::State_BOSS_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_BOSS_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_brssit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_brsdth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact
  },
  DOOM::AbstractThing::Attributs{ // Type_BRUISERSHOT
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 15,
    .radius = 6,
    .height = 8,
    .mass = 100,
    .damage = 8,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_BRBALL1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_BRBALLX1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_firsht,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_KNIGHT
    .id = 69,
    .spawnhealth = 500,
    .reactiontime = 8,
    .painchance = 50,
    .speed = 8,
    .radius = 24,
    .height = 64,
    .mass = 1000,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_BOS2_STND,
    .state_see = ThingState::State_BOS2_RUN1,
    .state_pain = ThingState::State_BOS2_PAIN,
    .state_melee = ThingState::State_BOS2_ATK1,
    .state_missile = ThingState::State_BOS2_ATK1,
    .state_death = ThingState::State_BOS2_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_BOS2_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_kntsit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_kntdth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact
  },
  DOOM::AbstractThing::Attributs{ // Type_SKULL
    .id = 3006,
    .spawnhealth = 100,
    .reactiontime = 8,
    .painchance = 256,
    .speed = 8,
    .radius = 16,
    .height = 56,
    .mass = 50,
    .damage = 3,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_SKULL_STND,
    .state_see = ThingState::State_SKULL_RUN1,
    .state_pain = ThingState::State_SKULL_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_SKULL_ATK1,
    .state_death = ThingState::State_SKULL_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_sklatk,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact
  },
  DOOM::AbstractThing::Attributs{ // Type_SPIDER
    .id = 7,
    .spawnhealth = 3000,
    .reactiontime = 8,
    .painchance = 40,
    .speed = 12,
    .radius = 128,
    .height = 100,
    .mass = 1000,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_SPID_STND,
    .state_see = ThingState::State_SPID_RUN1,
    .state_pain = ThingState::State_SPID_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_SPID_ATK1,
    .state_death = ThingState::State_SPID_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_spisit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_shotgn,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_spidth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact
  },
  DOOM::AbstractThing::Attributs{ // Type_BABY
    .id = 68,
    .spawnhealth = 500,
    .reactiontime = 8,
    .painchance = 128,
    .speed = 12,
    .radius = 64,
    .height = 64,
    .mass = 600,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_BSPI_STND,
    .state_see = ThingState::State_BSPI_SIGHT,
    .state_pain = ThingState::State_BSPI_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_BSPI_ATK1,
    .state_death = ThingState::State_BSPI_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_BSPI_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_bspsit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_bspdth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_bspact
  },
  DOOM::AbstractThing::Attributs{ // Type_CYBORG
    .id = 16,
    .spawnhealth = 4000,
    .reactiontime = 8,
    .painchance = 20,
    .speed = 16,
    .radius = 40,
    .height = 110,
    .mass = 1000,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_CYBER_STND,
    .state_see = ThingState::State_CYBER_RUN1,
    .state_pain = ThingState::State_CYBER_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_CYBER_ATK1,
    .state_death = ThingState::State_CYBER_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_cybsit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmpain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_cybdth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact
  },
  DOOM::AbstractThing::Attributs{ // Type_PAIN
    .id = 71,
    .spawnhealth = 400,
    .reactiontime = 8,
    .painchance = 128,
    .speed = 8,
    .radius = 31,
    .height = 56,
    .mass = 400,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_NoGravity | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_PAIN_STND,
    .state_see = ThingState::State_PAIN_RUN1,
    .state_pain = ThingState::State_PAIN_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_PAIN_ATK1,
    .state_death = ThingState::State_PAIN_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_PAIN_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_pesit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_pepain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_pedth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_dmact
  },
  DOOM::AbstractThing::Attributs{ // Type_WOLFSS
    .id = 84,
    .spawnhealth = 50,
    .reactiontime = 8,
    .painchance = 170,
    .speed = 8,
    .radius = 20,
    .height = 56,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_SSWV_STND,
    .state_see = ThingState::State_SSWV_RUN1,
    .state_pain = ThingState::State_SSWV_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_SSWV_ATK1,
    .state_death = ThingState::State_SSWV_DIE1,
    .state_xdeath = ThingState::State_SSWV_XDIE1,
    .state_raise = ThingState::State_SSWV_RAISE1,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_sssit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_popain,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_ssdth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_posact
  },
  DOOM::AbstractThing::Attributs{ // Type_KEEN
    .id = 72,
    .spawnhealth = 100,
    .reactiontime = 8,
    .painchance = 256,
    .speed = 0,
    .radius = 16,
    .height = 72,
    .mass = 10000000,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_CountKill),

    .state_spawn = ThingState::State_KEENSTND,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_KEENPAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_COMMKEEN,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_keenpn,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_keendt,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_BOSSBRAIN
    .id = 88,
    .spawnhealth = 250,
    .reactiontime = 8,
    .painchance = 255,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 10000000,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable),

    .state_spawn = ThingState::State_BRAIN,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_BRAIN_PAIN,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_BRAIN_DIE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_bospn,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_bosdth,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_BOSSSPIT
    .id = 89,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 64,
    .height = 32,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector),

    .state_spawn = ThingState::State_BRAINEYE,
    .state_see = ThingState::State_BRAINEYESEE,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_BOSSTARGET
    .id = 87,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 32,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector),

    .state_spawn = ThingState::State_None,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_SPAWNSHOT
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 10,
    .radius = 6,
    .height = 32,
    .mass = 100,
    .damage = 3,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity | DOOM::Enum::ThingProperty::ThingProperty_NoClip),

    .state_spawn = ThingState::State_SPAWN1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_bospit,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_SPAWNFIRE
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_SPAWNFIRE1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_BARREL
    .id = 2035,
    .spawnhealth = 20,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 10,
    .height = 42,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_NoBlood),

    .state_spawn = ThingState::State_BAR1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_BEXP,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_barexp,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_TROOPSHOT
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 10,
    .radius = 6,
    .height = 8,
    .mass = 100,
    .damage = 3,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_TBALL1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_TBALLX1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_firsht,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_HEADSHOT
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 10,
    .radius = 6,
    .height = 8,
    .mass = 100,
    .damage = 5,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_RBALL1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_RBALLX1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_firsht,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_ROCKET
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 20,
    .radius = 11,
    .height = 8,
    .mass = 100,
    .damage = 20,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_ROCKET,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_EXPLODE1,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_rlaunc,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_barexp,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_PLASMA
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 25,
    .radius = 13,
    .height = 8,
    .mass = 100,
    .damage = 5,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_PLASBALL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_PLASEXP,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_plasma,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_BFG
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 25,
    .radius = 13,
    .height = 8,
    .mass = 100,
    .damage = 100,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_BFGSHOT,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_BFGLAND,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_rxplod,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_ARACHPLAZ
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 25,
    .radius = 13,
    .height = 8,
    .mass = 100,
    .damage = 5,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_DropOff | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_ARACH_PLAZ,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_ARACH_PLEX,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_plasma,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_firxpl,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_PUFF
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_PUFF1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_BLOOD
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap),

    .state_spawn = ThingState::State_BLOOD1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_TFOG
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_TFOG,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_IFOG
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_IFOG,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_TELEPORTMAN
    .id = 14,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector),

    .state_spawn = ThingState::State_None,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_EXTRABFG
    .id = -1,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_BFGEXP,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC0
    .id = 2018,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_ARM1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC1
    .id = 2019,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_ARM2,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC2
    .id = 2014,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),

    .state_spawn = ThingState::State_BON1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC3
    .id = 2015,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),

    .state_spawn = ThingState::State_BON2,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC4
    .id = 5,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_BKEY,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC5
    .id = 13,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_RKEY,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC6
    .id = 6,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_YKEY,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC7
    .id = 39,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_YSKULL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC8
    .id = 38,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_RSKULL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC9
    .id = 40,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_BSKULL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC10
    .id = 2011,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_STIM,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC11
    .id = 2012,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_MEDI,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC12
    .id = 2013,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),

    .state_spawn = ThingState::State_SOUL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_INV
    .id = 2022,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),

    .state_spawn = ThingState::State_PINV,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC13
    .id = 2023,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),

    .state_spawn = ThingState::State_PSTR,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_INS
    .id = 2024,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),

    .state_spawn = ThingState::State_PINS,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC14
    .id = 2025,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_SUIT,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC15
    .id = 2026,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),

    .state_spawn = ThingState::State_PMAP,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC16
    .id = 2045,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),

    .state_spawn = ThingState::State_PVIS,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MEGA
    .id = 83,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special | DOOM::Enum::ThingProperty::ThingProperty_CountItem),

    .state_spawn = ThingState::State_MEGA,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_CLIP
    .id = 2007,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_CLIP,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC17
    .id = 2048,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_AMMO,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC18
    .id = 2010,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_ROCK,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC19
    .id = 2046,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_BROK,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC20
    .id = 2047,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_CELL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC21
    .id = 17,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_CELP,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC22
    .id = 2008,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_SHEL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC23
    .id = 2049,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_SBOX,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC24
    .id = 8,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_BPAK,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC25
    .id = 2006,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_BFUG,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_CHAINGUN
    .id = 2002,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_MGUN,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC26
    .id = 2005,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_CSAW,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC27
    .id = 2003,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_LAUN,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC28
    .id = 2004,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_PLAS,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_SHOTGUN
    .id = 2001,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_SHOT,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_SUPERSHOTGUN
    .id = 82,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Special),

    .state_spawn = ThingState::State_SHOT2,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC29
    .id = 85,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_TECHLAMP,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC30
    .id = 86,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_TECH2LAMP,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC31
    .id = 2028,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_COLU,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC32
    .id = 30,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_TALLGRNCOL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC33
    .id = 31,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_SHRTGRNCOL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC34
    .id = 32,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_TALLREDCOL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC35
    .id = 33,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_SHRTREDCOL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC36
    .id = 37,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_SKULLCOL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC37
    .id = 36,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_HEARTCOL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC38
    .id = 41,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_EVILEYE,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC39
    .id = 42,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_FLOATSKULL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC40
    .id = 43,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_TORCHTREE,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC41
    .id = 44,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_BLUETORCH,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC42
    .id = 45,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_GREENTORCH,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC43
    .id = 46,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_REDTORCH,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC44
    .id = 55,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_BTORCHSHRT,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC45
    .id = 56,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_GTORCHSHRT,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC46
    .id = 57,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_RTORCHSHRT,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC47
    .id = 47,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_STALAGTITE,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC48
    .id = 48,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_TECHPILLAR,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC49
    .id = 34,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_CANDLESTIK,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC50
    .id = 35,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_CANDELABRA,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC51
    .id = 49,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 68,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_BLOODYTWITCH,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC52
    .id = 50,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 84,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_MEAT2,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC53
    .id = 51,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 84,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_MEAT3,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC54
    .id = 52,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 68,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_MEAT4,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC55
    .id = 53,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 52,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_MEAT5,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC56
    .id = 59,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 84,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_MEAT2,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC57
    .id = 60,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 68,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_MEAT4,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC58
    .id = 61,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 52,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_MEAT3,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC59
    .id = 62,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 52,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_MEAT5,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC60
    .id = 63,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 68,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_BLOODYTWITCH,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC61
    .id = 22,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_HEAD_DIE6,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC62
    .id = 15,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_PLAY_DIE7,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC63
    .id = 18,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_POSS_DIE5,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC64
    .id = 21,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_SARG_DIE6,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC65
    .id = 23,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_SKULL_DIE6,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC66
    .id = 20,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_TROO_DIE5,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC67
    .id = 19,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_SPOS_DIE5,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC68
    .id = 10,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_PLAY_XDIE9,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC69
    .id = 12,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_PLAY_XDIE9,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC70
    .id = 28,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_HEADSONSTICK,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC71
    .id = 24,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_None),

    .state_spawn = ThingState::State_GIBS,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC72
    .id = 27,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_HEADONASTICK,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC73
    .id = 29,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_HEADCANDLES,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC74
    .id = 25,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_DEADSTICK,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC75
    .id = 26,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_LIVESTICK,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC76
    .id = 54,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 32,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_BIGTREE,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC77
    .id = 70,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid),

    .state_spawn = ThingState::State_BBAR1,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC78
    .id = 73,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 88,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_HANGNOGUTS,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC79
    .id = 74,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 88,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_HANGBNOBRAIN,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC80
    .id = 75,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 64,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_HANGTLOOKDN,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC81
    .id = 76,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 64,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_HANGTSKULL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC82
    .id = 77,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 64,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_HANGTLOOKUP,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC83
    .id = 78,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 16,
    .height = 64,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_Solid | DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling | DOOM::Enum::ThingProperty::ThingProperty_NoGravity),

    .state_spawn = ThingState::State_HANGTNOBRAIN,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC84
    .id = 79,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap),

    .state_spawn = ThingState::State_COLONGIBS,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC85
    .id = 80,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap),

    .state_spawn = ThingState::State_SMALLPOOL,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_MISC86
    .id = 81,
    .spawnhealth = 1000,
    .reactiontime = 8,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap),

    .state_spawn = ThingState::State_BRAINSTEM,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_PLAYER_SPAWN1
    .id = 1,
    .spawnhealth = 1000,
    .reactiontime = 0,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_None,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_PLAYER_SPAWN2
    .id = 2,
    .spawnhealth = 1000,
    .reactiontime = 0,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_None,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_PLAYER_SPAWN3
    .id = 3,
    .spawnhealth = 1000,
    .reactiontime = 0,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_None,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_PLAYER_SPAWN4
    .id = 4,
    .spawnhealth = 1000,
    .reactiontime = 0,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_None,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  },
  DOOM::AbstractThing::Attributs{ // Type_PLAYER_SPAWNDM
    .id = 11,
    .spawnhealth = 1000,
    .reactiontime = 0,
    .painchance = 0,
    .speed = 0,
    .radius = 20,
    .height = 16,
    .mass = 100,
    .damage = 0,
    .properties = (DOOM::Enum::ThingProperty)(DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap | DOOM::Enum::ThingProperty::ThingProperty_NoSector | DOOM::Enum::ThingProperty::ThingProperty_NoTDMatch),

    .state_spawn = ThingState::State_None,
    .state_see = ThingState::State_None,
    .state_pain = ThingState::State_None,
    .state_melee = ThingState::State_None,
    .state_missile = ThingState::State_None,
    .state_death = ThingState::State_None,
    .state_xdeath = ThingState::State_None,
    .state_raise = ThingState::State_None,

    .sound_see = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_attack = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_pain = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_death = DOOM::Doom::Resources::Sound::EnumSound::Sound_None,
    .sound_active = DOOM::Doom::Resources::Sound::EnumSound::Sound_None
  }
};

const std::array<Math::Vector<2>, DOOM::AbstractThing::Direction::DirectionNumber>  DOOM::AbstractThing::_directions =
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

DOOM::AbstractThing::AbstractThing(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Thing& thing) :
  DOOM::AbstractThing(
    doom,
    id_to_type(thing.type),
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
  _reactiontime(doom.skill == DOOM::Enum::Skill::SkillNightmare ? 0 : attributs.reactiontime),
  _move_direction(Direction::DirectionNone),
  _move_count(0),
  _remove(false),
  _thrust(0.f, 0.f, 0.f),
  _gravity((this->flags & DOOM::Enum::ThingProperty::ThingProperty_NoGravity) ? 0.f : -1.f),
  _state(attributs.state_spawn),
  _elapsed(0.f),
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

  std::set<int16_t> sectors = doom.level.getSectors(*this);
  float             floor = std::numeric_limits<int16_t>().min();
  float             ceiling = std::numeric_limits<int16_t>().max();

  // Get spawn floor and ceiling height
  if (sectors.empty() == true)
    floor = 0.f;
  else
    for (int16_t sector : sectors) {
      floor = std::max(floor, doom.level.sectors[sector].floor_current);
      ceiling = std::min(ceiling, doom.level.sectors[sector].ceiling_current);
    }

  // Set thing spawn Z position
  position.z() = ((this->flags & DOOM::Enum::ThingProperty::ThingProperty_SpawnCeiling) ? ceiling - height : floor);
}

DOOM::Enum::ThingType DOOM::AbstractThing::id_to_type(int16_t id)
{
  // Find index of thing with given id
  for (int index = 0; index < _attributs.size(); index++)
    if (_attributs[index].id == id)
      return (DOOM::Enum::ThingType)index;

  // Id not found
  throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

int16_t               DOOM::AbstractThing::type_to_id(DOOM::Enum::ThingType type)
{
  // Return WAD id of given type
  return _attributs[type].id;
}

bool  DOOM::AbstractThing::update(DOOM::Doom& doom, float elapsed)
{
  // Update state of thing
  updateState(doom, elapsed);

  // Update physics of thing
  updatePhysics(doom, elapsed);

  // Return remove flag
  return _remove;
}

void  DOOM::AbstractThing::updateState(DOOM::Doom& doom, float elapsed)
{
  // Update internal timer
  _elapsed += elapsed;

  while (_elapsed >= DOOM::Doom::Tic * _states[_state].duration)
  {
    // Stop when no duration
    if (_states[_state].duration == -1) {
      _elapsed = 0.f;
      break;
    }

    // Skip to next state
    _elapsed -= DOOM::Doom::Tic * _states[_state].duration;
    setState(doom, _states[_state].next);
  }
}

std::unique_ptr<DOOM::AbstractThing>  DOOM::AbstractThing::factory(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing)
{
  // Create a basic thing from WAD thing
  // NOTE: AbstractThing isn't really abstract anymore
  return std::make_unique<DOOM::AbstractThing>(doom, thing);
}

bool  DOOM::AbstractThing::teleport(DOOM::Doom& doom, const Math::Vector<2>& destination, float orientation, bool telefrag, bool silent)
{
  // Missiles don't teleport
  if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
    return false;

  // Telefrag this on landing or cancel jump
  for (const auto& thing : doom.level.getThings(destination, (float)attributs.radius)) {
    if (thing.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable) {
      if (telefrag == true && &thing.get() != this)
        thing.get().damage(doom, 10000.f);
      else
        return false;
    }
  }

  // Spawn teleport fog at start
  if (silent == false) {
    doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, DOOM::Enum::ThingType::ThingType_TFOG, DOOM::Enum::ThingFlag::FlagNone, position.x(), position.y(), 0.f));
    doom.level.things.back()->position.z() = position.z();
    doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_telept, doom.level.things.back()->position);
  }

  // Cancel thrust
  _thrust = Math::Vector<3>();

  // Update thing position and angle
  if (!(attributs.properties & DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap))
    doom.level.blockmap.moveThing(*this, position.convert<2>(), destination);
  position.convert<2>() = destination;
  angle = orientation;

  // Update thing vertical position
  // NOTE: handle floating objects ?
  position.z() = doom.level.sectors[doom.level.getSector(destination).first].floor_current;

  // Spawn teleport fog at end
  if (silent == false) {
    doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, DOOM::Enum::ThingType::ThingType_TFOG, DOOM::Enum::ThingFlag::FlagNone, position.x() + std::cos(angle) * 20.f, position.y() + std::sin(angle) * 20.f, 0.f));
    doom.level.things.back()->position.z() = position.z();
    doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_telept, doom.level.things.back()->position);
  }

  // Wait after teleport
  _reactiontime = 18;
  _move_count = 0;

  return true;
}

void  DOOM::AbstractThing::thrust(const Math::Vector<3> & acceleration)
{
  // Apply thrust vector to player based on acceleration and elapsed time (is 3.125 a magic number ?)
  _thrust += acceleration / (float)attributs.mass * 3.125f;
}

DOOM::AbstractThing::Sprite DOOM::AbstractThing::sprite(const DOOM::Doom & doom, float angle) const
{
  // Return a default empty texture if no state
  if (_state == DOOM::AbstractThing::ThingState::State_None)
    return { DOOM::Doom::Resources::Texture::Null, false, false };

  auto  iterator = doom.resources.animations.find(Game::Utilities::str_to_key<uint64_t>(_sprites[_states[_state].sprite]));

  // Cancel if sequence or frame not found
  if (iterator == doom.resources.animations.cend() || iterator->second.size() <= _states[_state].frame)
    return { DOOM::Doom::Resources::Texture::Null, false, false };

  const auto& texture = iterator->second[_states[_state].frame][Math::Modulo<8>((int)((std::fmod(angle, Math::Pi * 2.f) + Math::Pi * 2.f) * 4.f / Math::Pi + 16.5f))];

  return { texture.first.get(), texture.second, _states[_state].brightness };
}

void  DOOM::AbstractThing::A_SpawnFly(DOOM::Doom& doom)
{
  // Wait end of reaction time
  _reactiontime += -1;
  if (_reactiontime != 0)
    return;

  // Spawn teleport fog
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, DOOM::Enum::ThingType::ThingType_SPAWNFIRE, DOOM::Enum::ThingFlag::FlagNone, _target->position.x(), _target->position.y(), 0.f));
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_telept, _target->position);

  static const std::array<std::pair<int, DOOM::Enum::ThingType>, 11>  monsters =
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
  int                   r = std::rand() % 256;
  DOOM::Enum::ThingType type = std::find_if(monsters.cbegin(), monsters.cend(), [r](const std::pair<int, DOOM::Enum::ThingType>& monster) { return (r % 256) < monster.first; })->second;

  // Spawn new monster
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, type, DOOM::Enum::ThingFlag::FlagNone, _target->position.x(), _target->position.y(), 0.f));
  if (doom.level.things.back()->P_LookForPlayers(doom, true) == true)
    doom.level.things.back()->setState(doom, doom.level.things.back()->attributs.state_see);

  // Telefrag anything in this spot
  doom.level.things.back()->teleport(doom, doom.level.things.back()->position.convert<2>(), doom.level.things.back()->angle, true, true);

  // Remove seft (spawn cube)
  setState(doom, State_None);
}

void  DOOM::AbstractThing::A_BruisAttack(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::A_CyberAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Range attack
  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_ROCKET);
}

void  DOOM::AbstractThing::A_HeadAttack(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::A_TroopAttack(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::A_BspiAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);
    
  // Launch a missile
  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_ARACHPLAZ);
}

void  DOOM::AbstractThing::setState(DOOM::Doom & doom, DOOM::AbstractThing::ThingState state)
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

void  DOOM::AbstractThing::updatePhysics(DOOM::Doom& doom, float elapsed)
{
  // Compute physics if minimal thrust
  if (_thrust.convert<2>().length() > 0.001f)
  {
    // Compute movement with collision
    updatePhysicsThrust(doom, elapsed);

    // Apply friction slowdown to thing (except missiles and attacking flying skulls) for next tic (hard coded drag factor of 0.90625)
    if (_thrust.z() == 0 && !(flags & (DOOM::Enum::ThingProperty::ThingProperty_Missile | DOOM::Enum::ThingProperty::ThingProperty_SkullFly)))
      _thrust.convert<2>() *= std::pow(0.90625f, elapsed / DOOM::Doom::Tic);
  }

  // Special case for lost souls
  else if (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) {
    flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_SkullFly);
    setState(doom, attributs.state_see);
  }

  // Update gravity
  updatePhysicsGravity(doom, elapsed);
}

void  DOOM::AbstractThing::updatePhysicsThrust(DOOM::Doom& doom, float elapsed, int depth, int16_t linedef_ignored, const DOOM::AbstractThing* thing_ignored)
{
  // NOTE: glitch might happen if radius > 128
  // NOTE: we are using bounding circle instead of square

  // Limit movement to 30 units per tics
  Math::Vector<2> movement = ((_thrust.convert<2>().length() > 30.f) ? (_thrust.convert<2>() * 30.f / _thrust.convert<2>().length()) : _thrust.convert<2>())* elapsed / DOOM::Doom::Tic;

  // Stop if maximum recursion depth reach
  if (depth > 4 || (movement.x() == 0.f && movement.y() == 0.f)) {
    _thrust.convert<2>() = Math::Vector<2>(0.f, 0.f);
    return;
  }

  // Get intersectable linedefs and things
  std::pair<std::set<int16_t>, std::set<std::reference_wrapper<DOOM::AbstractThing>>> linedefs_things = updatePhysicsThrustLinedefsThings(doom, movement);

  int16_t               closest_linedef = -1;
  DOOM::AbstractThing*  closest_thing = nullptr;
  float                 closest_distance = 1.f;
  Math::Vector<2>       closest_normal = Math::Vector<2>();

  // Check collision with linedefs
  for (int16_t linedef_index : linedefs_things.first) {
    std::pair<float, Math::Vector<2>> intersection = updatePhysicsThrustLinedef(doom, movement, linedef_index, linedef_ignored);

    // Get nearest linedef
    if (intersection.first < closest_distance && !(flags & DOOM::Enum::ThingProperty::ThingProperty_NoClip)) {
      closest_linedef = linedef_index;
      closest_thing = nullptr;
      closest_distance = intersection.first;
      closest_normal = intersection.second;
    }
  }

  // Check collision with things
  if ((flags & DOOM::Enum::ThingProperty::ThingProperty_Solid) || (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile))
    for (const std::reference_wrapper<DOOM::AbstractThing>& thing : linedefs_things.second) {
      // Thing has already been removed
      if (thing.get()._remove == true)
        continue;

      std::pair<float, Math::Vector<2>> intersection = updatePhysicsThrustThing(doom, movement, thing.get(), thing_ignored);

      // Ignore missile emitter
      if ((flags & DOOM::Enum::ThingProperty::ThingProperty_Missile) && &thing.get() == _target)
        continue;

      // Get nearest thing
      if (intersection.first < closest_distance && !(flags & DOOM::Enum::ThingProperty::ThingProperty_NoClip)) {
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

    DOOM::AbstractLinedef&  linedef = *doom.level.linedefs[linedef_index].get();

    // Compute intersection of movement with linedef
    std::pair<float, float> intersection = Math::intersection(
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
    Math::Vector<2> destination = position.convert<2>() + movement * closest_distance;
    if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_NoBlockmap))
      doom.level.blockmap.moveThing(*this, position.convert<2>(), destination);
    position.convert<2>() = destination;
  }

  // Re-compute movement if obstacle found
  if (closest_linedef != -1 || closest_thing != nullptr) {
    Math::Vector<2> closest_direction = Math::Vector<2>(+closest_normal.y(), -closest_normal.x());

    // Slide against currently collisioned walls/things (change movement and thrust)
    _thrust.convert<2>() = closest_direction / closest_direction.length() * _thrust.convert<2>().length() * Math::Vector<2>::cos(_thrust.convert<2>(), closest_direction);

    // Reset lost soul when colliding something
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) {
      // Damage collided shootable
      if (closest_thing != nullptr && closest_thing->flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable)
        closest_thing->damage(doom, *this, (float)((std::rand() % 8 + 1) * attributs.damage));
      flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_SkullFly);
      setState(doom, attributs.state_spawn);
    }

    // Explode if thing is a missile
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile) {
      if (closest_thing != nullptr)
        P_ExplodeMissile(doom, *closest_thing);
      else {
        const DOOM::AbstractLinedef& linedef = *doom.level.linedefs[closest_linedef].get();

        // Remove missile if colliding the sky
        if (linedef.back == -1 ||
          (doom.level.sectors[doom.level.sidedefs[linedef.front].sector].floor_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1") ||
            doom.level.sectors[doom.level.sidedefs[linedef.back].sector].floor_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1")) &&
          (doom.level.sectors[doom.level.sidedefs[linedef.front].sector].ceiling_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1") ||
            doom.level.sectors[doom.level.sidedefs[linedef.back].sector].ceiling_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1")))
          P_ExplodeMissile(doom);
        else
          setState(doom, DOOM::AbstractThing::ThingState::State_None);
      }
    }

    // Attempt new move, ignoring collided linedef/thing
    else
      updatePhysicsThrust(doom, elapsed * (1.f - closest_distance), depth + 1, closest_linedef, closest_thing);
  }
}

bool  DOOM::AbstractThing::updatePhysicsThrustSidedefs(DOOM::Doom& doom, int16_t sidedef_front_index, int16_t sidedef_back_index)
{
  // Force movement if thing outside of the map
  if (sidedef_front_index == -1)
    return true;

  // Can't move outside of the map
  if (sidedef_back_index == -1)
    return false;

  DOOM::Doom::Level::Sidedef& sidedef_front = doom.level.sidedefs[sidedef_front_index];
  DOOM::Doom::Level::Sidedef& sidedef_back = doom.level.sidedefs[sidedef_back_index];

  // Only missile can move through middle texture
  if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_Missile) && sidedef_front.middle().width != 0)
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

std::pair<float, Math::Vector<2>> DOOM::AbstractThing::updatePhysicsThrustVertex(DOOM::Doom& doom, const Math::Vector<2>& movement, int16_t vertex_index, int16_t ignored_index)
{
  // Check if vertex should be ignored
  if (ignored_index != -1) {
    DOOM::AbstractLinedef& ignored = *doom.level.linedefs[ignored_index];

    if (ignored.start == vertex_index || ignored.end == vertex_index)
      return { 1.f, Math::Vector<2>() };
  }

  DOOM::Doom::Level::Vertex vertex = doom.level.vertexes[vertex_index];
  Math::Vector<2>           normal = position.convert<2>() - vertex;

  // Ignore intersection if moving "outside" of vertex
  if (Math::Vector<2>::cos(movement, normal) > 0.f)
    return { 1.f, Math::Vector<2>() };

  // Check if vertex is already in bounding box
  if (normal.length() <= attributs.radius)
    return { 0.f, normal / normal.length() };

  // Intersect bounding circle with vertex
  float a = Math::Pow<2>(movement.x()) + Math::Pow<2>(movement.y());
  float b = -2.f * ((vertex.x() - position.x()) * movement.x() + (vertex.y() - position.y()) * movement.y());
  float c = Math::Pow<2>(vertex.x() - position.x()) + Math::Pow<2>(vertex.y() - position.y()) - Math::Pow<2>((float)attributs.radius);
  float delta = Math::Pow<2>(b) - 4.f * a * c;

  // No intersection found
  if (delta <= 0.f)
    return { 1.f, Math::Vector<2>() };

  // Compute intersections
  float s0 = (-b - std::sqrt(delta)) / (2.f * a);
  float s1 = (-b + std::sqrt(delta)) / (2.f * a);

  // Return smaller solution
  if (s0 >= 0.f && s0 < 1.f)
    return { s0, normal / normal.length() };
  else if (s1 >= 0.f && s1 < 1.f)
    return { s1, normal / normal.length() };
  else
    return { 1.f, Math::Vector<2>() };
}

std::pair<float, Math::Vector<2>> DOOM::AbstractThing::updatePhysicsThrustLinedef(DOOM::Doom& doom, const Math::Vector<2>& movement, int16_t linedef_index, int16_t ignored_index)
{
  // Check if linedef is ignored
  if (linedef_index == ignored_index)
    return { 1.f, Math::Vector<2>() };

  const DOOM::AbstractLinedef&      linedef = *doom.level.linedefs[linedef_index];
  const DOOM::Doom::Level::Vertex&  linedef_start = doom.level.vertexes[linedef.start];
  const DOOM::Doom::Level::Vertex&  linedef_end = doom.level.vertexes[linedef.end];

  Math::Vector<2> linedef_direction = linedef_end - linedef_start;
  Math::Vector<2> linedef_normal(+linedef_direction.y(), -linedef_direction.x());
  int16_t         sidedef_front_index = linedef.front;
  int16_t         sidedef_back_index = linedef.back;

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
  if (((flags & DOOM::Enum::ThingProperty::ThingProperty_Missile) ||
    (!(linedef.flag & DOOM::AbstractLinedef::Flag::Impassible) &&
    !((linedef.flag & DOOM::AbstractLinedef::Flag::BlockMonsters) && (flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable) && type != DOOM::Enum::ThingType::ThingType_PLAYER))) &&
    updatePhysicsThrustSidedefs(doom, sidedef_front_index, sidedef_back_index) == true)
    return { 1.f, Math::Vector<2>() };

  // Get intersection of thing bounding sphere and linedef line
  std::pair<float, float> intersection = Math::intersection(
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

std::pair<float, Math::Vector<2>> DOOM::AbstractThing::updatePhysicsThrustThing(DOOM::Doom& doom, const Math::Vector<2>& movement, const DOOM::AbstractThing& thing, const DOOM::AbstractThing* ignored)
{
  // Check if linedef is ignored
  if (&thing == this || &thing == ignored || (thing.flags & DOOM::Enum::ThingProperty::ThingProperty_Solid) == 0)
    return { 1.f, Math::Vector<2>() };

  Math::Vector<2> initial(position.convert<2>() - thing.position.convert<2>());

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

  float a = Math::Pow<2>(movement.x()) + Math::Pow<2>(movement.y());
  float b = 2.f * (movement.x() * (position.x() - thing.position.x()) + movement.y() * (position.y() - thing.position.y()));
  float c = Math::Pow<2>(position.x() - thing.position.x()) + Math::Pow<2>(position.y() - thing.position.y()) + Math::Pow<2>((float)(attributs.radius + thing.attributs.radius));
  float delta = Math::Pow<2>(b) - 4.f * a * c;

  // No intersection found
  if (delta <= 0.f)
    return { 1.f, Math::Vector<2>() };

  // Compute intersections
  float s0 = (-b - std::sqrt(delta)) / (2.f * a);
  float s1 = (-b + std::sqrt(delta)) / (2.f * a);

  // Return smaller solution
  if (s0 >= 0.f && s0 < 1.f) {
    Math::Vector<2> normal = position.convert<2>() + movement * s0 - thing.position.convert<2>();

    return { s0, normal / normal.length() };
  }
  else if (s1 >= 0.f && s1 < 1.f) {
    Math::Vector<2> normal = position.convert<2>() + movement * s1 - thing.position.convert<2>();

    return { s1, normal / normal.length() };
  }
  else
    return { 1.f, Math::Vector<2>() };
}

std::pair<std::set<int16_t>, std::set<std::reference_wrapper<DOOM::AbstractThing>>> DOOM::AbstractThing::updatePhysicsThrustLinedefsThings(DOOM::Doom& doom, const Math::Vector<2>& movement)
{
  std::set<int16_t> blocks;

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

  std::pair<std::set<int16_t>, std::set<std::reference_wrapper<DOOM::AbstractThing>>> linedefs_things;

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

void  DOOM::AbstractThing::updatePhysicsGravity(DOOM::Doom& doom, float elapsed)
{
  std::set<int16_t> sectors = doom.level.getSectors(*this);
  float             floor = std::numeric_limits<int16_t>().min();
  float             ceiling = std::numeric_limits<int16_t>().max();

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
  _thrust.z() += _gravity / DOOM::Doom::Tic * elapsed;

  // Apply friction to vertical thrust for non attacking fly skulls
  if (type == DOOM::Enum::ThingType::ThingType_SKULL && !(flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly))
    _thrust.z() *= std::pow(0.90625f, elapsed / DOOM::Doom::Tic);

  // Remember thing speed
  float speed = _thrust.z();

  // Missile collision with wall/sky
  const auto collideMissile = [this, &doom](uint64_t flat) {
    if (flat == Game::Utilities::str_to_key<uint64_t>("F_SKY1"))
      setState(doom, DOOM::AbstractThing::ThingState::State_None);
    else
      P_ExplodeMissile(doom);
  };

  // Raise thing if below the floor
  if (position.z() < floor) {
    position.z() = std::min(floor, position.z() + std::max(_thrust.z(), (floor - position.z()) / 2.f + 3.2f) / DOOM::Doom::Tic * elapsed);
    _thrust.z() = std::max(_thrust.z(), (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) ? +std::abs(_thrust.z()) : 0.f);

    // Explode missile if colliding with floor
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
      collideMissile(doom.level.sectors[doom.level.getSector(position.convert<2>()).first].floor_name);
  }
  // Lower thing is upper than the ceiling (limit to floor)
  else if (position.z() > ceiling - height && position.z() > floor) {
    position.z() = std::max(std::max(ceiling - height, floor), position.z() + std::min(_thrust.z(), ((ceiling - attributs.height) - position.z()) / 2.f + 2.f) / DOOM::Doom::Tic * elapsed);
    _thrust.z() = std::min(_thrust.z(), (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) ? -std::abs(_thrust.z()) : 0.f);

    // Explode missile if colliding with ceiling
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
      collideMissile(doom.level.sectors[doom.level.getSector(position.convert<2>()).first].ceiling_name);
  }

  // Normal gravity
  else if (_thrust.z() < 0.f) {
    position.z() = std::max(floor, position.z() + _thrust.z() / DOOM::Doom::Tic * elapsed);
    if (position.z() == floor) {
      _thrust.z() = (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) ? +std::abs(_thrust.z()) : 0.f;

      // Explode missile if colliding with floor
      if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
        collideMissile(doom.level.sectors[doom.level.getSector(position.convert<2>()).first].floor_name);
    }
  }
  // Reverse gravity
  else if (_thrust.z() > 0.f) {
    position.z() = std::min(std::max(ceiling - height, floor), position.z() + _thrust.z() / DOOM::Doom::Tic * elapsed);
    if (position.z() == ceiling - height) {
      _thrust.z() = (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly) ? -std::abs(_thrust.z()) : 0.f;

      // Explode missile if colliding with floor
      if (flags & DOOM::Enum::ThingProperty::ThingProperty_Missile)
        collideMissile(doom.level.sectors[doom.level.getSector(position.convert<2>()).first].ceiling_name);
    }
  }

  // Player make a sound when colliding the floor/ceiling
  if (type == DOOM::Enum::ThingType::ThingType_PLAYER && std::abs(speed - _thrust.z()) > 9.f)
    doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_oof, position);
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

void  DOOM::AbstractThing::A_Chase(DOOM::Doom& doom)
{
    // Decrement reaction time
    if (_reactiontime > 0)
      _reactiontime += -1;

    // Update target threshold
    if (_target_threshold > 0) {
      if (_target == nullptr || _target->health <= 0)
        _target_threshold = 0;
      else
        _target_threshold += -1;
    }

    // Turn toward movement direction
    if (_move_direction != Direction::DirectionNone) {
      angle = Math::Modulo(angle, 2.f * Math::Pi);

      int thing_direction = Math::Modulo<Direction::DirectionNumber>((int)(0.5f + angle / (2.f * Math::Pi / (float)Direction::DirectionNumber)));

      if (thing_direction != _move_direction) {
        if (Math::Modulo<Direction::DirectionNumber>(thing_direction - _move_direction) < Direction::DirectionNumber / 2)
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
     (doom.skill == DOOM::Enum::Skill::SkillNightmare || _move_count == 0) &&
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
    _move_count += -1;
    if (_move_count < 0 || P_Move(doom) == false)
      P_NewChaseDir(doom);

    // Make active sound
    if (std::rand() % 256 < 3)
      doom.sound(attributs.sound_active, position);
}

void  DOOM::AbstractThing::A_MissileSmoke(DOOM::Doom& doom)
{
  // Spawn smoke behind the missile
  P_SpawnPuff(doom, position - _thrust / _thrust.length() * (float)attributs.radius + Math::Vector<3>(Math::Random() * 2.f - 1.f, Math::Random() * 2.f - 1.f, Math::Random() * 2.f - 1.f) * 4.f * Math::Random());
}

bool  DOOM::AbstractThing::P_LookForPlayers(DOOM::Doom& doom, bool full)
{
  // NOTE: do not reset target, might cause crash
  // Reset target
  // _target = nullptr;
  // _target_threshold = 0;

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

bool  DOOM::AbstractThing::P_CheckSight(DOOM::Doom& doom, const DOOM::AbstractThing& target)
{
  // Test if an attack angle is available
  return !std::isnan(P_AimLineAttack(doom, target));
}

bool  DOOM::AbstractThing::P_CheckMeleeRange(DOOM::Doom& doom)
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

bool  DOOM::AbstractThing::P_CheckMissileRange(DOOM::Doom& doom)
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
  if (_reactiontime > 0)
    return false;

  // The probability of firing depends on the distance, the closer we are the higher the chance of firing.
  float distance = (position.convert<2>() - _target->position.convert<2>()).length();

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

void  DOOM::AbstractThing::P_NewChaseDir(DOOM::Doom& doom)
{
  // Error if no target
  if (_target == nullptr)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  DOOM::AbstractThing::Direction  move_old = _move_direction;
  DOOM::AbstractThing::Direction  move_opposite = (DOOM::AbstractThing::Direction)(_move_direction == DOOM::AbstractThing::Direction::DirectionNone ? DOOM::AbstractThing::Direction::DirectionNone : ((_move_direction + DOOM::AbstractThing::Direction::DirectionNumber / 2) % DOOM::AbstractThing::Direction::DirectionNumber));
  Math::Vector<2>                 delta = _target->position.convert<2>() - position.convert<2>();

  DOOM::AbstractThing::Direction  move_x = DOOM::AbstractThing::Direction::DirectionNone;
  DOOM::AbstractThing::Direction  move_y = DOOM::AbstractThing::Direction::DirectionNone;

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
    _move_direction = move_x == Direction::DirectionEast ?
      (move_y == Direction::DirectionNorth ? Direction::DirectionNorthEast : Direction::DirectionSouthEast) :
      (move_y == Direction::DirectionNorth ? Direction::DirectionNorthWest : Direction::DirectionSouthWest);

    if (_move_direction != move_opposite && P_TryWalk(doom) == true) {
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
    _move_direction = move_x;
    if (P_TryWalk(doom) == true) {
      return;
    }
  }
  if (move_y != Direction::DirectionNone) {
    _move_direction = move_y;
    if (P_TryWalk(doom) == true) {
      return;
    }
  }

  // There is no direct path to the player, so pick another direction
  if (move_old != Direction::DirectionNone) {
    _move_direction = move_old;
    if (P_TryWalk(doom) == true) {
      return;
    }
  }

  // Randomly determine direction of search
  if (std::rand() % 2 == 0) {
    for (Direction dir = Direction::DirectionEast; dir <= Direction::DirectionSouthEast; dir = (Direction)(dir + 1)) {
      if (dir != move_opposite) {
        _move_direction = dir;
        if (P_TryWalk(doom) == true) {
          return;
        }
      }
    }
  }
  else {
    for (Direction dir = Direction::DirectionSouthEast; dir >= Direction::DirectionEast; dir = (Direction)(dir - 1)) {
      if (dir != move_opposite) {
        _move_direction = dir;
        if (P_TryWalk(doom) == true) {
          return;
        }
      }
    }
  }

  // Lastly, try the opposite direction
  if (move_opposite != Direction::DirectionNone) {
    _move_direction = move_opposite;
    if (P_TryWalk(doom) == true) {
      return;
    }
  }

  // We cannot move
  _move_direction = Direction::DirectionNone;
}

bool  DOOM::AbstractThing::P_TryWalk(DOOM::Doom& doom)
{
  // Attempt move
  if (P_Move(doom) == true) {
    _move_count = std::rand() % 16;
    return true;
  }

  return false;
}

bool  DOOM::AbstractThing::P_TryMove(DOOM::Doom& doom, const Math::Vector<2>& position)
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

  auto old_position = this->position.convert<2>();
  
  // Walkover linedef
  for (const std::pair<float, int16_t>& linedef_index : doom.level.getLinedefs(old_position, position - old_position))
    doom.level.linedefs[linedef_index.second]->walkover(doom, *this);

  // Move thing
  if (this->position.convert<2>() == old_position)
    this->position.convert<2>() = position;

  return true;
}

bool  DOOM::AbstractThing::P_CheckPosition(DOOM::Doom& doom, const Math::Vector<2>& position)
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
    const DOOM::AbstractLinedef&  linedef = *doom.level.linedefs[linedef_index];

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

bool  DOOM::AbstractThing::P_Move(DOOM::Doom& doom)
{
  // Cancel if no direction
  if (_move_direction == Direction::DirectionNone) {
    return false;
  }

  Math::Vector<2> move_position = position.convert<2>() + (_directions[_move_direction] * (float)attributs.speed);

  if (P_TryMove(doom, move_position) == false) {
    // Floating things
    if ((flags & DOOM::Enum::ThingProperty::ThingProperty_Float) && P_CheckPosition(doom, move_position) == true) {
      float target_floor = std::numeric_limits<float>::lowest();
      float target_ceiling = std::numeric_limits<float>::max();

      // Find target floor and ceiling height
      for (int16_t sector_index : doom.level.getSectors(position.convert<2>() + (_directions[_move_direction] * (float)attributs.speed), attributs.radius / 2.f)) {
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

    _move_direction = Direction::DirectionNone;
    
    // Try to open a door
    bool  switched = false;
    for (int16_t linedef_index : doom.level.getLinedefs(move_position, (float)attributs.radius))
      switched |= doom.level.linedefs[linedef_index]->switched(doom, *this);
    
    return switched;
  }
  else {
    flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_InFloat);
  }

  return true;
}

void  DOOM::AbstractThing::A_Hoof(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_hoof, position);
  A_Chase(doom);
}

void  DOOM::AbstractThing::A_Scream(DOOM::Doom& doom)
{
  DOOM::Doom::Resources::Sound::EnumSound sound;

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

void  DOOM::AbstractThing::A_VileStart(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_vilatk, position);
}

void  DOOM::AbstractThing::A_StartFire(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_flamst, position);
  A_Fire(doom);
}

void  DOOM::AbstractThing::A_FireCrackle(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_flame, position);
  A_Fire(doom);
}

void  DOOM::AbstractThing::A_CPosRefire(DOOM::Doom& doom)
{
  // Keep firing unless target got out of sight
  A_FaceTarget(doom);

  if (std::rand() % 256 < 40)
    return;

  if (_target == nullptr || _target->health <= 0 || P_CheckSight(doom, *_target) == false)
    setState(doom, attributs.state_see);
}

void  DOOM::AbstractThing::A_Metal(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_metal, position);
  A_Chase(doom);
}

void  DOOM::AbstractThing::A_SpidRefire(DOOM::Doom& doom)
{
  A_FaceTarget(doom);

  if (std::rand() % 256 < 10)
    return;

  if (_target == nullptr || _target->health <= 0 || P_CheckSight(doom, *_target) == false)
    setState(doom, attributs.state_see);
}

void  DOOM::AbstractThing::A_BabyMetal(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_bspwlk, position);
  A_Chase(doom);
}

void  DOOM::AbstractThing::A_FaceTarget(DOOM::Doom& doom)
{
  // Stop if no target
  if (_target == nullptr)
    return;

  // Remove ambush flag
  flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_Ambush);

  angle = Math::Vector<2>::angle(_target->position.convert<2>() - position.convert<2>());

  // If thing is invisible, randomize angle
  if (flags & DOOM::Enum::ThingProperty::ThingProperty_Shadow)
    angle += (Math::Random() * 2.f - 1.f) * Math::Pi / 4.f;
}

void  DOOM::AbstractThing::A_SkelWhoosh(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Whoosh!
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_skeswg, position);
}

void  DOOM::AbstractThing::A_PlayerScream(DOOM::Doom& doom)
{
  // If the player dies with less than -50% without gibbing, special sound
  doom.sound(health < -50 ? DOOM::Doom::Resources::Sound::EnumSound::Sound_pdiehi : DOOM::Doom::Resources::Sound::EnumSound::Sound_pldeth, position);
}

void  DOOM::AbstractThing::A_SpawnSound(DOOM::Doom& doom)
{
  // Travelling cube sound
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_boscub, position);
  A_SpawnFly(doom);
}

void  DOOM::AbstractThing::A_Fall(DOOM::Doom& doom)
{
  // Thing is on ground, it can be walked over
  flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_Solid);
}

void  DOOM::AbstractThing::A_Pain(DOOM::Doom& doom)
{
  doom.sound(attributs.sound_pain, position);
}

void  DOOM::AbstractThing::A_XScream(DOOM::Doom& doom)
{
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_slop, position);
}

void  DOOM::AbstractThing::A_PosAttack(DOOM::Doom& doom)
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

  atk_slope += (Math::Random() - Math::Random()) * Math::Pi / 8.f;

  float atk_angle = angle + (Math::Random() - Math::Random()) * Math::Pi / 8.f;
  float atk_damage = (float)((std::rand() % 5 + 1) * 3);

  // Attack
  P_LineAttack(
    doom,
    AbstractThing::MissileRange,
    Math::Vector<3>(position.x(), position.y(), position.z() + height / 2.f),
    Math::Vector<3>(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope)),
    atk_damage);
}

void  DOOM::AbstractThing::A_SPosAttack(DOOM::Doom& doom)
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
    float               atk_angle = angle + (Math::Random() - Math::Random()) * Math::Pi / 8.f;
    float               atk_slope = slope + (Math::Random() - Math::Random()) * Math::Pi / 8.f;
    Math::Vector<3>     atk_shot(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope));
    float               atk_damage = (float)((std::rand() % 5 + 1) * 3);

    // Attack
    P_LineAttack(doom, AbstractThing::MissileRange, position + Math::Vector<3>(0.f, 0.f, height * 0.5f), atk_shot, atk_damage);
  }
}

void  DOOM::AbstractThing::A_CPosAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Face target
  A_FaceTarget(doom);

  // Play pistol sound
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_shotgn, position);

  float atk_angle = angle + (Math::Random() - Math::Random()) * Math::Pi / 8.f;
  float atk_damage = (float)((std::rand() % 5 + 1) * 3);
  float target_height = P_AimLineAttack(doom, *_target);
  float atk_slope = (std::isnan(target_height) == true ? 0.f : std::atan((target_height - (position.z() + height * 0.5f)) / (_target->position.convert<2>() - position.convert<2>()).length())) + (Math::Random() - Math::Random()) * Math::Pi / 8.f;

  // Attack
  P_LineAttack(doom, AbstractThing::MissileRange, Math::Vector<3>(position.x(), position.y(), position.z() + height / 2.f), Math::Vector<3>(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope)), atk_damage);
}

float DOOM::AbstractThing::P_AimLineAttack(DOOM::Doom& doom, const DOOM::AbstractThing& target)
{
  float target_bottom = target.position.z();
  float target_top = target.position.z() + target.height;

  // Check every linedefs between thing and target
  for (const std::pair<float, int16_t>& linedef_index : doom.level.getLinedefs(position.convert<2>(), target.position.convert<2>() - position.convert<2>())) {
    DOOM::AbstractLinedef& linedef = *doom.level.linedefs[linedef_index.second];

    // NOTE: we can see through impassible walls
    
    // Can't see outside the map
    if (linedef.front == -1 || linedef.back == -1)
      return std::numeric_limits<float>::quiet_NaN();

    const float sector_bottom = std::max(doom.level.sectors[doom.level.sidedefs[linedef.front].sector].floor_current, linedef.back != -1 ? doom.level.sectors[doom.level.sidedefs[linedef.back].sector].floor_current : std::numeric_limits<float>::lowest());
    const float sector_top = std::min(doom.level.sectors[doom.level.sidedefs[linedef.front].sector].ceiling_current, linedef.back != -1 ? doom.level.sectors[doom.level.sidedefs[linedef.back].sector].ceiling_current : std::numeric_limits<float>::max());
    
    target_bottom = std::max(target_bottom, (sector_bottom - (position.z() + 0.75f * height)) / linedef_index.first + (position.z() + 0.75f * attributs.height));
    target_top = std::min(target_top, (sector_top - (position.z() + 0.75f * height)) / linedef_index.first + (position.z() + 0.75f * attributs.height));

    // Cancel if view is obstructed
    if (target_bottom >= target_top)
      return std::numeric_limits<float>::quiet_NaN();
  }

  // Line of sight
  return (target_bottom + target_top) / 2.f;
}

bool  DOOM::AbstractThing::P_LineAttack(DOOM::Doom& doom, float atk_range, const Math::Vector<3>& atk_origin, const Math::Vector<3>& atk_direction, float atk_damage)
{
  std::list<std::pair<float, int16_t>>                                      linedefs_list = doom.level.getLinedefs(atk_origin.convert<2>(), atk_direction.convert<2>(), atk_range);
  std::list<std::pair<float, std::reference_wrapper<DOOM::AbstractThing>>>  things_list = doom.level.getThings(atk_origin.convert<2>(), atk_direction.convert<2>(), atk_range);
  std::pair<float, int16_t>                                                 sector = { std::numeric_limits<float>::max(), -1 };

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

    Math::Vector<2> linedef_direction = linedef_end - linedef_start;
    int16_t         sidedef_front_index = linedef.front;
    int16_t         sidedef_back_index = linedef.back;

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
        sector = { (sector_front.floor_current - atk_origin.z()) / atk_direction.z(), sidedef_front.sector };
        break;
      }
      // Intersection with ceiling
      if (height > sector_front.ceiling_current) {
        sector = { (sector_front.ceiling_current - atk_origin.z()) / atk_direction.z(), sidedef_front.sector };
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
    
    // NOTE: no intersection with middle texture, we can shot through windows
    
    // No intersection with linedef
    linedefs_list.pop_front();
  }

  // Shoot sector floor or ceiling
  if (sector.first != std::numeric_limits<float>::max() &&
    (things_list.empty() || things_list.front().first > sector.first) &&
    (linedefs_list.empty() || linedefs_list.front().first > sector.first)) {
    // Don't spawn puff on sky
    if ((atk_direction.z() < 0.f && doom.level.sectors[sector.second].floor_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1")) ||
      (atk_direction.z() > 0.f && doom.level.sectors[sector.second].ceiling_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1")))
      P_SpawnPuff(doom, atk_origin + atk_direction * sector.first);

    // Floor is not a valid target
    return false;
  }

  // Shoot thing
  else if (things_list.empty() == false && (linedefs_list.empty() == true || things_list.front().first < linedefs_list.front().first)) {
    things_list.front().second.get().damage(doom, *this, atk_damage);

    // Spawn bullet puffs or blod spots, depending on target type.
    if (things_list.front().second.get().flags & DOOM::Enum::ThingProperty::ThingProperty_NoBlood)
      P_SpawnPuff(doom, atk_origin + atk_direction * things_list.front().first);
    else
      P_SpawnBlood(doom, atk_origin + atk_direction * things_list.front().first, atk_damage);

    // Things shot
    return true;
  }

  // Shoot linedef
  else if (linedefs_list.empty() == false && (things_list.empty() == true || linedefs_list.front().first < things_list.front().first)) {
    DOOM::AbstractLinedef& linedef = *doom.level.linedefs[linedefs_list.front().second].get();
    Math::Vector<2> linedef_direction = doom.level.vertexes[linedef.end] - doom.level.vertexes[linedef.start];
    Math::Vector<3> linedef_normal(Math::Vector<3>(+linedef_direction.y(), -linedef_direction.x(), 0.f) / linedef_direction.length());

    if (Math::Vector<2>::cos(linedef_normal.convert<2>(), atk_direction.convert<2>()) > 0.f)
      linedef_normal *= -1.f;

    // Spawn smoke puff
    if (linedef.back == -1 ||
      (doom.level.sectors[doom.level.sidedefs[linedef.front].sector].floor_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1") ||
       doom.level.sectors[doom.level.sidedefs[linedef.back].sector].floor_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1")) &&
      (doom.level.sectors[doom.level.sidedefs[linedef.front].sector].ceiling_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1") ||
       doom.level.sectors[doom.level.sidedefs[linedef.back].sector].ceiling_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1")))
      P_SpawnPuff(doom, atk_origin + atk_direction * linedefs_list.front().first + linedef_normal);

    // Gunfire trigger
    return linedef.gunfire(doom, *this);
  }

  // Nothing shot
  return false;
}

void  DOOM::AbstractThing::P_SpawnPuff(DOOM::Doom& doom, const Math::Vector<3>& coordinates) const
{
  // Spawn smoke puff and set its height
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, DOOM::Enum::ThingType::ThingType_SMOKE, DOOM::Enum::ThingFlag::FlagNone, coordinates.x(), coordinates.y(), 0.f));
  doom.level.things.back()->position.z() = coordinates.z();
  doom.level.things.back()->_thrust.z() = +1.f;
  doom.level.things.back()->_elapsed += DOOM::Doom::Tic * (float)std::min(std::rand() % 4, _states[doom.level.things.back()->_state].duration - 1);
}

void  DOOM::AbstractThing::P_SpawnBlood(DOOM::Doom& doom, const Math::Vector<3>& coordinates, float damage) const
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

void  DOOM::AbstractThing::P_LineSwitch(DOOM::Doom& doom, float swc_range, const Math::Vector<3>& swc_origin, const Math::Vector<3>& swc_direction)
{
  std::list<std::pair<float, int16_t>>                                      linedefs_list = doom.level.getLinedefs(swc_origin.convert<2>(), swc_direction.convert<2>(), swc_range);
  std::list<std::pair<float, std::reference_wrapper<DOOM::AbstractThing>>>  things_list = doom.level.getThings(swc_origin.convert<2>(), swc_direction.convert<2>(), swc_range);
  float                                                                     sector = std::numeric_limits<float>::max();

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

void  DOOM::AbstractThing::A_SargAttack(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::A_SkelFist(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::P_SpawnMissile(DOOM::Doom& doom, DOOM::Enum::ThingType type)
{
  // Does nothing if no target
  if (_target == nullptr)
    return;

  float           target_height = P_AimLineAttack(doom, *_target);
  float           atk_slope = std::isnan(target_height) == true ? 0.f : std::atan((target_height - (position.z() + height * 0.5f)) / (_target->position.convert<2>() - position.convert<2>()).length()) + ((_target->flags & DOOM::Enum::ThingProperty::ThingProperty_Shadow) ? (Math::Random() * 2.f - 1.f) * Math::Pi / 8.f : 0);
  float           atk_angle = Math::Vector<2>::angle((_target->position.convert<2>() - position.convert<2>())) + ((_target->flags & DOOM::Enum::ThingProperty::ThingProperty_Shadow) ? (Math::Random() * 2.f - 1.f) * Math::Pi / 8.f : 0);
  Math::Vector<3> direction(std::cos(atk_angle), std::sin(atk_angle), std::tan(atk_slope));

  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, type, DOOM::Enum::ThingFlag::FlagNone, position.x() + std::cos(atk_angle) * attributs.radius / 2.f, position.y() + std::sin(atk_angle) * attributs.radius / 2.f, atk_angle));
  doom.level.things.back()->position.z() = position.z() + 32.f;
  doom.level.things.back()->_thrust = direction * (doom.level.things.back()->attributs.speed / direction.length());
  doom.level.things.back()->_target = this;
  doom.level.things.back()->_tracer = _target;

  doom.sound(doom.level.things.back()->attributs.sound_see, doom.level.things.back()->position);
}

void  DOOM::AbstractThing::P_SpawnPlayerMissile(DOOM::Doom& doom, DOOM::Enum::ThingType type, float slope)
{
  Math::Vector<3> direction(std::cos(angle), std::sin(angle), std::tan(slope));

  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, type, DOOM::Enum::ThingFlag::FlagNone, position.x() + std::cos(angle) * attributs.radius / 2.f, position.y() + std::sin(angle) * attributs.radius / 2.f, angle));
  doom.level.things.back()->position.z() = position.z() + 32.f;
  doom.level.things.back()->_thrust = direction * (doom.level.things.back()->attributs.speed / direction.length());
  doom.level.things.back()->_target = this;
  doom.level.things.back()->_tracer = nullptr;

  doom.sound(doom.level.things.back()->attributs.sound_see, doom.level.things.back()->position);
}

void  DOOM::AbstractThing::A_Explode(DOOM::Doom& doom)
{
  P_RadiusAttack(doom, _target != nullptr ? *_target : *this, 128);
}

void  DOOM::AbstractThing::P_ExplodeMissile(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::P_ExplodeMissile(DOOM::Doom& doom, DOOM::AbstractThing& target)
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

void  DOOM::AbstractThing::damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, DOOM::AbstractThing& origin, float damage)
{
  // Cancel if already dead
  if (health <= 0.f)
    return;

  // Remember exceding damages for gibs
  float exceding = std::max(0.f, damage - health);

  // Deal damage
  health = std::max(0.f, health - damage);

  // Reset flying skull thrust
  if (flags & DOOM::Enum::ThingProperty::ThingProperty_SkullFly)
    _thrust = Math::Vector<3>();

  // We are awake now
  _reactiontime = 0;

  // Ignore thrust if thing is cheating or is attacked by player chainsaw
  // TDOO: check player chainsaw
  if (!(flags & DOOM::Enum::ThingProperty::ThingProperty_NoClip) &&
    &attacker != this &&
    (attacker.type != DOOM::Enum::ThingType::ThingType_PLAYER || true)) {
    float angle = Math::Vector<2>::angle(position.convert<2>() - attacker.position.convert<2>());
    float push = (float)(damage * 2);

    // Sometimes fall foward when killed
    if (damage < 40 && health <= 0.f && position.z() - attacker.position.z() > 64 && (std::rand() % 2 == 1)) {
      angle += Math::Pi;
      push *= 4.f;
    }

    // Apply thrust
    thrust(Math::Vector<3>(std::cos(angle), std::sin(angle), 0.f) * push);
  }

  // Thing killed
  if (health <= 0.f) {
    // Remove some flags
    flags = (DOOM::Enum::ThingProperty)(flags & ~(DOOM::Enum::ThingProperty::ThingProperty_Shootable | DOOM::Enum::ThingProperty::ThingProperty_Float | DOOM::Enum::ThingProperty::ThingProperty_SkullFly));
    if (type != DOOM::Enum::ThingType::ThingType_SKULL) {
      flags = (DOOM::Enum::ThingProperty)(flags & ~DOOM::Enum::ThingProperty::ThingProperty_NoGravity);
      _gravity = -1.f;
    }

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

    // Update level statistics
    if (flags & DOOM::Enum::ThingProperty::ThingProperty_CountKill && origin.type == DOOM::Enum::ThingType::ThingType_PLAYER)
      doom.level.statistics.players[static_cast<DOOM::PlayerThing&>(origin).id].kills += 1;
  }

  // Change target
  if (&origin != this && _target_threshold <= 0) {
    _target = &origin;
    _target_threshold = 100;
  }

  // Gibs monster
  if (attributs.state_xdeath != DOOM::AbstractThing::ThingState::State_None && exceding >= attributs.spawnhealth)
    setState(doom, attributs.state_xdeath);

  // Kill monster
  else if (attributs.state_death != DOOM::AbstractThing::ThingState::State_None && health <= 0)
    setState(doom, attributs.state_death);

  // Start pain state
  else if (attributs.state_pain != DOOM::AbstractThing::ThingState::State_None && std::rand() % 256 < attributs.painchance)
    setState(doom, attributs.state_pain);
}

void  DOOM::AbstractThing::damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, float dmg)
{
  // The attacker is also the origin
  damage(doom, attacker, attacker, dmg);
}

void  DOOM::AbstractThing::damage(DOOM::Doom& doom, float dmg)
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

void  DOOM::AbstractThing::A_SkelMissile(DOOM::Doom& doom)
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

  // 50% chance skell missile goes to target
  if (std::rand() % 2 == 0)
    doom.level.things.back()->_tracer = _target;
  else
    doom.level.things.back()->_tracer = nullptr;
}

void  DOOM::AbstractThing::A_SkullAttack(DOOM::Doom& doom)
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
  _thrust = Math::Vector<3>(
    std::cos(angle) * DOOM::AbstractThing::SkullSpeed,
    std::sin(angle) * DOOM::AbstractThing::SkullSpeed,
    (_target->position.z() + (float)_target->height / 2.f - position.z()) / std::max(1.f, (_target->position.convert<2>() - position.convert<2>()).length()) * DOOM::AbstractThing::SkullSpeed);
}

void  DOOM::AbstractThing::A_PainAttack(DOOM::Doom& doom)
{
  // Cancel if no target
  if (_target == nullptr)
    return;

  // Shoot skull to target
  A_FaceTarget(doom);
  A_PainShootSkull(doom, angle);
}

void  DOOM::AbstractThing::A_PainDie(DOOM::Doom& doom)
{
  // Transform to corpse
  A_Fall(doom);

  // Shoot skull in various directions
  A_PainShootSkull(doom, angle + 1.f * Math::Pi / 2.f);
  A_PainShootSkull(doom, angle + 2.f * Math::Pi / 2.f);
  A_PainShootSkull(doom, angle + 3.f * Math::Pi / 2.f);
}

void  DOOM::AbstractThing::A_PainShootSkull(DOOM::Doom& doom, float orientation)
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

void  DOOM::AbstractThing::A_KeenDie(DOOM::Doom& doom)
{
  A_Fall(doom);

  // Scan the remaining monsters to see if all Keens are dead
  for (const std::unique_ptr<DOOM::AbstractThing>& thing : doom.level.things)
    if (thing->type == type && thing->health > 0)
      return;

  // Lower floor to lowest neighbor
  for (DOOM::Doom::Level::Sector& sector : doom.level.sectors)
    if (sector.tag == 666)
      sector.action<DOOM::Doom::Level::Sector::Action::Leveling>(doom, 2);

  // NOTE: because of sphere collision instead of square, player can't jump on Keen platform
}

void  DOOM::AbstractThing::A_BossDeath(DOOM::Doom& doom)
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

  // Exit level
  doom.level.end = DOOM::Enum::End::EndNormal;
}

void  DOOM::AbstractThing::A_FatRaise(DOOM::Doom& doom)
{
  // Faace target and scream
  A_FaceTarget(doom);
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_manatk, position);
}

void  DOOM::AbstractThing::A_FatAttack1(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::A_FatAttack2(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::A_FatAttack3(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::A_VileChase(DOOM::Doom& doom)
{
  // Check for corpse to raise
  if (_move_direction != DOOM::AbstractThing::Direction::DirectionNone)
  {
    // Iterator over things touching Vile
    for (const auto& thing : doom.level.getThings(position.convert<2>() + (_directions[_move_direction] * (float)attributs.speed), (float)attributs.radius)) {
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

void  DOOM::AbstractThing::A_VileTarget(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::A_Fire(DOOM::Doom& doom)
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

void  DOOM::AbstractThing::A_VileAttack(DOOM::Doom& doom)
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
  _tracer->position = dest;

  // Blast damage target
  _tracer->P_RadiusAttack(doom, *this, 70.f);
}

void  DOOM::AbstractThing::A_Tracer(DOOM::Doom& doom)
{
  // NOTE: We use _move_count instead of game tics to spawn smoke puffs at the right time
  _move_count = (_move_count + _states[_state].duration) % 4;
  if (_move_count >= _states[_state].duration)
    return;

  // Spawn a puff of smoke behind the rocket
  P_SpawnPuff(doom, position - _thrust / _thrust.length() * (float)attributs.radius + Math::Vector<3>(Math::Random() * 2.f - 1.f, Math::Random() * 2.f - 1.f, Math::Random() * 2.f - 1.f) * 4.f * Math::Random());

  // Cancel if no tracer
  if (_tracer == nullptr || _tracer->health <= 0)
    return;

  float origin_angle = angle;
  float target_angle = Math::Vector<2>::angle(_tracer->position.convert<2>() - position.convert<2>());
  float ajust_angle = Math::Modulo(target_angle - origin_angle, Math::Pi * 2.f);

  if (ajust_angle > +Math::Pi)
    ajust_angle -= Math::Pi * 2.f;
  if (ajust_angle < -Math::Pi)
    ajust_angle += Math::Pi * 2.f;

  float origin_slope = std::atan(_thrust.z() / _thrust.convert<2>().length());
  float target_slope = std::atan(((_tracer->position.z() + _tracer->height / 2.f) - (position.z() + height / 2.f)) / (_tracer->position.convert<2>() - position.convert<2>()).length());
  float slope = origin_slope + std::clamp(target_slope - origin_slope, -DOOM::AbstractThing::TracerAngle, +DOOM::AbstractThing::TracerAngle);

  // Ajust angle
  angle += std::clamp(ajust_angle, -DOOM::AbstractThing::TracerAngle, +DOOM::AbstractThing::TracerAngle);

  Math::Vector<3> direction(std::cos(angle), std::sin(angle), std::tan(slope));

  // Update thrust direction
  _thrust = direction * (attributs.speed / direction.length());
}

void  DOOM::AbstractThing::A_BFGSpray(DOOM::Doom& doom)
{
  // Check that emitter is alive
  if (_target == nullptr || _target->health <= 0.f)
    return;

  // 40 BFG rays in a 90° cone
  for (float ray_angle = angle - Math::DegToRad(45.f); ray_angle <= angle + Math::DegToRad(45.f); ray_angle += Math::DegToRad(90.f / 40.f)) {
    auto things = doom.level.getThings(_target->position.convert<2>(), Math::Vector<2>(std::cos(ray_angle), std::sin(ray_angle)), 1024.f);

    // Find thing to shot
    for (const auto& thing : things) {
      // Do not shoot player or inanimated things
      if (&thing.second.get() == _target || !(thing.second.get().flags & DOOM::Enum::ThingProperty::ThingProperty_Shootable))
        continue;

      // Check that thing is in line of sight
      if (std::isnan(_target->P_AimLineAttack(doom, thing.second)) == true)
        continue;

      float damage = 0.f;

      // Compute damage
      for (int d = 0; d < 15; d++)
        damage += std::rand() % 8 + 1;

      // Damage thing
      thing.second.get().damage(doom, *this, *_target, damage);

      // Spawn green sparks on victim
      doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom, DOOM::Enum::ThingType::ThingType_EXTRABFG, DOOM::Enum::ThingFlag::FlagNone, thing.second.get().position.x(), thing.second.get().position.y(), 0.f));
      doom.level.things.back()->position.z() = thing.second.get().position.z() + thing.second.get().height / 2.f;

      break;
    }
  }
}

void  DOOM::AbstractThing::A_BrainPain(DOOM::Doom& doom)
{
  // Emit pain sound
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_bospn);
}

void  DOOM::AbstractThing::A_BrainDie(DOOM::Doom& doom)
{
  // Exit level, go to next level
  doom.level.end = DOOM::Enum::End::EndNormal;
}

void  DOOM::AbstractThing::A_BrainScream(DOOM::Doom& doom)
{
  // Spawn random explosion around brain
  // NOTE: we begin at -320 instead of -196 in original code for symetry
  for (float x = position.x() - 320.f; x < position.x() + 320.f; x += 8.f) {
    doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom,
      DOOM::Enum::ThingType::ThingType_ROCKET,
      DOOM::Enum::ThingFlag::FlagNone,
      x,
      position.y() - 320.f,
      0.f));
    doom.level.things.back()->position.z() = 128.f + (std::rand() % 256) * 2;
    doom.level.things.back()->_thrust.z() = (std::rand() % 256) / 128.f;
    doom.level.things.back()->setState(doom, DOOM::AbstractThing::ThingState::State_BRAINEXPLODE1);
    doom.level.things.back()->_elapsed += DOOM::Doom::Tic * std::min(_states[DOOM::AbstractThing::ThingState::State_BRAINEXPLODE1].duration - 1, (std::rand() % 8));
  }

  // Scream
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_bosdth);
}

void  DOOM::AbstractThing::A_BrainExplode(DOOM::Doom& doom)
{
  // Spawn an explosion
  doom.level.things.push_back(std::make_unique<DOOM::AbstractThing>(doom,
    DOOM::Enum::ThingType::ThingType_ROCKET,
    DOOM::Enum::ThingFlag::FlagNone,
    position.x() + (std::rand() % 256 - std::rand() % 256) / 32.f,
    position.y(),
    0.f));
  doom.level.things.back()->position.z() = 128.f + (std::rand() % 256) * 2;
  doom.level.things.back()->_thrust.z() = (std::rand() % 256) / 128.f;
  doom.level.things.back()->setState(doom, DOOM::AbstractThing::ThingState::State_BRAINEXPLODE1);
  doom.level.things.back()->_elapsed += DOOM::Doom::Tic * std::min(_states[DOOM::AbstractThing::ThingState::State_BRAINEXPLODE1].duration - 1, (std::rand() % 8));
}

void  DOOM::AbstractThing::A_BrainAwake(DOOM::Doom& doom)
{
  // NOTE: we find target spots on the fly in A_BrainSpit

  // Scream!
  doom.sound(DOOM::Doom::Resources::Sound::EnumSound::Sound_bossit);
}

void  DOOM::AbstractThing::A_BrainSpit(DOOM::Doom& doom)
{
  // NOTE: we use _target_threshold as counter for easy mode.
  // NOTE: we use _move_count to remember last target spot
  
  // Only shoot half monster in baby and easy skill mode
  if (doom.skill <= DOOM::Enum::Skill::SkillEasy && (++_target_threshold) % 2 == 0)
    return;

  std::list<std::reference_wrapper<DOOM::AbstractThing>>  targets;

  // Get targets in level
  for (const auto& thing : doom.level.things)
    if (thing->type == DOOM::Enum::ThingType::ThingType_BOSSTARGET)
      targets.push_back(*thing);

  // No target to shot!
  if (targets.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Select target
  _target = &std::next(targets.begin(), (_move_count++) % targets.size())->get();

  // Spawn brain missile
  P_SpawnMissile(doom, DOOM::Enum::ThingType::ThingType_SPAWNSHOT);
  doom.level.things.back()->_target = _target;
  doom.level.things.back()->_reactiontime = (int)((_target->position - doom.level.things.back()->position).length() / doom.level.things.back()->_thrust.length()) / _states[doom.level.things.back()->_state].duration;
}