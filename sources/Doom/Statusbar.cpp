#include <sstream>
#include <iomanip>
#include <cctype>

#include "Doom/Statusbar.hpp"

const std::array<std::array<uint64_t, DOOM::Statusbar::FaceSprite::SpriteCount>, DOOM::Statusbar::FacePain::PainCount>  DOOM::Statusbar::_sprites{
  std::array<uint64_t, DOOM::Statusbar::FaceSprite::SpriteCount>{ // 00-19 % health
    Game::Utilities::str_to_key<uint64_t>("STFST40"),  // SpriteLookForward
    Game::Utilities::str_to_key<uint64_t>("STFST41"),  // SpriteLookLeft
    Game::Utilities::str_to_key<uint64_t>("STFST42"),  // SpriteLookRight
    Game::Utilities::str_to_key<uint64_t>("STFKILL4"), // SpriteDamageForward
    Game::Utilities::str_to_key<uint64_t>("STFTL40"),  // SpriteDamageLeft
    Game::Utilities::str_to_key<uint64_t>("STFTR40"),  // SpriteDamageRight
    Game::Utilities::str_to_key<uint64_t>("STFOUCH4"), // SpriteOuch
    Game::Utilities::str_to_key<uint64_t>("STFEVL4"),  // SpriteEvil
    Game::Utilities::str_to_key<uint64_t>("STFKILL4"), // SpriteRampage
    Game::Utilities::str_to_key<uint64_t>("STFGOD0"),  // SpriteGod
    Game::Utilities::str_to_key<uint64_t>("STFDEAD0")  // SpriteDead
  },
  std::array<uint64_t, DOOM::Statusbar::FaceSprite::SpriteCount>{ // 20-39 % health
    Game::Utilities::str_to_key<uint64_t>("STFST30"),  // SpriteLookForward
    Game::Utilities::str_to_key<uint64_t>("STFST31"),  // SpriteLookLeft
    Game::Utilities::str_to_key<uint64_t>("STFST32"),  // SpriteLookRight
    Game::Utilities::str_to_key<uint64_t>("STFKILL3"), // SpriteDamageForward
    Game::Utilities::str_to_key<uint64_t>("STFTL30"),  // SpriteDamageLeft
    Game::Utilities::str_to_key<uint64_t>("STFTR30"),  // SpriteDamageRight
    Game::Utilities::str_to_key<uint64_t>("STFOUCH3"), // SpriteOuch
    Game::Utilities::str_to_key<uint64_t>("STFEVL3"),  // SpriteEvil
    Game::Utilities::str_to_key<uint64_t>("STFKILL3"), // SpriteRampage
    Game::Utilities::str_to_key<uint64_t>("STFGOD0"),  // SpriteGod
    Game::Utilities::str_to_key<uint64_t>("STFDEAD0")  // SpriteDead
  },
  std::array<uint64_t, DOOM::Statusbar::FaceSprite::SpriteCount>{ // 40-59 % health
    Game::Utilities::str_to_key<uint64_t>("STFST20"),  // SpriteLookForward
    Game::Utilities::str_to_key<uint64_t>("STFST21"),  // SpriteLookLeft
    Game::Utilities::str_to_key<uint64_t>("STFST22"),  // SpriteLookRight
    Game::Utilities::str_to_key<uint64_t>("STFKILL2"), // SpriteDamageForward
    Game::Utilities::str_to_key<uint64_t>("STFTL20"),  // SpriteDamageLeft
    Game::Utilities::str_to_key<uint64_t>("STFTR20"),  // SpriteDamageRight
    Game::Utilities::str_to_key<uint64_t>("STFOUCH2"), // SpriteOuch
    Game::Utilities::str_to_key<uint64_t>("STFEVL2"),  // SpriteEvil
    Game::Utilities::str_to_key<uint64_t>("STFKILL2"), // SpriteRampage
    Game::Utilities::str_to_key<uint64_t>("STFGOD0"),  // SpriteGod
    Game::Utilities::str_to_key<uint64_t>("STFDEAD0")  // SpriteDead
  },
  std::array<uint64_t, DOOM::Statusbar::FaceSprite::SpriteCount>{ // 60-79 % health
    Game::Utilities::str_to_key<uint64_t>("STFST10"),  // SpriteLookForward
    Game::Utilities::str_to_key<uint64_t>("STFST11"),  // SpriteLookLeft
    Game::Utilities::str_to_key<uint64_t>("STFST12"),  // SpriteLookRight
    Game::Utilities::str_to_key<uint64_t>("STFKILL1"), // SpriteDamageForward
    Game::Utilities::str_to_key<uint64_t>("STFTL10"),  // SpriteDamageLeft
    Game::Utilities::str_to_key<uint64_t>("STFTR10"),  // SpriteDamageRight
    Game::Utilities::str_to_key<uint64_t>("STFOUCH1"), // SpriteOuch
    Game::Utilities::str_to_key<uint64_t>("STFEVL1"),  // SpriteEvil
    Game::Utilities::str_to_key<uint64_t>("STFKILL1"), // SpriteRampage
    Game::Utilities::str_to_key<uint64_t>("STFGOD0"),  // SpriteGod
    Game::Utilities::str_to_key<uint64_t>("STFDEAD0")  // SpriteDead
  },
  std::array<uint64_t, DOOM::Statusbar::FaceSprite::SpriteCount>{ // 80-200 % health
    Game::Utilities::str_to_key<uint64_t>("STFST00"),  // SpriteLookForward
    Game::Utilities::str_to_key<uint64_t>("STFST01"),  // SpriteLookLeft
    Game::Utilities::str_to_key<uint64_t>("STFST02"),  // SpriteLookRight
    Game::Utilities::str_to_key<uint64_t>("STFKILL0"), // SpriteDamageForward
    Game::Utilities::str_to_key<uint64_t>("STFTL00"),  // SpriteDamageLeft
    Game::Utilities::str_to_key<uint64_t>("STFTR00"),  // SpriteDamageRight
    Game::Utilities::str_to_key<uint64_t>("STFOUCH0"), // SpriteOuch
    Game::Utilities::str_to_key<uint64_t>("STFEVL0"),  // SpriteEvil
    Game::Utilities::str_to_key<uint64_t>("STFKILL0"), // SpriteRampage
    Game::Utilities::str_to_key<uint64_t>("STFGOD0"),  // SpriteGod
    Game::Utilities::str_to_key<uint64_t>("STFDEAD0")  // SpriteDead
  }
};

DOOM::Statusbar::Statusbar(const DOOM::Doom& doom, int id) :
  _face{ 0, 1, DOOM::Statusbar::FaceSprite::SpriteLookForward },
  _elapsed(0.f),
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

void  DOOM::Statusbar::update(float elapsed)
{
  // Does nothing if state is infinite
  if (_face.duration == 0) {
    _elapsed = 0.f;
    return;
  }

  // Increment time counter
  _elapsed += elapsed;

  while (_elapsed >= DOOM::Doom::Tic * (float)_face.duration) {
    _elapsed -= DOOM::Doom::Tic * (float)_face.duration;

    int                         r = std::rand() % 3;
    DOOM::Statusbar::FaceSprite face;

    // Choose random looking direction
    if (r == 0)
      face = DOOM::Statusbar::FaceSprite::SpriteLookForward;
    else if (r == 1)
      face = DOOM::Statusbar::FaceSprite::SpriteLookLeft;
    else
      face = DOOM::Statusbar::FaceSprite::SpriteLookRight;

    // Apply new state
    setFace(10, { 0, 35 / 2, face });
  }
}

void  DOOM::Statusbar::setFace(unsigned int priority, const DOOM::Statusbar::Face& state)
{
  // Check priority order
  if (priority > _face.priority) {
    _face = state;
    _elapsed = 0.f;
  }
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
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>("STBAR")), 0, 0, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>("STARMS")), 104, 0, palette);
  renderTexture(doom, target, rect, doom.level.players.size() == 1 ? DOOM::Doom::Resources::Texture::Null : doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(std::string("STFB") + (char)('0' + Math::Modulo<4>(id - 1)))), 143, 1, palette);
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
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>("STTPRCNT")), 90, 3, palette);
}

void  DOOM::Statusbar::renderWeapons(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  // Draw arms numbers if small grey/yellow digits
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(weapons.at(1) == true ? "STYSNUM2" : "STGNUM2")), 111, 4, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(weapons.at(2) == true ? "STYSNUM3" : "STGNUM3")), 123, 4, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(weapons.at(3) == true ? "STYSNUM4" : "STGNUM4")), 135, 4, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(weapons.at(4) == true ? "STYSNUM5" : "STGNUM5")), 111, 14, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(weapons.at(5) == true ? "STYSNUM6" : "STGNUM6")), 123, 14, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(weapons.at(6) == true ? "STYSNUM7" : "STGNUM7")), 135, 14, palette);
}

void  DOOM::Statusbar::renderFace(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  const auto& texture = doom.resources.getMenu(_sprites[std::clamp((int)(health / 20.f), 0, (int)_sprites.size() - 1)][(health > 0.f) ? _face.sprite : DOOM::Statusbar::FaceSprite::SpriteDead]);

  // Draw face
  renderTexture(doom, target, rect, texture, 143 - texture.left, 0 - texture.top, palette);
}

void  DOOM::Statusbar::renderArmor(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  // Draw armor in big red digits
  renderDecimal(doom, target, rect, "STTNUM", (int)std::ceil(armor), 221, 3, palette);
  renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>("STTPRCNT")), 221, 3, palette);
}

void  DOOM::Statusbar::renderKey(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const
{
  for (int index = 0; index < keys.size(); index++)
    if (keys.at(index) != DOOM::Enum::KeyType::KeyTypeNone)
      renderTexture(doom, target, rect, doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(std::string("STKEYS") + (char)('0' + index + (keys.at(index) - 1) * 3))), 239, 4 + index * 10 - (keys.at(index) - 1), palette);
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
    const auto& texture = (bool)std::isdigit(string.at(index)) == true ? doom.resources.getMenu(Game::Utilities::str_to_key<uint64_t>(font + string.at(index))) : DOOM::Doom::Resources::Texture::Null;

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