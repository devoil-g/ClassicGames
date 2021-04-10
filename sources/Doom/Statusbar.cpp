#include <sstream>
#include <iomanip>
#include <cctype>

#include "Doom/Statusbar.hpp"

DOOM::Statusbar::Statusbar(const DOOM::Doom& doom, int id) :
  id(id),
  ammo(0),
  health(0),
  armor(0),
  weapons({ false }),
  keys({ DOOM::Enum::KeyType::KeyTypeNone }),
  ammos({ 0, 0, 0, 0 }),
  maximum({ 200, 50, 50, 300 })
{
  // Base weapons
  weapons[DOOM::Enum::Weapon::WeaponFist] = true;
  weapons[DOOM::Enum::Weapon::WeaponPistol] = true;
}

void  DOOM::Statusbar::update(sf::Time elapsed)
{
  // TODO: update face animation
}

void  DOOM::Statusbar::render(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  renderBackground(doom, target, rect, palette);
  renderAmmo(doom, target, rect, palette);
  renderHealth(doom, target, rect, palette);
  renderWeapons(doom, target, rect, palette);
  renderFace(doom, target, rect, palette);
  renderArmor(doom, target, rect, palette);
  renderKey(doom, target, rect, palette);
  renderAmmos(doom, target, rect, palette);
}

void  DOOM::Statusbar::renderBackground(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  // Draw statusbar background
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key("STBAR")), 0, 0, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key("STARMS")), 104, 0, palette);
  renderTexture(doom, target, rect, doom.level.players.size() == 1 ? DOOM::Doom::Resources::Texture::Null : doom.resources.getMenu(DOOM::str_to_key(std::string("STFB") + (char)('0' + Math::Modulo<4>(id - 1)))), 143, 1, palette);
}

void  DOOM::Statusbar::renderAmmo(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  // Draw number of ammo left in big red digits
  renderDecimal(doom, target, rect, "STTNUM", ammo, 44, 3, palette);
}

void  DOOM::Statusbar::renderHealth(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  // Draw health in big red digits
  renderDecimal(doom, target, rect, "STTNUM", (int)std::ceil(health), 90, 3, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key("STTPRCNT")), 90, 3, palette);
}

void  DOOM::Statusbar::renderWeapons(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  // Draw arms numbers if small grey/yellow digits
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(1) == true ? "STYSNUM2" : "STGNUM2")), 111, 4, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(2) == true ? "STYSNUM3" : "STGNUM3")), 123, 4, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(3) == true ? "STYSNUM4" : "STGNUM4")), 135, 4, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(4) == true ? "STYSNUM5" : "STGNUM5")), 111, 14, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(5) == true ? "STYSNUM6" : "STGNUM6")), 123, 14, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(weapons.at(6) == true ? "STYSNUM7" : "STGNUM7")), 135, 14, palette);
}

void  DOOM::Statusbar::renderFace(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  // TODO: animate face
}

void  DOOM::Statusbar::renderArmor(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  // Draw armor in big red digits
  renderDecimal(doom, target, rect, "STTNUM", (int)std::ceil(armor), 221, 3, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key("STTPRCNT")), 221, 3, palette);
}

void  DOOM::Statusbar::renderKey(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  for (int index = 0; index < keys.size(); index++)
    if (keys.at(index) != DOOM::Enum::KeyType::KeyTypeNone)
      renderTexture(doom, target, rect, doom.resources.getMenu(DOOM::str_to_key(std::string("STKEYS") + (char)('0' + index + (keys.at(index) - 1) * 3))), 239, 4 + index * 10 - (keys.at(index) - 1), palette);
}

void  DOOM::Statusbar::renderAmmos(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  // Draw current and maximum number of ammo of each type of bullet
  for (int index = 0; index < std::min(ammos.size(), maximum.size()); index++) {
    renderDecimal(doom, target, rect, "STYSNUM", ammos.at(index), 288, 5 + index * 6, palette);
    renderDecimal(doom, target, rect, "STYSNUM", maximum.at(index), 314, 5 + index * 6, palette);
  }
}

void  DOOM::Statusbar::renderDecimal(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, const std::string& font, int value, int x, int y, int16_t palette) const
{
  std::stringstream text;

  // Clamp value to be displayed
  text << std::setfill(' ') << std::setw(3) << std::clamp(value, 0, 999);

  // Get text to be displayed
  std::string string = text.str();

  // Render digits of decimal from right to left
  for (int index = (int)text.str().length() - 1; index >= 0; index--) {
    const auto& texture = (bool)std::isdigit(string.at(index)) == true ? doom.resources.getMenu(DOOM::str_to_key(font + string.at(index))) : DOOM::Doom::Resources::Texture::Null;

    x -= texture.width;
    renderTexture(doom, target, rect, texture, x, y, palette);
  }
}

void  DOOM::Statusbar::renderTexture(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, const DOOM::Doom::Resources::Texture& texture, int x, int y, int16_t palette) const
{
  sf::Vector2i  scale((int)rect.width / (int)DOOM::Doom::RenderWidth, (int)rect.height / 32);

  // Draw texture at given position/scale, ignore texture offset
  texture.draw(doom, target, sf::Vector2i(rect.left + (x + texture.left) * scale.x, rect.top + (y + texture.top) * scale.y), scale, palette);
}