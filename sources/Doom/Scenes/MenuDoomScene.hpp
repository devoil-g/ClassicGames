#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <list>

#include <SFML/Graphics/Image.hpp>

#include "Doom/Doom.hpp"
#include "Doom/Camera.hpp"
#include "Scenes/AbstractScene.hpp"
#include "System/Window.hpp"
  
namespace DOOM
{
  class MenuDoomScene : public Game::AbstractScene
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
        std::uint64_t     texture;    // Texture of item
        bool              selectable; // True if item can be selected
        int               x, y;       // Position of item on screen
        Game::Window::Key hotkey;     // Shortcut to item selection

        std::function<void()> select; // Callback when selected
        std::function<void()> left;   // Callback when left arrow is used
        std::function<void()> right;  // Callback when right arrow is used
      };

      struct Slider
      {
        int                   x, y; // Slider position
        std::function<int()>  get;  // Value getter for drawing (must return [0-16])
      };

      std::list<DOOM::MenuDoomScene::Menu::Item>    items;    // Items of menu
      std::list<DOOM::MenuDoomScene::Menu::Slider>  sliders;  // Sliders of menu

      std::function<void()> escape; // Callback when ESC is pressed
    };

    DOOM::Doom&   _doom;    // DOOM instance
    std::uint8_t  _episode; // Selected episode

    DOOM::Camera  _camera;  // Background camera

    MenuEnum                    _menuIndex;   // Current menu
    int                         _menuCursor;  // Menu cursor index
    std::array<Menu, MenuCount> _menuDesc;    // Menus descriptions
    float                       _menuElapsed; // Used for skull animation
    sf::Image                   _menuImage;   // Pre-render target of menu

    void  start();  // Start game

    void  updateSelect();
    void  updateUp();
    void  updateDown();
    void  updateLeft();
    void  updateRight();
    void  updateEscape();

  public:
    MenuDoomScene(Game::SceneMachine& machine, DOOM::Doom& doom, bool random = false);
    ~MenuDoomScene() override = default;

    bool  update(float elapsed) override; // Update state
    void  draw() override;                // Draw state
  };
}