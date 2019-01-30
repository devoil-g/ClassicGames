#ifndef _ABSTRACT_DYNAMIC_PHYSICS_THING_HPP_
#define _ABSTRACT_DYNAMIC_PHYSICS_THING_HPP_

#include "Doom/Doom.hpp"
#include "Math/Vector.hpp"

namespace DOOM
{
  template<unsigned int Mass>
  class AbstractDynamicPhysicsThing : public virtual DOOM::AbstractThing
  {
  private:
    Math::Vector<2>	_thrust;	// Thrust vector (unit/tic)

    std::set<int16_t>	moveLinedefs(DOOM::Doom & doom, const Math::Vector<2> & movement)	// Return set of intersectable linedef indexes
    {
      std::set<int16_t>	blocks;

      // Get blockmap index at current, using the four corners
      blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() - (float)radius, position.y() - (float)radius)));
      blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() - (float)radius, position.y() + (float)radius)));
      blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + (float)radius, position.y() - (float)radius)));
      blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + (float)radius, position.y() + (float)radius)));

      // Get blockmap index at target position, using the four corners
      blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + movement.x() - (float)radius, position.y() + movement.y() - (float)radius)));
      blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + movement.x() - (float)radius, position.y() + movement.y() + (float)radius)));
      blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + movement.x() + (float)radius, position.y() + movement.y() - (float)radius)));
      blocks.insert(doom.level.blockmap.index(Math::Vector<2>(position.x() + movement.x() + (float)radius, position.y() + movement.y() + (float)radius)));

      std::set<int16_t>	linedefs;

      // Get index of linedefs to test againt position
      for (int16_t block_index : blocks)
	if (block_index != -1)
	  for (int16_t linedef_index : doom.level.blockmap.blocks[block_index].linedefs)
	    linedefs.insert(linedef_index);

      // Return set of intersectable linedef
      return linedefs;
    }

    std::pair<float, Math::Vector<2>>	moveVertex(DOOM::Doom & doom, const Math::Vector<2> & movement, int16_t vertex_index, int16_t ignored_index)	// Return intersection of movement with linedef (coef. along movement / normal vector)
    {
      // Check if vertex should be ignored
      if (ignored_index != -1) {
	DOOM::AbstractLinedef &		ignored = *doom.level.linedefs[ignored_index];

	if (ignored.start == vertex_index || ignored.end == vertex_index)
	  return { 1.f, Math::Vector<2>() };
      }

      DOOM::Doom::Level::Vertex	vertex = doom.level.vertexes[vertex_index];
      Math::Vector<2>		normal = position.convert<2>() - vertex;

      // Ignore intersection if moving "outside" of vertex
      if (Math::Vector<2>::cos(movement, normal) > 0.f)
	return { 1.f, Math::Vector<2>() };

      // Check if vertex is already in bounding box
      if ((vertex - position.convert<2>()).length() <= radius)
	return { 0.f, position.convert<2>() - vertex };
      
      // Intersect bounding circle with vertex
      float	a = std::pow(movement.x(), 2) + std::pow(movement.x(), 2);
      float	b = -2.f * ((vertex.x() - position.x()) * movement.x() + (vertex.y() - position.y()) * movement.y());
      float	c = std::pow(vertex.x() - position.x(), 2) + std::pow(vertex.y() - position.y(), 2);
      float	delta = std::pow(b, 2) - 4.f * a * c;

      // No intersection found
      if (delta <= 0.f)
	return { 1.f, Math::Vector<2>() };

      // Compute intersections
      float	s0 = (-b - std::sqrt(delta)) / (2.f * a);
      float	s1 = (-b + std::sqrt(delta)) / (2.f * a);

      // Return smaller solution
      if (s0 >= 0.f && s0 < 1.f)
	return { s0, normal };
      else if (s1 >= 0.f && s1 < 1.f)
	return { s1, normal };
      else
	return { 1.f, Math::Vector<2>() };
    }

    bool	moveSidedefs(DOOM::Doom & doom, int16_t sidedef_front_index, int16_t sidedef_back_index)	// Return true if thing can move through sidedefs
    {
      // Force movement if thing outside of the map
      if (sidedef_front_index == -1)
	return true;

      // Can't move outside of the map
      if (sidedef_back_index == -1)
	return false;

      DOOM::Doom::Level::Sidedef &	sidedef_front = doom.level.sidedefs[sidedef_front_index];
      DOOM::Doom::Level::Sidedef &	sidedef_back = doom.level.sidedefs[sidedef_back_index];
      
      // Can't move if texture in middle section of sidedef
      if (sidedef_front.middle().width != 0)
	return false;

      DOOM::Doom::Level::Sector &	sector_front = doom.level.sectors[sidedef_front.sector];
      DOOM::Doom::Level::Sector &	sector_back = doom.level.sectors[sidedef_back.sector];

      // Can't take step over 24 units high
      if (sector_back.floor_current - position.z() > 24)
	return false;

      // Check if there is enough space between sector to move
      if (sector_back.ceiling_current - std::max(position.z(), sector_back.floor_current) >= height)
	return true;

      return false;
    }

    std::pair<float, Math::Vector<2>>	moveLinedef(DOOM::Doom & doom, const Math::Vector<2> & movement, int16_t linedef_index, int16_t ignored_index)	// Return intersection of movement with linedef (coef. along movement / normal vector)
    {
      // Check if linedef is ignored
      if (linedef_index == ignored_index)
	return { 1.f, Math::Vector<2>() };

      DOOM::AbstractLinedef &		linedef = *doom.level.linedefs[linedef_index];
      DOOM::Doom::Level::Vertex &	linedef_start = doom.level.vertexes[linedef.start];
      DOOM::Doom::Level::Vertex &	linedef_end = doom.level.vertexes[linedef.end];
      
      /*
      // Check if linedef is colinear to ignored linedef
      if (ignored_index != -1) {
	DOOM::AbstractLinedef &		ignored = *doom.level.linedefs[ignored_index];
	DOOM::Doom::Level::Vertex &	ignored_start = doom.level.vertexes[ignored.start];
	DOOM::Doom::Level::Vertex &	ignored_end = doom.level.vertexes[ignored.end];

	// Linedef equation a.X + b.Y + c = 0
	float	linedef_a = linedef_start.y() - linedef_end.y();
	float	linedef_b = linedef_end.x() - linedef_start.x();
	float	linedef_c = linedef_start.x() * linedef_end.y() - linedef_start.y() * linedef_end.x();

	// Check if both vertexes of ignored linedef belong to linedef
	if (linedef_a * ignored_start.x() + linedef_b * ignored_start.y() + linedef_c == 0.f &&
	  linedef_a * ignored_end.x() + linedef_b * ignored_end.y() + linedef_c == 0.f)
	  return { 1.f, Math::Vector<2>() };
      }
      */

      Math::Vector<2>	linedef_direction = linedef_end - linedef_start;
      Math::Vector<2>	linedef_normal(+linedef_direction.y(), -linedef_direction.x());
      int16_t		sidedef_front_index = linedef.front;
      int16_t		sidedef_back_index = linedef.back;

      // Normalize normal
      linedef_normal /= linedef_normal.length();

      // Flip normal if on left side of linedef
      if (Math::Vector<2>::cos(position.convert<2>() - linedef_start, linedef_normal) < 0.f) {
	linedef_normal *= -1.f;
	std::swap(sidedef_front_index, sidedef_back_index);
      }

      // Ignore intersection if moving "outside" of linedef
      if (Math::Vector<2>::cos(movement, linedef_normal) > 0.f)
	return { 1.f, Math::Vector<2>() };

      // TODO: check if intersection should be ignored using front and back sidedef
      if ((linedef.flag & DOOM::AbstractLinedef::Flag::Impassible) == 0 &&
	moveSidedefs(doom, sidedef_front_index, sidedef_back_index) == true)
	return { 1.f, Math::Vector<2>() };

      // Get intersection of thing bounding sphere and linedef line
      std::pair<float, float>		intersection = Math::intersection(
	Math::Vector<2>(position.x(), position.y()) - linedef_normal * (float)radius, movement,
	linedef_start, linedef_direction);

      // Handle parallele movement
      if (std::isnan(intersection.first) == true) {
	intersection = Math::intersection(position.convert<2>(), linedef_normal * -1.f, linedef_start, linedef_direction);
	intersection.first = 1.f;
      }

      // If linedef is already collided in initial collision, recompute intersection without movement
      if (intersection.first < 0.f) {
	intersection = Math::intersection(position.convert<2>(), linedef_normal * -1.f, linedef_start, linedef_direction);
	intersection.first = 0.f;
      }
      
      // Collide with start vertex, end vertex or linedef itself depending on intersection position
      if (intersection.second < 0.f)
	return moveVertex(doom, movement, linedef.start, ignored_index);
      else if (intersection.second > 1.f)
	return moveVertex(doom, movement, linedef.end, ignored_index);
      else
	return { intersection.first, linedef_normal };
    }

    void	move(DOOM::Doom & doom, sf::Time elapsed, int depth = 0, int16_t linedef_ignored = -1)
    {
      // NOTE: glitch might happen if radius > 128
      // TODO: recode this using square bounding box

      // Limit movement to 30 units per tics
      Math::Vector<2>	movement = ((_thrust.length() > 30.f) ? (_thrust * 30.f / _thrust.length()) : _thrust) * elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds();

      // Stop if maximum recursion depth reach
      if (depth > 4 || (movement.x() == 0.f && movement.y() == 0.f)) {
	_thrust = Math::Vector<2>(0.f, 0.f);
	return;
      }

      // Get intersectable linedefs
      std::set<int16_t>	linedefs = moveLinedefs(doom, movement);

      int16_t		closest_index = -1;
      float		closest_distance = 1.f;
      Math::Vector<2>	closest_normal = Math::Vector<2>();

      // Check collision with linedefs
      for (int16_t linedef_index : linedefs) {
	std::pair<float, Math::Vector<2>>	intersection = moveLinedef(doom, movement, linedef_index, linedef_ignored);

	// Get nearest linedef
	if (intersection.first < closest_distance) {
	  closest_index = linedef_index;
	  closest_distance = intersection.first;
	  closest_normal = intersection.second;
	}
      }

      // TODO: Check collision with things

      // Move player to closest obstacle or full movement if none found
      position.x() += movement.x() * closest_distance;
      position.y() += movement.y() * closest_distance;

      // Re-compute movement if obstable found
      if (closest_index != -1) {
	Math::Vector<2>	closest_direction = Math::Vector<2>(+closest_normal.y(), -closest_normal.x());

	// Slide against currently collisioned walls (change movement and thrust)
	_thrust = closest_direction / closest_direction.length() * _thrust.length() * Math::Vector<2>::cos(_thrust, closest_direction);

	// Attempt new move, ignoring collided linedef
	move(doom, elapsed * (1.f - closest_distance), depth + 1, closest_index);
      }
    }

  protected:
    AbstractDynamicPhysicsThing(DOOM::Doom & doom, int16_t height, int16_t radius, int16_t properties) :	// Special constructor for player only
      DOOM::AbstractThing(doom, height, radius, properties)
    {}

  public:
    AbstractDynamicPhysicsThing(DOOM::Doom & doom, const DOOM::Wad::RawLevel::Thing & thing, int16_t height, int16_t radius, int16_t properties) :
      DOOM::AbstractThing(doom, thing, height, radius, properties),
      _thrust()
    {}
     
    virtual ~AbstractDynamicPhysicsThing() = default;

    virtual void	thrust(const Math::Vector<2> & acceleration) override	// Apply acceleration to thing
    {
      // Apply thrust vector to player based on acceleration and elapsed time (1.5625 is a magic number ?)
      _thrust += acceleration / (float)Mass * 1.5625f;
    }

    virtual bool	update(DOOM::Doom & doom, sf::Time elapsed) override	// Update thing physics
    {
      // Clip thing's speed if it is too large
      // TODO: should we implement speed limitation as engine support any speed ?

      // Does nothing if no movements
      if (std::fabsf(_thrust.x()) < 0.001f && std::fabsf(_thrust.y()) < 0.001f)
	return false;

      // Actual checks, calculations, and movements
      move(doom, elapsed);

      // Apply friction slowdown to player for next tic (hard coded drag factor of 0.90625)
      _thrust *= std::powf(0.90625f, elapsed.asSeconds() / DOOM::Doom::Tic.asSeconds());

      return false;
    }
  };
};

#endif