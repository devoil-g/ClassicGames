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
  // Forward declaration of external components
  class AbstractAction;
  class AbstractFlat;
  class AbstractLinedef;
  class AbstractThing;
  class PlayerThing;
  
  class Doom
  {
  public:
    static sf::Time const	Tic;			// Duration of a game tic (1/35s)
    static const unsigned int	RenderWidth;		// Default rendering width size
    static const unsigned int	RenderHeight;		// Default rendering height size
    static const float		RenderStretching;	// Default rendering vertical stretching

    class Resources
    {
    public:
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
      private:
	Texture() = default;

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

    public:
      std::array<DOOM::Doom::Resources::Palette, 14>												palettes;	// Color palettes
      std::array<DOOM::Doom::Resources::Colormap, 34>												colormaps;	// Color brightness maps
      std::unordered_map<uint64_t, std::unique_ptr<DOOM::AbstractFlat>>										flats;		// Map of flat (ground/ceiling texture)
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>										textures;	// Map of wall textures
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>										sprites;	// Map of raw sprites (not ordered, should not be used)
      std::unordered_map<uint64_t, std::vector<std::array<std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool>, 8>>>	animations;	// Map of sprites sorted by animation sequence and angle
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Texture>										menus;		// Map of menu patches
      std::unordered_map<uint64_t, DOOM::Doom::Resources::Sound>										sounds;		// Map of sounds

      Resources() = default;
      ~Resources() = default;

      void	update(DOOM::Doom & doom, sf::Time elapsed);	// Update resources components
    };

    class Level
    {
    public:
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
	sf::Time	_elapsed;	// Total elapsed time
	sf::Time	_toggle;	// Time before switch re-toggle

	uint64_t	_upper_name;	// Upper texture name
	uint64_t	_lower_name;	// Lower texture name
	uint64_t	_middle_name;	// Middle texture name

	std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	_upper_textures;	// Pointer to upper textures
	std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	_lower_textures;	// Pointer to lower textures
	std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	_middle_textures;	// Pointer to middle textures

	bool										switched(DOOM::Doom & doom, std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>> & textures, uint64_t & name);	// Switch textures
	std::vector<std::reference_wrapper<const DOOM::Doom::Resources::Texture>>	animation(const DOOM::Doom & doom, uint64_t name) const;										// Return frames of animation

      public:
	Sidedef(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sidedef & sidedef);
	~Sidedef() = default;

	void					update(DOOM::Doom & doom, sf::Time elapsed);	// Update sidedef
	bool					switched(DOOM::Doom & doom, sf::Time toggle);	// Toggle texture switch, reversing it after time given as parameter (0 for no reverse)

	const DOOM::Doom::Resources::Texture &	upper() const;	// Return upper texture to be displayed
	const DOOM::Doom::Resources::Texture &	lower() const;	// Return lower texture to be displayed
	const DOOM::Doom::Resources::Texture &	middle() const;	// Return middle texture to be displayed
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
	
	enum Action
	{
	  Leveling,	// Leveling action
	  Lighting,	// Lighting action
	  Number	// Number of type of action
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
	std::array<std::unique_ptr<DOOM::AbstractAction>, DOOM::Doom::Level::Sector::Action::Number>	_actions;	// Actions on sector
	
	static std::unique_ptr<DOOM::AbstractAction>	_factory(DOOM::Doom & doom, DOOM::Doom::Level::Sector & sector, int16_t type, int16_t model);	// Action factory intermediate function (cycling inclusion problem)

      public:
	Sector(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Sector & sector);
	Sector(DOOM::Doom::Level::Sector && sector);
	~Sector() = default;

	void	update(DOOM::Doom & doom, sf::Time elapsed);	// Update sector

	template<DOOM::Doom::Level::Sector::Action Type>
	inline void	action(DOOM::Doom & doom, int16_t type, int16_t model = -1)	// Add action to sector if possible
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::Doom::Level::Sector::Action::Number, "Invalid action template parameters.");

	  // Push action if slot available
	  if (_actions.at(Type).get() == nullptr)
	    _actions.at(Type) = std::move(_factory(doom, *this, type, model));
	}

	template<DOOM::Doom::Level::Sector::Action Type>
	inline void	action(std::unique_ptr<DOOM::AbstractAction> && action)	// Add action to sector if possible
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::Doom::Level::Sector::Action::Number, "Invalid action template parameters.");

	  // Push action if slot available
	  if (_actions.at(Type).get() == nullptr)
	    _actions.at(Type) = std::move(action);
	}

	template<DOOM::Doom::Level::Sector::Action Type>
	inline const std::unique_ptr<DOOM::AbstractAction> &	action() const	// Get action of sector
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::Doom::Level::Sector::Action::Number, "Invalid action template parameters.");

	  return _actions.at(Type);
	}

	template<DOOM::Doom::Level::Sector::Action Type>
	inline std::unique_ptr<DOOM::AbstractAction> &	action()	// Get/set action of sector
	{
	  // Check template parameter
	  static_assert(Type >= 0 && Type < DOOM::Doom::Level::Sector::Action::Number, "Invalid action template parameters.");

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

    private:
      bool	getLinedefsNode(std::list<int16_t> & result, const Math::Vector<2> & position, const Math::Vector<2> & direction, float limit, int16_t index) const;		// Recursively find closest subsectors
      bool	getLinedefsSubsector(std::list<int16_t> & result, const Math::Vector<2> & position, const Math::Vector<2> & direction, float limit, int16_t index) const;	// Iterate through seg of subsector
      float	getLinedefsSeg(std::list<int16_t> & result, const Math::Vector<2> & position, const Math::Vector<2> & direction, float limit, int16_t index) const;		// Get intersection with sidedef

    public:
      std::pair<uint8_t, uint8_t>					episode;	// Level episode and episode's mission number
      std::reference_wrapper<const DOOM::Doom::Resources::Texture>	sky;		// Sky texture of the level
      std::vector<std::reference_wrapper<DOOM::PlayerThing>>		players;	// List of players (references to PlayerThing in things list)
      std::vector<std::unique_ptr<DOOM::AbstractThing>>			things;		// List of things
      std::vector<std::unique_ptr<DOOM::AbstractLinedef>>		linedefs;	// List of linedefs
      std::vector<DOOM::Doom::Level::Sidedef>				sidedefs;	// List of sidedefs
      std::vector<DOOM::Doom::Level::Vertex>				vertexes;	// List of vertexes
      std::vector<DOOM::Doom::Level::Segment>				segments;	// List of segs
      std::vector<DOOM::Doom::Level::Subsector>				subsectors;	// List of subsectors
      std::vector<DOOM::Doom::Level::Node>				nodes;		// List of nodes
      std::vector<DOOM::Doom::Level::Sector>				sectors;	// List of sectors
      DOOM::Doom::Level::Blockmap					blockmap;	// Blockmap of level

      std::pair<int16_t, int16_t>	getSector(const Math::Vector<2> & position, int16_t index = -1) const;										// Return sector/subsector at position
      std::list<int16_t>		getLinedefs(const Math::Vector<2> & position, const Math::Vector<2> & direction, float limit = std::numeric_limits<float>::max()) const;	// Return an ordered list of linedef index intersected by ray within distance limit

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

    void	addPlayer(int controller);	// Add player to current game
  };
};

// Definition of class forward-declared
#include "Doom/Action/AbstractAction.hpp"
#include "Doom/Flat/AbstractFlat.hpp"
#include "Doom/Linedef/AbstractLinedef.hpp"
#include "Doom/Thing/AbstractThing.hpp"

#endif