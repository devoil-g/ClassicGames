#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Doom/Wad.hpp"
#include "Math/Box.hpp"
#include "Math/Vector.hpp"
#include "System/Audio/Sound.hpp"
#include "System/Utilities.hpp"

namespace DOOM
{
  namespace Enum
  {
    enum End
    {
      EndNone,    // Not finished
      EndNormal,  // Go to next level
      EndSecret   // Go to secret level
    };

    enum Mode
    {
      ModeShareware,    // DOOM 1 shareware, E1, M9
      ModeRegistered,   // DOOM 1 registered, E3, M27
      ModeCommercial,   // DOOM 2 retail, E1 M34
      ModeRetail,       // DOOM 1 retail, E4, M36
      ModeIndetermined  // Well, no IWAD found
    };

    enum Skill
    {
      SkillBaby,     // Double ammo, half damage, least monsters or strength
      SkillEasy,     // Normal ammo, least monsters or strength
      SkillMedium,   // Normal ammo, default monsters of strength
      SkillHard,     // Normal ammo, greatest monsters or strength
      SkillNightmare // Double ammo, fast and respawning monsters, greatest monsters or strength
    };

    enum Weapon
    {
      WeaponFist = 0,
      WeaponPistol = 1,
      WeaponShotgun = 2,
      WeaponChaingun = 3,
      WeaponRocketLauncher = 4,
      WeaponPlasmaGun = 5,
      WeaponBFG9000 = 6,
      WeaponChainsaw = 7,
      WeaponSuperShotgun = 8,
      WeaponCount
    };

    enum Ammo
    {
      AmmoNone = -1,
      AmmoBullet = 0,
      AmmoShell = 1,
      AmmoRocket = 2,
      AmmoCell = 3,
      AmmoCount
    };

    enum KeyColor
    {
      KeyColorNone = -1,
      KeyColorBlue = 0,
      KeyColorYellow = 1,
      KeyColorRed = 2,
      KeyColorCount
    };

    enum KeyType
    {
      KeyTypeNone,
      KeyTypeKeycard,
      KeyTypeSkullkey
    };

    enum Armor
    {
      ArmorNone = 0,
      ArmorSecurity = 100,
      ArmorMega = 200
    };

    enum ThingType : std::int16_t
    {
      ThingType_PLAYER,         // -1
      ThingType_POSSESSED,      // Former Human Trooper
      ThingType_SHOTGUY,        // Former Human Sergeant
      ThingType_VILE,           // Arch-vile
      ThingType_FIRE,           // -1
      ThingType_UNDEAD,         // Revenant
      ThingType_TRACER,         // -1
      ThingType_SMOKE,          // -1
      ThingType_FATSO,          // Mancubus
      ThingType_FATSHOT,        // -1
      ThingType_CHAINGUY,       // Chaingunner
      ThingType_TROOP,          // Imp
      ThingType_SERGEANT,       // Demon
      ThingType_SHADOWS,        // Spectre
      ThingType_HEAD,           // Cacodemon
      ThingType_BRUISER,        // Baron of Hell
      ThingType_BRUISERSHOT,    // -1
      ThingType_KNIGHT,         // Hell Knight
      ThingType_SKULL,          // Lost Soul
      ThingType_SPIDER,         // Spider Mastermind
      ThingType_BABY,           // Arachnotron
      ThingType_CYBORG,         // Cyberdemon
      ThingType_PAIN,           // Pain elemental
      ThingType_WOLFSS,         // Wolfenstein SS
      ThingType_KEEN,           // Commander Keen
      ThingType_BOSSBRAIN,      // Boss brain
      ThingType_BOSSSPIT,       // Boss spawn shooter
      ThingType_BOSSTARGET,     // Boss spawn spot
      ThingType_SPAWNSHOT,      // -1
      ThingType_SPAWNFIRE,      // -1
      ThingType_BARREL,         // Barrel
      ThingType_TROOPSHOT,      // -1
      ThingType_HEADSHOT,       // -1
      ThingType_ROCKET,         // -1
      ThingType_PLASMA,         // -1
      ThingType_BFG,            // -1
      ThingType_ARACHPLAZ,      // -1
      ThingType_PUFF,           // -1
      ThingType_BLOOD,          // -1
      ThingType_TFOG,           // -1
      ThingType_IFOG,           // -1
      ThingType_TELEPORTMAN,    // Teleport landing
      ThingType_EXTRABFG,       // -1
      ThingType_MISC0,          // Green armor
      ThingType_MISC1,          // Blue armor
      ThingType_MISC2,          // Health potion
      ThingType_MISC3,          // Spiritual armor
      ThingType_MISC4,          // Blue keycard
      ThingType_MISC5,          // Red keycard
      ThingType_MISC6,          // Yellow keycard
      ThingType_MISC7,          // Yellow skull key
      ThingType_MISC8,          // Red skull key
      ThingType_MISC9,          // Blue skull key
      ThingType_MISC10,         // Stimpack
      ThingType_MISC11,         // Medikit
      ThingType_MISC12,         // Soul sphere
      ThingType_INV,            // Invulnerability
      ThingType_MISC13,         // Berserk
      ThingType_INS,            // Invisibility
      ThingType_MISC14,         // Radiation suit
      ThingType_MISC15,         // Computer map
      ThingType_MISC16,         // Light amplification visor
      ThingType_MEGA,           // Megasphere
      ThingType_CLIP,           // Ammo clip
      ThingType_MISC17,         // Box of ammo
      ThingType_MISC18,         // Rocket
      ThingType_MISC19,         // Box of rockets
      ThingType_MISC20,         // Cell charge
      ThingType_MISC21,         // Cell charge pack
      ThingType_MISC22,         // Shotgun shells
      ThingType_MISC23,         // Box of shells
      ThingType_MISC24,         // Backpack
      ThingType_MISC25,         // BFG 9000
      ThingType_CHAINGUN,       // Chaingun
      ThingType_MISC26,         // Chainsaw
      ThingType_MISC27,         // Rocket launcher
      ThingType_MISC28,         // Plasma rifle
      ThingType_SHOTGUN,        // Shotgun
      ThingType_SUPERSHOTGUN,   // Super shotgun
      ThingType_MISC29,         // Tall techno floor lamp
      ThingType_MISC30,         // Short techno floor lamp
      ThingType_MISC31,         // Floor lamp
      ThingType_MISC32,         // Tall green pillar
      ThingType_MISC33,         // Short green pillar
      ThingType_MISC34,         // Tall red pillar
      ThingType_MISC35,         // Short red pillar
      ThingType_MISC36,         // Short red pillar with skull
      ThingType_MISC37,         // Short green pillar with beating heart
      ThingType_MISC38,         // Evil eye
      ThingType_MISC39,         // Floating skull
      ThingType_MISC40,         // Burnt tree
      ThingType_MISC41,         // Tall blue firestick
      ThingType_MISC42,         // Tall green firestick
      ThingType_MISC43,         // Tall red firestick
      ThingType_MISC44,         // Short blue firestick
      ThingType_MISC45,         // Short green firestick
      ThingType_MISC46,         // Short red firestick
      ThingType_MISC47,         // Stalagmite
      ThingType_MISC48,         // Tall techno pillar
      ThingType_MISC49,         // Candle
      ThingType_MISC50,         // Candelabra
      ThingType_MISC51,         // Hanging victim, twitching
      ThingType_MISC52,         // Hanging victim, arms out
      ThingType_MISC53,         // Hanging victim, one-legged
      ThingType_MISC54,         // Hanging pair of legs
      ThingType_MISC55,         // Hanging leg
      ThingType_MISC56,         // Hanging victim, arms out
      ThingType_MISC57,         // Hanging pair of legs
      ThingType_MISC58,         // Hanging victim, one-legged
      ThingType_MISC59,         // Hanging leg
      ThingType_MISC60,         // Hanging victim, twitching
      ThingType_MISC61,         // Dead cacodemon
      ThingType_MISC62,         // Dead player
      ThingType_MISC63,         // Dead former human
      ThingType_MISC64,         // Dead demon
      ThingType_MISC65,         // Dead lost soul (invisible)
      ThingType_MISC66,         // Dead imp
      ThingType_MISC67,         // Dead former sergeant
      ThingType_MISC68,         // Bloody mess
      ThingType_MISC69,         // Bloody mess
      ThingType_MISC70,         // Five skull "shish kebab"
      ThingType_MISC71,         // Pool of blood and flesh
      ThingType_MISC72,         // Skull on a pole
      ThingType_MISC73,         // Pile of shulls and candles
      ThingType_MISC74,         // Impaled human
      ThingType_MISC75,         // Twitching impaled human
      ThingType_MISC76,         // Large brown tree
      ThingType_MISC77,         // Burning barrel
      ThingType_MISC78,         // Hanging victim, guts removed
      ThingType_MISC79,         // Hanging victim, guts and brain removed
      ThingType_MISC80,         // Hanging torso, looking down
      ThingType_MISC81,         // Hanging torso, open skull
      ThingType_MISC82,         // Hanging torso, looking up
      ThingType_MISC83,         // Hanging torso, brain removed
      ThingType_MISC84,         // Pool of blood
      ThingType_MISC85,         // Pool of blood
      ThingType_MISC86,         // Pool of brains
      ThingType_PLAYER_SPAWN1,  // Player 1 spawn
      ThingType_PLAYER_SPAWN2,  // Player 2 spawn
      ThingType_PLAYER_SPAWN3,  // Player 3 spawn
      ThingType_PLAYER_SPAWN4,  // Player 4 spawn
      ThingType_PLAYER_SPAWNDM, // Death match spawn
      ThingType_Number
    };

    enum ThingFlag : std::int16_t
    {
      FlagNone = 0x0000,
      FlagSkillLevel12 = 0x0001,  // Active on skill level 1 & 2
      FlagSkillLevel3 = 0x0002,   // Active on skill level 3
      FlagSkillLevel45 = 0x0004,  // Active on skill level 4 & 5
      FlagAmbush = 0x0008,        // Monster is in ambush mode
      FlagMultiplayer = 0x0010,   // Only active in multiplayer mode
    };

    enum ThingProperty : std::int32_t
    {
      ThingProperty_None = 0x00000000,          // Nothing
      ThingProperty_Special = 0x00000001,       // Call P_SpecialThing when touched.
      ThingProperty_Solid = 0x00000002,         // Blocks.
      ThingProperty_Shootable = 0x00000004,     // Can be hit.
      ThingProperty_NoSector = 0x00000008,      // Don't use the sector links (invisible but touchable).
      ThingProperty_NoBlockmap = 0x00000010,    // Don't use the blocklinks (inert but displayable)
      ThingProperty_Ambush = 0x00000020,        // Not to be activated by sound, deaf monster.
      ThingProperty_JustHit = 0x00000040,       // Will try to attack right back.
      ThingProperty_JustAttacked = 0x00000080,  // Will take at least one step before attacking.
      ThingProperty_SpawnCeiling = 0x00000100,  // On level spawning (initial position), hang from ceiling instead of stand on floor.
      ThingProperty_NoGravity = 0x00000200,     // Don't apply gravity (every tic), that is, object will float, keeping current height or changing it actively.
      ThingProperty_DropOff = 0x00000400,       // Movement flags. This allows jumps from high places.
      ThingProperty_PickUp = 0x00000800,        // For players, will pick up items.
      ThingProperty_NoClip = 0x00001000,        // Player cheat. ???
      ThingProperty_Slide = 0x00002000,         // Player: keep info about sliding along walls.
      ThingProperty_Float = 0x00004000,         // Allow moves to any height, no gravity. For active floaters, e.g. cacodemons, pain elementals.
      ThingProperty_Teleport = 0x00008000,      // Don't cross lines ??? or look at heights on teleport.
      ThingProperty_Missile = 0x00010000,       // Don't hit same species, explode on block. Player missiles as well as fireballs of various kinds.
      ThingProperty_Dropped = 0x00020000,       // Dropped by a demon, not level spawned. E.g. ammo clips dropped by dying former humans.
      ThingProperty_Shadow = 0x00040000,        // Use fuzzy draw (shadow demons or spectres), temporary player invisibility powerup.
      ThingProperty_NoBlood = 0x00080000,       // Flag: don't bleed when shot (use puff), barrels and shootable furniture shall not bleed.
      ThingProperty_Corpse = 0x00100000,        // Don't stop moving halfway off a step, that is, have dead bodies slide down all the way.
      ThingProperty_InFloat = 0x00200000,       // Floating to a height for a move, ??? don't auto float to target's height.
      ThingProperty_CountKill = 0x00400000,     // On kill, count this enemy object towards intermission kill total. Happy gathering.
      ThingProperty_CountItem = 0x00800000,     // On picking up, count this item object towards intermission item total.
      ThingProperty_SkullFly = 0x01000000,      // Special handling: skull in flight. Neither a cacodemon nor a missile.
      ThingProperty_NoTDMatch = 0x02000000,     // Don't spawn this object in death match mode (e.g. key cards).
      ThingProperty_Translation = 0x02000000,   // Player sprites in multiplayer modes are modified using an internal color lookup table for re-indexing. 0x4 0x8 or 0xc, use a translation table for player colormaps.
      ThingProperty_TransShift = 26             // Hmm ???.
    };
  };

  // Forward declaration of external components
  class AbstractAction;
  class AbstractFlat;
  class AbstractLinedef;
  class AbstractThing;
  class PlayerThing;

  class Doom
  {
  public:
    static float const        Tic;              // Duration of a game tic (1/35s)
    static const unsigned int RenderWidth;      // Default rendering width size
    static const unsigned int RenderHeight;     // Default rendering height size
    static unsigned int       RenderScale;      // Scaling factor of resolution
    static const float        RenderStretching; // Default rendering vertical stretching

    class Resources
    {
    public:
      class Palette : public std::array<sf::Color, 256>
      {
      public:
        Palette() = default;
        Palette(DOOM::Doom& doom, const DOOM::Wad::RawResources::Palette& palette);
        ~Palette() = default;
      };

      class Colormap : public std::array<std::uint8_t, 256>
      {
      public:
        Colormap() = default;
        Colormap(DOOM::Doom& doom, const DOOM::Wad::RawResources::Colormap& colormap);
        ~Colormap() = default;
      };

      class Texture
      {
      private:
        Texture() = default;

      public:
        static const DOOM::Doom::Resources::Texture Null; // Empty texture

        struct Column
        {
          struct Span
          {
            std::uint8_t              offset; // Offset of the span of pixels in the column
            std::vector<std::uint8_t> pixels; // Pixels colors indexes
          };

          std::vector<Span> spans;  // Vector of spans of pixels in the column
        };

        std::int16_t        width, height;  // Size of texture
        std::int16_t        left, top;      // Texture offset (sprite only)
        std::vector<Column> columns;        // Pre-computed texture from patches

        Texture(DOOM::Doom& doom, const DOOM::Wad::RawResources::Texture& texture);
        Texture(DOOM::Doom& doom, const DOOM::Wad::RawResources::Patch& patch);
        ~Texture() = default;

        sf::Image image(const DOOM::Doom& doom) const;                                                                                                                                                      // Create an SFML image from texture
        void      draw(const DOOM::Doom& doom, sf::Image& image, const Math::Vector<2, int>& position, const Math::Vector<2, int>& scale, std::int16_t palette = 0) const;                                  // Draw texture in SFML image at given position & scale
        void      draw(const DOOM::Doom& doom, sf::Image& image, Math::Box<2, std::int16_t> area, const Math::Vector<2, int>& position, const Math::Vector<2, int>& scale, std::int16_t palette = 0) const; // Draw texture in SFML image at given position & scale in area
      };

      class Sound
      {
      public:
        enum EnumSound
        {
          Sound_None,
          Sound_pistol,
          Sound_shotgn,
          Sound_sgcock,
          Sound_dshtgn,
          Sound_dbopn,
          Sound_dbcls,
          Sound_dbload,
          Sound_plasma,
          Sound_bfg,
          Sound_sawup,
          Sound_sawidl,
          Sound_sawful,
          Sound_sawhit,
          Sound_rlaunc,
          Sound_rxplod,
          Sound_firsht,
          Sound_firxpl,
          Sound_pstart,
          Sound_pstop,
          Sound_doropn,
          Sound_dorcls,
          Sound_stnmov,
          Sound_swtchn,
          Sound_swtchx,
          Sound_plpain,
          Sound_dmpain,
          Sound_popain,
          Sound_vipain,
          Sound_mnpain,
          Sound_pepain,
          Sound_slop,
          Sound_itemup,
          Sound_wpnup,
          Sound_oof,
          Sound_telept,
          Sound_posit1,
          Sound_posit2,
          Sound_posit3,
          Sound_bgsit1,
          Sound_bgsit2,
          Sound_sgtsit,
          Sound_cacsit,
          Sound_brssit,
          Sound_cybsit,
          Sound_spisit,
          Sound_bspsit,
          Sound_kntsit,
          Sound_vilsit,
          Sound_mansit,
          Sound_pesit,
          Sound_sklatk,
          Sound_sgtatk,
          Sound_skepch,
          Sound_vilatk,
          Sound_claw,
          Sound_skeswg,
          Sound_pldeth,
          Sound_pdiehi,
          Sound_podth1,
          Sound_podth2,
          Sound_podth3,
          Sound_bgdth1,
          Sound_bgdth2,
          Sound_sgtdth,
          Sound_cacdth,
          Sound_skldth,
          Sound_brsdth,
          Sound_cybdth,
          Sound_spidth,
          Sound_bspdth,
          Sound_vildth,
          Sound_kntdth,
          Sound_pedth,
          Sound_skedth,
          Sound_posact,
          Sound_bgact,
          Sound_dmact,
          Sound_bspact,
          Sound_bspwlk,
          Sound_vilact,
          Sound_noway,
          Sound_barexp,
          Sound_punch,
          Sound_hoof,
          Sound_metal,
          Sound_chgun,
          Sound_tink,
          Sound_bdopn,
          Sound_bdcls,
          Sound_itmbk,
          Sound_flame,
          Sound_flamst,
          Sound_getpow,
          Sound_bospit,
          Sound_boscub,
          Sound_bossit,
          Sound_bospn,
          Sound_bosdth,
          Sound_manatk,
          Sound_mandth,
          Sound_sssit,
          Sound_ssdth,
          Sound_keenpn,
          Sound_keendt,
          Sound_skeact,
          Sound_skesit,
          Sound_skeatk,
          Sound_radio,
          Sound_Number
        };

        struct SoundInfo
        {
          std::string name;         // Sound name
          bool        singularity;  // Sound singularity (only one at a time)
          int         priority;     // Sound priority
          EnumSound   link;         // Referenced sound if a link
          int         pitch;        // Pitch if a link
          int         volume;       // Volume if a link
        };

        static const std::array<SoundInfo, Sound_Number>  sound_info;

      public:
        sf::SoundBuffer buffer; // Sound buffer

        Sound(DOOM::Doom& doom, const DOOM::Wad::RawResources::Sound& sound);
        ~Sound() = default;
      };

      template <typename Type>
      const Type& getResource(std::uint64_t key, const std::unordered_map<std::uint64_t, Type>& container) const
      {
        const auto& iterator = container.find(key);

        // Error if not found
        if (iterator == container.end())
          throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__) + " " + Game::Utilities::key_to_str(key)).c_str());

        return iterator->second;
      }

    public:
      std::array<DOOM::Doom::Resources::Palette, 14>                                                                                                palettes;   // Color palettes
      std::array<DOOM::Doom::Resources::Colormap, 34>                                                                                               colormaps;  // Color brightness maps
      std::unordered_map<std::uint64_t, std::unique_ptr<DOOM::AbstractFlat>>                                                                        flats;      // Map of flat (ground/ceiling texture)
      std::unordered_map<std::uint64_t, DOOM::Doom::Resources::Texture>                                                                             textures;   // Map of wall textures
      std::unordered_map<std::uint64_t, DOOM::Doom::Resources::Texture>                                                                             sprites;    // Map of raw sprites (not ordered, should not be used)
      std::unordered_map<std::uint64_t, std::vector<std::array<std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>, 8>>>  animations; // Map of sprites sorted by animation sequence and angle
      std::unordered_map<std::uint64_t, DOOM::Doom::Resources::Texture>                                                                             menus;      // Map of menu patches
      std::unordered_map<std::uint64_t, DOOM::Doom::Resources::Sound>                                                                               sounds;     // Map of sounds

      Resources() = default;
      ~Resources() = default;

      const DOOM::AbstractFlat&             getFlat(std::uint64_t key) const { return *getResource<std::unique_ptr<DOOM::AbstractFlat>>(key, flats); }; // Get flat texture, throw an error if not found
      const DOOM::Doom::Resources::Texture& getTexture(std::uint64_t key) const { return getResource<DOOM::Doom::Resources::Texture>(key, textures); }; // Get texture, throw an error if not found
      const DOOM::Doom::Resources::Texture& getSprite(std::uint64_t key) const { return getResource<DOOM::Doom::Resources::Texture>(key, sprites); };   // Get sprite, throw an error if not found
      const DOOM::Doom::Resources::Texture& getMenu(std::uint64_t key) const { return getResource<DOOM::Doom::Resources::Texture>(key, menus); };       // Get menu texture, throw an error if not found
      const DOOM::Doom::Resources::Sound&   getSound(std::uint64_t key) const { return getResource<DOOM::Doom::Resources::Sound>(key, sounds); };       // Get sound, throw an error if not found

      void  update(DOOM::Doom& doom, float elapsed);  // Update resources components
    };

    class Level
    {
    public:
      class Vertex : public Math::Vector<2>
      {
      public:
        Vertex(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Vertex& vertex);
        ~Vertex() = default;
      };

      class Sidedef
      {
        static const int  FrameDuration = 8;  // Tics between two frames of animation

      public:
        float         x, y;   // Texture offset
        std::int16_t  sector; // Index of the sector it references

      private:
        float _elapsed; // Total elapsed time
        float _toggle;  // Time before switch re-toggle

        std::uint64_t _upper_name;  // Upper texture name
        std::uint64_t _lower_name;  // Lower texture name
        std::uint64_t _middle_name; // Middle texture name

        std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>> _upper_textures;  // Pointer to upper textures
        std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>> _lower_textures;  // Pointer to lower textures
        std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>> _middle_textures; // Pointer to middle textures

        bool                                                                      switched(DOOM::Doom& doom, std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>& textures, std::uint64_t& name);  // Switch textures
        std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>> animation(const DOOM::Doom& doom, std::uint64_t name) const;                                                                           // Return frames of animation

      public:
        Sidedef(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Sidedef& sidedef);
        ~Sidedef() = default;

        void  update(DOOM::Doom& doom, float elapsed);                      // Update sidedef
        bool  switched(DOOM::Doom& doom, float toggle, bool exit = false);  // Toggle texture switch, reversing it after time given as parameter (0 for no reverse), special sound if exit switch

        const DOOM::Doom::Resources::Texture& upper() const;  // Return upper texture to be displayed
        const DOOM::Doom::Resources::Texture& lower() const;  // Return lower texture to be displayed
        const DOOM::Doom::Resources::Texture& middle() const; // Return middle texture to be displayed
      };

      class Segment
      {
      public:
        std::int16_t  start, end; // Start/end vertexes indexes
        float         angle;      // Segment angle (rad.)
        std::int16_t  linedef;    // Segment linedef index
        std::int16_t  direction;  // 0 (same as linedef) or 1 (opposite of linedef)
        std::int16_t  offset;     // Distance along linedef to start of seg

        Segment(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Segment& segment);
        ~Segment() = default;
      };

      class Subsector
      {
      public:
        std::int16_t  count;  // Seg count
        std::int16_t  index;  // First seg number
        std::int16_t  sector; // Index of sector that this subsector is part of

        Subsector(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Subsector& subsector);
        ~Subsector() = default;
      };

      class Node
      {
      public:
        struct BoundingBox
        {
          std::int16_t  top, bottom;  // Top and bottom limits
          std::int16_t  left, right;  // Left and right limits
        };

        Math::Vector<2> origin, direction;      // Partition line of nodes
        BoundingBox     rightbound, leftbound;  // Children bounding boxes
        std::int16_t    rightchild, leftchild;  // Children node index, or subsector index if bit 15 is set

        Node(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Node& node);
        ~Node() = default;
      };

      class Sector
      {
      public:
        enum Special : int16_t
        {
          Normal = 0x0000,            // Normal
          LightBlinkRandom = 0x0001,  // Blink random
          LightBlink05 = 0x0002,      // Blink 0.5 second
          LightBlink10 = 0x0003,      // Blink 1.0 second
          Damage20Blink05 = 0x0004,   // 20 % damage every 32 tics plus light blink 0.5 second
          Damage10 = 0x0005,          // 10 % damage every 32 tics
          Damage5 = 0x0007,           // 5 % damage every 32 tics
          LightOscillates = 0x0008,   // Oscillates
          Secret = 0x0009,            // Player entering this sector gets credit for finding a secret
          DoorClose = 0x000A,         // 30 seconds after level start, ceiling closes like a door
          End = 0x000B,               // Cancel God mode if active, 20 % damage every 32 tics, when player dies, level ends
          LightBlink05Sync = 0x000C,  // Blink 0.5 second, synchronized
          LightBlink10Sync = 0x000D,  // Blink 1.0 second, synchronized
          DoorOpen = 0x000E,          // 300 seconds after level start, ceiling opens like a door
          Damage20 = 0x0010,          // 20 % damage per second
          LightFlickers = 0x0011      // Flickers randomly (fire)
        };

        enum Action
        {
          Leveling, // Leveling action
          Lighting, // Lighting action
          Number    // Number of type of action
        };

        std::uint64_t                                     floor_name, ceiling_name; // Textures names
        std::reference_wrapper<const DOOM::AbstractFlat>  floor_flat, ceiling_flat; // Textures pointers (null if sky)

        std::int16_t  light_current, light_base;      // Sector base and current light level
        float         floor_current, floor_base;      // Sector base and current floor height
        float         ceiling_current, ceiling_base;  // Sector base and current ceiling height

        float         damage;   // Sector damage/sec
        std::int16_t  tag;      // Sector/linedef tag
        std::int16_t  special;  // Sector special attribute

        DOOM::AbstractThing* sound_target;  // Last sound emitter

      protected:
        std::set<std::int16_t>  _neighbors; // List of neighbor sectors (sorted)

      private:
        std::array<std::unique_ptr<DOOM::AbstractAction>, DOOM::Doom::Level::Sector::Action::Number>  _actions; // Actions on sector

        static std::unique_ptr<DOOM::AbstractAction>  _factory(DOOM::Doom& doom, DOOM::Doom::Level::Sector& sector, std::int16_t type, std::int16_t model); // Action factory proxy function (cycling inclusion problem)

      public:
        Sector(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Sector& sector);
        Sector(DOOM::Doom::Level::Sector&& sector);
        ~Sector() = default;

        void  update(DOOM::Doom& doom, float elapsed);  // Update sector

        template<DOOM::Doom::Level::Sector::Action Type>
        inline void action(DOOM::Doom& doom, std::int16_t type, std::int16_t model = -1)  // Add action to sector if possible
        {
          // Check template parameter
          static_assert(Type >= 0 && Type < DOOM::Doom::Level::Sector::Action::Number, "Invalid action template parameters.");

          // Push action if slot available
          if (_actions.at(Type).get() == nullptr)
            _actions.at(Type) = std::move(_factory(doom, *this, type, model));
        }

        template<DOOM::Doom::Level::Sector::Action Type>
        inline void action(std::unique_ptr<DOOM::AbstractAction>&& action)  // Add action to sector if possible
        {
          // Check template parameter
          static_assert(Type >= 0 && Type < DOOM::Doom::Level::Sector::Action::Number, "Invalid action template parameters.");

          // Push action if slot available
          if (_actions.at(Type).get() == nullptr)
            _actions.at(Type) = std::move(action);
        }

        template<DOOM::Doom::Level::Sector::Action Type>
        inline const std::unique_ptr<DOOM::AbstractAction>& action() const  // Get action of sector
        {
          // Check template parameter
          static_assert(Type >= 0 && Type < DOOM::Doom::Level::Sector::Action::Number, "Invalid action template parameters.");

          return _actions.at(Type);
        }

        template<DOOM::Doom::Level::Sector::Action Type>
        inline std::unique_ptr<DOOM::AbstractAction>& action()  // Get/set action of sector
        {
          // Check template parameter
          static_assert(Type >= 0 && Type < DOOM::Doom::Level::Sector::Action::Number, "Invalid action template parameters.");

          return _actions.at(Type);
        }

        std::pair<std::int16_t, float>  getNeighborLowestFloor(const DOOM::Doom& doom) const;                     // Get lowest neighbor floor level
        std::pair<std::int16_t, float>  getNeighborHighestFloor(const DOOM::Doom& doom) const;                    // Get highest neighbor floor level
        std::pair<std::int16_t, float>  getNeighborNextLowestFloor(const DOOM::Doom& doom, float height) const;   // Get next lowest neighbor floor level from height
        std::pair<std::int16_t, float>  getNeighborNextHighestFloor(const DOOM::Doom& doom, float height) const;  // Get next highest neighbor floor level from height

        std::pair<std::int16_t, float>  getNeighborLowestCeiling(const DOOM::Doom& doom) const;                     // Get lowest neighbor floor level
        std::pair<std::int16_t, float>  getNeighborHighestCeiling(const DOOM::Doom& doom) const;                    // Get highest neighbor floor level
        std::pair<std::int16_t, float>  getNeighborNextLowestCeiling(const DOOM::Doom& doom, float height) const;   // Get next lowest neighbor floor level from height
        std::pair<std::int16_t, float>  getNeighborNextHighestCeiling(const DOOM::Doom& doom, float height) const;  // Get next highest neighbor floor level from height

        const std::set<std::int16_t>& getNeighbors() const; // Get neighbors indexes

        std::int16_t  getShortestLowerTexture(const DOOM::Doom& doom) const;  // Get shortest lower texture height on the boundary of the sector

        std::int16_t  getNeighborLowestLight(const DOOM::Doom& doom) const;   // Get lowest neighbor light level
        std::int16_t  getNeighborHighestLight(const DOOM::Doom& doom) const;  // Get highest neighbor light level
      };

      class Blockmap
      {
      public:
        struct Block
        {
          std::vector<std::int16_t>                             linedefs; // Indexes of linedefs in block
          std::set<std::reference_wrapper<DOOM::AbstractThing>> things;   // Things in block
        };

        std::int16_t  x;      // Blockmap X origin
        std::int16_t  y;      // Blockmap Y origin
        std::int16_t  column; // Number of column in blockmap
        std::int16_t  row;    // Number of row in blockmap

        std::vector<Block>                                                  blocks;   // Blockmap
        std::unordered_map<const DOOM::Doom::Level::Sector*, std::set<int>> sectors;  // Index of blocks of each sector

        Blockmap() = default;
        Blockmap(DOOM::Doom& doom, const DOOM::Wad::RawLevel::Blockmap& blockmap);
        ~Blockmap() = default;

        int index(const Math::Vector<2>& position) const; // Get index of block at given position

        void  addThing(DOOM::AbstractThing& thing, const Math::Vector<2>& position);                                            // Add thing to blockmap
        void  moveThing(DOOM::AbstractThing& thing, const Math::Vector<2>& old_position, const Math::Vector<2>& new_position);  // Update thing position in blockmap
        void  removeThing(DOOM::AbstractThing& thing, const Math::Vector<2>& position);                                         // Remove thing from blockmap
      };

      class Statistics
      {
      public:
        struct Stats
        {
          unsigned int  kills, items, secrets;  // Kills, items and secrets counters

          Stats() : kills(0), items(0), secrets(0) {};
          ~Stats() = default;
        };

        std::map<int, DOOM::Doom::Level::Statistics::Stats> players;  // Individual counter for each player
        DOOM::Doom::Level::Statistics::Stats                total;    // Total number in map
        float                                               time;     // Time played in level

        Statistics();
        ~Statistics() = default;

        void  update(DOOM::Doom& doom, float elapsed); // Update statistics
      };

    private:
      bool  getLinedefsNode(std::list<std::pair<float, std::int16_t>>& result, const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit, std::int16_t index) const;       // Recursively find closest subsectors
      bool  getLinedefsSubsector(std::list<std::pair<float, std::int16_t>>& result, const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit, std::int16_t index) const;  // Iterate through seg of subsector
      float getLinedefsSeg(std::list<std::pair<float, std::int16_t>>& result, const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit, std::int16_t index) const;        // Get intersection with sidedef

    public:
      std::pair<std::uint8_t, std::uint8_t>                         episode;    // Level episode and episode's mission number
      DOOM::Enum::End                                               end;
      std::reference_wrapper<const DOOM::Doom::Resources::Texture>  sky;        // Sky texture of the level
      std::vector<std::reference_wrapper<DOOM::PlayerThing>>        players;    // List of players (references to PlayerThing in things list)
      std::list<std::unique_ptr<DOOM::AbstractThing>>               things;     // List of things
      std::vector<std::unique_ptr<DOOM::AbstractLinedef>>           linedefs;   // List of linedefs
      std::vector<DOOM::Doom::Level::Sidedef>                       sidedefs;   // List of sidedefs
      std::vector<DOOM::Doom::Level::Vertex>                        vertexes;   // List of vertexes
      std::vector<DOOM::Doom::Level::Segment>                       segments;   // List of segs
      std::vector<DOOM::Doom::Level::Subsector>                     subsectors; // List of subsectors
      std::vector<DOOM::Doom::Level::Node>                          nodes;      // List of nodes
      std::vector<DOOM::Doom::Level::Sector>                        sectors;    // List of sectors
      DOOM::Doom::Level::Blockmap                                   blockmap;   // Blockmap of level
      DOOM::Doom::Level::Statistics                                 statistics; // Statistics of level
      

      std::set<std::int16_t>                                                    getSectors(const Math::Vector<2>& position, float radius) const;                                                                                // Return sector indexes at position/radius
      std::set<std::int16_t>                                                    getSectors(const DOOM::AbstractThing& thing) const;                                                                                             // Return sector indexes that thing (position and radius/2) is over
      std::pair<std::int16_t, std::int16_t>                                     getSector(const Math::Vector<2>& position, std::int16_t index = -1) const;                                                                      // Return sector/subsector at position
      std::list<std::pair<float, std::int16_t>>                                 getLinedefs(const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit = 1.f) const;                                        // Return an ordered list of linedef index intersected by ray within distance limit
      std::set<std::int16_t>                                                    getLinedefs(const Math::Vector<2>& position, float radius) const;                                                                               // Return a list of linedef index at a position/radius
      std::list<std::reference_wrapper<DOOM::AbstractThing>>                    getThings(const DOOM::Doom::Level::Sector& sector, DOOM::Enum::ThingProperty properties = DOOM::Enum::ThingProperty::ThingProperty_None) const; // Return things in sector with corresponding properties
      std::set<std::reference_wrapper<DOOM::AbstractThing>>                     getThings(const Math::Vector<2>& position, float radius) const;                                                                                 // Return things at given position / radius
      std::list<std::pair<float, std::reference_wrapper<DOOM::AbstractThing>>>  getThings(const Math::Vector<2>& position, const Math::Vector<2>& direction, float limit = 1.f) const;                                          // Return an ordered list of things intersected by ray within distance limit

      Level();
      ~Level() = default;

      void  update(DOOM::Doom& doom, float elapsed); // Update level components
    };

  private:
    void  clear();          // Clear previously loaded resources
    void  clearResources(); // Clear resources
    void  clearLevel();     // Clear current level

    void  buildResources();           // Build resources from WAD
    void  buildResourcesPalettes();   // Build color palettes from WAD
    void  buildResourcesColormaps();  // Build color maps from WAD
    void  buildResourcesTextures();   // Build textures from WAD
    void  buildResourcesSprites();    // Build sprites textures from WAD
    void  buildResourcesMenus();      // Build menus textures from WAD
    void  buildResourcesFlats();      // Build flats from WAD
    void  buildResourcesSounds();     // Build sounds from WAD

    void  buildLevel(const std::pair<uint8_t, uint8_t>& level); // Build level from WAD file
    void  buildLevelVertexes();                                 // Build level's vertexes from WAD file
    void  buildLevelSectors();                                  // Build level's sectors from WAD file
    void  buildLevelLinedefs();                                 // Build level's linedefs from WAD file
    void  buildLevelSidedefs();                                 // Build level's sidedefs from WAD file
    void  buildLevelSubsectors();                               // Build level's subsectors from WAD file
    void  buildLevelThings();                                   // Build level's things from WAD file
    void  buildLevelSegments();                                 // Build level's segments from WAD file
    void  buildLevelNodes();                                    // Build level's nodes from WAD file
    void  buildLevelBlockmap();                                 // Build level's blockmap from WAD file
    void  buildLevelStatistics();                               // Initialize level statistics for loaded level

  public:
    Doom();
    ~Doom() = default;

    DOOM::Wad             wad;        // File holding WAD datas
    DOOM::Doom::Resources resources;  // Resources built from WAD
    DOOM::Doom::Level     level;      // Current level datas build from WAD
    DOOM::Enum::Mode      mode;       // Current game mode
    DOOM::Enum::Skill     skill;      // Current game skill level
    float                 sensivity;  // Mouse sensivity [0-1]
    float                 sfx;        // SFX sound volume [0-1]
    float                 music;      // Music valume [0-1]
    bool                  message;    // Message enabled
    sf::Image             image;      // DOOM rendering target

    void  load(const std::filesystem::path& file, DOOM::Enum::Mode mode); // Load WAD file and build resources
    void  update(float elapsed);                                          // Update current level and resources

    std::list<std::pair<std::uint8_t, std::uint8_t>>  getLevels() const;                                                          // Return list of available level in WAD
    void                                              setLevel(std::pair<std::uint8_t, std::uint8_t> level, bool reset = false);  // Build specified level from WAD, use reset to hard reset players

    void  addPlayer(int controller);  // Add player to current game

    void  sound(DOOM::Doom::Resources::Sound::EnumSound sound, bool loop = false);                                                                      // Play a sound
    void  sound(DOOM::Doom::Resources::Sound::EnumSound sound, const Math::Vector<3>& position, bool loop = false);                                     // Play a sound relatively to a position
    void  sound(Game::Audio::Sound::Reference& ref, DOOM::Doom::Resources::Sound::EnumSound sound, bool loop = false);                                  // Play a sound with given reference
    void  sound(Game::Audio::Sound::Reference& ref, DOOM::Doom::Resources::Sound::EnumSound sound, const Math::Vector<3>& position, bool loop = false); // Play a sound relatively to a position with given reference
  };
}

namespace std
{
  // Define comparison of Thing ref to build a std::set
  bool  operator<(const std::reference_wrapper<DOOM::AbstractThing>& left, const std::reference_wrapper<DOOM::AbstractThing>& right);
}

// Definition of forward-declared class (NOTE: we shouldn't do this)
#include "Doom/Action/AbstractAction.hpp"
#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Thing/AbstractThing.hpp"