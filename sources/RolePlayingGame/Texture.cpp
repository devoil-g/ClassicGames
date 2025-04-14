#include <iostream>
#include <stdexcept>

#include "RolePlayingGame/Texture.hpp"
#include "System/Config.hpp"

RPG::Texture::Texture(const std::string& name) :
  _name(name),
  _texture(),
  _error(false)
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
  // Reset error flag
  _error = false;

  // Load texture
  if (_texture.loadFromFile((Game::Config::ExecutablePath / "assets" / "rpg" / _name).string()) == true) {
    _error = false;
    _texture.setRepeated(false);
  }

  // Failed to load texture
  else
  {
    // Load error texture
    if (_texture.loadFromFile((Game::Config::ExecutablePath / "assets" / "rpg" / "error.png").string()) == false)
      throw std::runtime_error((std::string(__FILE__) + ": l." + std::to_string(__LINE__)).c_str());

    _error = true;
    _texture.setRepeated(true);
  }

  // Pixel art
  _texture.setSmooth(false);

  // Generate mipmap
  if (_texture.generateMipmap() == false)
    std::cerr << "[RPG::Texture] Warning: failed to generate mipmap for " << _name << "." << std::endl;
}

bool  RPG::Texture::error() const
{
  // Get error flag
  return _error;
}