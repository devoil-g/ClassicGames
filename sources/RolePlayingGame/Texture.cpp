#include <stdexcept>

#include "RolePlayingGame/Texture.hpp"
#include "System/Config.hpp"

RPG::Texture::Texture(const std::string& name) :
  _name(name),
  _texture()
{
  // First load of texture
  reload();
}

sf::Texture&  RPG::Texture::get()
{
  // Get texture primitive
  return _texture;
}

const sf::Texture&  RPG::Texture::get() const
{
  // Get texture primitive
  return _texture;
}

void  RPG::Texture::reload()
{
  // Load texture
  if (_texture.loadFromFile((Game::Config::ExecutablePath / "assets" / "rpg" / _name).string()) == false) {
    sf::Image image;

    // Create white/red checkboard
    image.create(2, 2, sf::Color::White);
    image.setPixel(0, 0, sf::Color::Red);
    image.setPixel(1, 1, sf::Color::Red);

    // Load error texture
    if (_texture.loadFromImage(image) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    // Repeat error texture
    _texture.setRepeated(true);
  }
}