#pragma once

#include <array>
#include <cstdint>
#include <string>

#include <SFML/Graphics.hpp>

#include "GameBoyColor/GameBoyColor.hpp"
#include "Scenes/AbstractScene.hpp"
#include "System/Window.hpp"

namespace GBC
{
  class MenuScene : public Game::AbstractScene
  {
  private:
    sf::Image           _image;   // Rendering buffer
    sf::Texture         _texture; // Rendering target
    GBC::GameBoyColor&  _gbc;     // Emulated Game Boy

    struct Character
    {
      unsigned int                  width;  // Width of the character
      std::array<std::uint16_t, 10> bitmap; // Bitmap of the character
    };

    static const std::unordered_map<char, Character>                Characters; // Characters bitmap
    static const std::unordered_map<Game::Window::Key, std::string> Keys;       // Bindable keys names

    struct Item
    {
      int         x, y; // Item position
      std::string text; // Item text
    };

    std::array<Item, 11>  _items; // Items of the menu
    unsigned int          _index; // Index of current element selected
    bool                  _press; // True when binding if pending

    void  move(int offset);             // Move select in menu
    void  select();                     // Select current item
    void  bind(Game::Window::Key key);  // Bind the key

    void  renderMenu();   // Render menu
    void  renderPress();  // Render press key screen

    void  render(const Item& item); // Draw a menu item
    void  render(int x, int y, const std::string& string, sf::Color color); // Draw string at position
    void  render(int x, int y, char c, sf::Color color);                    // Draw character at position
    void  render(int x, int y, Character character, sf::Color color);       // Draw character bitmap at position

    unsigned int   width(const std::string& string) const;                         // Width of a string
    unsigned int   width(char c) const;                                            // Width of a character

  public:
    MenuScene(Game::SceneMachine& machine, GBC::GameBoyColor& gbc);
    ~MenuScene() = default;

    bool  update(float elapsed) override; // Update the menu
    void  draw() override;                // Draw menu
  };
}