#pragma once

#include <functional>
#include <vector>

#include <SFML/Graphics/Text.hpp>

#include "Scenes/AbstractScene.hpp"
#include "System/Config.hpp"
#include "System/Library/FontLibrary.hpp"

namespace Game
{
  class AbstractMenuScene : public Game::AbstractScene
  {
  public:
    class Item : public sf::Text
    {
    private:
      std::function<void(Item&)>  _callback;  // Function to call when item is selected

      Item() = delete;

    public:
      Item(const sf::String& string, const std::function<void(Item&)>& callback) :
        sf::Text(string, Game::FontLibrary::Instance().get(Game::Config::ExecutablePath + "assets/fonts/pixelated.ttf")),
        _callback(callback)
      {}

      ~Item() = default;

      void  select() { _callback(*this); }  // Call item function
    };

  private:
    sf::Text          _title;   // Top text
    std::vector<Item> _items;   // Hold menu text and select handler
    Item              _footer;  // Bottom menu item
    int               _select;  // Index of selected item
    int               _target;  // Scroll target position
    float             _scroll;  // Scroll current position

    void  drawTitle();
    void  drawItems();
    void  drawFooter();

  public:
    AbstractMenuScene(Game::SceneMachine& machine);
    virtual ~AbstractMenuScene() = 0;

    virtual bool  update(sf::Time elapsed) override;  // Update menu state
    virtual void  draw() override;                    // Draw menu state

    void  title(const std::string& string);                                               // Change title
    void  add(const std::string& string, const std::function<void(Item&)>& callback);     // Add item to menu
    void  footer(const std::string& string, const std::function<void(Item&)>& callback);  // Change footer
    void  clear();                                                                        // Clear menu
    bool  empty() const;                                                                  // Check for empty menu
  };
}