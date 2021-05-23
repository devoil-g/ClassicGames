#pragma once

#include <functional>
#include <vector>

#include <SFML/Graphics/Text.hpp>

#include "Scenes/AbstractScene.hpp"

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
      Item(const sf::String& string, const sf::Font& font, std::function<void(Item&)>& callback) : sf::Text(string, font), _callback(callback) {}
      ~Item() = default;

      void  select() { _callback(*this); }  // Call item function
    };

  private:
    std::vector<Item> _menu;      // Hold menu text and select handler
    int               _selected;  // Index of selected menu (-1 for no selection)

  public:
    AbstractMenuScene(Game::SceneMachine& machine);
    virtual ~AbstractMenuScene() = 0;

    virtual bool  update(sf::Time elapsed) override;  // Update menu state
    virtual void  draw() override;                    // Draw menu state

    std::vector<Item>&        menu() { return _menu; }        // Get/set menu
    const std::vector<Item>&  menu() const { return _menu; }  // Get menu
  };
}