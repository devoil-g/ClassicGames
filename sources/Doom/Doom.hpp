#ifndef _DOOM_HPP_
#define _DOOM_HPP_

#include <list>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Doom/Wad.hpp"
#include "Math/Vector.hpp"

namespace Game
{
  class AbstractFlat;
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

      static const Game::Doom::Resources::Texture	NullTexture;	// Empty texture

      std::vector<Game::Doom::Resources::Palette>			palettes;	// Color palettes
      std::vector<Game::Doom::Resources::Colormap>			colormaps;	// Color brightness maps
      std::unordered_map<uint64_t, Game::Doom::Resources::Texture>	textures;	// Map of wall textures
      std::unordered_map<uint64_t, Game::Doom::Resources::Texture>	sprites;	// Map of sprites
      std::unordered_map<uint64_t, Game::Doom::Resources::Texture>	menus;		// Map of menu patches
      std::unordered_map<uint64_t, Game::AbstractFlat *>		flats;		// Map of flat (ground/ceiling texture)
      std::unordered_map<uint64_t, Game::Doom::Resources::Sound>	sounds;		// Map of sounds

      void	update(sf::Time);	// Update resources components
    };

    struct Level
    {
      typedef Math::Vector<2>		Vertex;

      struct Linedef
      {
	enum Flag
	{
	  Impassible = 0x0001,		// Players and monsters cannot cross this line
	  BlockMonsters = 0x0002,	// Monsters cannot cross this line
	  TwoSided = 0x0004,		// See documentation
	  UpperUnpegged = 0x0008,	// The upper texture is pasted onto the wall from the top down instead of from the bottom up like usual
	  LowerUnpegged = 0x0010,	// Lower and middle textures are drawn from the bottom up, instead of from the top down like usual
	  Secret = 0x0020,		// See documentation
	  BlockSound = 0x0040,		// For purposes of monsters hearing sounds and thus becoming alerted
	  NotOnMap = 0x0080,		// The line is not shown on the automap
	  AlreadyOnMap = 0x0100		// When the level is begun, this line is already on the automap
	};

	enum Type
	{
	  // TODO
	};

	int16_t	start, end;	// Start and end vertexes
	int16_t	flag;		// Linedef flag (see enum)
	int16_t	type;		// Linedef type (see enum)
	int16_t	tag;		// Linedef sector tag
	int16_t	front, back;	// Front and back sidedefs indexes (-1 if no sidedef)
      };

      class Sidedef
      {
	static const int	FrameDuration = 8;	// Tics between two frames of animation

      public:
	int16_t					x, y;	// Texture offset
	int16_t					sector;	// Index of the sector it references

      private:
	sf::Time						_elapsed;	// Total elapsed time
	std::vector<const Game::Doom::Resources::Texture *>	_upper;		// Pointer to upper/lower/middle textures
	std::vector<const Game::Doom::Resources::Texture *>	_lower;		// Pointer to lower textures
	std::vector<const Game::Doom::Resources::Texture *>	_middle;	// Pointer to middle textures

	std::vector<const Game::Doom::Resources::Texture *>	animation(const Game::Doom &, uint64_t) const;	// Return frames of animation

      public:
	Sidedef(const Game::Doom &, const Game::Wad::RawLevel::Sidedef &);
	~Sidedef();

	void					update(sf::Time);	// Update sidedef
	const Game::Doom::Resources::Texture &	upper() const;		// Return upper texture to be displayed
	const Game::Doom::Resources::Texture &	lower() const;		// Return lower texture to be displayed
	const Game::Doom::Resources::Texture &	middle() const;		// Return middle texture to be displayed
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

      struct Sector
      {
	enum Special
	{
	  Normal = 0x0000,		// Normal
	  LightBlinkRandom = 0x0001,	// Blink random
	  LightBlink05 = 0x0002,	// Blink 0.5 second
	  LightBlink10 = 0x0003,	// Blink 1.0 second
	  Damage20Blink05 = 0x0004,	// 20 % damage per second plus light blink 0.5 second
	  Damage10 = 0x0005,		// 10 % damage per second
	  Damage5 = 0x0007,		// 5 % damage per second
	  LightOscillates = 0x0008,	// Oscillates
	  Secret = 0x0009,		// Player entering this sector gets credit for finding a secret
	  DoorClose = 0x000A,		// 30 seconds after level start, ceiling closes like a door
	  End = 0x000B,			// Cancel God mode if active, 20 % damage per second, when player dies, level ends
	  LightBlink05Sync = 0x000C,	// Blink 0.5 second, synchronized
	  LightBlink10Sync = 0x000D,	// Blink 1.0 second, synchronized
	  DoorOpen = 0x000E,		// 300 seconds after level start, ceiling opens like a door
	  Damage20 = 0x0010,		// 20 % damage per second
	  LightFlickers = 0x0011	// Flickers randomly
	};

	float				floor_height, ceiling_height;	// Floor and ceiling height
	uint64_t			floor, ceiling;			// Texture name
	const Game::AbstractFlat *	floor_flat;			// Floor texture
	const Game::AbstractFlat *	ceiling_flat;			// Ceiling texture
	int16_t				light;				// Light level
	int16_t				special;			// Sector special type
	int16_t				tag;				// Sector/linedef tag
      };
      
      std::pair<uint8_t, uint8_t>		episode;	// Level episode and episode's mission number
      Game::Doom::Resources::Texture		sky;		// Sky texture of the level
      std::vector<Game::AbstractThing *>	things;		// List of things
      std::vector<Game::Doom::Level::Linedef>	linedefs;	// List of linedefs
      std::vector<Game::Doom::Level::Sidedef>	sidedefs;	// List of sidedefs
      std::vector<Game::Doom::Level::Vertex>	vertexes;	// List of vertexes
      std::vector<Game::Doom::Level::Segment>	segments;	// List of segs
      std::vector<Game::Doom::Level::Subsector>	subsectors;	// List of subsectors
      std::vector<Game::Doom::Level::Node>	nodes;		// List of nodes
      std::vector<Game::Doom::Level::Sector>	sectors;	// List of sectors

      std::pair<int16_t, int16_t>	sector(const Math::Vector<2> &, int16_t = -1) const;	// Return sector/subsector at position

      void	update(sf::Time);	// Update level components
    };

  private:
    Game::Wad	_wad;	// File holding WAD datas
    
    void	clear();		// Clear previously loaded resources
    void	clearResources();	// Clear resources
    void	clearLevel();		// Clear current level

    bool	buildResources();		// Build resources from WAD
    bool	buildResourcesPalettes();	// Build color palettes from WAD
    bool	buildResourcesColormaps();	// Build color maps from WAD
    bool	buildResourcesTextures();	// Build textures from WAD
    bool	buildResourcesSprites();	// Build sprites textures from WAD
    bool	buildResourcesMenus();		// Build menus textures from WAD
    bool	buildResourcesFlats();		// Build flats from WAD
    bool	buildResourcesSounds();		// Build sounds from WAD
    
    bool	buildLevel(const std::pair<uint8_t, uint8_t> &);		// Build level from WAD file
    bool	buildLevelVertexes(const std::pair<uint8_t, uint8_t> &);	// Build level's vertexes from WAD file
    bool	buildLevelSectors(const std::pair<uint8_t, uint8_t> &);		// Build level's sectors from WAD file
    bool	buildLevelLinedefs(const std::pair<uint8_t, uint8_t> &);	// Build level's linedefs from WAD file
    bool	buildLevelSidedefs(const std::pair<uint8_t, uint8_t> &);	// Build level's sidedefs from WAD file
    bool	buildLevelSubsectors(const std::pair<uint8_t, uint8_t> &);	// Build level's subsectors from WAD file
    bool	buildLevelThings(const std::pair<uint8_t, uint8_t> &);		// Build level's things from WAD file
    bool	buildLevelSegments(const std::pair<uint8_t, uint8_t> &);	// Build level's segments from WAD file
    bool	buildLevelNodes(const std::pair<uint8_t, uint8_t> &);		// Build level's nodes from WAD file
    
  public:
    Game::Doom::Resources	resources;	// Resources built from WAD
    Game::Doom::Level		level;		// Current level datas build from WAD

    Doom();
    ~Doom();

    bool	load(const std::string &);	// Load WAD file and build resources
    void	update(sf::Time);		// Update current level and resources

    std::list<std::pair<uint8_t, uint8_t>>	getLevel() const;				// Return list of available level in WAD
    bool					setLevel(const std::pair<uint8_t, uint8_t> &);	// Build specified level from WAD, return true if successful
  };
};

#endif