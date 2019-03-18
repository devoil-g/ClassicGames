#include <unordered_map>

#include "Doom/Camera.hpp"

DOOM::Camera::Camera() :
  position(0.f, 0.f, 0.f),
  angle(Math::DegToRad(0.f)),
  orientation(Math::DegToRad(0.f)),
  fov(Math::DegToRad(90.f))
{}

void	DOOM::Camera::render(DOOM::Doom const & doom, sf::Image & target, sf::Rect<int16_t> rect)
{
  // NOTE: target is expected to be clear (sf::Color(0, 0, 0, 0))

  // Cancel if nothing to render
  if (doom.level.nodes.size() == 0)
    return;

  // Reset optimization structure
  _sbuffer.assign(rect.width * rect.height, { -1, std::numeric_limits<float>::quiet_NaN() });
  _vertical.assign(rect.width, { 0, rect.height });
  _horizontal = { 0, rect.width };
  
  // Pre-compute values
  _fov2_tan = std::tan(fov / 2.f);
  _horizon = (float)rect.height / 2.f + std::tan(orientation) * (float)rect.width / (2.f * _fov2_tan);
  _factor = (float)rect.height / (2.f * _fov2_tan * ((float)rect.height / (float)rect.width));

  // Pre-compute screen coordinates
  _screen_start = Math::Vector<2>(std::cos(angle) - _fov2_tan * std::sin(angle), std::sin(angle) + _fov2_tan * std::cos(angle)) + position.convert<2>();
  _screen_end = Math::Vector<2>(std::cos(angle) + _fov2_tan * std::sin(angle), std::sin(angle) - _fov2_tan * std::cos(angle)) + position.convert<2>();
  _screen = _screen_end - _screen_start;

  // Draw level walls and flats from BSP root node
  renderNode(doom, target, rect, (int16_t)(doom.level.nodes.size() - 1));

  // Simplify column of pixel segment index
  for (int col = 0; col < (int)rect.width; col++)
  {
    // Simplify ceiling
    for (int row = std::min((int)_horizon - 1, (int)rect.height - 1); row >= 0; row--)
      if (_sbuffer[col * rect.height + row].second == _sbuffer[col * rect.height + row + 1].second)
	_sbuffer[col * rect.height + row].first = _sbuffer[col * rect.height + row + 1].first;

    // Simplify floor
    for (int row = std::max((int)_horizon + 1, 1); row < (int)rect.height; row++)
      if (_sbuffer[col * rect.height + row].second == _sbuffer[col * rect.height + row - 1].second)
	_sbuffer[col * rect.height + row].first = _sbuffer[col * rect.height + row - 1].first;
  }

  // Draw things
  renderThings(doom, target, rect);
}

bool	DOOM::Camera::renderNode(DOOM::Doom const & doom, sf::Image & target, sf::Rect<int16_t> rect, int16_t index)
{
  // Draw subsector if node ID has subsector mask
  if (index & 0b1000000000000000)
    return renderSubsector(doom, target, rect, index & 0b0111111111111111);

  DOOM::Doom::Level::Node const &	node(doom.level.nodes[index]);

  // Use derterminant to find which side should be rendered first
  if (Math::Vector<2>::determinant(node.direction, position.convert<2>() - node.origin) > 0.f)
    return renderNode(doom, target, rect, node.leftchild) == true ||
    ((Math::Vector<2>::determinant(node.origin - position.convert<2>(), node.direction) / Math::Vector<2>::determinant(_screen_start - position.convert<2>(), node.direction) >= 0.f || Math::Vector<2>::determinant(node.origin - position.convert<2>(), node.direction) / Math::Vector<2>::determinant(_screen_end - position.convert<2>(), node.direction) >= 0.f) && renderNode(doom, target, rect, node.rightchild) == true);
  else
    return renderNode(doom, target, rect, node.rightchild) == true ||
    ((Math::Vector<2>::determinant(node.origin - position.convert<2>(), node.direction) / Math::Vector<2>::determinant(_screen_start - position.convert<2>(), node.direction) >= 0.f || Math::Vector<2>::determinant(node.origin - position.convert<2>(), node.direction) / Math::Vector<2>::determinant(_screen_end - position.convert<2>(), node.direction) >= 0.f) && renderNode(doom, target, rect, node.leftchild) == true);
}

bool	DOOM::Camera::renderSubsector(DOOM::Doom const & doom, sf::Image & target, sf::Rect<int16_t> rect, int16_t index)
{
  DOOM::Doom::Level::Subsector const &	subsector(doom.level.subsectors[index]);

  // Render subsector segs
  for (int16_t i = 0; i < subsector.count; i++)
    if (renderSeg(doom, target, rect, subsector.index + i) == true)
      return true;

  return false;
}

bool	DOOM::Camera::renderSeg(DOOM::Doom const & doom, sf::Image & target, sf::Rect<int16_t> rect, int16_t index)
{
  // Get segment from level data
  const DOOM::Doom::Level::Segment &	seg(doom.level.segments[index]);
  const DOOM::Doom::Level::Vertex &	seg_start(doom.level.vertexes[seg.start]);
  const DOOM::Doom::Level::Vertex &	seg_end(doom.level.vertexes[seg.end]);

  // Compute intersection of vertexes with screen
  std::pair<float, float>	start(Math::intersection(_screen_start, _screen, position.convert<2>(), seg_start - position.convert<2>()));
  std::pair<float, float>	end(Math::intersection(_screen_start, _screen, position.convert<2>(), seg_end - position.convert<2>()));
  std::pair<float, float>	intermediate(Math::intersection(_screen_start, _screen, seg_start, seg_end - seg_start));

  // Find a valid pair of vertexes
  struct Projection { float screen, seg, distance; };
  Projection	left = (std::isnan(start.first) == true || start.second < 0.f) ?
    Projection({ intermediate.first, intermediate.second, 1.f }) :
    Projection({ start.first, 0.f, (seg_start - position.convert<2>()).length() / (_screen_start + _screen * start.first - position.convert<2>()).length() });
  Projection	right = (std::isnan(end.first) == true || end.second < 0.f) ?
    Projection({ intermediate.first, intermediate.second, 1.f }) :
    Projection({ end.first, 1.f, (seg_end - position.convert<2>()).length() / (_screen_start + _screen * end.first - position.convert<2>()).length() });
  
  // Check if valid vertexes are out of screen
  if (std::isnan(left.screen) == true || std::isnan(right.screen) == true || (left.screen < 0.f && right.screen < 0.f) || (left.screen >= 1.f && right.screen >= 1.f) || (left.screen == right.screen))
    return false;

  DOOM::AbstractLinedef const &	linedef(*doom.level.linedefs[seg.linedef]);
  bool				side(seg.direction == 0 ? left.screen > right.screen : left.screen < right.screen);

  // Cancel if no sidedef on left side
  if (side == true && linedef.back == -1)
    return false;

  // Draw from left to right
  if (left.screen > right.screen)
    std::swap(left, right);
  
  // Projection of vertexes on screen's pixels
  int	column_start = std::max((int)std::lroundf(left.screen * rect.width), _horizontal.first);
  int	column_end = std::min((int)std::lroundf(right.screen * rect.width + 0), _horizontal.second);

  // Stop if nothing to draw
  if (column_start >= column_end)
    return false;

  const DOOM::Doom::Level::Sidedef &	sidedef_front(doom.level.sidedefs[side == true ? (linedef.back != -1 ? linedef.back : linedef.front) : linedef.front]);
  const DOOM::Doom::Level::Sidedef &	sidedef_back(doom.level.sidedefs[side == true ? linedef.front : (linedef.back != -1 ? linedef.back : linedef.front)]);
  const DOOM::Doom::Level::Sector &	sector_front(doom.level.sectors[sidedef_front.sector]);
  const DOOM::Doom::Level::Sector &	sector_back(doom.level.sectors[sidedef_back.sector]);
  
  const DOOM::Doom::Resources::Texture &	texture_upper(sidedef_front.upper());
  const DOOM::Doom::Resources::Texture &	texture_lower(sidedef_front.lower());
  const DOOM::Doom::Resources::Texture &	texture_middle(sidedef_front.middle());

  // Y projection of first and second vertexes
  float	left_c = _factor / left.distance;
  float	right_c = _factor / right.distance;
  float	left_lower_front = _horizon - (sector_front.floor_current - position.z()) * left_c;
  float	left_lower_back = _horizon - (sector_back.floor_current - position.z()) * left_c;
  float	left_upper_front = _horizon - (sector_front.ceiling_current - position.z()) * left_c;
  float	left_upper_back = _horizon - (sector_back.ceiling_current - position.z()) * left_c;
  float	right_lower_front = _horizon - (sector_front.floor_current - position.z()) * right_c;
  float	right_lower_back = _horizon - (sector_back.floor_current - position.z()) * right_c;
  float	right_upper_front = _horizon - (sector_front.ceiling_current - position.z()) * right_c;
  float	right_upper_back = _horizon - (sector_back.ceiling_current - position.z()) * right_c;
  
  // Texture Y offsets
  float	upper_offset_y = (linedef.flag & DOOM::AbstractLinedef::Flag::UpperUnpegged) ?
    (sidedef_front.y - (sector_front.ceiling_current - std::max(sector_front.ceiling_current, sector_back.ceiling_current))) :
    (sidedef_front.y - (sector_front.ceiling_current - std::min(sector_front.ceiling_current, sector_back.ceiling_current)) + texture_upper.height);
  float	middle_offset_y = (linedef.flag & DOOM::AbstractLinedef::Flag::LowerUnpegged) ?
    (sidedef_front.y - (sector_front.ceiling_current - std::max(sector_front.floor_current, sector_back.floor_current)) + texture_middle.height) :
    (sidedef_front.y - (sector_front.ceiling_current - std::min(sector_front.ceiling_current, sector_back.ceiling_current)));
  float	lower_offset_y = (linedef.flag & DOOM::AbstractLinedef::Flag::LowerUnpegged) ?
    (sidedef_front.y - (sector_back.floor_current - sector_front.ceiling_current)) :
    (sidedef_front.y - (sector_back.floor_current - std::max(sector_front.floor_current, sector_back.floor_current)));

  // Compute sector light
  int16_t	light = sector_front.light_current;

  // Iterate through pixels of projection
  for (int column = column_start; column < column_end; column++)
  {
    // Skip column if already completed
    if (_vertical[column].first == _vertical[column].second)
      continue;

    // Projection screen offset
    float	screen_offset = (std::max((float)column / (float)rect.width, left.screen) - left.screen) / (right.screen - left.screen);
    
    // Walls Y projection
    float	upper_front = left_upper_front + (right_upper_front - left_upper_front) * screen_offset;
    float	upper_back = left_upper_back + (right_upper_back - left_upper_back) * screen_offset;
    float	lower_front = left_lower_front + (right_lower_front - left_lower_front) * screen_offset;
    float	lower_back = left_lower_back + (right_lower_back - left_lower_back) * screen_offset;

    // TODO: solve nan values
    // Projection segment offset
    float	seg_offset = std::clamp(left.seg + (right.seg - left.seg) * ((std::abs(right.distance - left.distance) < 0.1f) ? screen_offset : ((((sector_front.ceiling_current - position.z()) * _factor / (_horizon - upper_front)) - left.distance) / (right.distance - left.distance))), 0.f, 1.f);

    // Compute light level according to distance TODO: improve this process
    float	distance = (position.convert<2>() - (seg_start + (seg_end - seg_start) * seg_offset)).length() / (_screen_start + _screen * ((float)column / (float)rect.width) - position.convert<2>()).length();
    int16_t	shaded = renderLight(doom, target, rect, light, distance);

    // Texture X offset
    int		texture_offset_x = seg.offset + (int)(sidedef_front.x + ((seg_end - seg_start).length() * (((bool)seg.direction == side) ? seg_offset : (1.f - seg_offset))));

    // Draw ceiling/sky
    if (sector_front.ceiling_name == DOOM::str_to_key("F_SKY1")) {
      if (linedef.back == -1 || sector_back.ceiling_name != DOOM::str_to_key("F_SKY1")) {
	renderSky(doom, target, rect, column, 0, (int)std::lroundf(std::min(upper_front, upper_back)), sector_front.ceiling_current, index);
	_vertical[column].first = std::max(_vertical[column].first, (int)std::lroundf(std::min(upper_front, upper_back)));
      }
    }
    else if (position.z() < sector_front.ceiling_current) {
      renderFlat(doom, target, rect, sector_front.ceiling_flat, column, 0, (int)std::lroundf(upper_front), sector_front.ceiling_current, light, index);
      _vertical[column].first = std::max(_vertical[column].first, (int)std::lroundf(upper_front));
    }

    // Draw floor/sky
    if (sector_front.floor_name == DOOM::str_to_key("F_SKY1")) {
      if (linedef.back == -1 || sector_back.floor_name != DOOM::str_to_key("F_SKY1")) {
	renderSky(doom, target, rect, column, (int)std::lroundf(std::max(lower_front, lower_back)), rect.height, sector_front.floor_current, index);
	_vertical[column].second = std::min(_vertical[column].second, (int)std::lroundf(std::max(lower_front, lower_back)));
      }
    }
    else if (position.z() > sector_front.floor_current) {
      renderFlat(doom, target, rect, sector_front.floor_flat, column, (int)std::lroundf(lower_front), rect.height, sector_front.floor_current, light, index);
      _vertical[column].second = std::min(_vertical[column].second, (int)std::lroundf(lower_front));
    }

    // Draw upper/lower/middle walls
    if (upper_front < upper_back && texture_upper.height != 0 && (sector_front.ceiling_name != DOOM::str_to_key("F_SKY1") || sector_back.ceiling_name != DOOM::str_to_key("F_SKY1")))
      renderTexture(doom, target, rect, texture_upper, column, upper_front, upper_back, std::abs(sector_front.ceiling_current - sector_back.ceiling_current), texture_offset_x, upper_offset_y, shaded, index);
    if (lower_front > lower_back && texture_lower.height != 0 && (sector_front.floor_name != DOOM::str_to_key("F_SKY1") || sector_back.floor_name != DOOM::str_to_key("F_SKY1")))
      renderTexture(doom, target, rect, texture_lower, column, lower_back, lower_front, std::abs(sector_front.floor_current - sector_back.floor_current), texture_offset_x, lower_offset_y, shaded, index);
    if (upper_front < lower_front && texture_middle.height != 0)
      renderTexture(doom, target, rect, texture_middle, column, upper_front, lower_front, std::abs(sector_front.ceiling_current - sector_front.floor_current), texture_offset_x, middle_offset_y, shaded, index);

    // Complete column if final wall
    if (linedef.back == -1)
      _vertical[column] = { 0, 0 };
  }
  
  // Mark column as completed
  if (linedef.back == -1)
  {
    if (_horizontal.first == column_start)
      _horizontal.first = column_end;
    if (_horizontal.second == column_end)
      _horizontal.second = column_end;
  }

  // Check for horizontal completion from sides
  while (_horizontal.first < _horizontal.second && _vertical[_horizontal.first].first == _vertical[_horizontal.first].second)
    _horizontal.first++;
  while (_horizontal.second > _horizontal.first && _vertical[_horizontal.second - 1].first == _vertical[_horizontal.second - 1].second)
    _horizontal.second--;

  // Return true if image completed
  return _horizontal.first == _horizontal.second;
}

int16_t	DOOM::Camera::renderLight(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect, int16_t light, float distance)
{
  // Magic formula found in a creepy forum on the dark side of the internet
  return (int16_t)std::clamp((int)((int)light * 2 - 255 + 255 * DOOM::Camera::LightFade / distance), (int)0, (int)255);
}

void	DOOM::Camera::renderTexture(DOOM::Doom const & doom, sf::Image & target, sf::Rect<int16_t> rect, DOOM::Doom::Resources::Texture const & texture, int column, float top, float bottom, float height, int offset_x, float offset_y, int16_t light, int16_t seg)
{
  int									pixel_x(Math::Modulo(offset_x, texture.width));
  std::vector<DOOM::Doom::Resources::Texture::Column::Span> const &	spans(texture.columns[pixel_x].spans);

  // Draw column of pixels
  for (int row = std::max(_vertical[column].first, (int)std::lroundf(top)); row < std::min((int)std::lroundf(bottom) + 0, _vertical[column].second); row++)
    if (target.getPixel(rect.left + column, rect.top + row).a == 0)
    {
      int	pixel_y(Math::Modulo<128>((int)(offset_y + std::max((height * (row - top) / (bottom - top)), 0.f))));

      // Find pixel in column span
      for (DOOM::Doom::Resources::Texture::Column::Span const & span : spans)
      {
	// TODO: optimize this (remember last span ?)

	// Skip spans if before targeted pixel
	if (span.offset + span.pixels.size() <= pixel_y)
	  continue;

	// Pixel already passed
	if (span.offset > pixel_y)
	  break;

	// Get color in column span, draw it and register segment index in seg-buffer
	target.setPixel(rect.left + column, rect.top + row, doom.resources.palettes[0][doom.resources.colormaps[31 - light / 8][span.pixels[pixel_y - span.offset]]]);
	_sbuffer[column * rect.height + row] = { seg, std::numeric_limits<float>::quiet_NaN() };

	break;
      }
    }
}

void	DOOM::Camera::renderFlat(DOOM::Doom const & doom, sf::Image & target, sf::Rect<int16_t> rect, const DOOM::AbstractFlat & flat, int column, int start, int end, float altitude, int16_t light, int16_t seg)
{
  Math::Vector<2>			direction(_screen_start + _screen * ((float)column / (float)rect.width) - position.convert<2>());
  const std::array<uint8_t, 4096> &	texture = flat.flat();
  float					distance_distortion = direction.length();

  for (int row = std::max(start, _vertical[column].first); row < std::min(end, _vertical[column].second); row++)
    if (target.getPixel(rect.left + column, rect.top + row).a == 0)
    {
      float	k = std::abs((altitude - position.z()) / (2.f * _fov2_tan * (0.5f - (row - _horizon + rect.height / 2.f) / (float)rect.height) * (rect.height / (float)rect.width)));

      // Get grid coordinates
      Math::Vector<2>	coord(position.convert<2>() + direction * k);

      // Compute light level according to distance TODO: improve this process
      int16_t	shaded = renderLight(doom, target, rect, light, (position.convert<2>() - coord).length() / distance_distortion);

      // Get color in flat from coordinates and register segment index in seg-buffer
      target.setPixel(rect.left + column, rect.top + row, doom.resources.palettes[0][doom.resources.colormaps[31 - shaded / 8][texture[(Math::Modulo<64>(64 - (int)coord.y())) * 64 + Math::Modulo<64>((int)coord.x() - 1)]]]);
      _sbuffer[column * rect.height + row] = { seg, altitude };
    }
}

void	DOOM::Camera::renderSky(DOOM::Doom const & doom, sf::Image & target, sf::Rect<int16_t> rect, int column, int start, int end, float altitude, int16_t seg)
{
  DOOM::Doom::Resources::Texture const &				sky(doom.level.sky);
  Math::Vector<2>							direction(_screen_start + _screen * ((float)column / (float)rect.width) - position.convert<2>());
  int									pixel_x(Math::Modulo((int)(Math::Vector<2>::angle(Math::Vector<2>(1.f, 0.f), direction) * (Math::Vector<2>::determinant(Math::Vector<2>(1.f, 0.f), direction) > 0 ? +1.f : -1.f) * 4.f / (2.f * Math::Pi) * sky.width), sky.width));
  std::vector<DOOM::Doom::Resources::Texture::Column::Span> const &	spans(sky.columns[pixel_x].spans);
  float									sky_factor(_screen.length() * 2.f * sky.width / (rect.width * direction.length() * Math::Pi));

  for (int row = std::max(_vertical[column].first, start); row < std::min(end, _vertical[column].second); row++)
    if (target.getPixel(rect.left + column, rect.top + row).a == 0)
    {
      int	pixel_y = (int)std::lroundf((row - _horizon) * sky_factor * 0.72f) + sky.height / 2;

      // Skip pixel if outside of sky texture
      if (pixel_y < 0 || pixel_y >= sky.height)
	continue;
      
      // Find pixel in column span
      for (DOOM::Doom::Resources::Texture::Column::Span const & span : spans)
      {
	// Skip spans if before targeted pixel
	if (span.offset + span.pixels.size() <= pixel_y)
	  continue;

	// Pixel already passed
	if (span.offset > pixel_y)
	  break;

	// Get color in column span, draw it and register segment index in seg-buffer
	target.setPixel(rect.left + column, rect.top + row, doom.resources.palettes[0][doom.resources.colormaps[31 - 192 / 8][span.pixels[pixel_y - span.offset]]]);
	_sbuffer[column * rect.height + row] = { seg, altitude };

	break;
      }
    }
}

void	DOOM::Camera::renderThings(DOOM::Doom const & doom, sf::Image & target, sf::Rect<int16_t> rect)
{
  std::list<std::pair<const DOOM::AbstractThing &, float>>	things;		// List of things to render and their distance to camera
  std::unordered_map<int16_t, float>				vertexes;	// Map of vertexes and their distance to camera

  // Pre-calculated projection factors
  Math::Vector<2>	eye_0(std::cos(angle), std::sin(angle));
  Math::Vector<2>	eye_90(-eye_0.y(), eye_0.x());
  float			eye_r(Math::Vector<2>::determinant(eye_0, eye_90));

  // Render every things of current level
  for (const std::unique_ptr<DOOM::AbstractThing> & thing : doom.level.things)
    if (thing->sprite(0).first.get().width > 0)
    {
      float	distance = Math::Vector<2>::determinant(thing->position.convert<2>() - position.convert<2>(), eye_90) / eye_r;

      // Push thing to list to render only if front of camera
      if (distance > 1.f)
	things.push_back({ *thing.get(), distance });
    }

  // Order list of things of render
  things.sort([](const std::pair<const DOOM::AbstractThing &, float> & a, const std::pair<const DOOM::AbstractThing &, float> & b) { return a.second > b.second; });

  // Render things in reverse order
  for (std::list<std::pair<const DOOM::AbstractThing &, float>>::const_iterator iterator = things.begin(); iterator != things.end(); iterator++)
  {
    const DOOM::AbstractThing &									thing(iterator->first);
    const std::pair<std::reference_wrapper<const DOOM::Doom::Resources::Texture>, bool> &	texture(thing.sprite(Math::Vector<2>::angle(position.convert<2>() - thing.position.convert<2>()) - thing.angle));
    
    // Compute thing position on screen
    std::pair<float, float>	thing_projection(Math::intersection(_screen_start, _screen, position.convert<2>(), thing.position.convert<2>() - position.convert<2>()));
    std::pair<int16_t, int16_t>	thing_sector(doom.level.getSector(thing.position.convert<2>()));

    float	thing_factor = _factor / ((thing.position.convert<2>() - position.convert<2>()).length() / (_screen_start + _screen * thing_projection.first - position.convert<2>()).length());
    float	first_x = thing_projection.first * rect.width + ((texture.second == false ? -texture.first.get().left : -texture.first.get().width + texture.first.get().left)) * thing_factor;
    float	first_y = _horizon - (((thing.properties & DOOM::AbstractThing::Properties::Hanging) ? thing.position.z() + texture.first.get().height - texture.first.get().top : thing.position.z() + texture.first.get().top) - position.z()) * thing_factor;
    float	second_x = thing_projection.first * rect.width + ((texture.second == false ? texture.first.get().width - texture.first.get().left : +texture.first.get().left)) * thing_factor;
    float	second_y = _horizon - (((thing.properties & DOOM::AbstractThing::Properties::Hanging) ? thing.position.z() - texture.first.get().top : thing.position.z() - texture.first.get().height + texture.first.get().top) - position.z()) * thing_factor;

    // Map of thing visibility against segments
    std::unordered_map<int16_t, bool>	visible;

    // Compute light level of thing
    int16_t	shaded = renderLight(doom, target, rect, doom.level.sectors[thing_sector.first].light_current, iterator->second);

    // Render pixels of the sprite
    for (int column = std::max((int)std::lroundf(first_x), 0); column < std::min((int)std::lroundf(second_x), (int)rect.width); column++)
      for (int row = std::max((int)std::lroundf(first_y), 0); row < std::min((int)std::lroundf(second_y), (int)rect.height); row++)
      {
	int16_t					segment_index = _sbuffer[column * rect.height + row].first;

	// Process segment if valid index
	if (segment_index != -1)
	{
	  const DOOM::Doom::Level::Segment &	segment = doom.level.segments[segment_index];

	  // Compute vertexes distances if not already registered
	  if (vertexes.find(segment.start) == vertexes.end())
	    vertexes[segment.start] = Math::Vector<2>::determinant(doom.level.vertexes[segment.start] - position.convert<2>(), eye_90) / eye_r;
	  if (vertexes.find(segment.end) == vertexes.end())
	    vertexes[segment.end] = Math::Vector<2>::determinant(doom.level.vertexes[segment.end] - position.convert<2>(), eye_90) / eye_r;

	  // Test segment if not already in visibility map
	  if (visible.find(segment_index) == visible.end())
	  {
	    // Visible if segment is behind thing from camera point of view
	    if (vertexes[segment.start] > iterator->second && vertexes[segment.end] > iterator->second)
	      visible[segment_index] = true;
	    else if (vertexes[segment.start] < iterator->second && vertexes[segment.end] < iterator->second)
	      visible[segment_index] = false;
	    else
	    {
	      // Compute intersection of segment with eye-thing vector
	      std::pair<float, float>		intersection(Math::intersection(position.convert<2>(), thing.position.convert<2>() - position.convert<2>(), doom.level.vertexes[segment.start], doom.level.vertexes[segment.end] - doom.level.vertexes[segment.start]));

	      visible[segment_index] = (std::isnan(intersection.first) == true || intersection.first < 0.f || intersection.first > 1.f);
	    }
	  }
	}

	// Skip pixel if not visible
	if (visible[segment_index] == false)
	  continue;

	int	pixel_x = (int)(std::clamp(((texture.second == false) ? (column - first_x) : (second_x - column)) / (second_x - first_x), 0.f, 0.9999999f) * texture.first.get().width);
	int	pixel_y = (int)(std::clamp((row - first_y) / (second_y - first_y), 0.f, 0.9999999f) * texture.first.get().height);

	for (DOOM::Doom::Resources::Texture::Column::Span const & span : texture.first.get().columns[pixel_x].spans)
	{
	  // Interrupt if pixel missed
	  if (span.offset > pixel_y)
	    break;

	  if (span.offset + span.pixels.size() > pixel_y)
	  {
	    target.setPixel(rect.left + column, rect.top + row, doom.resources.palettes[0][doom.resources.colormaps[31 - shaded / 8][span.pixels[pixel_y - span.offset]]]);
	    break;
	  }
	}
      }
  }
}
