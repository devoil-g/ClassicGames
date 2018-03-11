#ifndef _WAD_HPP_
#define _WAD_HPP_

#include <fstream>
#include <list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <SFML/Audio.hpp>
#include <SFML/Graphics/Color.hpp>

#include "Math/Vector.hpp"

namespace Game
{
  inline uint64_t	str_to_key(const std::string & str)
  {
    uint64_t	key = 0;

    // Copy string in key (ignore warning of std::strncpy)
#pragma warning(suppress:4996)
    std::strncpy((char *)&key, str.c_str(), sizeof(uint64_t));

    return key;
  }

  inline std::string	key_to_str(const uint64_t key)
  {
    char	str[9] = { 0 };

    // Copy key in string (ignore warning of std::strncpy)
#pragma warning(suppress:4996)
    std::strncpy(str, (const char *)&key, sizeof(uint64_t));

    return std::string(str);
  }

  class AbstractThing;

  class Wad
  {
  private:
    struct Lump
    {
      uint32_t	position;	// Lump position in file
      uint32_t	size;		// Lump size in bytes
      uint64_t	name;		// Lump name

      Lump() : position(0), size(0), name(0) {};
    };

  public:
    struct RawResources
    {
#pragma pack(push, 1)
      struct Palette
      {
	struct Color
	{
	  uint8_t	r, g, b;	// Color components
	};

	Game::Wad::RawResources::Palette::Color	colors[256];	// Color palette
      };

      struct Colormap
      {
	uint8_t	index[256];	// Index of color in palette
      };

      struct Endoom
      {
	uint16_t	message[80 * 25];	// Text displayed when the game exit, characters in 16-bit color format
      };

      struct Texture
      {
	struct Patch
	{
	  int16_t	x, y;		// Offset of patch in texture
	  int16_t	pname;		// Index of patch's name in pnames table
	  int16_t	stepdir;	// Always 1, is for something called "stepdir"
	  int16_t	colormap;	// Always 0, is for "colormap"
	};

	int16_t			width;		// Total width of texture
	int16_t			height;		// Total height of texture
	std::vector<Patch>	patches;	// Patches that compose texture
      };

      struct Patch
      {
	struct Column
	{
	  struct Span
	  {
	    uint8_t			offset;	// Offset of the span of pixels in the column
	    std::vector<uint8_t>	pixels;	// Pixels colors indexes
	  };

	  std::vector<Span>	spans;	// Vector of span of pixels in the column
	};

	int16_t			width, height;	// Sprite size
	int16_t			left, top;	// Sprite offset
	std::vector<Column>	columns;	// Sprite texture map
      };

      struct Flat
      {
	uint8_t	texture[64 * 64];	// Flat texture map of index in palette (64x64)
      };

      struct Genmidi
      {
	struct Voice
	{
	  int8_t	modulator0;	// Modulator Tremolo / vibrato / sustain / KSR / multi
	  int8_t	modulator1;	// Modulator Attack rate / decay rate
	  int8_t	modulator2;	// Modulator Sustain level / release rate
	  int8_t	modulator3;	// Modulator Waveform select
	  int8_t	modulator4;	// Modulator Key scale level
	  int8_t	modulator5;	// Modulator Output level
	  int8_t	feedback;	// Feedback
	  int8_t	carrier0;	// Carrier Tremolo / vibrato / sustain / KSR / multi
	  int8_t	carrier1;	// Carrier Attack rate / decay rate
	  int8_t	carrier2;	// Carrier Sustain level / release rate
	  int8_t	carrier3;	// Carrier Waveform select
	  int8_t	carrier4;	// Carrier Key scale level
	  int8_t	carrier5;	// Carrier Output level
	  int8_t	unused;		// Unused
	  int16_t	offset;		// Base note offset. This is used to offset the MIDI note values. Several of the GENMIDI instruments have a base note offset of - 12, causing all notes to be offset down by one octave.
	};

	enum Flag
	{
	  FixedPitch = 0x01,
	  Unknown = 0x02,
	  DoubleVoice = 0x04
	};

	uint16_t	flag;		// Record flags
	uint8_t		tuning;		// Fine tuning
	uint8_t		note;		// Note number used for fixed pitch instruments
	Voice		voice0;		// First OPL voice data
	Voice		voice1;		// Second OPL voice data
	int8_t		name[32];	// Instrument name
      };

      struct Music
      {
	int16_t			primary;	// Number of primary channels
	int16_t			secondary;	// Number of secondary channels
	int16_t			instrument;	// Number of instrument patches
	int16_t			patch;		// Instrument patch number
	std::vector<uint8_t>	data;		// Music data
      };

      struct Sound
      {
	uint16_t		rate;		// Sound rate
	uint16_t		samples;	// Number of samples in buffer
	std::vector<uint8_t>	buffer;		// Sound buffer (unsigned 8bit format)
      };

      struct Demo
      {
	enum Mode
	{
	  Single = 0,		// Single player mode
	  Cooperative = 0,	// Cooperative mode
	  Deathmatch = 1,	// Deathmatch mode
	  Altdeathmatch = 2	// Alternative deathmatch mode
	};

	enum ViewPoint
	{
	  Player1 = 0,	// View from player 1
	  Player2 = 1,	// View from player 2
	  Player3 = 2,	// View from player 3
	  Player4 = 3	// View from player 4
	};

	struct Record
	{
	  enum Other
	  {
	    Fire = 0b00000001,			// Fire current weapon
	    Use = 0b00000010,			// Use a switch, open a door, etc...
	    WeaponChange = 0b00000100,		// Change weapon
	    WeaponMask = 0b00111000,		// Weapon selection mask
	    WeaponFist = 0b00000000,		// Fist of chainsaw
	    WeaponPistol = 0b00001000,		// Pistol
	    WeaponShotgun = 0b00010000,		// Shotgun
	    WeaponChaingun = 0b00011000,	// Chaingun
	    WeaponRocketlauncher = 0b00100000,	// Rocket Launcher
	    WeaponPlasmarifle = 0b00101000,	// Plasma Rifle
	    WeaponBfg9000 = 0b00110000,		// BFG 9000
	    WeaponSupershotgun = 0b00111000,	// Super Shotgun (DOOM 2 only)
	    Special = 0b10000000		// Special actions (see manual)
	  };

	  int8_t	front;	// Forward/Backward movement
	  int8_t	strafe;	// Strafe right/left movement
	  int8_t	turn;	// Turn left/right
	  int8_t	other;	// Other actions
	};

	int8_t			skill;		// Skill level
	int8_t			episode;	// Episode to be played
	int8_t			mission;	// Mission to be played
	int8_t			mode;		// Game mode
	int8_t			respawn;	// Respawn mode (0 = no respawn)
	int8_t			fast;		// Fast mode (0 = not fast)
	int8_t			nomonster;	// No monster moode (0 = monsters exist)
	int8_t			viewpoint;	// Point of view
	int8_t			player1;	// Player 1 is present if this is 1.
	int8_t			player2;	// Player 2 is present if this is 1.
	int8_t			player3;	// Player 3 is present if this is 1.
	int8_t			player4;	// Player 4 is present if this is 1.
	std::vector<Record>	records;	// Players movements records
      };
#pragma pack(pop)

      std::vector<Game::Wad::RawResources::Palette>			palettes;	// Color palettes
      std::vector<Game::Wad::RawResources::Colormap>			colormaps;	// Color brightness maps
      std::vector<Game::Wad::RawResources::Genmidi>			genmidis;	// Vector of General MIDI instruments records
      std::vector<Game::Wad::RawResources::Demo>			demos;		// Vector of demos to be played
      std::vector<uint64_t>						pnames;		// Vector of patch name
      std::unordered_map<uint64_t, Game::Wad::RawResources::Texture>	textures;	// Map of wall textures
      std::unordered_map<uint64_t, Game::Wad::RawResources::Patch>	patches;	// Map of texture patches
      std::unordered_map<uint64_t, Game::Wad::RawResources::Patch>	sprites;	// Map of sprites
      std::unordered_map<uint64_t, Game::Wad::RawResources::Patch>	menus;		// Map of menu patches
      std::unordered_map<uint64_t, Game::Wad::RawResources::Flat>	flats;		// Map of flat (ground/ceiling texture)
      std::unordered_map<uint64_t, Game::Wad::RawResources::Music>	musics;		// Map of musics (MUS format)
      std::unordered_map<uint64_t, Game::Wad::RawResources::Sound>	sounds;		// Map of sounds
      Game::Wad::RawResources::Endoom					endoom;		// End message displayed in console
    };

    struct RawLevel
    {
#pragma pack(push, 1)
      struct Thing
      {
	int16_t	x, y;	// Thing position
	int16_t	angle;	// Angle facing [-32768 (-Pi/2): +32767 (+Pi/2)]
	int16_t	type;	// Thing type ID
	int16_t	flag;	// Thing options
      };

      struct Linedef
      {
	int16_t	start, end;	// Start and end vertexes indexes
	int16_t	flag;		// Linedef flag (see enum)
	int16_t	type;		// Linedef type (see enum)
	int16_t	tag;		// Linedef/sector tag
	int16_t	front, back;	// Front and back sidedefs indexes (-1 if no sidedef)
      };

      struct Sidedef
      {
	int16_t		x, y;			// Texture offest
	uint64_t	upper, lower, middle;	// Name of upper/lower/middle textures
	int16_t		sector;			// Index of the sector it references
      };

      struct Vertex
      {
	int16_t	x, y;			// Vertex coordinates
      };

      struct Segment
      {
	int16_t	start, end;	// Start/end segment vertexes indexes
	int16_t	angle;		// Segment angle [-32768 (-Pi/2): +32767 (+Pi/2)]
	int16_t	linedef;	// Segment linedef index
	int16_t	direction;	// 0 (same as linedef) or 1 (opposite of linedef)
	int16_t	offset;		// Distance along linedef to start of seg
      };

      struct Subsector
      {
	int16_t	count;	// Segment count
	int16_t	index;	// First segment index
      };

      struct Node
      {
	struct WadNodeBB
	{
	  int16_t	top, bottom;	// Top and bottom limits
	  int16_t	left, right;	// Left and right limits
	};

	int16_t		origin_x, origin_y;		// Partition line origin
	int16_t		direction_x, direction_y;	// Partition line direction
	WadNodeBB	right_bb, left_bb;		// Children bounding boxes
	int16_t		right_ss, left_ss;		// Children node index, or subsector index if bit 15 is set
      };

      struct Sector
      {
	int16_t		floor_height, ceiling_height;	// Floor and ceiling height
	uint64_t	floor_texture, ceiling_texture;	// Name of floor and ceiling texture
	int16_t		light;				// Light level
	int16_t		special;			// Sector special type
	int16_t		tag;				// Sector/linedef tag
      };

      struct Blockmap
      {
	int16_t			x, y;		// Coordinates of block-grid origin
	int16_t			column, row;	// Number of columns and rows
	std::vector<int16_t>	offset;
	std::vector<int16_t>	blocklist;
      };

      struct Reject
      {
	std::vector<uint8_t>	rejects;	// Map of visibility for monsters
      };
#pragma pack(pop)

      std::vector<Thing>	things;
      std::vector<Vertex>	vertexes;
      std::vector<Sector>	sectors;
      std::vector<Sidedef>	sidedefs;
      std::vector<Linedef>	linedefs;
      std::vector<Segment>	segments;
      std::vector<Subsector>	subsectors;
      std::vector<Node>		nodes;
      Blockmap			blockmap;
      Reject			reject;
    };

  private:
    bool	loadLumps(std::ifstream &, const int32_t, const int32_t);	// Load lumps from file

    bool	loadResourceFlats(std::ifstream &, const int32_t, const int32_t, int32_t &);	// Load flats from F_START to F_END scope
    bool	loadResourceSprites(std::ifstream &, const int32_t, const int32_t, int32_t &);	// Load sprites from S_START to S_END scope
    bool	loadResourcePatches(std::ifstream &, const int32_t, const int32_t, int32_t &);	// Load patches from P_START to P_END scope

    bool	loadResourceFlat(std::ifstream &, const Game::Wad::Lump &);	// Load lumps in F_START & F_END scopes
    bool	loadResourceSprite(std::ifstream &, const Game::Wad::Lump &);	// Load lumps in S_START & S_END scopes
    bool	loadResourcePatch(std::ifstream &, const Game::Wad::Lump &);	// Load lumps in P_START & P_END scopes
    bool	loadResourceTexture(std::ifstream &, const Game::Wad::Lump &);	// Load TEXTURE1 and TEXTURE2 lump
    bool	loadResourceMenu(std::ifstream &, const Game::Wad::Lump &);	// Load menu textures and sprites
    bool	loadResourceGenmidi(std::ifstream &, const Game::Wad::Lump &);	// Load GENMIDI lump
    bool	loadResourceMusic(std::ifstream &, const Game::Wad::Lump &);	// Load a D_* (music) lump
    bool	loadResourceSound(std::ifstream &, const Game::Wad::Lump &);	// Load a DS* (sound) lump
    bool	loadResourceDemox(std::ifstream &, const Game::Wad::Lump &);	// Load DEMOx lump
    bool	loadResourcePlaypal(std::ifstream &, const Game::Wad::Lump &);	// Load PLAYPAL lump
    bool	loadResourceColormap(std::ifstream &, const Game::Wad::Lump &);	// Load COLORMAP lump
    bool	loadResourcePnames(std::ifstream &, const Game::Wad::Lump &);	// Load PNAMES lump
    bool	loadResourceEndoom(std::ifstream &, const Game::Wad::Lump &);	// Load ENDOOM lump

    bool	loadResourcePatch(std::ifstream &, const Game::Wad::Lump &, std::unordered_map<uint64_t, Game::Wad::RawResources::Patch> &);	// Load image from file

    bool	loadLevelExmy(const Game::Wad::Lump &, std::pair<uint8_t, uint8_t> &);				// Change current loaded level
    bool	loadLevelMapxy(const Game::Wad::Lump &, std::pair<uint8_t, uint8_t> &);				// Change current loaded level
    bool	loadLevelThings(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);		// Load THINGS lump
    bool	loadLevelLinedefs(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);	// Load LINEDEFS lump
    bool	loadLevelSidedefs(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);	// Load SIDEDEFS lump
    bool	loadLevelVertexes(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);	// Load VERTEXES lump
    bool	loadLevelSegs(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);		// Load SEGS lump
    bool	loadLevelSsectors(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);	// Load SSECTORS lump
    bool	loadLevelNodes(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);		// Load NODES lump
    bool	loadLevelSectors(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);	// Load SECTORS lump
    bool	loadLevelReject(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);		// Load REJECT lump
    bool	loadLevelBlockmap(std::ifstream &, const Game::Wad::Lump &, std::pair<uint8_t, uint8_t>);	// Load BLOCKMAP lump

    bool	loadIgnore();	// Ignore lump

    template <typename Data>
    bool	loadLump(std::ifstream & file, const Game::Wad::Lump & lump, std::vector<Data> & datas)
    {
      // Check for invalid lump size
      if (lump.size % sizeof(Data) != 0)
	return false;

      // Reset data container
      datas.clear();

      // Load lump datas
      file.seekg(lump.position, file.beg);
      for (unsigned int i = 0; i < lump.size / sizeof(Data); i++)
      {
	datas.push_back(Data());
	file.read((char *)&datas.back(), sizeof(Data));
      }

      return true;
    };

    static inline uint64_t &	uppercase(uint64_t & key)	// Force uppercase in WAD name
    {
      std::transform((char *)&key, (char *)&key + 8, (char *)&key, ::toupper);

      return key;
    }

  public:
    std::map<std::pair<uint8_t, uint8_t>, Game::Wad::RawLevel>	levels;		// Levels definition (key is [Ex, My])
    Game::Wad::RawResources					resources;	// File resources

    Wad();
    ~Wad();

    bool	load(const std::string &);	// Load levels from file
  };
};

#endif