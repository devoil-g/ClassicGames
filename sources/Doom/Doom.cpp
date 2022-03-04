#include "Doom/Doom.hpp"
#include "Doom/Action/BlinkLightingAction.hpp"
#include "Doom/Action/DoorLevelingAction.hpp"
#include "Doom/Action/FlickerLightingAction.hpp"
#include "Doom/Action/OscillateLightingAction.hpp"
#include "Doom/Action/RandomLightingAction.hpp"
#include "Doom/Thing/PlayerThing.hpp"

const sf::Time      DOOM::Doom::Tic = sf::seconds(1.f / 35.f);
const unsigned int  DOOM::Doom::RenderWidth = 320;
const unsigned int  DOOM::Doom::RenderHeight = 200;
unsigned int        DOOM::Doom::RenderScale = 1;
const float         DOOM::Doom::RenderStretching = 6.f / 5.f;

const std::array<DOOM::Doom::Resources::Sound::SoundInfo, DOOM::Doom::Resources::Sound::EnumSound::Sound_Number>        DOOM::Doom::Resources::Sound::sound_info = {
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "None", .singularity = false, .priority = 0, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PISTOL", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SHOTGN", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SGCOCK", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "DSHTGN", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "DBOPN", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "DBCLS", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "DBLOAD", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PLASMA", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BFG", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SAWUP", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SAWIDL", .singularity = false, .priority = 118, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SAWFUL", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SAWHIT", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "RLAUNC", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "RXPLOD", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "FIRSHT", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "FIRXPL", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PSTART", .singularity = false, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PSTOP", .singularity = false, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "DOROPN", .singularity = false, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "DORCLS", .singularity = false, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "STNMOV", .singularity = false, .priority = 119, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SWTCHN", .singularity = false, .priority = 78, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SWTCHX", .singularity = false, .priority = 78, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PLPAIN", .singularity = false, .priority = 96, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "DMPAIN", .singularity = false, .priority = 96, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "POPAIN", .singularity = false, .priority = 96, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "VIPAIN", .singularity = false, .priority = 96, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "MNPAIN", .singularity = false, .priority = 96, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PEPAIN", .singularity = false, .priority = 96, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SLOP", .singularity = false, .priority = 78, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "ITEMUP", .singularity = true, .priority = 78, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "WPNUP", .singularity = true, .priority = 78, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "OOF", .singularity = false, .priority = 96, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "TELEPT", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "POSIT1", .singularity = true, .priority = 98, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "POSIT2", .singularity = true, .priority = 98, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "POSIT3", .singularity = true, .priority = 98, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BGSIT1", .singularity = true, .priority = 98, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BGSIT2", .singularity = true, .priority = 98, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SGTSIT", .singularity = true, .priority = 98, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "CACSIT", .singularity = true, .priority = 98, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BRSSIT", .singularity = true, .priority = 94, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "CYBSIT", .singularity = true, .priority = 92, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SPISIT", .singularity = true, .priority = 90, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BSPSIT", .singularity = true, .priority = 90, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "KNTSIT", .singularity = true, .priority = 90, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "VILSIT", .singularity = true, .priority = 90, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "MANSIT", .singularity = true, .priority = 90, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PESIT", .singularity = true, .priority = 90, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SKLATK", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SGTATK", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SKEPCH", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "VILATK", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "CLAW", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SKESWG", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PLDETH", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PDIEHI", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PODTH1", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PODTH2", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PODTH3", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BGDTH1", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BGDTH2", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SGTDTH", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "CACDTH", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SKLDTH", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BRSDTH", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "CYBDTH", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SPIDTH", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BSPDTH", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "VILDTH", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "KNTDTH", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PEDTH", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SKEDTH", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "POSACT", .singularity = true, .priority = 120, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BGACT", .singularity = true, .priority = 120, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "DMACT", .singularity = true, .priority = 120, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BSPACT", .singularity = true, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BSPWLK", .singularity = true, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "VILACT", .singularity = true, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "NOWAY", .singularity = false, .priority = 78, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BAREXP", .singularity = false, .priority = 60, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "PUNCH", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "HOOF", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "METAL", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "CHGUN", .singularity = false, .priority = 64, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_pistol, .pitch = 150, .volume = 0 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "TINK", .singularity = false, .priority = 60, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BDOPN", .singularity = false, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BDCLS", .singularity = false, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "ITMBK", .singularity = false, .priority = 100, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "FLAME", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "FLAMST", .singularity = false, .priority = 32, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "GETPOW", .singularity = false, .priority = 60, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BOSPIT", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BOSCUB", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BOSSIT", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BOSPN", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "BOSDTH", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "MANATK", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "MANDTH", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SSSIT", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SSDTH", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "KEENPN", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "KEENDT", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SKEACT", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SKESIT", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "SKEATK", .singularity = false, .priority = 70, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 },
  DOOM::Doom::Resources::Sound::SoundInfo{ .name = "RADIO", .singularity = false, .priority = 60, .link = DOOM::Doom::Resources::Sound::EnumSound::Sound_None, .pitch = -1, .volume = -1 }
};

DOOM::Doom::Resources::Texture const  DOOM::Doom::Resources::Texture::Null = DOOM::Doom::Resources::Texture();

DOOM::Doom::Doom() :
  wad(),
  resources(),
  level(),
  mode(DOOM::Enum::Mode::ModeIndetermined),
  skill(DOOM::Enum::Skill::SkillMedium),
  sensivity(0.8f),
  sfx(0.125f),
  music(1.f),
  message(true),
  image()
{}

void  DOOM::Doom::load(const std::string& path, DOOM::Enum::Mode mode)
{
  // Clear resources
  clear();

  // Load WAD file 
  wad.load(path);

  // Set game mode
  this->mode = mode;

  // Build resources of WAD file
  buildResources();
}

void  DOOM::Doom::update(sf::Time elapsed)
{
  // Update components
  resources.update(*this, elapsed);
  level.update(*this, elapsed);

  // Sound centered on the player in single player
  if (level.players.size() == 1) {
    sf::Listener::setPosition(
      level.players.front().get().position.x(),
      level.players.front().get().position.y(),
      level.players.front().get().position.z());
    sf::Listener::setDirection(
      std::cos(level.players.front().get().angle),
      std::sin(level.players.front().get().angle),
      std::tan(level.players.front().get().camera.orientation));
    sf::Listener::setUpVector(0.f, 0.f, 1.f);
    sf::Listener::setGlobalVolume(100.f);
  }

  // Sound zero centred in multiplayer
  else {
    sf::Listener::setPosition(0.f, 0.f, 0.f);
    sf::Listener::setDirection(1.f, 0.f, 0.f);
    sf::Listener::setUpVector(0.f, 0.f, 1.f);
    sf::Listener::setGlobalVolume(100.f);
  }
}

std::list<std::pair<uint8_t, uint8_t>>  DOOM::Doom::getLevels() const
{
  std::list<std::pair<uint8_t, uint8_t>>  list;

  // Build list of available levels in WAD file
  for (const std::pair<std::pair<uint8_t, uint8_t>, DOOM::Wad::RawLevel>& level : wad.levels)
    list.emplace_back(level.first);

  return list;
}

void  DOOM::Doom::setLevel(std::pair<uint8_t, uint8_t> level, bool reset)
{
  // Build level
  buildLevel(level);

  // Reset players
  for (const auto& player : this->level.players)
    player.get().reset(*this, reset);
}

void  DOOM::Doom::addPlayer(int controller)
{
  // Cancel if invalid controller id
  if (controller < 0)
    return;

  // Cancel if controller already registered
  for (const auto& player : level.players)
    if (player.get().controller == controller)
      return;

  // Push new player
  level.things.push_back(std::make_unique<DOOM::PlayerThing>(*this, (int)level.players.size() + 1, controller));
}

void  DOOM::Doom::sound(DOOM::Doom::Resources::Sound::EnumSound sound, bool loop)
{
  auto ref = Game::Audio::Sound::Instance().get();

  this->sound(ref, sound, loop);
}

void  DOOM::Doom::sound(DOOM::Doom::Resources::Sound::EnumSound sound, const Math::Vector<3>& position, bool loop)
{
  auto ref = Game::Audio::Sound::Instance().get();

  this->sound(ref, sound, position, loop);
}

void  DOOM::Doom::sound(Game::Audio::Sound::Reference& ref, DOOM::Doom::Resources::Sound::EnumSound sound, bool loop)
{
  // Does nothing if no sound
  if (sound == DOOM::Doom::Resources::Sound::EnumSound::Sound_None) {
    ref.sound.stop();
    return;
  }

  std::unordered_map<uint64_t, DOOM::Doom::Resources::Sound>::const_iterator  iterator = resources.sounds.find(Game::Utilities::str_to_key<uint64_t>(std::string("DS") + DOOM::Doom::Resources::Sound::sound_info[sound].name));

  // Cancel if no sound found
  if (iterator == resources.sounds.cend())
    return;

  // Play sound
  ref.sound.setBuffer(iterator->second.buffer);
  ref.sound.setRelativeToListener(true);
  ref.sound.setVolume(sfx * 100.f);
  ref.sound.setLoop(loop);
  ref.sound.play();
}

void  DOOM::Doom::sound(Game::Audio::Sound::Reference& ref, DOOM::Doom::Resources::Sound::EnumSound sound, const Math::Vector<3>& position, bool loop)
{
  // Does nothing if no sound
  if (sound == DOOM::Doom::Resources::Sound::EnumSound::Sound_None) {
    ref.sound.stop();
    return;
  }

  auto  iterator = resources.sounds.find(Game::Utilities::str_to_key<uint64_t>(std::string("DS") + DOOM::Doom::Resources::Sound::sound_info[sound].name));

  // Cancel if no sound found
  if (iterator == resources.sounds.cend())
    return;

  // Set sound properties
  // NOTE: a sound should be heard from a maximum distance of 1200 units
  ref.sound.setBuffer(iterator->second.buffer);
  ref.sound.setRelativeToListener(false);
  ref.sound.setAttenuation(3.2f);
  ref.sound.setVolume(sfx * 100.f);
  ref.sound.setMinDistance(256.f);
  ref.sound.setLoop(loop);

  // Single player
  if (level.players.size() == 1) {
    ref.sound.setPosition(position.x(), position.y(), position.z());
  }

  // Multiplayer
  else {
    float distance = std::numeric_limits<float>::max();

    // Get smallest distance from a player
    for (const auto& player : level.players)
      distance = std::min(distance, (player.get().position - position).length());

    ref.sound.setPosition(distance, 0.f, 0.f);
  }

  ref.sound.play();
}

void  DOOM::Doom::clear()
{
  // Clear resources and current level
  clearResources();
  clearLevel();
}

void  DOOM::Doom::clearResources()
{
  // Clear level to avoid reference to deleted datas
  clearLevel();

  // Force deletion of players
  level.players.clear();
  level.things.clear();

  // Clear resources data containers
  resources.palettes = std::array<DOOM::Doom::Resources::Palette, 14>();
  resources.colormaps = std::array<DOOM::Doom::Resources::Colormap, 34>();
  resources.flats.clear();
  resources.textures.clear();
  resources.sprites.clear();
  resources.menus.clear();
  resources.sounds.clear();
}

void  DOOM::Doom::clearLevel()
{
  // Reset level base info
  level.episode = { 0, 0 };
  level.end = DOOM::Enum::End::EndNone;
  level.sky = std::ref(DOOM::Doom::Resources::Texture::Null);

  // Remove all things except players
  level.things.remove_if([](const std::unique_ptr<DOOM::AbstractThing>& ptr) { return dynamic_cast<DOOM::PlayerThing*>(ptr.get()) == nullptr; });

  // Reset level components
  level.linedefs.clear();
  level.sidedefs.clear();
  level.vertexes.clear();
  level.segments.clear();
  level.subsectors.clear();
  level.nodes.clear();
  level.sectors.clear();
  level.blockmap = DOOM::Doom::Level::Blockmap();
  level.statistics = DOOM::Doom::Level::Statistics();
}

void  DOOM::Doom::buildResources()
{
  // Remove old resources and level
  clear();

  // Build every component of resources
  try
  {
    buildResourcesPalettes();
    buildResourcesColormaps();
    buildResourcesTextures();
    buildResourcesSprites();
    buildResourcesMenus();
    buildResourcesFlats();
    buildResourcesSounds();
  }
  catch (const std::exception& e)
  {
    clear();
    throw std::runtime_error(e.what());
  }

  // Update components (might be useful for initializations)
  resources.update(*this, sf::Time::Zero);
}

void  DOOM::Doom::buildResourcesPalettes()
{
  // Check palettes data
  if (wad.resources.palettes.size() != 14)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load palettes from WAD resources
  for (unsigned int index = 0; index < 14; index++)
    resources.palettes[index] = DOOM::Doom::Resources::Palette(*this, wad.resources.palettes[index]);
}

void  DOOM::Doom::buildResourcesColormaps()
{
  // Check color maps data
  if (wad.resources.colormaps.size() != 34)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Load color maps from WAD resources
  for (unsigned int index = 0; index < 34; index++)
    resources.colormaps[index] = DOOM::Doom::Resources::Colormap(*this, wad.resources.colormaps[index]);
}

void  DOOM::Doom::buildResourcesTextures()
{
  // Load textures from WAD resources
  for (const auto& texture : wad.resources.textures)
    resources.textures.emplace(std::piecewise_construct, std::forward_as_tuple(texture.first), std::forward_as_tuple(*this, texture.second));
}

void  DOOM::Doom::buildResourcesSprites()
{
  // Load sprites textures from WAD resources
  for (const auto& patch : wad.resources.sprites) {
    std::string name = Game::Utilities::key_to_str(patch.first);

    // Build sprite and get its reference
    auto  sprite = std::ref(resources.sprites.emplace(std::piecewise_construct, std::forward_as_tuple(patch.first), std::forward_as_tuple(*this, patch.second)).first->second);

    if (name.length() >= 6)
    {
      // Resize animation sequence
      if (resources.animations[Game::Utilities::str_to_key<uint64_t>(name.substr(0, 4))].size() < name.at(4) - 'A' + 1)
        resources.animations[Game::Utilities::str_to_key<uint64_t>(name.substr(0, 4))].resize(name.at(4) - 'A' + 1, { {
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false }
          } });

      // Push sprite in animation sequence
      if (name.at(5) == '0')
        for (auto& frame : resources.animations[Game::Utilities::str_to_key<uint64_t>(name.substr(0, 4))][name.at(4) - 'A'])
          frame = { sprite, false };
      else
        resources.animations[Game::Utilities::str_to_key<uint64_t>(name.substr(0, 4))][name.at(4) - 'A'][name.at(5) - '1'] = { sprite, false };
    }

    if (name.length() >= 8)
    {
      // Resize animation sequence
      if (resources.animations[Game::Utilities::str_to_key<uint64_t>(name.substr(0, 4))].size() < name.at(6) - 'A' + 1)
        resources.animations[Game::Utilities::str_to_key<uint64_t>(name.substr(0, 4))].resize(name.at(6) - 'A' + 1, { {
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false },
          { std::ref(DOOM::Doom::Resources::Texture::Null), false }
          } });

      // Push sprite in animation sequence
      if (name.at(7) == '0')
        for (auto& frame : resources.animations[Game::Utilities::str_to_key<uint64_t>(name.substr(0, 4))][name.at(6) - 'A'])
          frame = { sprite, true };
      else
        resources.animations[Game::Utilities::str_to_key<uint64_t>(name.substr(0, 4))][name.at(6) - 'A'][name.at(7) - '1'] = { sprite, true };
    }
  }
}

void  DOOM::Doom::buildResourcesMenus()
{
  // Load menus textures from WAD resources
  for (const auto& menu : wad.resources.menus)
    resources.menus.emplace(std::piecewise_construct, std::forward_as_tuple(menu.first), std::forward_as_tuple(*this, menu.second));
}

void  DOOM::Doom::buildResourcesFlats()
{
  // Load flats from WAD resources
  for (const auto& flat : wad.resources.flats)
  {
    // Convert flat from WAD
    auto  converted = DOOM::AbstractFlat::factory(*this, flat.first, flat.second);

    // Check for error
    if (converted.get() == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    else
      resources.flats[flat.first] = std::move(converted);
  }
}

void  DOOM::Doom::buildResourcesSounds()
{
  // Load sounds from WAD resources
  for (const auto& sound : wad.resources.sounds)
    resources.sounds.emplace(std::piecewise_construct, std::forward_as_tuple(sound.first), std::forward_as_tuple(*this, sound.second));
}

void  DOOM::Doom::buildLevel(const std::pair<uint8_t, uint8_t>& level)
{
  // Remove old level
  clearLevel();

  // Check if level exist in WAD file
  if (wad.levels.find(level) == wad.levels.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Check for sky texture
  if (resources.textures.find(0x0000000000594B53 | (((int64_t)level.first + '0') << 24)) == resources.textures.end())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  this->level.episode = level;
  this->level.end = DOOM::Enum::End::EndNone;
  this->level.sky = std::cref(resources.textures.find(Game::Utilities::str_to_key<uint64_t>(std::string("SKY") + std::to_string((int)level.first)))->second);

  // DOOM 2 sky
  if (mode == DOOM::Enum::Mode::ModeCommercial) {
    if (level.second < 12)
      this->level.sky = std::cref(resources.textures.find(Game::Utilities::str_to_key<uint64_t>("SKY1"))->second);
    else if (level.second < 21)
      this->level.sky = std::cref(resources.textures.find(Game::Utilities::str_to_key<uint64_t>("SKY2"))->second);
    else
      this->level.sky = std::cref(resources.textures.find(Game::Utilities::str_to_key<uint64_t>("SKY3"))->second);
  }

  // Build every component of resources
  try
  {
    buildLevelVertexes();
    buildLevelSectors();
    buildLevelLinedefs();
    buildLevelSidedefs();
    buildLevelSubsectors();
    buildLevelSegments();
    buildLevelNodes();
    buildLevelBlockmap();
    buildLevelThings();
    buildLevelStatistics();
  }
  catch (const std::exception& e)
  {
    clearLevel();
    throw std::runtime_error(e.what());
  }

  // Update components (might be useful for initializations)
  this->level.update(*this, sf::Time::Zero);
}

void  DOOM::Doom::buildLevelVertexes()
{
  // Load level's vertexes from WAD
  for (const auto& vertex : wad.levels[level.episode].vertexes)
    level.vertexes.emplace_back(*this, vertex);
}

void  DOOM::Doom::buildLevelSectors()
{
  // Load level's sectors from WAD
  for (const auto& sector : wad.levels[level.episode].sectors)
    level.sectors.emplace_back(*this, sector);
}

void  DOOM::Doom::buildLevelSubsectors()
{
  // Load level's sectors from WAD
  for (const auto& subsector : wad.levels[level.episode].subsectors)
    level.subsectors.emplace_back(*this, subsector);
}

void  DOOM::Doom::buildLevelLinedefs()
{
  // Load level's linedefs from WAD
  for (const auto& linedef : wad.levels[level.episode].linedefs)
  {
    // Convert linedef from WAD
    auto  converted = DOOM::AbstractLinedef::factory(*this, linedef);

    // Check for error
    if (converted.get() == nullptr)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    else
      level.linedefs.emplace_back(std::move(converted));
  }
}

void  DOOM::Doom::buildLevelSidedefs()
{
  // Load level's sidedefs from WAD
  for (const auto& sidedef : wad.levels[level.episode].sidedefs)
    level.sidedefs.emplace_back(*this, sidedef);
}

void  DOOM::Doom::buildLevelSegments()
{
  // Load level's segments from WAD
  for (const auto& segment : wad.levels[level.episode].segments)
    level.segments.emplace_back(*this, segment);
}

void  DOOM::Doom::buildLevelNodes()
{
  // Load level's nodes from WAD
  for (const auto& node : wad.levels[level.episode].nodes)
    level.nodes.emplace_back(*this, node);
}

void  DOOM::Doom::buildLevelThings()
{
  // Load level's things from WAD
  for (const auto& thing : wad.levels[level.episode].things)
  {
    // Only build thing of current skill level
    if (((thing.flag & DOOM::Enum::ThingFlag::FlagMultiplayer) == 0 &&
      (((skill == DOOM::Enum::Skill::SkillBaby || skill == DOOM::Enum::Skill::SkillEasy) && (thing.flag & DOOM::Enum::ThingFlag::FlagSkillLevel12)) ||
        ((skill == DOOM::Enum::Skill::SkillMedium) && (thing.flag & DOOM::Enum::ThingFlag::FlagSkillLevel3)) ||
        ((skill == DOOM::Enum::Skill::SkillHard || skill == DOOM::Enum::Skill::SkillNightmare) && (thing.flag & DOOM::Enum::ThingFlag::FlagSkillLevel45)))) ||
      // NOTE: ugly fix for when spawn difficulty setting is not given
      (DOOM::AbstractThing::id_to_type(thing.type) == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN1 ||
        DOOM::AbstractThing::id_to_type(thing.type) == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN2 ||
        DOOM::AbstractThing::id_to_type(thing.type) == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN3 ||
        DOOM::AbstractThing::id_to_type(thing.type) == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWN4 ||
        DOOM::AbstractThing::id_to_type(thing.type) == DOOM::Enum::ThingType::ThingType_PLAYER_SPAWNDM))
    {
      // Convert thing from WAD
      auto  converted = DOOM::AbstractThing::factory(*this, thing);

      // Check for error
      if (converted.get() == nullptr)
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
      
      // Push thing in level
      level.things.emplace_back(std::move(converted));
    }
  }
}

void  DOOM::Doom::buildLevelBlockmap()
{
  // Convert WAD bockmap
  level.blockmap = DOOM::Doom::Level::Blockmap(*this, wad.levels[level.episode].blockmap);
}

void  DOOM::Doom::buildLevelStatistics()
{
  // Reset counters
  level.statistics = DOOM::Doom::Level::Statistics();

  // Count monsters and items
  for (const auto& thing : level.things) {
    if (thing.get()->flags & DOOM::Enum::ThingProperty::ThingProperty_CountKill)
      level.statistics.total.kills += 1;
    if (thing.get()->flags & DOOM::Enum::ThingProperty::ThingProperty_CountItem)
      level.statistics.total.items += 1;
  }

  // Count secret sectors
  for (const auto& sector : level.sectors) {
    if (sector.special == DOOM::Doom::Level::Sector::Special::Secret)
      level.statistics.total.secrets += 1;
  }

  // Initialize player counters
  for (const auto& player : level.players)
    level.statistics.players[player.get().id] = DOOM::Doom::Level::Statistics::Stats();
}

void  DOOM::Doom::Resources::update(DOOM::Doom& doom, sf::Time elapsed)
{
  // Update resources flats
  for (const auto& flat : flats)
    flat.second->update(doom, elapsed);
}

void  DOOM::Doom::Level::update(DOOM::Doom& doom, sf::Time elapsed)
{
  // Update level linedef
  for (const auto& linedef : linedefs)
    linedef->update(doom, elapsed);

  // Update level sectors
  for (auto& sector : sectors)
    sector.update(doom, elapsed);

  // Update level sidedef
  for (auto& sidedef : sidedefs)
    sidedef.update(doom, elapsed);

  // Update level things
  for (auto iterator = things.begin(); iterator != things.end(); )
    // Remove thing if update return true
    if (iterator->get()->update(doom, elapsed) == true) {
      blockmap.removeThing(*iterator->get(), iterator->get()->position.convert<2>());
      iterator = things.erase(iterator);
    }
    else {
      iterator++;
    }

  // Update level statistics
  statistics.update(doom, elapsed);
}

std::set<int16_t> DOOM::Doom::Level::getSectors(const Math::Vector<2>& position, float radius) const
{
  // No sector
  if (sectors.empty() == true)
    return {};

  std::set<int16_t> result;

  // Get sector at thing central position
  result.insert(getSector(position).first);

  // Return nothing if thing is outside of level
  if (*result.cbegin() == -1)
    return {};

  // Get near linedef using blockmap
  std::set<int16_t> blocks;
  std::set<int16_t> linedefs;

  // Get blocks thing stand in
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() - radius, position.y() - radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() - radius, position.y() + radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() + radius, position.y() - radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() + radius, position.y() + radius)));

  // Get index of linedefs to test againt position
  for (int16_t block_index : blocks)
    if (block_index != -1) {
      const auto& block = blockmap.blocks[block_index];

      linedefs.insert(block.linedefs.begin(), block.linedefs.end());
    }

  // Check for intersection with each linedef
  for (int16_t linedef_index : linedefs) {
    const auto& linedef = *this->linedefs[linedef_index].get();
    const auto& linedef_start = vertexes[linedef.start];
    const auto& linedef_end = vertexes[linedef.end];

    // Get closest point to thing along linedef
    float s = std::clamp(-((linedef_start.x() - position.x()) * (linedef_end.x() - linedef_start.x()) + (linedef_start.y() - position.y()) * (linedef_end.y() - linedef_start.y())) / (Math::Pow<2>(linedef_end.x() - linedef_start.x()) + Math::Pow<2>(linedef_end.y() - linedef_start.y())), 0.f, 1.f);

    // Add linedef sectors to result if intersecting with thing bounds
    if ((linedef_start + (linedef_end - linedef_start) * s - position).length() < radius) {
      result.insert(sidedefs[linedef.front].sector);
      if (linedef.back != -1)
        result.insert(sidedefs[linedef.back].sector);
    }
  }

  return result;
}

std::set<int16_t> DOOM::Doom::Level::getSectors(const DOOM::AbstractThing& thing) const
{
  // Only half of thing radius is considered
  return getSectors(thing.position.convert<2>(), thing.attributs.radius / 2.f);
}

std::pair<int16_t, int16_t> DOOM::Doom::Level::getSector(const Math::Vector<2>& position, int16_t index) const
{
  // No node
  if (nodes.empty() == true)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Start to search sector from top node
  if (index == -1)
    return getSector(position, (int16_t)nodes.size() - 1);

  // Return subsector's sector if node ID has subsector mask
  if (index & 0b1000000000000000)
    return { subsectors[index & 0b0111111111111111].sector, index & 0b0111111111111111 };

  const auto& node(nodes[index]);

  // Use derterminant to find on which side the position is
  if (Math::Vector<2>::determinant(node.direction, position - node.origin) > 0.f)
    return getSector(position, node.leftchild);
  else
    return getSector(position, node.rightchild);
}

std::list<std::pair<float, int16_t>>  DOOM::Doom::Level::getLinedefs(const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit) const
{
  std::list<std::pair<float, int16_t>>  result;

  // Start to search subsector from top node
  getLinedefsNode(result, position, direction, limit, (int16_t)nodes.size() - 1);

  result.sort([](const std::pair<float, int16_t>& a, const std::pair<float, int16_t>& b) { return a.first < b.first; });

  return result;
}

bool  DOOM::Doom::Level::getLinedefsNode(std::list<std::pair<float, int16_t>>& result, const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit, int16_t index) const
{
  // Draw subsector if node ID has subsector mask
  if (index & 0b1000000000000000)
    return getLinedefsSubsector(result, position, direction, limit, index & 0b0111111111111111);

  const auto& node(nodes[index]);

  // Use derterminant to find on which side the position is
  if (Math::Vector<2>::determinant(node.direction, position - node.origin) > 0.f)
    return getLinedefsNode(result, position, direction, limit, node.leftchild) == true ||
    (Math::Vector<2>::determinant(node.origin - position, node.direction) / Math::Vector<2>::determinant(direction, node.direction) >= 0.f && getLinedefsNode(result, position, direction, limit, node.rightchild) == true);
  else
    return getLinedefsNode(result, position, direction, limit, node.rightchild) == true ||
    (Math::Vector<2>::determinant(node.origin - position, node.direction) / Math::Vector<2>::determinant(direction, node.direction) >= 0.f && getLinedefsNode(result, position, direction, limit, node.leftchild) == true);
}

bool  DOOM::Doom::Level::getLinedefsSubsector(std::list<std::pair<float, int16_t>>& result, const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit, int16_t index) const
{
  const auto& subsector(subsectors[index]);
  float       distance = -1.f;

  // Render subsector segs
  for (int16_t i = 0; i < subsector.count; i++)
    distance = std::max(distance, getLinedefsSeg(result, position, direction, limit, subsector.index + i));

  // Return true if limit has been reached
  return distance > limit;
}

float DOOM::Doom::Level::getLinedefsSeg(std::list<std::pair<float, int16_t>>& result, const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit, int16_t index) const
{
  // Get segment from level data
  const auto& seg(segments[index]);
  const auto& seg_start(vertexes[seg.start]);
  const auto& seg_end(vertexes[seg.end]);

  // Compute intersection of ray with segment
  std::pair<float, float> intersection(Math::intersection(position, direction, seg_start, seg_end - seg_start));

  // Check if segment is intersected in acceptable bounds
  if (std::isnan(intersection.first) == true || std::isnan(intersection.second) == true || intersection.first < 0.f || intersection.second < 0.f || intersection.second > 1.f)
    return -1.f;

  // Limit reached
  if (intersection.first > limit)
    return intersection.first;

  // Push linedef index as result
  result.push_back({ intersection.first, seg.linedef });

  // Return segment distance
  return intersection.first;
}

std::set<int16_t> DOOM::Doom::Level::getLinedefs(const Math::Vector<2>& position, float radius) const
{
  std::set<int16_t> blocks;

  // Get blockmap index at position, using the four corners
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() - (float)radius, position.y() - (float)radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() - (float)radius, position.y() + (float)radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() + (float)radius, position.y() - (float)radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() + (float)radius, position.y() + (float)radius)));

  std::set<int16_t> linedefs;

  // Get index of linedefs to test against position
  for (int16_t block_index : blocks)
    if (block_index != -1) {
      const auto& block = blockmap.blocks[block_index];

      linedefs.insert(block.linedefs.begin(), block.linedefs.end());
    }

  // Only keep intersected linedef
  for (auto iterator = linedefs.begin(); iterator != linedefs.end();) {
    const auto&     linedef = *this->linedefs[*iterator];
    const auto&     linedef_start = vertexes[linedef.start];
    const auto&     linedef_end = vertexes[linedef.end];
    Math::Vector<2> linedef_direction = linedef_end - linedef_start;
    Math::Vector<2> linedef_normal(+linedef_direction.y(), -linedef_direction.x());

    std::pair<float, float> intersection = Math::intersection(position, linedef_normal / linedef_normal.length(), linedef_start, linedef_direction);

    if ((std::abs(intersection.first) < radius && intersection.second > 0.f && intersection.second < 1.f) ||
      (position - vertexes[linedef.start]).length() < radius ||
      (position - vertexes[linedef.end]).length() < radius)
      iterator++;
    else
      iterator = linedefs.erase(iterator);
  }


  // Return set of intersectable linedef
  return linedefs;
}

std::set<std::reference_wrapper<DOOM::AbstractThing>> DOOM::Doom::Level::getThings(const Math::Vector<2>& position, float radius) const
{
  std::set<int16_t> blocks;

  // Get blockmap index at position, using the four corners
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() - (float)radius, position.y() - (float)radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() - (float)radius, position.y() + (float)radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() + (float)radius, position.y() - (float)radius)));
  blocks.insert(blockmap.index(Math::Vector<2>(position.x() + (float)radius, position.y() + (float)radius)));

  std::set<std::reference_wrapper<DOOM::AbstractThing>> things;

  // Get index of linedefs to test against position
  for (int16_t block_index : blocks)
    if (block_index != -1) {
      const auto& block = blockmap.blocks[block_index];

      things.insert(block.things.begin(), block.things.end());
    }

  // Only keep intersected things
  for (auto iterator = things.begin(); iterator != things.end();) {
    if ((position - iterator->get().position.convert<2>()).length() < radius + iterator->get().attributs.radius)
      iterator++;
    else
      iterator = things.erase(iterator);
  }

  // Return set of intersectable things
  return things;
}

std::list<std::reference_wrapper<DOOM::AbstractThing>>  DOOM::Doom::Level::getThings(const DOOM::Doom::Level::Sector& sector, DOOM::Enum::ThingProperty properties) const
{
  std::set<DOOM::AbstractThing*>  things;

  // Iterate blocks of sectors
  for (int block_index : blockmap.sectors.find(&sector)->second)
  {
    // Check if things have correct properties before testing against linedefs
    for (const auto& thing : blockmap.blocks[block_index].things) {
      if ((thing.get().attributs.properties & properties) == properties && things.find(&(thing.get())) == things.end())
      {
        // Check if things center stand in sector
        if (&sectors[getSector(thing.get().position.convert<2>()).first] == &sector) {
          things.insert(&(thing.get()));
          continue;
        }

        // Test if thing bounds intersect with a linedef of the sector
        for (int16_t linedef_index : blockmap.blocks[block_index].linedefs) {
          const auto& linedef = *linedefs[linedef_index].get();

          // Only test thing against linedef of sector
          if (&sectors[sidedefs[linedef.front].sector] == &sector || (linedef.back != -1 && &sectors[sidedefs[linedef.back].sector] == &sector)) {
            const auto& linedef_start = vertexes[linedef.start];
            const auto& linedef_end = vertexes[linedef.end];

            // Get closest point to thing along linedef
            float s = std::clamp(-((linedef_start.x() - thing.get().position.x()) * (linedef_end.x() - linedef_start.x()) + (linedef_start.y() - thing.get().position.y()) * (linedef_end.y() - linedef_start.y())) / (Math::Pow<2>(linedef_end.x() - linedef_start.x()) + Math::Pow<2>(linedef_end.y() - linedef_start.y())), 0.f, 1.f);

            // Add linedef sectors to result if intersecting with thing bounds
            if ((linedef_start + (linedef_end - linedef_start) * s - thing.get().position.convert<2>()).length() < thing.get().attributs.radius / 2.f) {
              things.insert(&(thing.get()));
              break;
            }
          }
        }
      }
    }
  }

  std::list<std::reference_wrapper<DOOM::AbstractThing>>  result;

  // Convert things set to reference list
  for (auto thing : things)
    result.push_back(*thing);

  return result;
}

std::list<std::pair<float, std::reference_wrapper<DOOM::AbstractThing>>>  DOOM::Doom::Level::getThings(const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit) const
{
  // TODO: optimize this using blockmap ?
  std::list<std::pair<float, std::reference_wrapper<DOOM::AbstractThing>>>  result;

  // Test every things
  for (const auto& thing : things)
  {
    float a = Math::Pow<2>(direction.x()) + Math::Pow<2>(direction.y());
    float b = 2.f * (((position.x() - thing->position.x()) * direction.x()) + (position.y() - thing->position.y()) * direction.y());
    float c = Math::Pow<2>((position.x() - thing->position.x())) + Math::Pow<2>((position.y() - thing->position.y())) - Math::Pow<2>((float)thing->attributs.radius);

    float delta = Math::Pow<2>(b) - 4.f * a * c;

    if (delta < 0)
      continue;

    float x1 = (-b - std::sqrt(delta)) / (2.f * a);
    if (x1 >= 0.f && x1 <= limit) {
      result.push_back({ x1, *thing.get() });
      continue;
    }

    float x2 = (-b + std::sqrt(delta)) / (2.f * a);
    if (x2 >= 0.f && x2 <= limit) {
      result.push_back({ x2, *thing.get() });
      continue;
    }
  }

  // Sort list of things by distance
  result.sort([](const std::pair<float, std::reference_wrapper<DOOM::AbstractThing>>& a, const std::pair<float, std::reference_wrapper<DOOM::AbstractThing>>& b) { return a.first < b.first; });

  return result;
}

DOOM::Doom::Resources::Palette::Palette(DOOM::Doom& doom, const DOOM::Wad::RawResources::Palette& palette) :
  std::array<sf::Color, 256>()
{
  // Convert WAD color to sf::Color
  for (unsigned int index = 0; index < 256; index++)
    at(index) = sf::Color(palette.colors[index].r, palette.colors[index].g, palette.colors[index].b);
}

DOOM::Doom::Resources::Colormap::Colormap(DOOM::Doom& doom, const DOOM::Wad::RawResources::Colormap& colormap) :
  std::array<uint8_t, 256>()
{
  // Convert color map indexes
  for (unsigned int index = 0; index < 256; index++)
    at(index) = colormap.index[index];
}

DOOM::Doom::Resources::Texture::Texture(DOOM::Doom& doom, const DOOM::Wad::RawResources::Texture& texture) :
  width(texture.width),
  height(texture.height),
  left(0),
  top(0),
  columns()
{
  // Initialize full texture map (-1 for transparency)
  std::vector<std::vector<int>>        texture_map(width, std::vector<int>(height, -1));

  // Build full texture map from texture patches
  for (const auto& texture_patch : texture.patches)
  {
    // Check patch datas
    if (texture_patch.colormap != 0 ||
      texture_patch.stepdir != 1 ||
      texture_patch.pname >= doom.wad.resources.pnames.size() ||
      doom.wad.resources.patches.find(doom.wad.resources.pnames[texture_patch.pname]) == doom.wad.resources.patches.end())
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Get patch from WAD
    const auto& patch = doom.wad.resources.patches[doom.wad.resources.pnames[texture_patch.pname]];

    // Print patch on full texture map
    for (int x = std::max(0, -patch.left); x < std::min((int)patch.width, width - texture_patch.x); x++)
      for (const auto& span : patch.columns[x].spans)
        for (int y = std::max(0, -(texture_patch.y + span.offset)); y < std::min((int)span.pixels.size(), height - (texture_patch.y + span.offset)); y++)
          if (x + texture_patch.x >= 0 && x + texture_patch.x < width && y + texture_patch.y + span.offset >= 0 && y + texture_patch.y + span.offset < height)
            texture_map[x + texture_patch.x][y + texture_patch.y + span.offset] = span.pixels[y];
  }

  // Allocate number of column of texture according to its width
  columns.resize(width);

  // Iterate through every pixel of the texture
  for (int x = 0; x < width; x++)
    for (int y = 0; y < height;)
    {
      // Ignore pixel if transparent
      if (texture_map[x][y] == -1)
        y++;

      // Add column span if not transparent
      else
      {
        columns[x].spans.push_back(DOOM::Doom::Resources::Texture::Column::Span());
        columns[x].spans.back().offset = y;

        // Push whole span of pixels in column span
        for (; y < height && texture_map[x][y] != -1; y++)
          columns[x].spans.back().pixels.push_back(texture_map[x][y]);
      }
    }
}

DOOM::Doom::Resources::Texture::Texture(DOOM::Doom& doom, const DOOM::Wad::RawResources::Patch& patch) :
  width(patch.width),
  height(patch.height),
  left(patch.left),
  top(patch.top),
  columns()
{
  // Copy texture data structures
  for (const auto& column : patch.columns)
  {
    columns.push_back(DOOM::Doom::Resources::Texture::Column());
    for (const auto& span : column.spans)
    {
      columns.back().spans.push_back(DOOM::Doom::Resources::Texture::Column::Span());
      columns.back().spans.back().offset = span.offset;
      for (const uint8_t pixel : span.pixels)
        columns.back().spans.back().pixels.push_back(pixel);
    }
  }
}

sf::Image DOOM::Doom::Resources::Texture::image(const DOOM::Doom& doom) const
{
  sf::Image image;

  // Draw texture
  image.create(width, height);
  draw(doom, image, { 0, 0 }, { 1, 1 });

  return image;
}

void  DOOM::Doom::Resources::Texture::draw(const DOOM::Doom& doom, sf::Image& image, sf::Vector2i position, sf::Vector2i scale, int16_t palette) const
{
  draw(doom, image, sf::Rect<int16_t>(0, 0, image.getSize().x, image.getSize().y), position, scale, palette);
}

void  DOOM::Doom::Resources::Texture::draw(const DOOM::Doom& doom, sf::Image& image, sf::Rect<int16_t> area, sf::Vector2i position, sf::Vector2i scale, int16_t palette) const
{
  // NOTE: optimize this?

  // Draw texture
  for (int texture_x = 0; texture_x < width; texture_x++)
    for (const auto& span : columns.at(texture_x).spans)
      for (int texture_y = 0; texture_y < span.pixels.size(); texture_y++) {
        sf::Color color = doom.resources.palettes[palette][doom.resources.colormaps[0][span.pixels[texture_y]]];

        for (int image_x = std::max(std::max(0, (int)area.left), position.x + (texture_x - left + 0) * scale.x); image_x < std::min(std::min((int)image.getSize().x, area.left + area.width), position.x + (texture_x - left + 1) * scale.x); image_x++)
          for (int image_y = std::max(std::max(0, (int)area.top), position.y + (span.offset + texture_y - top + 0) * scale.y); image_y < std::min(std::min((int)image.getSize().y, area.top + area.height), position.y + (span.offset + texture_y - top + 1) * scale.y); image_y++)
            image.setPixel(image_x, image_y, color);
      }
}

DOOM::Doom::Resources::Sound::Sound(DOOM::Doom& doom, const DOOM::Wad::RawResources::Sound& raw) :
  buffer()
{
  std::vector<int16_t>  converted;

  // Convert uint8 format to int16
  for (const uint8_t sample : raw.buffer)
    converted.push_back((int32_t)sample * 256 - 32768);

  // Load sound buffer (mono)
  if (buffer.loadFromSamples(converted.data(), raw.samples, 1, raw.rate) == false)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

DOOM::Doom::Level::Level() :
  episode(0, 0),
  end(DOOM::Enum::End::EndNone),
  sky(DOOM::Doom::Resources::Texture::Null),
  things(),
  linedefs(),
  sidedefs(),
  vertexes(),
  segments(),
  subsectors(),
  nodes(),
  sectors(),
  blockmap()
{}

DOOM::Doom::Level::Vertex::Vertex(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Vertex& vertex) :
  Math::Vector<2>((float)vertex.x, (float)vertex.y)
{}

DOOM::Doom::Level::Sidedef::Sidedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Sidedef& sidedef) :
  x((float)sidedef.x), y((float)sidedef.y), sector(sidedef.sector), _elapsed(),
  _upper_name(sidedef.upper),
  _lower_name(sidedef.lower),
  _middle_name(sidedef.middle),
  _upper_textures(animation(doom, sidedef.upper)),
  _lower_textures(animation(doom, sidedef.lower)),
  _middle_textures(animation(doom, sidedef.middle))
{
  // Check for errors
  if (sidedef.sector < 0 || sidedef.sector >= doom.wad.levels[doom.level.episode].sectors.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>> DOOM::Doom::Level::Sidedef::animation(const DOOM::Doom& doom, uint64_t name) const
{
  // Check for null texture
  if (name == Game::Utilities::str_to_key<uint64_t>("-"))
    return { DOOM::Doom::Resources::Texture::Null };

  // List of registered animations
  const static std::vector<std::vector<uint64_t>> animations = {
    { Game::Utilities::str_to_key<uint64_t>("BLODGR1"), Game::Utilities::str_to_key<uint64_t>("BLODGR2"), Game::Utilities::str_to_key<uint64_t>("BLODGR3"), Game::Utilities::str_to_key<uint64_t>("BLODGR4") },
    { Game::Utilities::str_to_key<uint64_t>("BLODRIP1"), Game::Utilities::str_to_key<uint64_t>("BLODRIP2"), Game::Utilities::str_to_key<uint64_t>("BLODRIP3"), Game::Utilities::str_to_key<uint64_t>("BLODRIP4") },
    { Game::Utilities::str_to_key<uint64_t>("FIREBLU1"), Game::Utilities::str_to_key<uint64_t>("FIREBLU2") },
    { Game::Utilities::str_to_key<uint64_t>("FIRLAV3"), Game::Utilities::str_to_key<uint64_t>("FIRLAVA") },
    { Game::Utilities::str_to_key<uint64_t>("FIREWALA"), Game::Utilities::str_to_key<uint64_t>("FIREWALB"), Game::Utilities::str_to_key<uint64_t>("FIREWALL") },
    { Game::Utilities::str_to_key<uint64_t>("GSTFONT1"), Game::Utilities::str_to_key<uint64_t>("GSTFONT2"), Game::Utilities::str_to_key<uint64_t>("GSTFONT3") },
    { Game::Utilities::str_to_key<uint64_t>("ROCKRED1"), Game::Utilities::str_to_key<uint64_t>("ROCKRED2"), Game::Utilities::str_to_key<uint64_t>("ROCKRED3") },
    { Game::Utilities::str_to_key<uint64_t>("SLADRIP1"), Game::Utilities::str_to_key<uint64_t>("SLADRIP2"), Game::Utilities::str_to_key<uint64_t>("SLADRIP3") },
    { Game::Utilities::str_to_key<uint64_t>("BFALL1"), Game::Utilities::str_to_key<uint64_t>("BFALL2"), Game::Utilities::str_to_key<uint64_t>("BFALL3"), Game::Utilities::str_to_key<uint64_t>("BFALL4") },
    { Game::Utilities::str_to_key<uint64_t>("SFALL1"), Game::Utilities::str_to_key<uint64_t>("SFALL2"), Game::Utilities::str_to_key<uint64_t>("SFALL3"), Game::Utilities::str_to_key<uint64_t>("SFALL4") },
    { Game::Utilities::str_to_key<uint64_t>("WFALL1"), Game::Utilities::str_to_key<uint64_t>("WFALL2"), Game::Utilities::str_to_key<uint64_t>("WFALL3"), Game::Utilities::str_to_key<uint64_t>("WFALL4") },
    { Game::Utilities::str_to_key<uint64_t>("DBRAIN1"), Game::Utilities::str_to_key<uint64_t>("DBRAIN2"), Game::Utilities::str_to_key<uint64_t>("DBRAIN3"), Game::Utilities::str_to_key<uint64_t>("DBRAIN4") }
  };

  std::vector<uint64_t> animation = { name };

  // Find if frame is part of an animation
  for (const auto& frames : animations)
    for (uint64_t frame : frames)
      if (name == frame)
        animation = frames;

  std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>> result;

  // Check if frames are registered in resources
  for (uint64_t frame : animation) {
    if (doom.resources.textures.find(frame) == doom.resources.textures.end()) {
      std::cout << Game::Utilities::key_to_str(frame) << std::endl;
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
    }

    result.push_back(doom.resources.textures.find(frame)->second);
  }

  // Return animation
  return result;
}

void  DOOM::Doom::Level::Sidedef::update(DOOM::Doom& doom, sf::Time elapsed)
{
  // Add elapsed time to total
  _elapsed += elapsed;

  // Re-toggle switch
  if (_toggle > sf::Time::Zero) {
    _toggle -= elapsed;

    // Switch texture if toggle time elapsed
    if (_toggle <= sf::Time::Zero)
      switched(doom, sf::Time::Zero);
  }
}

bool  DOOM::Doom::Level::Sidedef::switched(DOOM::Doom& doom, sf::Time toggle, bool exit)
{
  // Does nothing if a toggle is already running
  if (_toggle > sf::Time::Zero)
    return false;

  // Set toggle timer
  _toggle = toggle;

  enum Vertical {
    None,
    Upper,
    Lower,
    Middle
  };

  Vertical vertical = None;

  // Call animation method to find texture set of switch
  if (switched(doom, _upper_textures, _upper_name) == true)
    vertical = Upper;
  else if (switched(doom, _lower_textures, _lower_name) == true)
    vertical = Lower;
  else if (switched(doom, _middle_textures, _middle_name) == true)
    vertical = Middle;
  else
    return false;

  int16_t index = (int16_t)(((uint64_t)this - (uint64_t)doom.level.sidedefs.data()) / sizeof(*this));

  // Find linedef of current sidedef
  for (const auto& linedef : doom.level.linedefs) {
    if (linedef->front == index || linedef->back == index) {
      auto  origin = (doom.level.vertexes[linedef->start] + doom.level.vertexes[linedef->end]) / 2.f;

      // Use front and back sector to find vertical coordinate of sound
      const auto& sector_front = doom.level.sectors[doom.level.sidedefs[linedef->front].sector];
      const auto& sector_back = doom.level.sectors[doom.level.sidedefs[linedef->back == -1 ? linedef->front : linedef->back].sector];

      // Play switch sound at linedef position
      doom.sound(
        exit == true ? DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchx : DOOM::Doom::Resources::Sound::EnumSound::Sound_swtchn,
        Math::Vector<3>(
          origin.x(),
          origin.y(),
          (vertical == Upper) ? ((sector_front.ceiling_current + sector_back.ceiling_current) / 2.f) :
          (vertical == Lower) ? ((sector_front.floor_current + sector_back.floor_current) / 2.f) :
          (linedef->front == index) ? ((sector_front.floor_current + sector_front.ceiling_current) / 2.f) :
          ((sector_back.floor_current + sector_back.ceiling_current) / 2.f)
          )
      );

      break;
    }
  }

  return true;
}

bool  DOOM::Doom::Level::Sidedef::switched(DOOM::Doom& doom, std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>& textures, uint64_t& name)
{
  // NOTE: the switched is performed using name convention method,
  // switches should be hard coded

  std::string name_str = Game::Utilities::key_to_str(name);

  // Check if texture name is switchable
  if (name_str.substr(0, 3) == "SW1")
    name_str.at(2) = '2';
  else if (name_str.substr(0, 3) == "SW2")
    name_str.at(2) = '1';
  else
    return false;

  // Find new texture set
  name = Game::Utilities::str_to_key<uint64_t>(name_str);
  textures = animation(doom, name);

  return true;
}

const DOOM::Doom::Resources::Texture& DOOM::Doom::Level::Sidedef::upper() const
{
  // Return upper frame
  return _upper_textures[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _upper_textures.size()];
}

const DOOM::Doom::Resources::Texture& DOOM::Doom::Level::Sidedef::lower() const
{
  // Return lower frame
  return _lower_textures[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _lower_textures.size()];
}

const DOOM::Doom::Resources::Texture& DOOM::Doom::Level::Sidedef::middle() const
{
  // Return middle frame
  return _middle_textures[_elapsed.asMicroseconds() / (DOOM::Doom::Tic.asMicroseconds() * DOOM::Doom::Level::Sidedef::FrameDuration) % _middle_textures.size()];
}

DOOM::Doom::Level::Segment::Segment(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Segment& segment) :
  start(segment.start),
  end(segment.end),
  angle(segment.angle / 32768.f * Math::Pi),
  linedef(segment.linedef),
  direction(segment.direction),
  offset(segment.offset)
{
  // Check for errors
  if (segment.start < 0 || segment.start >= doom.wad.levels[doom.level.episode].segments.size() ||
    segment.end < 0 || segment.end >= doom.wad.levels[doom.level.episode].segments.size() ||
    segment.linedef < 0 || segment.linedef >= doom.wad.levels[doom.level.episode].linedefs.size() ||
    (segment.direction != 0 && segment.direction != 1))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

DOOM::Doom::Level::Subsector::Subsector(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Subsector& subsector) :
  count(subsector.count),
  index(subsector.index),
  sector(0)
{
  // Check for errors
  if (subsector.index < 0 || subsector.count < 0 ||
    subsector.index + subsector.count > doom.wad.levels[doom.level.episode].segments.size())
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Find sector of subsector
  sector = doom.wad.levels[doom.level.episode].sidedefs[
    doom.wad.levels[doom.level.episode].segments[subsector.index].direction == 0 ?
      doom.wad.levels[doom.level.episode].linedefs[doom.wad.levels[doom.level.episode].segments[subsector.index].linedef].front :
      doom.wad.levels[doom.level.episode].linedefs[doom.wad.levels[doom.level.episode].segments[subsector.index].linedef].back
  ].sector;
}

DOOM::Doom::Level::Node::Node(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Node& node) :
  origin((float)node.origin_x, (float)node.origin_y),
  direction((float)node.direction_x, (float)node.direction_y),
  rightbound(
    {
      node.right_bb.top,
      node.right_bb.bottom,
      node.right_bb.left,
      node.right_bb.right
    }),
  leftbound(
    {
      node.left_bb.top,
      node.left_bb.bottom,
      node.left_bb.left,
      node.left_bb.right
    }),
  rightchild(node.right_ss),
  leftchild(node.left_ss)

{
  // Check for errors
  if ((node.direction_x == 0 && node.direction_y == 0) ||
    node.right_bb.top < node.right_bb.bottom || node.right_bb.right < node.right_bb.left ||
    node.left_bb.top < node.left_bb.bottom || node.left_bb.right < node.left_bb.left ||
    ((node.right_ss & 0b1000000000000000) ? ((node.right_ss & 0b0111111111111111) >= doom.wad.levels[doom.level.episode].subsectors.size()) : (node.right_ss >= doom.wad.levels[doom.level.episode].nodes.size())) ||
    ((node.left_ss & 0b1000000000000000) ? ((node.left_ss & 0b0111111111111111) >= doom.wad.levels[doom.level.episode].subsectors.size()) : (node.left_ss >= doom.wad.levels[doom.level.episode].nodes.size())))
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
}

DOOM::Doom::Level::Blockmap::Blockmap(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Blockmap& blockmap) :
  x(blockmap.x),
  y(blockmap.y),
  column(blockmap.column),
  row(blockmap.row),
  blocks()
{
  // Load blockmap from WAD
  for (const uint16_t offset : blockmap.offset)
  {
    uint16_t        index = (uint16_t)(offset - (4 + blockmap.offset.size()));

    // Check first blocklist delimiter
    if (index >= blockmap.blocklist.size() || blockmap.blocklist[index] != 0x0000)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Skip blocklist delimiter
    index += 1;

    // Push new block in blockmap
    blocks.push_back(DOOM::Doom::Level::Blockmap::Block());

    // Iterate over blocklist
    for (; index < blockmap.blocklist.size() && blockmap.blocklist[index] != (int16_t)0xFFFF; index++)
    {
      // Check for errors
      if (blockmap.blocklist[index] < 0 || blockmap.blocklist[index] >= doom.wad.levels[doom.level.episode].linedefs.size())
        throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

      blocks.back().linedefs.push_back(blockmap.blocklist[index]);
    }

    // Check last blocklist delimiter
    if (index >= blockmap.blocklist.size() || blockmap.blocklist[index] != (int16_t)0xFFFF)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());
  }

  // Build sectors to block indexes map
  for (int16_t sector_index = 0; sector_index < doom.level.sectors.size(); sector_index++)
    for (int16_t block_index = 0; block_index < blocks.size(); block_index++)
      for (int16_t linedef_index : blocks[block_index].linedefs)
        if (doom.level.sidedefs[doom.level.linedefs[linedef_index].get()->front].sector == sector_index || (doom.level.linedefs[linedef_index].get()->back != -1 && doom.level.sidedefs[doom.level.linedefs[linedef_index].get()->back].sector == sector_index))
          sectors[&doom.level.sectors[sector_index]].insert(block_index);
}

int DOOM::Doom::Level::Blockmap::index(const Math::Vector<2>& position) const
{
  // Check if position out of bound
  if ((int)position.x() < x || (int)position.x() >= x + 128 * column ||
    (int)position.y() < y || (int)position.y() >= y + 128 * row)
    return -1;

  // Compute position index in blockmap
  return ((int)position.y() - y) / 128 * column + ((int)position.x() - x) / 128;
}

void  DOOM::Doom::Level::Blockmap::addThing(DOOM::AbstractThing& thing, const Math::Vector<2>& position)
{
  std::set<int> thing_blocks;

  // Get blocks of thing
  thing_blocks.insert(index(Math::Vector<2>(position.x() - thing.attributs.radius, position.y() - thing.attributs.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() - thing.attributs.radius, position.y() + thing.attributs.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() + thing.attributs.radius, position.y() - thing.attributs.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() + thing.attributs.radius, position.y() + thing.attributs.radius)));

  // Insert thing in blocks
  for (int index : thing_blocks)
    if (index != -1)
      blocks[index].things.insert(std::ref(thing));
}

void  DOOM::Doom::Level::Blockmap::moveThing(DOOM::AbstractThing& thing, const Math::Vector<2>& old_position, const Math::Vector<2>& new_position)
{
  // Remove and insert thing of blockmap
  removeThing(thing, old_position);
  addThing(thing, new_position);
}

void  DOOM::Doom::Level::Blockmap::removeThing(DOOM::AbstractThing& thing, const Math::Vector<2>& position)
{
  std::set<int> thing_blocks;

  // Get blocks of thing
  thing_blocks.insert(index(Math::Vector<2>(position.x() - thing.attributs.radius, position.y() - thing.attributs.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() - thing.attributs.radius, position.y() + thing.attributs.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() + thing.attributs.radius, position.y() - thing.attributs.radius)));
  thing_blocks.insert(index(Math::Vector<2>(position.x() + thing.attributs.radius, position.y() + thing.attributs.radius)));

  // Remove thing from blocks
  for (int index : thing_blocks)
    if (index != -1)
      blocks[index].things.erase(std::ref(thing));
}

DOOM::Doom::Level::Sector::Sector(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Sector& sector) :
  floor_name(sector.floor_texture),
  ceiling_name(sector.ceiling_texture),
  floor_flat(DOOM::AbstractFlat::Null),
  ceiling_flat(DOOM::AbstractFlat::Null),
  light_current(sector.light), light_base(sector.light),
  floor_current(sector.floor_height), floor_base(sector.floor_height),
  ceiling_current(sector.ceiling_height), ceiling_base(sector.ceiling_height),
  damage(0.f),
  tag(sector.tag),
  special(sector.special),
  sound_target(nullptr),
  _neighbors(),
  _actions()
{
  // Check for errors
  if (light_current < 0 || light_current > 255)
    throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

  // Retrieve flat textures
  if (floor_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1"))
    floor_flat = std::cref(doom.resources.getFlat(floor_name));
  if (ceiling_name != Game::Utilities::str_to_key<uint64_t>("F_SKY1"))
    ceiling_flat = std::cref(doom.resources.getFlat(ceiling_name));

  // Index of this sector
  int16_t index = (int16_t)doom.level.sectors.size();

  // Compute neighbor sectors
  for (const DOOM::Wad::RawLevel::Linedef& linedef : doom.wad.levels.find(doom.level.episode)->second.linedefs) {
    if (doom.wad.levels.find(doom.level.episode)->second.sidedefs[linedef.front].sector == index && linedef.back != -1)
      _neighbors.insert(doom.wad.levels.find(doom.level.episode)->second.sidedefs[linedef.back].sector);
    if (linedef.back != -1 && doom.wad.levels.find(doom.level.episode)->second.sidedefs[linedef.back].sector == index)
      _neighbors.insert(doom.wad.levels.find(doom.level.episode)->second.sidedefs[linedef.front].sector);
  }

  // Push action for specific specials
  switch (this->special)
  {
  case DOOM::Doom::Level::Sector::Special::Normal:
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlinkRandom:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::RandomLightingAction<24, 4>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink05:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::BlinkLightingAction<15, 5, false>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink10:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::BlinkLightingAction<35, 5, false>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::Damage20Blink05:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::BlinkLightingAction<15, 5, false>>(doom, *this));
    damage = 20.f;
    break;
  case DOOM::Doom::Level::Sector::Special::Damage10:
    damage = 10.f;
    break;
  case DOOM::Doom::Level::Sector::Special::Damage5:
    damage = 5.f;
    break;
  case DOOM::Doom::Level::Sector::Special::LightOscillates:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::OscillateLightingAction<>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::Secret:
    doom.level.statistics.total.secrets += 1;
    break;
  case DOOM::Doom::Level::Sector::Special::DoorClose:
    action<DOOM::Doom::Level::Sector::Action::Leveling>(std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitClose, DOOM::EnumAction::Speed::SpeedFast, 1050>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::End:
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink05Sync:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::BlinkLightingAction<15, 5, true>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::LightBlink10Sync:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::BlinkLightingAction<35, 5, true>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::DoorOpen:
    action<DOOM::Doom::Level::Sector::Action::Leveling>(std::make_unique<DOOM::DoorLevelingAction<DOOM::EnumAction::Door::DoorWaitOpen, DOOM::EnumAction::Speed::SpeedFast, 10500>>(doom, *this));
    break;
  case DOOM::Doom::Level::Sector::Special::Damage20:
    damage = 20.f;
    break;
  case DOOM::Doom::Level::Sector::Special::LightFlickers:
    action<DOOM::Doom::Level::Sector::Action::Lighting>(std::make_unique<DOOM::FlickerLightingAction<>>(doom, *this));
    break;

  default:
    break;
  }
}

DOOM::Doom::Level::Sector::Sector(DOOM::Doom::Level::Sector&& sector) :
  floor_name(sector.floor_name),
  ceiling_name(sector.ceiling_name),
  floor_flat(sector.floor_flat),
  ceiling_flat(sector.ceiling_flat),
  light_current(sector.light_current), light_base(sector.light_base),
  floor_current(sector.floor_current), floor_base(sector.floor_base),
  ceiling_current(sector.ceiling_current), ceiling_base(sector.ceiling_base),
  damage(sector.damage),
  tag(sector.tag),
  special(sector.special),
  sound_target(sector.sound_target),
  _neighbors(std::move(sector._neighbors)),
  _actions(std::move(sector._actions))
{}

std::unique_ptr<DOOM::AbstractAction> DOOM::Doom::Level::Sector::_factory(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, int16_t type, int16_t model)
{
  // Just a relay (cycling inclusion problem)
  return DOOM::AbstractAction::factory(doom, sector, type, model);
}

void  DOOM::Doom::Level::Sector::update(DOOM::Doom& doom, sf::Time elapsed)
{
  // Update sector actions
  for (unsigned int type = 0; type < DOOM::Doom::Level::Sector::Action::Number; type++)
    if (_actions.at(type).get() != nullptr)
      _actions.at(type)->update(doom, *this, elapsed);
}

std::pair<int16_t, float> DOOM::Doom::Level::Sector::getNeighborLowestFloor(const DOOM::Doom& doom) const
{
  std::pair<int16_t, float> result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find lowest neighboor floor
  for (int16_t index : _neighbors)
    if (result.first == -1 || doom.level.sectors[index].floor_base < doom.level.sectors[result.first].floor_base)
      result = { index, doom.level.sectors[index].floor_base };

  return result;
}

std::pair<int16_t, float> DOOM::Doom::Level::Sector::getNeighborHighestFloor(const DOOM::Doom& doom) const
{
  std::pair<int16_t, float> result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find highest neighboor floor
  for (int16_t index : _neighbors)
    if (result.first == -1 || doom.level.sectors[index].floor_base > doom.level.sectors[result.first].floor_base)
      result = { index, doom.level.sectors[index].floor_base };

  return result;
}

std::pair<int16_t, float> DOOM::Doom::Level::Sector::getNeighborNextLowestFloor(const DOOM::Doom& doom, float height) const
{
  std::pair<int16_t, float> result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find next lowest neighboor floor
  for (int16_t index : _neighbors) {
    float floor = doom.level.sectors[index].floor_base;

    if (floor < height && (result.first == -1 || floor > result.second))
      result = { index, floor };
  }

  return result;
}

std::pair<int16_t, float> DOOM::Doom::Level::Sector::getNeighborNextHighestFloor(const DOOM::Doom& doom, float height) const
{
  std::pair<int16_t, float> result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float floor = doom.level.sectors[index].floor_base;

    if (floor > height && (result.first == -1 || floor < result.second))
      result = { index, floor };
  }

  return result;
}

std::pair<int16_t, float> DOOM::Doom::Level::Sector::getNeighborLowestCeiling(const DOOM::Doom& doom) const
{
  std::pair<int16_t, float> result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find lowest neighboor ceiling
  for (int16_t index : _neighbors)
    if (result.first == -1 || doom.level.sectors[index].ceiling_base < doom.level.sectors[result.first].ceiling_base)
      result = { index, doom.level.sectors[index].ceiling_base };

  return result;
}

std::pair<int16_t, float> DOOM::Doom::Level::Sector::getNeighborHighestCeiling(const DOOM::Doom& doom) const
{
  std::pair<int16_t, float> result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find lowest neighboor ceiling
  for (int16_t index : _neighbors)
    if (result.first == -1 || doom.level.sectors[index].ceiling_base > doom.level.sectors[result.first].ceiling_base)
      result = { index, doom.level.sectors[index].ceiling_base };

  return result;
}

std::pair<int16_t, float> DOOM::Doom::Level::Sector::getNeighborNextLowestCeiling(const DOOM::Doom& doom, float height) const
{
  std::pair<int16_t, float> result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find next lowest neighboor ceiling
  for (int16_t index : _neighbors) {
    float ceiling = doom.level.sectors[index].ceiling_base;

    if (ceiling < height && (result.first == -1 || ceiling > result.second))
      result = { index, ceiling };
  }

  return result;
}

std::pair<int16_t, float> DOOM::Doom::Level::Sector::getNeighborNextHighestCeiling(const DOOM::Doom& doom, float height) const
{
  std::pair<int16_t, float> result = { -1, std::numeric_limits<float>::quiet_NaN() };

  // Find next highest neighboor floor
  for (int16_t index : _neighbors) {
    float ceiling = doom.level.sectors[index].ceiling_base;

    if (ceiling > height && (result.first == -1 || ceiling < result.second))
      result = { index, ceiling };
  }

  return result;
}

const std::set<int16_t>&  DOOM::Doom::Level::Sector::getNeighbors() const
{
  return _neighbors;
}

int16_t DOOM::Doom::Level::Sector::getShortestLowerTexture(const DOOM::Doom& doom) const
{
  int16_t result = 0;
  int16_t index = (int16_t)(((uint64_t)this - (uint64_t)doom.level.sectors.data()) / sizeof(DOOM::Doom::Level::Sector));

  // Find shortest lower texture of sector
  for (const auto& linedef : doom.level.linedefs)
    if (linedef->back != -1 && linedef->front != -1) {
      if (doom.level.sidedefs[linedef->back].sector == index)
        result = ((result == 0) ? doom.level.sidedefs[linedef->front].lower().height : std::min(doom.level.sidedefs[linedef->front].lower().height, result));
      else if (doom.level.sidedefs[linedef->front].sector == index)
        result = ((result == 0) ? doom.level.sidedefs[linedef->back].lower().height : std::min(doom.level.sidedefs[linedef->back].lower().height, result));
    }

  return result;
}

int16_t DOOM::Doom::Level::Sector::getNeighborLowestLight(const DOOM::Doom& doom) const
{
  int16_t result = light_base;

  // Find lowest neighboor light level
  for (int16_t index : _neighbors)
    result = std::min(result, doom.level.sectors[index].light_base);

  return result;
}

int16_t DOOM::Doom::Level::Sector::getNeighborHighestLight(const DOOM::Doom& doom) const
{
  int16_t result = light_base;

  // Find highest neighboor light level
  for (int16_t index : _neighbors)
    result = std::min(result, doom.level.sectors[index].light_base);

  return result;
}

DOOM::Doom::Level::Statistics::Statistics() :
  players(),
  total(),
  time(sf::Time::Zero)
{}

void  DOOM::Doom::Level::Statistics::update(DOOM::Doom& doom, sf::Time elapsed)
{
  // Add elapsed time to level time
  time += elapsed;
}

namespace std
{
  // Define comparison of Thing ref to build a std::set
  bool  operator<(const std::reference_wrapper<DOOM::AbstractThing>& left, const std::reference_wrapper<DOOM::AbstractThing>& right)
  {
    return &left.get() < &right.get();
  }
}