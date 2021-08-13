#pragma once

#include "Doom/Doom.hpp"
#include "Doom/Statusbar.hpp"

namespace DOOM
{
  class AbstractThing
  {
  protected:
    enum ThingSprite
    {
      Sprite_TROO, Sprite_SHTG, Sprite_PUNG, Sprite_PISG, Sprite_PISF, Sprite_SHTF, Sprite_SHT2, Sprite_CHGG, Sprite_CHGF, Sprite_MISG,
      Sprite_MISF, Sprite_SAWG, Sprite_PLSG, Sprite_PLSF, Sprite_BFGG, Sprite_BFGF, Sprite_BLUD, Sprite_PUFF, Sprite_BAL1, Sprite_BAL2,
      Sprite_PLSS, Sprite_PLSE, Sprite_MISL, Sprite_BFS1, Sprite_BFE1, Sprite_BFE2, Sprite_TFOG, Sprite_IFOG, Sprite_PLAY, Sprite_POSS,
      Sprite_SPOS, Sprite_VILE, Sprite_FIRE, Sprite_FATB, Sprite_FBXP, Sprite_SKEL, Sprite_MANF, Sprite_FATT, Sprite_CPOS, Sprite_SARG,
      Sprite_HEAD, Sprite_BAL7, Sprite_BOSS, Sprite_BOS2, Sprite_SKUL, Sprite_SPID, Sprite_BSPI, Sprite_APLS, Sprite_APBX, Sprite_CYBR,
      Sprite_PAIN, Sprite_SSWV, Sprite_KEEN, Sprite_BBRN, Sprite_BOSF, Sprite_ARM1, Sprite_ARM2, Sprite_BAR1, Sprite_BEXP, Sprite_FCAN,
      Sprite_BON1, Sprite_BON2, Sprite_BKEY, Sprite_RKEY, Sprite_YKEY, Sprite_BSKU, Sprite_RSKU, Sprite_YSKU, Sprite_STIM, Sprite_MEDI,
      Sprite_SOUL, Sprite_PINV, Sprite_PSTR, Sprite_PINS, Sprite_MEGA, Sprite_SUIT, Sprite_PMAP, Sprite_PVIS, Sprite_CLIP, Sprite_AMMO,
      Sprite_ROCK, Sprite_BROK, Sprite_CELL, Sprite_CELP, Sprite_SHEL, Sprite_SBOX, Sprite_BPAK, Sprite_BFUG, Sprite_MGUN, Sprite_CSAW,
      Sprite_LAUN, Sprite_PLAS, Sprite_SHOT, Sprite_SGN2, Sprite_COLU, Sprite_SMT2, Sprite_GOR1, Sprite_POL2, Sprite_POL5, Sprite_POL4,
      Sprite_POL3, Sprite_POL1, Sprite_POL6, Sprite_GOR2, Sprite_GOR3, Sprite_GOR4, Sprite_GOR5, Sprite_SMIT, Sprite_COL1, Sprite_COL2,
      Sprite_COL3, Sprite_COL4, Sprite_CAND, Sprite_CBRA, Sprite_COL6, Sprite_TRE1, Sprite_TRE2, Sprite_ELEC, Sprite_CEYE, Sprite_FSKU,
      Sprite_COL5, Sprite_TBLU, Sprite_TGRN, Sprite_TRED, Sprite_SMBT, Sprite_SMGT, Sprite_SMRT, Sprite_HDB1, Sprite_HDB2, Sprite_HDB3,
      Sprite_HDB4, Sprite_HDB5, Sprite_HDB6, Sprite_POB1, Sprite_POB2, Sprite_BRS1, Sprite_TLMP, Sprite_TLP2,
      Sprite_Number
    };

    enum ThingState
    {
      State_None,
      State_BLOOD1, State_BLOOD2, State_BLOOD3,
      State_PUFF1, State_PUFF2, State_PUFF3, State_PUFF4,
      State_TBALL1, State_TBALL2,
      State_TBALLX1, State_TBALLX2, State_TBALLX3,
      State_RBALL1, State_RBALL2,
      State_RBALLX1, State_RBALLX2, State_RBALLX3,
      State_PLASBALL, State_PLASBALL2,
      State_PLASEXP, State_PLASEXP2, State_PLASEXP3, State_PLASEXP4, State_PLASEXP5,
      State_ROCKET,
      State_BFGSHOT, State_BFGSHOT2,
      State_BFGLAND, State_BFGLAND2, State_BFGLAND3, State_BFGLAND4, State_BFGLAND5, State_BFGLAND6,
      State_BFGEXP, State_BFGEXP2, State_BFGEXP3, State_BFGEXP4,
      State_EXPLODE1, State_EXPLODE2, State_EXPLODE3,
      State_TFOG, State_TFOG01, State_TFOG02, State_TFOG2, State_TFOG3, State_TFOG4, State_TFOG5, State_TFOG6, State_TFOG7, State_TFOG8, State_TFOG9, State_TFOG10,
      State_IFOG, State_IFOG01, State_IFOG02, State_IFOG2, State_IFOG3, State_IFOG4, State_IFOG5,
      State_PLAY, State_PLAY_RUN1, State_PLAY_RUN2, State_PLAY_RUN3, State_PLAY_RUN4, State_PLAY_ATK1, State_PLAY_ATK2, State_PLAY_PAIN, State_PLAY_PAIN2, State_PLAY_DIE1, State_PLAY_DIE2, State_PLAY_DIE3, State_PLAY_DIE4, State_PLAY_DIE5, State_PLAY_DIE6, State_PLAY_DIE7, State_PLAY_XDIE1, State_PLAY_XDIE2, State_PLAY_XDIE3, State_PLAY_XDIE4, State_PLAY_XDIE5, State_PLAY_XDIE6, State_PLAY_XDIE7, State_PLAY_XDIE8, State_PLAY_XDIE9,
      State_POSS_STND, State_POSS_STND2, State_POSS_RUN1, State_POSS_RUN2, State_POSS_RUN3, State_POSS_RUN4, State_POSS_RUN5, State_POSS_RUN6, State_POSS_RUN7, State_POSS_RUN8, State_POSS_ATK1, State_POSS_ATK2, State_POSS_ATK3, State_POSS_PAIN, State_POSS_PAIN2, State_POSS_DIE1, State_POSS_DIE2, State_POSS_DIE3, State_POSS_DIE4, State_POSS_DIE5, State_POSS_XDIE1, State_POSS_XDIE2, State_POSS_XDIE3, State_POSS_XDIE4, State_POSS_XDIE5, State_POSS_XDIE6, State_POSS_XDIE7, State_POSS_XDIE8, State_POSS_XDIE9, State_POSS_RAISE1, State_POSS_RAISE2, State_POSS_RAISE3, State_POSS_RAISE4, State_SPOS_STND,
      State_SPOS_STND2, State_SPOS_RUN1, State_SPOS_RUN2, State_SPOS_RUN3, State_SPOS_RUN4, State_SPOS_RUN5, State_SPOS_RUN6, State_SPOS_RUN7, State_SPOS_RUN8, State_SPOS_ATK1, State_SPOS_ATK2, State_SPOS_ATK3, State_SPOS_PAIN, State_SPOS_PAIN2, State_SPOS_DIE1, State_SPOS_DIE2, State_SPOS_DIE3, State_SPOS_DIE4, State_SPOS_DIE5, State_SPOS_XDIE1, State_SPOS_XDIE2, State_SPOS_XDIE3, State_SPOS_XDIE4, State_SPOS_XDIE5, State_SPOS_XDIE6, State_SPOS_XDIE7, State_SPOS_XDIE8, State_SPOS_XDIE9, State_SPOS_RAISE1, State_SPOS_RAISE2, State_SPOS_RAISE3, State_SPOS_RAISE4, State_SPOS_RAISE5,
      State_VILE_STND, State_VILE_STND2, State_VILE_RUN1, State_VILE_RUN2, State_VILE_RUN3, State_VILE_RUN4, State_VILE_RUN5, State_VILE_RUN6, State_VILE_RUN7, State_VILE_RUN8, State_VILE_RUN9, State_VILE_RUN10, State_VILE_RUN11, State_VILE_RUN12, State_VILE_ATK1, State_VILE_ATK2, State_VILE_ATK3, State_VILE_ATK4, State_VILE_ATK5, State_VILE_ATK6, State_VILE_ATK7, State_VILE_ATK8, State_VILE_ATK9, State_VILE_ATK10, State_VILE_ATK11, State_VILE_HEAL1, State_VILE_HEAL2, State_VILE_HEAL3, State_VILE_PAIN, State_VILE_PAIN2, State_VILE_DIE1, State_VILE_DIE2, State_VILE_DIE3, State_VILE_DIE4, State_VILE_DIE5, State_VILE_DIE6, State_VILE_DIE7, State_VILE_DIE8, State_VILE_DIE9, State_VILE_DIE10,
      State_FIRE1, State_FIRE2, State_FIRE3, State_FIRE4, State_FIRE5, State_FIRE6, State_FIRE7, State_FIRE8, State_FIRE9, State_FIRE10, State_FIRE11, State_FIRE12, State_FIRE13, State_FIRE14, State_FIRE15, State_FIRE16, State_FIRE17, State_FIRE18, State_FIRE19, State_FIRE20, State_FIRE21, State_FIRE22, State_FIRE23, State_FIRE24, State_FIRE25, State_FIRE26, State_FIRE27, State_FIRE28, State_FIRE29, State_FIRE30,
      State_SMOKE1, State_SMOKE2, State_SMOKE3, State_SMOKE4, State_SMOKE5,
      State_TRACER, State_TRACER2, State_TRACEEXP1, State_TRACEEXP2, State_TRACEEXP3,
      State_SKEL_STND, State_SKEL_STND2, State_SKEL_RUN1, State_SKEL_RUN2, State_SKEL_RUN3, State_SKEL_RUN4, State_SKEL_RUN5, State_SKEL_RUN6, State_SKEL_RUN7, State_SKEL_RUN8, State_SKEL_RUN9, State_SKEL_RUN10, State_SKEL_RUN11, State_SKEL_RUN12, State_SKEL_FIST1, State_SKEL_FIST2, State_SKEL_FIST3, State_SKEL_FIST4, State_SKEL_MISS1, State_SKEL_MISS2, State_SKEL_MISS3, State_SKEL_MISS4, State_SKEL_PAIN, State_SKEL_PAIN2, State_SKEL_DIE1, State_SKEL_DIE2, State_SKEL_DIE3, State_SKEL_DIE4, State_SKEL_DIE5, State_SKEL_DIE6, State_SKEL_RAISE1, State_SKEL_RAISE2, State_SKEL_RAISE3, State_SKEL_RAISE4, State_SKEL_RAISE5, State_SKEL_RAISE6,
      State_FATSHOT1, State_FATSHOT2, State_FATSHOTX1, State_FATSHOTX2, State_FATSHOTX3,
      State_FATT_STND, State_FATT_STND2, State_FATT_RUN1, State_FATT_RUN2, State_FATT_RUN3, State_FATT_RUN4, State_FATT_RUN5, State_FATT_RUN6, State_FATT_RUN7, State_FATT_RUN8, State_FATT_RUN9, State_FATT_RUN10, State_FATT_RUN11, State_FATT_RUN12, State_FATT_ATK1, State_FATT_ATK2, State_FATT_ATK3, State_FATT_ATK4, State_FATT_ATK5, State_FATT_ATK6, State_FATT_ATK7, State_FATT_ATK8, State_FATT_ATK9, State_FATT_ATK10, State_FATT_PAIN, State_FATT_PAIN2, State_FATT_DIE1, State_FATT_DIE2, State_FATT_DIE3, State_FATT_DIE4, State_FATT_DIE5, State_FATT_DIE6, State_FATT_DIE7, State_FATT_DIE8, State_FATT_DIE9, State_FATT_DIE10, State_FATT_RAISE1, State_FATT_RAISE2, State_FATT_RAISE3, State_FATT_RAISE4, State_FATT_RAISE5, State_FATT_RAISE6, State_FATT_RAISE7, State_FATT_RAISE8,
      State_CPOS_STND, State_CPOS_STND2, State_CPOS_RUN1, State_CPOS_RUN2, State_CPOS_RUN3, State_CPOS_RUN4, State_CPOS_RUN5, State_CPOS_RUN6, State_CPOS_RUN7, State_CPOS_RUN8, State_CPOS_ATK1, State_CPOS_ATK2, State_CPOS_ATK3, State_CPOS_ATK4, State_CPOS_PAIN, State_CPOS_PAIN2, State_CPOS_DIE1, State_CPOS_DIE2, State_CPOS_DIE3, State_CPOS_DIE4, State_CPOS_DIE5, State_CPOS_DIE6, State_CPOS_DIE7, State_CPOS_XDIE1, State_CPOS_XDIE2, State_CPOS_XDIE3, State_CPOS_XDIE4, State_CPOS_XDIE5, State_CPOS_XDIE6, State_CPOS_RAISE1, State_CPOS_RAISE2, State_CPOS_RAISE3, State_CPOS_RAISE4, State_CPOS_RAISE5, State_CPOS_RAISE6, State_CPOS_RAISE7,
      State_TROO_STND, State_TROO_STND2, State_TROO_RUN1, State_TROO_RUN2, State_TROO_RUN3, State_TROO_RUN4, State_TROO_RUN5, State_TROO_RUN6, State_TROO_RUN7, State_TROO_RUN8, State_TROO_ATK1, State_TROO_ATK2, State_TROO_ATK3, State_TROO_PAIN, State_TROO_PAIN2, State_TROO_DIE1, State_TROO_DIE2, State_TROO_DIE3, State_TROO_DIE4, State_TROO_DIE5, State_TROO_XDIE1, State_TROO_XDIE2, State_TROO_XDIE3, State_TROO_XDIE4, State_TROO_XDIE5, State_TROO_XDIE6, State_TROO_XDIE7, State_TROO_XDIE8, State_TROO_RAISE1, State_TROO_RAISE2, State_TROO_RAISE3, State_TROO_RAISE4, State_TROO_RAISE5,
      State_SARG_STND, State_SARG_STND2, State_SARG_RUN1, State_SARG_RUN2, State_SARG_RUN3, State_SARG_RUN4, State_SARG_RUN5, State_SARG_RUN6, State_SARG_RUN7, State_SARG_RUN8, State_SARG_ATK1, State_SARG_ATK2, State_SARG_ATK3, State_SARG_PAIN, State_SARG_PAIN2, State_SARG_DIE1, State_SARG_DIE2, State_SARG_DIE3, State_SARG_DIE4, State_SARG_DIE5, State_SARG_DIE6, State_SARG_RAISE1, State_SARG_RAISE2, State_SARG_RAISE3, State_SARG_RAISE4, State_SARG_RAISE5, State_SARG_RAISE6,
      State_HEAD_STND, State_HEAD_RUN1, State_HEAD_ATK1, State_HEAD_ATK2, State_HEAD_ATK3, State_HEAD_PAIN, State_HEAD_PAIN2, State_HEAD_PAIN3, State_HEAD_DIE1, State_HEAD_DIE2, State_HEAD_DIE3, State_HEAD_DIE4, State_HEAD_DIE5, State_HEAD_DIE6, State_HEAD_RAISE1, State_HEAD_RAISE2, State_HEAD_RAISE3, State_HEAD_RAISE4, State_HEAD_RAISE5, State_HEAD_RAISE6,
      State_BRBALL1, State_BRBALL2, State_BRBALLX1, State_BRBALLX2, State_BRBALLX3,
      State_BOSS_STND, State_BOSS_STND2, State_BOSS_RUN1, State_BOSS_RUN2, State_BOSS_RUN3, State_BOSS_RUN4, State_BOSS_RUN5, State_BOSS_RUN6, State_BOSS_RUN7, State_BOSS_RUN8, State_BOSS_ATK1, State_BOSS_ATK2, State_BOSS_ATK3, State_BOSS_PAIN, State_BOSS_PAIN2, State_BOSS_DIE1, State_BOSS_DIE2, State_BOSS_DIE3, State_BOSS_DIE4, State_BOSS_DIE5, State_BOSS_DIE6, State_BOSS_DIE7, State_BOSS_RAISE1, State_BOSS_RAISE2, State_BOSS_RAISE3, State_BOSS_RAISE4, State_BOSS_RAISE5, State_BOSS_RAISE6, State_BOSS_RAISE7,
      State_BOS2_STND, State_BOS2_STND2, State_BOS2_RUN1, State_BOS2_RUN2, State_BOS2_RUN3, State_BOS2_RUN4, State_BOS2_RUN5, State_BOS2_RUN6, State_BOS2_RUN7, State_BOS2_RUN8, State_BOS2_ATK1, State_BOS2_ATK2, State_BOS2_ATK3, State_BOS2_PAIN, State_BOS2_PAIN2, State_BOS2_DIE1, State_BOS2_DIE2, State_BOS2_DIE3, State_BOS2_DIE4, State_BOS2_DIE5, State_BOS2_DIE6, State_BOS2_DIE7, State_BOS2_RAISE1, State_BOS2_RAISE2, State_BOS2_RAISE3, State_BOS2_RAISE4, State_BOS2_RAISE5, State_BOS2_RAISE6, State_BOS2_RAISE7,
      State_SKULL_STND, State_SKULL_STND2, State_SKULL_RUN1, State_SKULL_RUN2, State_SKULL_ATK1, State_SKULL_ATK2, State_SKULL_ATK3, State_SKULL_ATK4, State_SKULL_PAIN, State_SKULL_PAIN2, State_SKULL_DIE1, State_SKULL_DIE2, State_SKULL_DIE3, State_SKULL_DIE4, State_SKULL_DIE5, State_SKULL_DIE6,
      State_SPID_STND, State_SPID_STND2, State_SPID_RUN1, State_SPID_RUN2, State_SPID_RUN3, State_SPID_RUN4, State_SPID_RUN5, State_SPID_RUN6, State_SPID_RUN7, State_SPID_RUN8, State_SPID_RUN9, State_SPID_RUN10, State_SPID_RUN11, State_SPID_RUN12, State_SPID_ATK1, State_SPID_ATK2, State_SPID_ATK3, State_SPID_ATK4, State_SPID_PAIN, State_SPID_PAIN2, State_SPID_DIE1, State_SPID_DIE2, State_SPID_DIE3, State_SPID_DIE4, State_SPID_DIE5, State_SPID_DIE6, State_SPID_DIE7, State_SPID_DIE8, State_SPID_DIE9, State_SPID_DIE10, State_SPID_DIE11,
      State_BSPI_STND, State_BSPI_STND2, State_BSPI_SIGHT, State_BSPI_RUN1, State_BSPI_RUN2, State_BSPI_RUN3, State_BSPI_RUN4, State_BSPI_RUN5, State_BSPI_RUN6, State_BSPI_RUN7, State_BSPI_RUN8, State_BSPI_RUN9, State_BSPI_RUN10, State_BSPI_RUN11, State_BSPI_RUN12, State_BSPI_ATK1, State_BSPI_ATK2, State_BSPI_ATK3, State_BSPI_ATK4, State_BSPI_PAIN, State_BSPI_PAIN2, State_BSPI_DIE1, State_BSPI_DIE2, State_BSPI_DIE3, State_BSPI_DIE4, State_BSPI_DIE5, State_BSPI_DIE6, State_BSPI_DIE7, State_BSPI_RAISE1, State_BSPI_RAISE2, State_BSPI_RAISE3, State_BSPI_RAISE4, State_BSPI_RAISE5, State_BSPI_RAISE6, State_BSPI_RAISE7,
      State_ARACH_PLAZ, State_ARACH_PLAZ2, State_ARACH_PLEX, State_ARACH_PLEX2, State_ARACH_PLEX3, State_ARACH_PLEX4, State_ARACH_PLEX5,
      State_CYBER_STND, State_CYBER_STND2, State_CYBER_RUN1, State_CYBER_RUN2, State_CYBER_RUN3, State_CYBER_RUN4, State_CYBER_RUN5, State_CYBER_RUN6, State_CYBER_RUN7, State_CYBER_RUN8, State_CYBER_ATK1, State_CYBER_ATK2, State_CYBER_ATK3, State_CYBER_ATK4, State_CYBER_ATK5, State_CYBER_ATK6, State_CYBER_PAIN, State_CYBER_DIE1, State_CYBER_DIE2, State_CYBER_DIE3, State_CYBER_DIE4, State_CYBER_DIE5, State_CYBER_DIE6, State_CYBER_DIE7, State_CYBER_DIE8, State_CYBER_DIE9, State_CYBER_DIE10,
      State_PAIN_STND, State_PAIN_RUN1, State_PAIN_RUN2, State_PAIN_RUN3, State_PAIN_RUN4, State_PAIN_RUN5, State_PAIN_RUN6, State_PAIN_ATK1, State_PAIN_ATK2, State_PAIN_ATK3, State_PAIN_ATK4, State_PAIN_PAIN, State_PAIN_PAIN2, State_PAIN_DIE1, State_PAIN_DIE2, State_PAIN_DIE3, State_PAIN_DIE4, State_PAIN_DIE5, State_PAIN_DIE6, State_PAIN_RAISE1, State_PAIN_RAISE2, State_PAIN_RAISE3, State_PAIN_RAISE4, State_PAIN_RAISE5, State_PAIN_RAISE6,
      State_SSWV_STND, State_SSWV_STND2, State_SSWV_RUN1, State_SSWV_RUN2, State_SSWV_RUN3, State_SSWV_RUN4, State_SSWV_RUN5, State_SSWV_RUN6, State_SSWV_RUN7, State_SSWV_RUN8, State_SSWV_ATK1, State_SSWV_ATK2, State_SSWV_ATK3, State_SSWV_ATK4, State_SSWV_ATK5, State_SSWV_ATK6, State_SSWV_PAIN, State_SSWV_PAIN2, State_SSWV_DIE1, State_SSWV_DIE2, State_SSWV_DIE3, State_SSWV_DIE4, State_SSWV_DIE5, State_SSWV_XDIE1, State_SSWV_XDIE2, State_SSWV_XDIE3, State_SSWV_XDIE4, State_SSWV_XDIE5, State_SSWV_XDIE6, State_SSWV_XDIE7, State_SSWV_XDIE8, State_SSWV_XDIE9, State_SSWV_RAISE1, State_SSWV_RAISE2, State_SSWV_RAISE3, State_SSWV_RAISE4, State_SSWV_RAISE5,
      State_KEENSTND, State_COMMKEEN, State_COMMKEEN2, State_COMMKEEN3, State_COMMKEEN4, State_COMMKEEN5, State_COMMKEEN6, State_COMMKEEN7, State_COMMKEEN8, State_COMMKEEN9, State_COMMKEEN10, State_COMMKEEN11, State_COMMKEEN12, State_KEENPAIN, State_KEENPAIN2,
      State_BRAIN, State_BRAIN_PAIN, State_BRAIN_DIE1, State_BRAIN_DIE2, State_BRAIN_DIE3, State_BRAIN_DIE4, State_BRAINEYE, State_BRAINEYESEE, State_BRAINEYE1,
      State_SPAWN1, State_SPAWN2, State_SPAWN3, State_SPAWN4, State_SPAWNFIRE1, State_SPAWNFIRE2, State_SPAWNFIRE3, State_SPAWNFIRE4, State_SPAWNFIRE5, State_SPAWNFIRE6, State_SPAWNFIRE7, State_SPAWNFIRE8,
      State_BRAINEXPLODE1, State_BRAINEXPLODE2, State_BRAINEXPLODE3,
      State_ARM1, State_ARM1A,
      State_ARM2, State_ARM2A,
      State_BAR1, State_BAR2, State_BEXP, State_BEXP2, State_BEXP3, State_BEXP4, State_BEXP5,
      State_BBAR1, State_BBAR2, State_BBAR3,
      State_BON1, State_BON1A, State_BON1B, State_BON1C, State_BON1D, State_BON1E,
      State_BON2, State_BON2A, State_BON2B, State_BON2C, State_BON2D, State_BON2E,
      State_BKEY, State_BKEY2,
      State_RKEY, State_RKEY2,
      State_YKEY, State_YKEY2,
      State_BSKULL, State_BSKULL2,
      State_RSKULL, State_RSKULL2,
      State_YSKULL, State_YSKULL2,
      State_STIM,
      State_MEDI,
      State_SOUL, State_SOUL2, State_SOUL3, State_SOUL4, State_SOUL5, State_SOUL6,
      State_PINV, State_PINV2, State_PINV3, State_PINV4,
      State_PSTR,
      State_PINS, State_PINS2, State_PINS3, State_PINS4,
      State_MEGA, State_MEGA2, State_MEGA3, State_MEGA4,
      State_SUIT,
      State_PMAP, State_PMAP2, State_PMAP3, State_PMAP4, State_PMAP5, State_PMAP6,
      State_PVIS, State_PVIS2,
      State_CLIP,
      State_AMMO,
      State_ROCK,
      State_BROK,
      State_CELL,
      State_CELP,
      State_SHEL,
      State_SBOX,
      State_BPAK,
      State_BFUG,
      State_MGUN,
      State_CSAW,
      State_LAUN,
      State_PLAS,
      State_SHOT, State_SHOT2,
      State_COLU,
      State_STALAG,
      State_BLOODYTWITCH, State_BLOODYTWITCH2, State_BLOODYTWITCH3, State_BLOODYTWITCH4,
      State_DEADTORSO,
      State_DEADBOTTOM,
      State_HEADSONSTICK,
      State_GIBS,
      State_HEADONASTICK,
      State_HEADCANDLES, State_HEADCANDLES2,
      State_DEADSTICK,
      State_LIVESTICK, State_LIVESTICK2,
      State_MEAT2, State_MEAT3, State_MEAT4, State_MEAT5,
      State_STALAGTITE,
      State_TALLGRNCOL,
      State_SHRTGRNCOL,
      State_TALLREDCOL,
      State_SHRTREDCOL,
      State_CANDLESTIK,
      State_CANDELABRA,
      State_SKULLCOL,
      State_TORCHTREE,
      State_BIGTREE,
      State_TECHPILLAR,
      State_EVILEYE, State_EVILEYE2, State_EVILEYE3, State_EVILEYE4,
      State_FLOATSKULL, State_FLOATSKULL2, State_FLOATSKULL3,
      State_HEARTCOL, State_HEARTCOL2,
      State_BLUETORCH, State_BLUETORCH2, State_BLUETORCH3, State_BLUETORCH4,
      State_GREENTORCH, State_GREENTORCH2, State_GREENTORCH3, State_GREENTORCH4,
      State_REDTORCH, State_REDTORCH2, State_REDTORCH3, State_REDTORCH4,
      State_BTORCHSHRT, State_BTORCHSHRT2, State_BTORCHSHRT3, State_BTORCHSHRT4,
      State_GTORCHSHRT, State_GTORCHSHRT2, State_GTORCHSHRT3, State_GTORCHSHRT4,
      State_RTORCHSHRT, State_RTORCHSHRT2, State_RTORCHSHRT3, State_RTORCHSHRT4,
      State_HANGNOGUTS,
      State_HANGBNOBRAIN,
      State_HANGTLOOKDN,
      State_HANGTSKULL,
      State_HANGTLOOKUP,
      State_HANGTNOBRAIN,
      State_COLONGIBS,
      State_SMALLPOOL,
      State_BRAINSTEM,
      State_TECHLAMP, State_TECHLAMP2, State_TECHLAMP3, State_TECHLAMP4,
      State_TECH2LAMP, State_TECH2LAMP2, State_TECH2LAMP3, State_TECH2LAMP4,
      State_Number
    };

  private:
    class State
    {
    public:
      using Action = void(DOOM::AbstractThing::*)(DOOM::Doom&);

      DOOM::AbstractThing::ThingSprite  sprite;     // Sprite sequence to use
      int                               frame;      // Frame of sprite to display
      bool                              brightness; // True if thing should be displayed at full brightness
      int                               duration;   // Duration of the state
      Action                            action;     // Action to be taken at the start of the state (nullptr if none)
      DOOM::AbstractThing::ThingState   next;       // Index of the next state

      State() = default;
      ~State() = default;
    };

    class Attributs
    {
    public:
      int                       id;           // Thing ID
      int                       spawnhealth;  // Health points at spawn
      int                       reactiontime; // Reaction time of monster (tics)
      int                       painchance;   // Chance of a pain state after damage (/255)
      int                       speed;        // Units/frame (3 tics)
      int                       radius;       // Thing's radius
      int                       height;       // Thing's height
      int                       mass;         // Thing's mass
      int                       damage;       // Thing damage multiplier (missiles)
      DOOM::Enum::ThingProperty properties;   // Properties of thing

      // States of thing
      DOOM::AbstractThing::ThingState state_spawn;
      DOOM::AbstractThing::ThingState state_see;
      DOOM::AbstractThing::ThingState state_pain;
      DOOM::AbstractThing::ThingState state_melee;
      DOOM::AbstractThing::ThingState state_missile;
      DOOM::AbstractThing::ThingState state_death;
      DOOM::AbstractThing::ThingState state_xdeath;
      DOOM::AbstractThing::ThingState state_raise;

      // Sounds of thing
      DOOM::Doom::Resources::Sound::EnumSound sound_see;
      DOOM::Doom::Resources::Sound::EnumSound sound_attack;
      DOOM::Doom::Resources::Sound::EnumSound sound_pain;
      DOOM::Doom::Resources::Sound::EnumSound sound_death;
      DOOM::Doom::Resources::Sound::EnumSound sound_active;

      Attributs() = default;
      ~Attributs() = default;
    };

    enum Direction
    {
      DirectionNone = -1,
      DirectionEast,
      DirectionNorthEast,
      DirectionNorth,
      DirectionNorthWest,
      DirectionWest,
      DirectionSouthWest,
      DirectionSouth,
      DirectionSouthEast,
      DirectionNumber
    };

  protected:
    static const std::array<std::string, DOOM::AbstractThing::ThingSprite::Sprite_Number>               _sprites;     // Table of thing sprites
    static const std::array<DOOM::AbstractThing::State, DOOM::AbstractThing::ThingState::State_Number>  _states;      // Table of thing states
    static const std::array<DOOM::AbstractThing::Attributs, DOOM::Enum::ThingType::ThingType_Number>    _attributs;   // Table of thing attributs
    static const std::array<Math::Vector<2>, DOOM::AbstractThing::Direction::DirectionNumber>	        _directions;  // Table of move direction vectors

    // Constant values
    static const float  MeleeRange;
    static const float  MissileRange;
    static const float  FloatSpeed;
    static const float  SkullSpeed;
    static const int    TargetThreshold;
    static const int    MaxRadius;
    static const float  FatSpread;
    static const float  TracerAngle;

  public:
    static std::unique_ptr<DOOM::AbstractThing> factory(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Thing& thing); // Thing factory

    Math::Vector<3>                       position;   // Thing position
    float                                 angle;      // Thing orientation (rad.)
    const DOOM::Enum::ThingType           type;       // Thing type
    const DOOM::AbstractThing::Attributs& attributs;  // Attributs of thing
    DOOM::Enum::ThingProperty             flags;      // Thing properties (from attributs + ambush + justattacked)

    float health; // Thing health points
    int   height; // Thing height

  protected:
    int       reactiontime;   // Thing reaction time counter
    Direction move_direction; // Movement direction
    int       move_count;     // When 0, select a new direction

    bool                            _remove;  // True if thing should be deleted
    Math::Vector<3>                 _thrust;  // Thrust applied on thing
    float                           _gravity; // Gravity applied on thing
    DOOM::AbstractThing::ThingState _state;   // Current state of thing
    sf::Time                        _elapsed; // Elapsed time since beginning of state

    DOOM::AbstractThing*  _target;            // Thing targeted or missile emitter
    DOOM::AbstractThing*  _tracer;            // Thing being chased/attacked
    int                   _target_threshold;  // Time focusing exclusively on target

  protected:
    void  A_Explode(DOOM::Doom& doom);
    void  A_Pain(DOOM::Doom& doom);
    void  A_PlayerScream(DOOM::Doom& doom);
    void  A_Fall(DOOM::Doom& doom);
    void  A_XScream(DOOM::Doom& doom);
    void  A_FaceTarget(DOOM::Doom& doom);
    void  A_PosAttack(DOOM::Doom& doom);
    void  A_Scream(DOOM::Doom& doom);
    void  A_SPosAttack(DOOM::Doom& doom);
    void  A_VileChase(DOOM::Doom& doom);
    void  A_VileStart(DOOM::Doom& doom);
    void  A_VileTarget(DOOM::Doom& doom);
    void  A_VileAttack(DOOM::Doom& doom);
    void  A_StartFire(DOOM::Doom& doom);
    void  A_Fire(DOOM::Doom& doom);
    void  A_FireCrackle(DOOM::Doom& doom);
    void  A_Tracer(DOOM::Doom& doom);
    void  A_SkelWhoosh(DOOM::Doom& doom);
    void  A_SkelFist(DOOM::Doom& doom);
    void  A_SkelMissile(DOOM::Doom& doom);
    void  A_FatRaise(DOOM::Doom& doom);
    void  A_FatAttack1(DOOM::Doom& doom);
    void  A_FatAttack2(DOOM::Doom& doom);
    void  A_FatAttack3(DOOM::Doom& doom);
    void  A_BossDeath(DOOM::Doom& doom);
    void  A_CPosAttack(DOOM::Doom& doom);
    void  A_CPosRefire(DOOM::Doom& doom);
    void  A_TroopAttack(DOOM::Doom& doom);
    void  A_SargAttack(DOOM::Doom& doom);
    void  A_HeadAttack(DOOM::Doom& doom);
    void  A_BruisAttack(DOOM::Doom& doom);
    void  A_SkullAttack(DOOM::Doom& doom);
    void  A_Metal(DOOM::Doom& doom);
    void  A_SpidRefire(DOOM::Doom& doom);
    void  A_BabyMetal(DOOM::Doom& doom);
    void  A_BspiAttack(DOOM::Doom& doom);
    void  A_Hoof(DOOM::Doom& doom);
    void  A_CyberAttack(DOOM::Doom& doom);
    void  A_PainAttack(DOOM::Doom& doom);
    void  A_PainShootSkull(DOOM::Doom& doom, float orientation);  // Spawn a lost soul and launch it at the target
    void  A_PainDie(DOOM::Doom& doom);
    void  A_KeenDie(DOOM::Doom& doom);
    void  A_BrainPain(DOOM::Doom& doom);
    void  A_BrainScream(DOOM::Doom& doom);
    void  A_BrainDie(DOOM::Doom& doom);
    void  A_BrainAwake(DOOM::Doom& doom);
    void  A_BrainSpit(DOOM::Doom& doom);
    void  A_SpawnSound(DOOM::Doom& doom);
    void  A_SpawnFly(DOOM::Doom& doom);
    void  A_BrainExplode(DOOM::Doom& doom);
    void  A_BFGSpray(DOOM::Doom& doom);

    void  A_Look(DOOM::Doom& doom);   // Default look action, try to find a target before running.
    void  A_Chase(DOOM::Doom& doom);  // Default movement action, move to target.

    void  A_MissileSmoke(DOOM::Doom& doom); // Spawn smoke behind the missile

    // TODO: merge these methods
    float P_AimLineAttack(DOOM::Doom& doom, const DOOM::AbstractThing& target);                                                                       // Compute visible height of target, NaN if target is not visible
    bool  P_CheckSight(DOOM::Doom& doom, const DOOM::AbstractThing& target);                                                                          // Check if target is in the line of sight of thing.
    bool  P_CheckMeleeRange(DOOM::Doom& doom);                                                                                                        // Return true if a melee attack can be performed.
    bool  P_CheckMissileRange(DOOM::Doom& doom);                                                                                                      // Return true if a missile attack can be performed.
    bool  P_CheckPosition(DOOM::Doom& doom, const Math::Vector<2>& position);                                                                         // Check destination position against linedefs and other things.
    bool  P_LookForPlayers(DOOM::Doom& doom, bool full = false);                                                                                      // Find a player to target. Return true if target found. If full is false, only look 180 degree in front.
    bool  P_Move(DOOM::Doom& doom);                                                                                                                   // Move in the current direction, return false if the move is blocked.
    void  P_NewChaseDir(DOOM::Doom& doom);                                                                                                            // Find a new direction to chase target.
    bool  P_TryMove(DOOM::Doom& doom, const Math::Vector<2>& position);                                                                               // Attempt to move thing to a new position, crossing special lines unless property Teleport is set.
    bool  P_TryWalk(DOOM::Doom& doom);                                                                                                                // Attempt to move thing in its direction. If blocked if either a wall or a thing, return false. If move is either clear or blocked only by a door, return true and sets (and open the door).
    bool  P_LineAttack(DOOM::Doom& doom, float atk_range, const Math::Vector<3>& atk_origin, const Math::Vector<3>& atk_direction, float atk_damage); // Compute an attack from current 
    void  P_LineSwitch(DOOM::Doom& doom, float swc_range, const Math::Vector<3>& swc_origin, const Math::Vector<3>& swc_direction);                   // Switch the first non obstructed linedef in range 
    void  P_SpawnMissile(DOOM::Doom& doom, DOOM::Enum::ThingType type);                                                                               // Spawn a missile from thing to target
    void  P_SpawnPlayerMissile(DOOM::Doom& doom, DOOM::Enum::ThingType type, float slope);                                                            // Spawn a missile
    void  P_ExplodeMissile(DOOM::Doom& doom);                                                                                                         // Destroy missile
    void  P_ExplodeMissile(DOOM::Doom& doom, DOOM::AbstractThing& target);                                                                            // Destroy missile on a target
    void  P_RadiusAttack(DOOM::Doom& doom, DOOM::AbstractThing& source, float damage);                                                                // Blast damage from current thing position
    void  P_SpawnPuff(DOOM::Doom& doom, const Math::Vector<3>& coordinates) const;                                                                    // Spawn a smoke puff at coordinates
    void  P_SpawnBlood(DOOM::Doom& doom, const Math::Vector<3>& coordinates, float damage) const;                                                     // Spawn blood splats at coordinates

    void  setState(DOOM::Doom& doom, DOOM::AbstractThing::ThingState state);  // Switch state

    virtual bool  pickup(DOOM::Doom& doom, DOOM::AbstractThing& item); // Try to pick-up item, return true if item shall be deleted

    void                                                                                updateState(DOOM::Doom& doom, sf::Time elapsed);                                                                                                          // Update state of thing
    void                                                                                updatePhysics(DOOM::Doom& doom, sf::Time elapsed);                                                                                                        // Update physics of thing
    void                                                                                updatePhysicsThrust(DOOM::Doom& doom, sf::Time elapsed, int depth = 0, int16_t linedef_ignored = -1, const DOOM::AbstractThing* thing_ignored = nullptr); // Update thrust component of thing
    bool                                                                                updatePhysicsThrustSidedefs(DOOM::Doom& doom, int16_t sidedef_front_index, int16_t sidedef_back_index);                                                   // Return true if thing can move through sidedefs
    std::pair<float, Math::Vector<2>>                                                   updatePhysicsThrustVertex(DOOM::Doom& doom, const Math::Vector<2>& movement, int16_t vertex_index, int16_t ignored_index);                                // Return intersection of movement with vertex (coef. along movement / normal vector)
    std::pair<float, Math::Vector<2>>                                                   updatePhysicsThrustLinedef(DOOM::Doom& doom, const Math::Vector<2>& movement, int16_t linedef_index, int16_t ignored_index);                              // Return intersection of movement with linedef (coef. along movement / normal vector)
    std::pair<float, Math::Vector<2>>                                                   updatePhysicsThrustThing(DOOM::Doom& doom, const Math::Vector<2>& movement, const DOOM::AbstractThing& thing, const DOOM::AbstractThing* ignored);        // Return intersection of movement with thing (coef. along movement / normal vector)
    std::pair<std::set<int16_t>, std::set<std::reference_wrapper<DOOM::AbstractThing>>> updatePhysicsThrustLinedefsThings(DOOM::Doom& doom, const Math::Vector<2>& movement);                                                                     // Return set of intersectable linedef indexes
    void                                                                                updatePhysicsGravity(DOOM::Doom& doom, sf::Time elapsed);                                                                                                 // Update gravity component of thing

  public:
    AbstractThing(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Thing& thing);
    AbstractThing(DOOM::Doom& doom, DOOM::Enum::ThingType type, DOOM::Enum::ThingFlag flags, float x, float y, float angle);
    virtual ~AbstractThing() = default;

    static DOOM::Enum::ThingType  id_to_type(int16_t id);                 // Convert WAD id to DOOM type
    static int16_t                type_to_id(DOOM::Enum::ThingType type); // Convert DOOM type to WAD id

    bool  teleport(DOOM::Doom& doom, const Math::Vector<2>& destination, float angle, bool telefrag = false); // Teleport thing to position (reset physics)
    void  thrust(const Math::Vector<3>& acceleration);                                                        // Apply acceleration to thing

    virtual void  damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, DOOM::AbstractThing& origin, float damage); // Receive attack from attacker caused by origin
    virtual void  damage(DOOM::Doom& doom, DOOM::AbstractThing& attacker, float damage);                              // Receive attack from attacker
    virtual void  damage(DOOM::Doom& doom, float damage);                                                             // Receive attack from no one

    struct Sprite
    {
      const DOOM::Doom::Resources::Texture& texture;
      bool                                  mirror;
      bool                                  full_brightness;
    };

    virtual bool                update(DOOM::Doom& doom, sf::Time elapsed);         // Update thing, return true if thing should be deleted
    DOOM::AbstractThing::Sprite sprite(const DOOM::Doom& doom, float angle) const;  // Return sprite to be displayed

    virtual bool  key(DOOM::Enum::KeyColor color) const;  // Return true thing thing has the key (default: false)
  };
}