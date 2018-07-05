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
  class AbstractFlat;
  class AbstractLinedef;
  class AbstractSector;
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
      std::vector<std::unique_ptr<DOOM::AbstractSector>>	sectors;	// List of sectors
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