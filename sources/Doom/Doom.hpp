#ifndef _DOOM_HPP_
#define _DOOM_HPP_

#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Doom/Wad.hpp"
#include "Math/Vector.hpp"

namespace DOOM
{
  namespace EnumAction
  {
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
      DirectionUp,
      DirectionDown
    };

    enum Crush
    {
      CrushFalse = false,
      CrushTrue = true
    };
  }

  class AbstractFlat;
  class AbstractLinedef;
  class AbstractThing;

  class Doom
  {
  public:
    static sf::Time const	Tic;

    struct Resources
    {
      typedef std::vector<sf::Color>	Palette;
      typedef std::vector<uint8_t>	Colormap;

      struct Texture
      {
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
      };

      struct Sound
      {
	sf::SoundBuffer	buffer;	// Sound buffer
	sf::Sound	sound;	// Actual sound to be played
      };

      static const DOOM::Doom::Resources::Texture	NullTexture;	// Empty texture

      std::vector<DOOM::Doom::Resources::Palette>			palettes;	// Color palettes
      std::vector<DOOM::Doom::Resources::Colormap>			colormaps;	// Color brightness maps
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>	textures;	// Map of wall textures
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>	sprites;	// Map of sprites
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>	menus;		// Map of menu patches
      std::unordered_map<uint64_t, std::unique_ptr<DOOM::AbstractFlat>>	flats;		// Map of flat (ground/ceiling texture)
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Sound>	sounds;		// Map of sounds

      void	update(sf::Time elapsed);	// Update resources components
    };

    struct Level
    {
      typedef Math::Vector<2>	Vertex;

      class Sidedef
      {
	static const int	FrameDuration = 8;	// Tics between two frames of animation

      public:
	int16_t	x, y;	// Texture offset
	int16_t	sector;	// Index of the sector it references

      private:
	sf::Time						_elapsed;	// Total elapsed time
	std::vector<const DOOM::Doom::Resources::Texture *>	_upper;		// Pointer to upper/lower/middle textures
	std::vector<const DOOM::Doom::Resources::Texture *>	_lower;		// Pointer to lower textures
	std::vector<const DOOM::Doom::Resources::Texture *>	_middle;	// Pointer to middle textures

	std::vector<const DOOM::Doom::Resources::Texture *>	animation(const DOOM::Doom & doom, uint64_t name) const;	// Return frames of animation

      public:
	Sidedef(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sidedef & sidedef);
	~Sidedef();

	void					update(sf::Time elapsed);	// Update sidedef
	const DOOM::Doom::Resources::Texture &	upper() const;			// Return upper texture to be displayed
	const DOOM::Doom::Resources::Texture &	lower() const;			// Return lower texture to be displayed
	const DOOM::Doom::Resources::Texture &	middle() const;			// Return middle texture to be displayed
      };

      struct Segment
      {
	int16_t	start, end;	// Start/end segment vertexes indexes
	float	angle;		// Segment angle (rad.)
	int16_t	linedef;	// Segment linedef index
	int16_t	direction;	// 0 (same as linedef) or 1 (opposite of linedef)
	int16_t	offset;		// Distance along linedef to start of seg
      };

      struct Subsector
      {
	int16_t	count;	// Seg count
	int16_t	index;	// First seg number
	int16_t	sector;	// Index of sector that this subsector is part of
      };

      struct Node
      {
	struct BoundingBox
	{
	  int16_t	top, bottom;	// Top and bottom limits
	  int16_t	left, right;	// Left and right limits
	};

	Math::Vector<2>	origin, direction;	// Partition line of nodes
	BoundingBox	rightbound, leftbound;	// Children bounding boxes
	int16_t		rightchild, leftchild;	// Children node index, or subsector index if bit 15 is set
      };

      class Sector
      {
      public:
	class AbstractAction
	{
	public:
	  static std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction>	factory(DOOM::Doom::Level::Sector & sector, int16_t type);	// Factory of sector action build from type

	protected:
	  void		remove(DOOM::Doom::Level::Sector & sector);	// Remove action from sector

	public:
	  AbstractAction();
	  virtual ~AbstractAction() = 0;

	  virtual void	update(DOOM::Doom::Level::Sector & sector, sf::Time elapsed) = 0;	// Update sector's action
	  virtual void	stop();									// Request action to stop (for lift & crusher)
	};

	enum Special
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

	uint64_t			floor_name, ceiling_name;	// Textures names
	const DOOM::AbstractFlat	*floor_flat, *ceiling_flat;	// Textures pointers (null if sky)
	int16_t				tag;				// Sector/linedef tag
	int16_t				special;			// Sector special attribute

      protected:
	int16_t	_light, _baseLight;	// Sector base and current light level
	float	_floor, _baseFloor;	// Sector base and current floor height
	float	_ceiling, _baseCeiling;	// Sector base and current ceiling height
	float	_damage, _baseDamage;	// Sector base and current damage/sec

	std::vector<int16_t>	_neighbors;	// List of neighbor sectors
	const DOOM::Doom &	_doom;		// Reference to DOOM instance

      private:
	std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction>	_leveling;	// Current floor/ceiling leveling effect
	std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction>	_lighting;	// Current lighting effect

      public:
	Sector(const DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
	Sector(DOOM::Doom::Level::Sector && sector);
	~Sector();

	void	update(sf::Time elapsed);	// Update sector

	// Const getter
	inline int16_t	light() const { return _light; }
	inline int16_t	baseLight() const { return _baseLight; }
	inline float	floor() const { return _floor; }
	inline float	baseFloor() const { return _baseFloor; }
	inline float	ceiling() const { return _ceiling; }
	inline float	baseCeiling() const { return _baseCeiling; }
	inline float	damage() const { return _damage; }
	inline float	baseDamage() const { return _baseDamage; }

	// Setter
	inline int16_t &	light() { return _light; }
	inline int16_t &	baseLight() { return _baseLight; }
	inline float &		floor() { return _floor; }
	inline float &		baseFloor() { return _baseFloor; }
	inline float &		ceiling() { return _ceiling; }
	inline float &		baseCeiling() { return _baseCeiling; }
	inline float &		damage() { return _damage; }
	inline float &		baseDamage() { return _baseDamage; }

	void	leveling(int16_t type);								// Add action to sector if possible
	void	leveling(std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction> && action);	// Add action to sector if possible
	void	lighting(int16_t type);								// Add action to sector if possible
	void	lighting(std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction> && action);	// Add action to sector if possible

	inline const std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction> &	leveling() const { return _leveling; }	// Get action of sector
	inline const std::unique_ptr<DOOM::Doom::Level::Sector::AbstractAction> &	lighting() const { return _lighting; }	// Get action of sector

	float	getNeighborLowestFloor() const;				// Get lowest neighbor floor level
	float	getNeighborHighestFloor() const;			// Get highest neighbor floor level
	float	getNeighborNextLowestFloor(float height) const;		// Get next lowest neighbor floor level from height
	float	getNeighborNextHighestFloor(float height) const;	// Get next highest neighbor floor level from height

	float	getNeighborLowestCeiling() const;			// Get lowest neighbor floor level
	float	getNeighborHighestCeiling() const;			// Get highest neighbor floor level
	float	getNeighborNextLowestCeiling(float height) const;	// Get next lowest neighbor floor level from height
	float	getNeighborNextHighestCeiling(float height) const;	// Get next highest neighbor floor level from height

	int16_t	getNeighborLowestLight() const;		// Get lowest neighbor light level
	int16_t	getNeighborHighestLight() const;	// Get highest neighbor light level
      };

      struct Blockmap
      {
	struct Block
	{
	  std::vector<int16_t>			linedefs;	// Indexes of linedefs in block
	  std::list<DOOM::AbstractThing *>	things;		// Things in block
	};

	int16_t	x;	// Blockmap X origin
	int16_t	y;	// Blockmap Y origin
	int16_t	column;	// Number of column in blockmap
	int16_t	row;	// Number of row in blockmap

	std::vector<Block>	blocks;	// Blockmap
      };

      std::pair<uint8_t, uint8_t>				episode;	// Level episode and episode's mission number
      DOOM::Doom::Resources::Texture				sky;		// Sky texture of the level
      std::vector<std::unique_ptr<DOOM::AbstractThing>>		things;		// List of things
      std::vector<std::unique_ptr<DOOM::AbstractLinedef>>	linedefs;	// List of linedefs
      std::vector<DOOM::Doom::Level::Sidedef>			sidedefs;	// List of sidedefs
      std::vector<DOOM::Doom::Level::Vertex>			vertexes;	// List of vertexes
      std::vector<DOOM::Doom::Level::Segment>			segments;	// List of segs
      std::vector<DOOM::Doom::Level::Subsector>			subsectors;	// List of subsectors
      std::vector<DOOM::Doom::Level::Node>			nodes;		// List of nodes
      std::vector<DOOM::Doom::Level::Sector>			sectors;	// List of sectors
      DOOM::Doom::Level::Blockmap				blockmap;	// Blockmap of level

      std::pair<int16_t, int16_t>	sector(const Math::Vector<2> & position, int16_t index = -1) const;	// Return sector/subsector at position

      void	update(sf::Time elapsed);	// Update level components
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
    
    void	buildLevel(const std::pair<uint8_t, uint8_t> & level);			// Build level from WAD file
    void	buildLevelVertexes(const std::pair<uint8_t, uint8_t> & level);		// Build level's vertexes from WAD file
    void	buildLevelSectors(const std::pair<uint8_t, uint8_t> & level);		// Build level's sectors from WAD file
    void	buildLevelLinedefs(const std::pair<uint8_t, uint8_t> & level);		// Build level's linedefs from WAD file
    void	buildLevelSidedefs(const std::pair<uint8_t, uint8_t> & level);		// Build level's sidedefs from WAD file
    void	buildLevelSubsectors(const std::pair<uint8_t, uint8_t> & level);	// Build level's subsectors from WAD file
    void	buildLevelThings(const std::pair<uint8_t, uint8_t> & level);		// Build level's things from WAD file
    void	buildLevelSegments(const std::pair<uint8_t, uint8_t> & level);		// Build level's segments from WAD file
    void	buildLevelNodes(const std::pair<uint8_t, uint8_t> & level);		// Build level's nodes from WAD file
    void	buildLevelBlockmap(const std::pair<uint8_t, uint8_t> & level);		// Build level's blockmap from WAD file
    
  public:
    DOOM::Wad			wad;		// File holding WAD datas
    DOOM::Doom::Resources	resources;	// Resources built from WAD
    DOOM::Doom::Level		level;		// Current level datas build from WAD

    Doom();
    ~Doom();

    void	load(const std::string & file);	// Load WAD file and build resources
    void	update(sf::Time elapsed);	// Update current level and resources

    std::list<std::pair<uint8_t, uint8_t>>	getLevel() const;					// Return list of available level in WAD
    void					setLevel(const std::pair<uint8_t, uint8_t> & level);	// Build specified level from WAD, return true if successful
  };
};

#endif