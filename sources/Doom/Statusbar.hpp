#ifndef _STATUSBAR_HPP_
#define _STATUSBAR_HPP_

#include <SFML/Graphics/Image.hpp>

#include "Doom/Doom.hpp"

namespace DOOM
{
  class Statusbar
  {
  public:
    enum Arms
    {
      Pistol = 0,
      Shotgun = 1,
      Chaingun = 2,
      RocketLauncher = 3,
      PlasmaGun = 4,
      BFG9000 = 5,
      ArmsCount

    };

    enum KeyColor
    {
      Blue = 0,
      Yellow = 1,
      Red = 2,
      KeyColorCount
    };

    enum KeyType
    {
      None,
      Keycard,
      Skullkey
    };

    enum Ammo
    {
      Bullets = 0,
      Shells = 1,
      Rockets = 2,
      Cells = 3,
      AmmoCount
    };

    int					id;		// Player ID in game
    int					ammo;		// Ammo count
    int					health;		// Health percentage
    int					armor;		// Amor percentage
    std::array<bool, ArmsCount>		arms;		// Currently owned weapon
    std::array<KeyType, KeyColorCount>	keys;		// Currently owned keys
    std::array<int, AmmoCount>		ammos;		// Current number of ammos in inventory
    std::array<int, AmmoCount>		maximum;	// Maximum number of ammos in inventory

  private:
    void	renderBackground(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const;
    void	renderAmmo(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const;
    void	renderHealth(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const;
    void	renderArms(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const;
    void	renderFace(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const;
    void	renderArmor(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const;
    void	renderKey(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const;
    void	renderAmmos(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect) const;

    void	renderDecimal(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect, const std::string & font, int value, int x, int y) const;		// Render a decimal number at given non-scaled coordinates from right to left
    void	renderTexture(const DOOM::Doom & doom, sf::Image & target, sf::Rect<int16_t> rect, const DOOM::Doom::Resources::Texture & texture, int x, int y) const;	// Render texture at given non-scaled coordinates
    
  public:
    Statusbar(const DOOM::Doom & doom, int id);
    ~Statusbar() = default;

    void	update(sf::Time elapsed);	// Update statusbar
    void	render(DOOM::Doom const & doom, sf::Image & target, sf::Rect<int16_t> rect) const;	// Render statusbat to target image
  };
};

#endif