#pragma once

#include <array>
#include <cstdint>

#include <SFML/Graphics/Image.hpp>

#include "Doom/Doom.hpp"
#include "Math/Box.hpp"

namespace DOOM
{
  class Statusbar
  {
  public:
    enum FaceSprite
    {
      SpriteLookForward,    // Idle, looking forward
      SpriteLookLeft,       // Idle, looking left
      SpriteLookRight,      // Idle, looking right
      SpriteDamageForward,  // Taking damage from front/bottom
      SpriteDamageLeft,     // Taking damage from left
      SpriteDamageRight,    // Taking damage from right
      SpriteOuch,           // Taking massive damage
      SpriteEvil,           // Pick-up a new weapon
      SpriteRampage,        // Continuous shooting
      SpriteGod,            // Invincible
      SpriteDead,           // Dead player

      SpriteCount
    };

    struct Face
    {
      unsigned int                priority; // Priority level of state
      unsigned int                duration; // State duration (in tics, 0 when infinite)
      DOOM::Statusbar::FaceSprite sprite;    // State to be displayed
    };

  private:
    enum FacePain
    {
      Pain80 = 80,  // Completely healthy
      Pain60 = 60,  // Bloody nose, hair slightly mussed
      Pain40 = 40,  // Face swollen, grimacing
      Pain20 = 20,  // Eyes bloodshot, face dirty and bleeding
      Pain0 = 0,    // Similar to 20-39%, but even bloodier

      PainCount = 5
    };
    
  static const std::array<std::array<uint64_t, DOOM::Statusbar::FaceSprite::SpriteCount>, DOOM::Statusbar::FacePain::PainCount> _sprites; // Face sprites

  DOOM::Statusbar::Face _face;    // Face of player
  float                 _elapsed; // Time since begining of face state

  public:
    const int                                                             id;               // Player ID in game
    int                                                                   ammo;             // Ammo count
    float                                                                 health;           // Health percentage
    float                                                                 armor;            // Armor percentage
    std::array<bool, DOOM::Enum::WeaponCount>                             weapons;          // Currently owned weapon
    std::array<DOOM::Enum::KeyType, DOOM::Enum::KeyColor::KeyColorCount>  keys;             // Currently owned keys
    std::array<unsigned int, DOOM::Enum::Ammo::AmmoCount>                 ammos;            // Current number of ammos in inventory
    std::array<unsigned int, DOOM::Enum::Ammo::AmmoCount>                 maximum;          // Maximum number of ammos in inventory

  private:
    void  renderBackground(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, std::int16_t palette) const;
    void  renderAmmo(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, std::int16_t palette) const;
    void  renderHealth(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, std::int16_t palette) const;
    void  renderWeapons(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, std::int16_t palette) const;
    void  renderFace(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, std::int16_t palette) const;
    void  renderArmor(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, std::int16_t palette) const;
    void  renderKey(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, std::int16_t palette) const;
    void  renderAmmos(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, std::int16_t palette) const;

    void  renderDecimal(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, const std::string& font, int value, int x, int y, std::int16_t palette) const;             // Render a decimal number at given non-scaled coordinates from right to left
    void  renderTexture(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, const DOOM::Doom::Resources::Texture& texture, int x, int y, std::int16_t palette) const;  // Render texture at given non-scaled coordinates

  public:
    Statusbar() = delete;
    Statusbar(const DOOM::Doom& doom, int id);
    ~Statusbar() = default;

    void  setFace(unsigned int priority, const DOOM::Statusbar::Face& state); // Change current face

    void  update(float elapsed);                                                                         // Update statusbar
    void  render(const DOOM::Doom& doom, sf::Image& target, Math::Box<2, std::int16_t> rect, std::int16_t palette) const; // Render statusbar to target image
  };
}