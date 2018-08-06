#ifndef _DOOM_HPP_
#define _DOOM_HPP_

#include <array>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Doom/Wad.hpp"
#include "Math/Vector.hpp"

namespace DOOM
{
  namespace EnumLinedef
  {
    enum Trigger
    {
      TriggerNone = 0b0000,
      TriggerPushed = 0b0001,
      TriggerSwitched = 0b0010,
      TriggerWalkover = 0b0100,
      TriggerGunfire = 0b1000
    };

    enum Action
    {
      ActionLeveling,
      ActionLighting
    };

    enum Key
    {
      KeyNone,
      KeyBlue,
      KeyRed,
      KeyYellow,
    };

    enum Repeat
    {
      RepeatFalse = false,
      RepeatTrue = true
    };

    enum Monster
    {
      MonsterFalse = false,
      MonsterTrue = true
    };

    enum Light
    {
      Light35,
      Light255,
      LightMinimum,
      LightMaximum
    };

    enum Direction
    {
      DirectionDown,
      DirectionUp
    };
  };

  namespace EnumAction
  {
    enum Type
    {
      TypeLeveling,	// Leveling action
      TypeLighting,	// Lighting action
      TypeNumber	// Number of type of action
    };

    enum Speed
    {
      SpeedSlow = 1,	// Move 1 units/tic
      SpeedNormal = 2,	// Move 2 units/tic
      SpeedFast = 4,	// Move 4 units/tic
      SpeedTurbo = 8	// Move 8 units/tic
    };

    enum DoorState
    {
      DoorStateOpen,		// Open the door
      DoorStateClose,		// Close the door
      DoorStateForceClose,	// Close the door without bouncing on thing
      DoorStateWait,		// Wait for the specified time
      DoorStateForceWait	// Wait for a fixed time
    };

    enum LiftState
    {
      LiftStateRaise,	// Raise the lift
      LiftStateLower,	// Lower the lift
      LiftStateWait,	// Wait for the specified time
      LiftStateStop	// Stop lift
    };

    enum PlatformState
    {
      PlatformStateRaise,	// Raise the platform
      PlatformStateLower,	// Lower the platform
      PlatformStateStop		// Stop platform
    };

    enum CrusherState
    {
      CrusherStateRaise,	// Raise the crusher
      CrusherStateLower		// Lower the crusher
    };

    enum Direction
    {
      DirectionUp,	// Move upward
      DirectionDown	// Move downward
    };

    enum Repeat
    {
      RepeatFalse = false,
      RepeatTrue = true
    };

    enum Silent
    {
      SilentFalse = false,
      SilentTrue = true
    };

    enum Crush
    {
      CrushFalse = false,	// Doesn't crush things
      CrushTrue = true		// Crush things
    };

    enum Change
    {
      ChangeNone,		// No change
      ChangeTexture,		// Copy texture from model
      ChangeTextureZeroed,	// Copy texture from model and zeroed type
      ChangeTextureType,	// Copy texture and type from model
    };
  }

  // Forward declaration of external components
  class AbstractAction;
  class AbstractFlat;
  class AbstractLinedef;
  class AbstractThing;

  class Doom
  {
  public:
    static sf::Time const	Tic;

    struct Resources
    {
      class Palette : public std::array<sf::Color, 256>
      {
      public:
	Palette() = default;
	Palette(DOOM::Doom & doom, const DOOM::Wad::RawResources::Palette & palette);
	~Palette() = default;
      };

      class Colormap : public std::array<uint8_t, 256>
      {
      public:
	Colormap() = default;
	Colormap(DOOM::Doom & doom, const DOOM::Wad::RawResources::Colormap & colormap);
	~Colormap() = default;
      };

      class Texture
      {
      public:
	static const DOOM::Doom::Resources::Texture	Null;	// Empty texture

	struct Column
	{
	  struct Span
	  {
	    uint8_t			offset;	// Offset of the span of pixels in the column
	    std::vector<uint8_t>	pixels;	// Pixels colors indexes
	  };

	  std::vector<Span>	spans;	// Vector of spans of pixels in the column
	};

	int16_t			width, height;	// Size of texture
	int16_t			left, top;	// Texture offset (sprite only)
	std::vector<Column>	columns;	// Pre-computed texture from patches

	//Texture(const Texture &) = default;
	Texture() = default;
	Texture(DOOM::Doom & doom, const DOOM::Wad::RawResources::Texture & texture);
	Texture(DOOM::Doom & doom, const DOOM::Wad::RawResources::Patch & patch);
	~Texture() = default;
      };

      class Sound
      {
      public:
	sf::SoundBuffer	buffer;	// Sound buffer
	sf::Sound	sound;	// Actual sound to be played

	Sound(DOOM::Doom & doom, const DOOM::Wad::RawResources::Sound & sound);
	~Sound() = default;
      };

      std::array<DOOM::Doom::Resources::Palette, 14>			palettes;	// Color palettes
      std::array<DOOM::Doom::Resources::Colormap, 34>			colormaps;	// Color brightness maps
      std::unordered_map<uint64_t, std::unique_ptr<DOOM::AbstractFlat>>	flats;		// Map of flat (ground/ceiling texture)
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>	textures;	// Map of wall textures
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>	sprites;	// Map of sprites
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>	menus;		// Map of menu patches
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Sound>	sounds;		// Map of sounds

      Resources() = default;
      ~Resources() = default;

      void	update(DOOM::Doom & doom, sf::Time elapsed);	// Update resources components
    };

    struct Level
    {
      class Vertex : public Math::Vector<2>
      {
      public:
	Vertex(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Vertex & vertex);
	~Vertex() = default;
      };

      class Sidedef
      {
	static const int	FrameDuration = 8;	// Tics between two frames of animation

      public:
	float	x, y;	// Texture offset
	int16_t	sector;	// Index of the sector it references

      private:
	sf::Time									_elapsed;	// Total elapsed time
	std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	_upper;		// Pointer to upper/lower/middle textures
	std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	_lower;		// Pointer to lower textures
	std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	_middle;	// Pointer to middle textures

	std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	animation(const DOOM::Doom & doom, uint64_t name) const;	// Return frames of animation

      public:
	Sidedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sidedef & sidedef);
	~Sidedef() = default;

	void					update(DOOM::Doom & doom, sf::Time elapsed);	// Update sidedef
	const DOOM::Doom::Resources::Texture &	upper() const;					// Return upper texture to be displayed
	const DOOM::Doom::Resources::Texture &	lower() const;					// Return lower texture to be displayed
	const DOOM::Doom::Resources::Texture &	middle() const;					// Return middle texture to be displayed
      };

      class Segment
      {
      public:
	int16_t	start, end;	// Start/end segment vertexes indexes
	float	angle;		// Segment angle (rad.)
	int16_t	linedef;	// Segment linedef index
	int16_t	direction;	// 0 (same as linedef) or 1 (opposite of linedef)
	int16_t	offset;		// Distance along linedef to start of seg

	Segment(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Segment & segment);
	~Segment() = default;
      };

      class Subsector
      {
      public:
	int16_t	count;	// Seg count
	int16_t	index;	// First seg number
	int16_t	sector;	// Index of sector that this subsector is part of

	Subsector(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Subsector & subsector);
	~Subsector() = default;
      };

      class Node
      {
      public:
	struct BoundingBox
	{
	  int16_t	top, bottom;	// Top and bottom limits
	  int16_t	left, right;	// Left and right limits
	};

	Math::Vector<2>	origin, direction;	// Partition line of nodes
	BoundingBox	rightbound, leftbound;	// Children bounding boxes
	int16_t		rightchild, leftchild;	// Children node index, or subsector index if bit 15 is set

	Node(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Node & node);
	~Node() = default;
      };

      class Sector
      {
      public:
	enum Special : int16_t
	{
	  Normal = 0x0000,		// Normal
	  LightBlinkRandom = 0x0001,	// Blink random
	  LightBlink05 = 0x0002,	// Blink 0.5 second
	  LightBlink10 = 0x0003,	// Blink 1.0 second
	  Damage20Blink05 = 0x0004,	// 20 % damage every 32 tics plus light blink 0.5 second
	  Damage10 = 0x0005,		// 10 % damage every 32 tics
	  Damage5 = 0x0007,		// 5 % damage every 32 tics
	  LightOscillates = 0x0008,	// Oscillates
	  Secret = 0x0009,		// Player entering this sector gets credit for finding a secret
	  DoorClose = 0x000A,		// 30 seconds after level start, ceiling closes like a door
	  End = 0x000B,			// Cancel God mode if active, 20 % damage every 32 tics, when player dies, level ends
	  LightBlink05Sync = 0x000C,	// Blink 0.5 second, synchronized
	  LightBlink10Sync = 0x000D,	// Blink 1.0 second, synchronized
	  DoorOpen = 0x000E,		// 300 seconds after level start, ceiling opens like a door
	  Damage20 = 0x0010,		// 20 % damage per second
	  LightFlickers = 0x0011	// Flickers randomly (fire)
	};

	uint64_t						floor_name, ceiling_name;	// Textures names
	std::reference_wrapper<const DOOM::AbstractFlat>	floor_flat, ceiling_flat;	// Textures pointers (null if sky)
	
	int16_t	light_current, light_base;	// Sector base and current light level
	float	floor_current, floor_base;	// Sector base and current floor height
	float	ceiling_current, ceiling_base;	// Sector base and current ceiling height
	
	float	damage;		// Sector damage/sec
	int16_t	tag;		// Sector/linedef tag
	int16_t	special;	// Sector special attribute

      protected:
	std::vector<int16_t>	_neighbors;	// List of neighbor sectors (sorted)

      private:
	std::array<std::unique_ptr<DOOM::AbstractAction>, DOOM::EnumAction::Type::TypeNumber>	_actions;	// Actions on sector
	
	static std::unique_ptr<DOOM::AbstractAction>	_factory(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t type, int16_t model);	// Action factory intermediate function (cycling inclusion problem)

      public:
	Sector(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
	Sector(DOOM::Doom::Level::Sector && sector);
	~Sector() = default;

	void	update(DOOM::Doom & doom, sf::Time elapsed);	// Update sector

	template<DOOM::EnumAction::Type Type>
	inline void	action(DOOM::Doom & doom, int16_t type, int16_t model = -1)	// Add action to sector if possible
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::EnumAction::Type::TypeNumber, "Invalid action template parameters.");

	  // Push action if slot available
	  if (_actions.at(Type).get() == nullptr)
	    _actions.at(Type) = std::move(_factory(doom, *this, type, model));
	}

	template<DOOM::EnumAction::Type Type>
	inline void	action(std::unique_ptr<DOOM::AbstractAction> && action)	// Add action to sector if possible
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::EnumAction::Type::TypeNumber, "Invalid action template parameters.");

	  // Push action if slot available
	  if (_actions.at(Type).get() == nullptr)
	    _actions.at(Type) = std::move(action);
	}

	template<DOOM::EnumAction::Type Type>
	inline const std::unique_ptr<DOOM::AbstractAction> &	action() const	// Get action of sector
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::EnumAction::Type::TypeNumber, "Invalid action template parameters.");

	  return _actions.at(Type);
	}

	template<DOOM::EnumAction::Type Type>
	inline std::unique_ptr<DOOM::AbstractAction> &	action()	// Get/set action of sector
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::EnumAction::Type::TypeNumber, "Invalid action template parameters.");

	  return _actions.at(Type);
	}

	std::pair<int16_t, float>	getNeighborLowestFloor(const DOOM::Doom & doom) const;				// Get lowest neighbor floor level
	std::pair<int16_t, float>	getNeighborHighestFloor(const DOOM::Doom & doom) const;				// Get highest neighbor floor level
	std::pair<int16_t, float>	getNeighborNextLowestFloor(const DOOM::Doom & doom, float height) const;	// Get next lowest neighbor floor level from height
	std::pair<int16_t, float>	getNeighborNextHighestFloor(const DOOM::Doom & doom, float height) const;	// Get next highest neighbor floor level from height

	std::pair<int16_t, float>	getNeighborLowestCeiling(const DOOM::Doom & doom) const;			// Get lowest neighbor floor level
	std::pair<int16_t, float>	getNeighborHighestCeiling(const DOOM::Doom & doom) const;			// Get highest neighbor floor level
	std::pair<int16_t, float>	getNeighborNextLowestCeiling(const DOOM::Doom & doom, float height) const;	// Get next lowest neighbor floor level from height
	std::pair<int16_t, float>	getNeighborNextHighestCeiling(const DOOM::Doom & doom, float height) const;	// Get next highest neighbor floor level from height

	int16_t	getShortestLowerTexture(const DOOM::Doom & doom) const;	// Get shortest lower texture height on the boundary of the sector

	int16_t	getNeighborLowestLight(const DOOM::Doom & doom) const;	// Get lowest neighbor light level
	int16_t	getNeighborHighestLight(const DOOM::Doom & doom) const;	// Get highest neighbor light level
      };

      class Blockmap
      {
      public:
	struct Block
	{
	  std::vector<int16_t>						linedefs;	// Indexes of linedefs in block
	  std::list<std::reference_wrapper<DOOM::AbstractThing>>	things;		// Things in block
	};

	int16_t	x;	// Blockmap X origin
	int16_t	y;	// Blockmap Y origin
	int16_t	column;	// Number of column in blockmap
	int16_t	row;	// Number of row in blockmap

	std::vector<Block>	blocks;	// Blockmap

	Blockmap() = default;
	Blockmap(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Blockmap & blockmap);
	~Blockmap() = default;
      };

      std::pair<uint8_t, uint8_t>					episode;	// Level episode and episode's mission number
      std::reference_wrapper<const DOOM::Doom::Resources::Texture>	sky;		// Sky texture of the level
      std::vector<std::unique_ptr<DOOM::AbstractThing>>			things;		// List of things
      std::vector<std::unique_ptr<DOOM::AbstractLinedef>>		linedefs;	// List of linedefs
      std::vector<DOOM::Doom::Level::Sidedef>				sidedefs;	// List of sidedefs
      std::vector<DOOM::Doom::Level::Vertex>				vertexes;	// List of vertexes
      std::vector<DOOM::Doom::Level::Segment>				segments;	// List of segs
      std::vector<DOOM::Doom::Level::Subsector>				subsectors;	// List of subsectors
      std::vector<DOOM::Doom::Level::Node>				nodes;		// List of nodes
      std::vector<DOOM::Doom::Level::Sector>				sectors;	// List of sectors
      DOOM::Doom::Level::Blockmap					blockmap;	// Blockmap of level

      std::pair<int16_t, int16_t>	sector(const Math::Vector<2> & position, int16_t index = -1) const;	// Return sector/subsector at position

      Level();
      ~Level() = default;

      void	update(DOOM::Doom & doom, sf::Time elapsed);	// Update level components
    };

  private:
    void	clear();		// Clear previously loaded resources
    void	clearResources();	// Clear resources
    void	clearLevel();		// Clear current level

    void	buildResources();		// Build resources from WAD
    void	buildResourcesPalettes();	// Build color palettes from WAD
    void	buildResourcesColormaps();	// Build color maps from WAD
    void	buildResourcesTextures();	// Build textures from WAD
    void	buildResourcesSprites();	// Build sprites textures from WAD
    void	buildResourcesMenus();		// Build menus textures from WAD
    void	buildResourcesFlats();		// Build flats from WAD
    void	buildResourcesSounds();		// Build sounds from WAD
    
    void	buildLevel(std::pair<uint8_t, uint8_t> const & level);	// Build level from WAD file
    void	buildLevelVertexes();					// Build level's vertexes from WAD file
    void	buildLevelSectors();					// Build level's sectors from WAD file
    void	buildLevelLinedefs();					// Build level's linedefs from WAD file
    void	buildLevelSidedefs();					// Build level's sidedefs from WAD file
    void	buildLevelSubsectors();					// Build level's subsectors from WAD file
    void	buildLevelThings();					// Build level's things from WAD file
    void	buildLevelSegments();					// Build level's segments from WAD file
    void	buildLevelNodes();					// Build level's nodes from WAD file
    void	buildLevelBlockmap();					// Build level's blockmap from WAD file

  public:
    Doom() = default;
    ~Doom() = default;

    DOOM::Wad			wad;		// File holding WAD datas
    DOOM::Doom::Resources	resources;	// Resources built from WAD
    DOOM::Doom::Level		level;		// Current level datas build from WAD

    void	load(const std::string & file);	// Load WAD file and build resources
    void	update(sf::Time elapsed);	// Update current level and resources

    std::list<std::pair<uint8_t, uint8_t>>	getLevel() const;					// Return list of available level in WAD
    void					setLevel(const std::pair<uint8_t, uint8_t> & level);	// Build specified level from WAD, return true if successful
  };
};

// Definition of class forward-declared
#include "Doom/Action/AbstractAction.hpp"
#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Thing/AbstractThing.hpp"

#endif