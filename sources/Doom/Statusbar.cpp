#include <sstream>
#include <iomanip>
#include <cctype>

#include "Doom/Statusbar.hpp"

DOOM::Statusbar::Statusbar(const DOOM::Doom & doom, int id) :
  id(id),
  ammo(0),
  health(0),
  armor(0),
  weapons({ false }),
  keys({ DOOM::Enum::KeyType::KeyTypeNone }),
  ammos({ 0, 0, 0, 0 }),
  maximum({ 200, 50, 50, 300 })
{}

void	DOOM::Statusbar::update(sf::Time elapsed)
{
  // TODO: update face animation
}

void	DOOM::Statusbar::render(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const
{
  renderBackground(doom, target, rect);
  renderAmmo(doom, target, rect);
  renderHealth(doom, target, rect);
  renderWeapons(doom, target, rect);
  renderFace(doom, target, rect);
  renderArmor(doom, target, rect);
  renderKey(doom, target, rect);
  renderAmmos(doom, target, rect);
}

void	DOOM::Statusbar::renderBackground(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const
{
  // Draw statusbar background
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key("STBAR")), 0, 0);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key("STARMS")), 104, 0);
  renderTexture(doom, target, rect, doom.level.players.size() == 1 ? DOOM::Doom::Resources::Texture::Null : doom.resources.getMenu(DOOM::str_to_key(std::string("STFB") + (char)('0' + Math::Modulo<4>(id - 1)))), 143, 1);
}

void	DOOM::Statusbar::renderAmmo(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const
{
  // Draw number of ammo left in big red digits
  renderDecimal(doom, target, rect, "STTNUM", ammo, 44, 3);
}

void	DOOM::Statusbar::renderHealth(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const
{
  // Draw health in big red digits
  renderDecimal(doom, target, rect, "STTNUM", (int)std::ceil(health), 90, 3);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key("STTPRCNT")), 90, 3);
}

void	DOOM::Statusbar::renderWeapons(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const
{
  // Draw arms numbers if small grey/yellow digits
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(0) == true ? "STYSNUM2" : "STGNUM2")), 111, 4);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(1) == true ? "STYSNUM3" : "STGNUM3")), 123, 4);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(2) == true ? "STYSNUM4" : "STGNUM4")), 135, 4);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(3) == true ? "STYSNUM5" : "STGNUM5")), 111, 14);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(4) == true ? "STYSNUM6" : "STGNUM6")), 123, 14);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(5) == true ? "STYSNUM7" : "STGNUM7")), 135, 14);
}

void	DOOM::Statusbar::renderFace(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const
{
  // TODO: animate face
}

void	DOOM::Statusbar::renderArmor(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const
{
  // Draw armor in big red digits
  renderDecimal(doom, target, rect, "STTNUM", (int)std::ceil(armor), 221, 3);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key("STTPRCNT")), 221, 3);
}

void	DOOM::Statusbar::renderKey(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const
{
  for (int index = 0; index < keys.size(); index++) {
    if (keys.at(index) != DOOM::Enum::KeyType::KeyTypeNone)
      renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(std::string("STKEYS") + (char)('0' + index + (keys.at(index) - 1) * 3))), 239, 4 + index * 10 - (keys.at(index) - 1));
  }

}

void	DOOM::Statusbar::renderAmmos(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const
{
  // Draw current and maximum number of ammo of each type of bullet
  for (int index = 0; index < std::min(ammos.size(), maximum.size()); index++) {
    renderDecimal(doom, target, rect, "STYSNUM", ammos.at(index), 288, 5 + index * 6);
    renderDecimal(doom, target, rect, "STYSNUM", maximum.at(index), 314, 5 + index * 6);
  }
}

void	DOOM::Statusbar::renderDecimal(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect, const std::string & font, int value, int x, int y) const
{
  std::stringstream	text;

  // Clamp value to be displayed
  text << std::setfill(' ') << std::setw(3) << std::clamp(value, 0, 999);

  // Get text to be displayed
  std::string	string = text.str();

  // Render digits of decimal from right to left
  for (int index = (int)text.str().length() - 1; index >= 0; index--) {
    const DOOM::Doom::Resources::Texture &	texture = (bool)std::isdigit(string.at(index)) == true ? doom.resources.getMenu(DOOM::str_to_key(font + string.at(index))) : DOOM::Doom::Resources::Texture::Null;

    x -= texture.width;
    renderTexture(doom, target, rect, texture, x, y);
  }
}

void	DOOM::Statusbar::renderTexture(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect, const DOOM::Doom::Resources::Texture & texture, int x, int y) const
{
  sf::Vector2i	scale((int)rect.width / (int)DOOM::Doom::RenderWidth, (int)rect.height / 32);

  // Draw texture at given position/scale, ignore texture offset
  texture.draw(doom, target, sf::Vector2i(rect.left + (x + texture.left) * scale.x, rect.top + (y + texture.top) * scale.y), scale);
}