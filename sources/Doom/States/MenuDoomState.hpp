#pragma once

#include <functional>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Camera.hpp"
#include "States/AbstractState.hpp"
  
namespace DOOM
{
  class MenuDoomState : public Game::AbstractState
  {
  private:
    static const unsigned int SkullDuration;  // Duration of skull animation frame (tics)

    enum MenuEnum
    {
      MenuMain,
      MenuEpisode,
      MenuSkill,
      MenuOptions,
      MenuVolume,
      MenuRead1,
      MenuRead2,
      MenuCount
    };

    struct Menu
    {
      struct Item
      {
        uint64_t          texture;    // Texture of item
        bool              selectable; // True if item can be selected
        int               x, y;       // Position of item on screen
        sf::Keyboard::Key hotkey;     // Shortcut to item selection

        std::function<void()> select; // Callback when selected
        std::function<void()> left;   // Callback when left arrow is used
        std::function<void()> right;  // Callback when right arrow is used
      };

      struct Slider
      {
        int                   x, y; // Slider position
        std::function<int()>  get;  // Value getter for drawing (must return [0-16])
      };

      std::list<DOOM::MenuDoomState::Menu::Item>    items;    // Items of menu
      std::list<DOOM::MenuDoomState::Menu::Slider>  sliders;  // Sliders of menu

      std::function<void()> escape; // Callback when ESC is pressed
    };

    DOOM::Doom& _doom;    // DOOM instance
    uint8_t     _episode; // Selected episode

    DOOM::Camera  _backCamera;  // Background camera
    sf::Image     _backImage;   // Camera rendering target
    sf::Texture   _backTexture; // VRAM image
    sf::Sprite    _backSprite;  // Background drawable

    MenuEnum                    _menuIndex;   // Current menu
    int                         _menuCursor;  // Menu cursor index
    std::array<Menu, MenuCount> _menuDesc;    // Menus descriptions
    sf::Time                    _menuElapsed; // Used for skull animation
    sf::Image                   _menuImage;   // Camera rendering target
    sf::Texture                 _menuTexture; // VRAM image
    sf::Sprite                  _menuSprite;  // Menu drawable

    void  start();  // Start game

    void  updateSelect();
    void  updateUp();
    void  updateDown();
    void  updateLeft();
    void  updateRight();
    void  updateEscape();

    void  drawMenu(const DOOM::Doom::Resources::Texture& texture, int texture_x, int texture_y, bool shadow); // Draw a texture at specified position in menu

  public:
    MenuDoomState(Game::StateMachine& machine, DOOM::Doom& doom);
    ~MenuDoomState() override = default;

    bool  update(sf::Time elapsed) override;  // Update state
    void  draw() override;                    // Draw state
  };
}