#pragma once

#include <string>

#include <SFML/Graphics/Texture.hpp>

namespace RPG
{
  class Texture
  {
  private:
    std::string _name;
    sf::Texture _texture;

  public:
    Texture() = delete;
    Texture(const std::string& name);
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;
    ~Texture() = default;

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    sf::Texture&       get();       // Get texture primitive
    const sf::Texture& get() const; // Get texture primitive

    void  reload(); // Reload texture
  };
}