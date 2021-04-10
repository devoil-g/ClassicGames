#pragma once

#include <SFML/Graphics/Image.hpp>

#include "Doom/Doom.hpp"

namespace DOOM
{
  class Statusbar
  {
  public:
    int                                                         id;       // Player ID in game
    int                                                         ammo;     // Ammo count
    float                                                       health;   // Health percentage
    float                                                       armor;    // Armor percentage
    std::array<bool, DOOM::Enum::WeaponCount>                   weapons;  // Currently owned weapon
    std::array<DOOM::Enum::KeyType, DOOM::Enum::KeyColorCount>  keys;     // Currently owned keys
    std::array<unsigned int, DOOM::Enum::AmmoCount>             ammos;    // Current number of ammos in inventory
    std::array<unsigned int, DOOM::Enum::AmmoCount>             maximum;  // Maximum number of ammos in inventory

  private:
    void  renderBackground(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const;
    void  renderAmmo(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const;
    void  renderHealth(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const;
    void  renderWeapons(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const;
    void  renderFace(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const;
    void  renderArmor(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const;
    void  renderKey(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const;
    void  renderAmmos(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const;

    void  renderDecimal(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, const std::string& font, int value, int x, int y, int16_t palette) const;             // Render a decimal number at given non-scaled coordinates from right to left
    void  renderTexture(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, const DOOM::Doom::Resources::Texture& texture, int x, int y, int16_t palette) const;  // Render texture at given non-scaled coordinates

  public:
    Statusbar(const DOOM::Doom& doom, int id);
    Statusbar() = delete;
    Statusbar(const Statusbar&) = delete;
    ~Statusbar() = default;

    void  update(sf::Time elapsed);                                                                         // Update statusbar
    void  render(const DOOM::Doom& doom, sf::Image& target, sf::Rect<int16_t> rect, int16_t palette) const; // Render statusbat to target image
  };
}