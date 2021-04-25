#include "Doom/Automap.hpp"
#include "Doom/Thing/PlayerThing.hpp"

DOOM::Automap::Automap() :
  position(),
  zoom(0.125f),
  mode(DOOM::Automap::Mode::ModeFollow)
{}

void  DOOM::Automap::render(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, unsigned int scale, int16_t palette) const
{
  // Draw linedefs of level
  for (const auto& linedef : doom.level.linedefs) {
    DOOM::Automap::Color  color;

    // Find linedef color
    if (linedef->flag & DOOM::AbstractLinedef::Flag::NotOnMap)
      continue;
    else if (linedef->flag & DOOM::AbstractLinedef::Flag::OnMap) {
      if (linedef->back == -1 || linedef->flag & DOOM::AbstractLinedef::Flag::Secret)
        color = DOOM::Automap::Color::ColorSolid;
      else if (doom.level.sectors[doom.level.sidedefs[linedef->front].sector].floor_current != doom.level.sectors[doom.level.sidedefs[linedef->back].sector].floor_current)
        color = DOOM::Automap::Color::ColorFloor;
      else if (doom.level.sectors[doom.level.sidedefs[linedef->front].sector].ceiling_current != doom.level.sectors[doom.level.sidedefs[linedef->back].sector].ceiling_current)
        color = DOOM::Automap::Color::ColorCeiling;
      else
        continue;
    }
    else if (linedef->flag & DOOM::AbstractLinedef::Flag::AlreadyOnMap)
      color = DOOM::Automap::Color::ColorMap;
    else
      continue;

    // Render linedef
    renderLine(
      doom,
      target,
      rect,
      renderTransform(rect, scale, doom.level.vertexes[linedef->start].convert<2>()),
      renderTransform(rect, scale, doom.level.vertexes[linedef->end].convert<2>()),
      color,
      palette);
  }

  // Render players
  for (const auto& player : doom.level.players) {
    const std::array<std::pair<Math::Vector<2>, Math::Vector<2>>, 7>  arrow = {
      std::pair<Math::Vector<2>, Math::Vector<2>>({ -0.875f, 0.f }, { 1.f, 0.f }),
      std::pair<Math::Vector<2>, Math::Vector<2>>({ 1.f, 0.f }, { 0.5f, 0.25f }),
      std::pair<Math::Vector<2>, Math::Vector<2>>({ 1.f, 0.f }, { 0.5f, -0.25f }),
      std::pair<Math::Vector<2>, Math::Vector<2>>({ -0.875f, 0.f }, { -1.125f, -0.25f }),
      std::pair<Math::Vector<2>, Math::Vector<2>>({ -0.875f, 0.f }, { -1.125f, 0.25f }),
      std::pair<Math::Vector<2>, Math::Vector<2>>({ -0.625f, 0.f }, { -0.875f, -0.25f }),
      std::pair<Math::Vector<2>, Math::Vector<2>>({ -0.625f, 0.f }, { -0.875f, 0.25f })
    };

    if (player.get().health <= 0.f)
      continue;

    // Draw arrow lines
    for (const auto& line : arrow) {
      renderLine(
        doom,
        target,
        rect,
        renderTransform(rect, scale, Math::Vector<2>(line.first.x() * std::cos(player.get().angle) + line.first.y() * std::sin(player.get().angle), line.first.x() * std::sin(player.get().angle) - line.first.y() * std::cos(player.get().angle)) * (float)player.get().attributs.radius + player.get().position.convert<2>()),
        renderTransform(rect, scale, Math::Vector<2>(line.second.x() * std::cos(player.get().angle) + line.second.y() * std::sin(player.get().angle), line.second.x() * std::sin(player.get().angle) - line.second.y() * std::cos(player.get().angle)) * (float)player.get().attributs.radius + player.get().position.convert<2>()),
        DOOM::Automap::Color::ColorPlayer,
        palette);
    }
  }
}

Math::Vector<2> DOOM::Automap::renderTransform(sf::Rect<int16_t> rect, unsigned int scale, const Math::Vector<2>& point) const
{
  Math::Vector<2> pos(point - position);
  Math::Vector<2> rot(pos.x() * std::cos(-angle + Math::Pi / 2.f) - pos.y() * std::sin(-angle + Math::Pi / 2.f), pos.x() * std::sin(-angle + Math::Pi / 2.f) + pos.y() * std::cos(-angle + Math::Pi / 2.f));
  Math::Vector<2> sca(rot * zoom * (float)scale);
  Math::Vector<2> ratio(sca.x(), sca.y() / DOOM::Doom::RenderStretching);
  Math::Vector<2> screen(ratio.x() + (float)rect.width / 2.f, ratio.y() + (float)rect.height / 2.f);

  return screen;
}

void  DOOM::Automap::renderLine(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, Math::Vector<2> point_a, Math::Vector<2> point_b, DOOM::Automap::Color color, int16_t palette) const
{
  // Skip line outside of render area
  if ((point_a.x() < 0 && point_b.x() < 0) ||
    (point_a.x() >= rect.width && point_b.x() >= rect.width) ||
    (point_a.y() < 0 && point_b.y() < 0) ||
    (point_a.y() >= rect.height && point_b.y() >= rect.height))
    return;

  // Render horizontal
  if (std::abs(point_a.x() - point_b.x()) > std::abs(point_a.y() - point_b.y()))
  {
    // Render from left to right
    if (point_a.x() > point_b.x())
      std::swap(point_a, point_b);

    for (int x = std::max(0, (int)point_a.x()); x <= std::min((int)rect.width - 1, (int)point_b.x()); x++) {
      int y = (int)(point_a.y() + ((point_a.x() != point_b.x()) ? ((point_b.y() - point_a.y()) * std::max((float)x - point_a.x(), 0.f) / (point_b.x() - point_a.x())) : 0.f));

      if (y >= 0 && y < rect.height)
        target.setPixel(x + rect.left, rect.height - y - 1 + rect.top, doom.resources.palettes[palette][doom.resources.colormaps[0][color]]);
    }
  }

  // Render vertical
  else
  {
    // Render from left to right
    if (point_a.y() > point_b.y())
      std::swap(point_a, point_b);

    for (int y = std::max(0, (int)point_a.y()); y <= std::min((int)rect.height - 1, (int)point_b.y()); y++) {
      int x = (int)(point_a.x() + ((point_a.y() != point_b.y()) ? ((point_b.x() - point_a.x()) * std::max((float)y - point_a.y(), 0.f) / (point_b.y() - point_a.y())) : 0.f));

      if (x >= 0 && x < rect.width)
        target.setPixel(x + rect.left, rect.height - y - 1 + rect.top, doom.resources.palettes[0][doom.resources.colormaps[0][color]]);
    }
  }
}