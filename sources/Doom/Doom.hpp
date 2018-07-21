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

    enum State
    {
      StateOpen,	// Open the door
      StateClose,	// Close the door
      StateForceClose,	// Close the door without bouncing on thing
      StateWait,	// Wait for the spcified time
      StateForceWait	// Wait for a fixed time
    };

    enum Direction
    {
      DirectionUp,	// Move upward
      DirectionDown	// Move downward
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

      class AbstractFlat
      {
      public:
	static std::unique_ptr<DOOM::Doom::Resources::AbstractFlat>	factory(DOOM::Doom & doom, uint64_t name, const DOOM::Wad::RawResources::Flat & flat);

      protected:
	AbstractFlat() = default;

      public:
	AbstractFlat(DOOM::Doom & doom);
	virtual ~AbstractFlat() = default;

	virtual void					update(DOOM::Doom & doom, sf::Time elapsed);	// Update flat
	virtual const std::array<uint8_t, 4096> &	flat() const = 0;				// Return flat to be displayed
      };

      class Texture
      {
      public:
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

      static const DOOM::Doom::Resources::Texture	NullTexture;	// Empty texture

      std::array<DOOM::Doom::Resources::Palette, 14>						palettes;	// Color palettes
      std::array<DOOM::Doom::Resources::Colormap, 34>						colormaps;	// Color brightness maps
      std::unordered_map<uint64_t, std::unique_ptr<DOOM::Doom::Resources::AbstractFlat>>	flats;		// Map of flat (ground/ceiling texture)
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>				textures;	// Map of wall textures
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>				sprites;	// Map of sprites
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>				menus;		// Map of menu patches
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Sound>				sounds;		// Map of sounds

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
	int16_t	x, y;	// Texture offset
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

      class AbstractThing
      {
      public:
	static std::unique_ptr<DOOM::Doom::Level::AbstractThing>	factory(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing);

	enum Flag : int16_t
	{
	  SkillLevel12 = 0x0001,	// Active on skill level 1 & 2
	  SkillLevel3 = 0x0002,	// Active on skill level 3
	  SkillLevel45 = 0x0004,	// Active on skill level 4 & 5
	  Ambush = 0x0008,		// Monster is in ambush mode
	  Multiplayer = 0x0010	// Only active in multiplayer mode
	};

	enum Properties : int16_t
	{
	  None = 0x0000,		// No properties
	  Artifact = 0x0001,	// Artifact item, counts toward ITEMS percentage at the end of a level
	  Hanging = 0x0002,		// Hangs from ceiling, or floats if a monster
	  Monster = 0x0004,		// Monster, counts towards kill percentage
	  Obstacle = 0x0008,	// Obstacle, players and monsters must walk around
	  Pickup = 0x0010		// Pickup, player can pick the thing up by walking over it
	};

	Math::Vector<2>	position;	// Thing position
	float		height;		// Thing height from floor (or ceiling is hanging)
	float		angle;		// Thing orientation (rad.)
	const int16_t	type;		// Thing flags (see enum)
	const int16_t	flag;		// Thing flags (see enum)
	const int16_t	radius;		// Thing radius (square box)
	const int16_t	properties;	// Thing properties (see enum)

	AbstractThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t radius, int16_t properties);
	virtual ~AbstractThing() = default;

	virtual bool											update(DOOM::Doom & doom, sf::Time elapsed);	// Update thing, return true if thing should be deleted
	virtual const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	sprite(float angle) const = 0;			// Return sprite to be displayed
      };

      class AbstractLinedef
      {
      public:
	static std::unique_ptr<DOOM::Doom::Level::AbstractLinedef>	factory(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef);

	enum Flag
	{
	  Impassible = 0x0001,	// Players and monsters cannot cross this line
	  BlockMonsters = 0x0002,	// Monsters cannot cross this line
	  TwoSided = 0x0004,	// See documentation
	  UpperUnpegged = 0x0008,	// The upper texture is pasted onto the wall from the top down instead of from the bottom up like usual
	  LowerUnpegged = 0x0010,	// Lower and middle textures are drawn from the bottom up, instead of from the top down like usual
	  Secret = 0x0020,		// See documentation
	  BlockSound = 0x0040,	// For purposes of monsters hearing sounds and thus becoming alerted
	  NotOnMap = 0x0080,	// The line is not shown on the automap
	  AlreadyOnMap = 0x0100	// When the level is begun, this line is already on the automap
	};

	int16_t	start, end;	// Start and end vertexes
	int16_t	flag;		// Linedef flag (see enum)
	int16_t	type;		// Linedef type (see enum)
	int16_t	tag;		// Linedef sector tag
	int16_t	front, back;	// Front (right) and back (left) sidedefs indexes (-1 if no sidedef)

	AbstractLinedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Linedef & linedef);
	AbstractLinedef(DOOM::Doom & doom, const DOOM::Doom::Level::AbstractLinedef & linedef);
	virtual ~AbstractLinedef() = default;

	virtual void	update(DOOM::Doom & doom, sf::Time elapsed) = 0;	// Update linedef (TODO: remove this ?)

	virtual void	pushed(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing);	// To call when linedef is pushed by thing
	virtual void	switched(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing);	// To call when linedef is switched (used) by thing
	virtual void	walkover(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing);	// To call when thing walk over the linedef
	virtual void	gunfire(DOOM::Doom & doom, DOOM::Doom::Level::AbstractThing & thing);	// To call when thing shot the linedef
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
	class AbstractAction
	{
	public:
	  static std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction>	factory(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t type);	// Factory of sector action build from type

	public:
	  AbstractAction(DOOM::Doom & doom);
	  virtual ~AbstractAction() = default;

	  virtual void	update(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, sf::Time elapsed) = 0;	// Update sector's action
	  virtual void	stop();											// Request action to stop (for lift & crusher)
	};

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

	uint64_t								floor_name, ceiling_name;	// Textures names
	std::reference_wrapper<const DOOM::Doom::Resources::AbstractFlat>	floor_flat, ceiling_flat;	// Textures pointers (null if sky)
	
	int16_t	light_current, light_base;	// Sector base and current light level
	float	floor_current, floor_base;	// Sector base and current floor height
	float	ceiling_current, ceiling_base;	// Sector base and current ceiling height
	
	float	damage;		// Sector damage/sec
	int16_t	tag;		// Sector/linedef tag
	int16_t	special;	// Sector special attribute

      protected:
	std::vector<int16_t>	_neighbors;	// List of neighbor sectors

      private:
	std::array<std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction>, DOOM::EnumAction::Type::TypeNumber>	_actions;	// Actions on sector
	
      public:
	Sector(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
	Sector(DOOM::Doom::Level::Sector && sector);
	~Sector() = default;

	void	update(DOOM::Doom & doom, sf::Time elapsed);	// Update sector

	template<DOOM::EnumAction::Type Type>
	inline void	action(DOOM::Doom & doom, int16_t type)	// Add action to sector if possible
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::EnumAction::Type::TypeNumber, "Invalid action template parameters.");

	  // Push action if slot available
	  if (_actions.at(Type).get() == nullptr)
	    _actions.at(Type) = std::move(DOOM::Doom::Level::Sector::AbstractAction::factory(doom, *this, type));
	}

	template<DOOM::EnumAction::Type Type>
	inline void	action(std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction> && action)	// Add action to sector if possible
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::EnumAction::Type::TypeNumber, "Invalid action template parameters.");

	  // Push action if slot available
	  if (_actions.at(Type).get() == nullptr)
	    _actions.at(Type) = std::move(action);
	}

	template<DOOM::EnumAction::Type Type>
	inline const std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction> &	action() const	// Get action of sector
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::EnumAction::Type::TypeNumber, "Invalid action template parameters.");

	  return _actions.at(Type);
	}

	template<DOOM::EnumAction::Type Type>
	inline std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction> &	action()	// Get/set action of sector
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::EnumAction::Type::TypeNumber, "Invalid action template parameters.");

	  return _actions.at(Type);
	}

	float	getNeighborLowestFloor(const DOOM::Doom & doom) const;				// Get lowest neighbor floor level
	float	getNeighborHighestFloor(const DOOM::Doom & doom) const;				// Get highest neighbor floor level
	float	getNeighborNextLowestFloor(const DOOM::Doom & doom, float height) const;	// Get next lowest neighbor floor level from height
	float	getNeighborNextHighestFloor(const DOOM::Doom & doom, float height) const;	// Get next highest neighbor floor level from height

	float	getNeighborLowestCeiling(const DOOM::Doom & doom) const;			// Get lowest neighbor floor level
	float	getNeighborHighestCeiling(const DOOM::Doom & doom) const;			// Get highest neighbor floor level
	float	getNeighborNextLowestCeiling(const DOOM::Doom & doom, float height) const;	// Get next lowest neighbor floor level from height
	float	getNeighborNextHighestCeiling(const DOOM::Doom & doom, float height) const;	// Get next highest neighbor floor level from height

	int16_t	getNeighborLowestLight(const DOOM::Doom & doom) const;	// Get lowest neighbor light level
	int16_t	getNeighborHighestLight(const DOOM::Doom & doom) const;	// Get highest neighbor light level
      };

      class Blockmap
      {
      public:
	struct Block
	{
	  std::vector<int16_t>				linedefs;	// Indexes of linedefs in block
	  std::list<DOOM::Doom::Level::AbstractThing *>	things;		// Things in block
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
      std::vector<std::unique_ptr<DOOM::Doom::Level::AbstractThing>>	things;		// List of things
      std::vector<std::unique_ptr<DOOM::Doom::Level::AbstractLinedef>>	linedefs;	// List of linedefs
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

#endif